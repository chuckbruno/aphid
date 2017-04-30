#include "cu/ImageBase.cuh"
#include "cu/VectorMath.cuh"
#include "cu/RayIntersection.cuh"
#include "cu/NTreeTraverse.cuh"
#include <cu/VoxelPrim.cuh>
#include "cuSMem.cuh"
#include "cuReduceInBlock.cuh"

__constant__ float3 c_frustumVec[6];
__constant__ int4 c_renderRect;

template<int NumThreads>
__global__ void assetBox_kernel(uint * pix, 
                                float * nearDepth,
                                float * farDepth,
                                NTreeBranch4 * branches,
                                NTreeLeaf * leaves,
                                Rope * ropes,
                                int * indirections,
                                Voxel * primitives)
{
    float *sdata = SharedMemory<float>();
/// 0  -> 5     grid translation and scaling
///             as translate scale
    float3 *sgts = (float3 *)&sdata[0];
/// 6           Ntraverse is leaf
    int * sntisLeaf = (int *)&sdata[6];
/// 7           Ntraverse inner split axis
    int * sntaxis = (int *)&sdata[7];
/// 8  -> 13    grid tight box
    float *sgtb = &sdata[8];
/// 14          Ntraverse inner split position
    float *sntsplitPos = &sdata[14];
/// 15          Ntraverse inner offset
    int * sntinnerOffset = (int *)&sdata[15];
/// 16 -> 21    grid box
    float *sgb = &sdata[16];
/// 24 -> NumThreads + 24 - 1   Ncurrent 
///             branch | node  for each ray
///             max reduce to Ncurrent[0] as Ntraverse
    int * scurrentNode = (int *)&sdata[24];
/// NumThreads + 24 -> NumThreads + 24 + 3 * NumThreads - 1
///             shading normal for each ray
    float3 * sshadingNormal = (float3 *)&sdata[NumThreads + 24];
/// 4 * NumThreads + 24 -> 4 * NumThreads + 24 + 8 - 1
///             current tree leaf
    NTreeLeaf * snttreeleaf = (NTreeLeaf *)&sdata[4 * NumThreads + 24];
/// 4 * NumThreads + 32 -> 4 * NumThreads + 32 + 10 * MAX_NUM_PRIM_PER_LEAF - 1
///             current leaf voxels
    Voxel * sleafPrims = (Voxel *)&sdata[4 * NumThreads + 32];
    
    const int tidx = threadIdx.x + blockDim.x * threadIdx.y;
    if(tidx < 1) {
        const NTreeBranch4 & rootBranch = branches[0];
/// get relative transform
/// stored in branch[0] node[1]
        const float3 * rt = (const float3 *)get_branch_node(rootBranch, 1);
        sgts[0] = rt[0];
        sgts[1] = rt[1];
        
/// get tight box
/// stored in branch[0] node[8]
        const float * tb = (const float *)get_branch_node(rootBranch, 8);
        sgtb[0] = tb[0];
        sgtb[1] = tb[1];
        sgtb[2] = tb[2];
        sgtb[3] = tb[3];
        sgtb[4] = tb[4];
        sgtb[5] = tb[5];
/// get grid box
/// stored in branch[0] node[4]
        const float * gb = (const float *)get_branch_node(rootBranch, 4);
        sgb[0] = gb[0];
        sgb[1] = gb[1];
        sgb[2] = gb[2];
        sgb[3] = gb[3];
        sgb[4] = gb[4];
        sgb[5] = gb[5];
        
    }
    __syncthreads();    
    
    uint px = getPixelCoordx();
    uint py = getPixelCoordy();
    
    Ray4 incident;
    
    v3_convert<float4, float3>(incident.o, c_frustumVec[0]);
    v3_add_mult<float4, float3, uint>(incident.o, c_frustumVec[1], px);
    v3_add_mult<float4, float3, uint>(incident.o, c_frustumVec[2], py);
    
    v3_convert<float4, float3>(incident.d, c_frustumVec[3]);
    v3_add_mult<float4, float3, uint>(incident.d, c_frustumVec[4], px);
    v3_add_mult<float4, float3, uint>(incident.d, c_frustumVec[5], py);

/// transform into grid space
    v3_minus<float4, float3>(incident.o, sgts[0]);
    v3_minus<float4, float3>(incident.d, sgts[0]);
    v3_divide<float4, float3>(incident.o, sgts[1]);
    v3_divide<float4, float3>(incident.d, sgts[1]);
    
    v3_minus<float4, float4>(incident.d, incident.o);
    v3_normalize_inplace<float4>(incident.d);
    
    Aabb4 box;
/// test tight box
    aabb4_r(box, sgtb);
    
    incident.o.w = -1e28f;
    incident.d.w = 1e28f;
    
    float t0, t1;
    
    scurrentNode[tidx] = ray_box_slab1(t0, t1, incident, box);
    __syncthreads();
    reduceMaxInBlock<NumThreads, int>(tidx, scurrentNode);
	__syncthreads();
	
/// exit if no ray intersects tight box
    if(scurrentNode[0] < 1)
        return;
    
/// start with grid box
    aabb4_r(box, sgb);
/// branch 0 node 0
    int iBranch = 0, iNode = 0;
	scurrentNode[tidx] = 0;
	__syncthreads();
    
    float3 t0Normal, t1Normal, t0Position;
    sshadingNormal[tidx] = make_float3(0.f, 0.f, 0.f);
    
    //for(int it=0;it<120;++it) {
    for(;;) {
        if(tidx < 1) {
            load_traverseNode(sntisLeaf,
                              snttreeleaf,
                              sntaxis,
                              sntsplitPos,
                              sntinnerOffset,
                              scurrentNode[tidx],
                              branches, leaves);
            
        }
        
        __syncthreads();
        
        if(*sntisLeaf) {
            if(tidx < snttreeleaf->_primLength)
                load_leafPrims<Voxel>(sleafPrims, 
                            primitives,
                            indirections, 
                            snttreeleaf, 
                            tidx);            
            __syncthreads();
        }
        
        if(((iBranch << 9) | iNode) == scurrentNode[0]) { 
/// update Ncurrent if ray is active       
            if(ray_box_slab(t0, t1,
                            t0Normal, t1Normal, 
                            incident, box)) {
            
            if(*sntisLeaf) {
/// leaf node          
                if(hit_leaf(t0, t1,
                    t0Normal, t1Normal, 
                    sshadingNormal[tidx],
                    incident,
                    sleafPrims,
                    snttreeleaf->_primLength) ) {
/// end of ray
                    iBranch = 0;
                    iNode = 0; 
                }
                else {
                    climb_rope_traverse(box, 
                            iBranch, 
                            iNode,
                            t1Normal, 
                            *snttreeleaf,
                            ropes);        
                }
            }
            else {
/// internal node
                ray_progress(t0Position, incident, t0);
                inner_traverse(box, iBranch, iNode,
                                   t0Position,
                                   *sntaxis, *sntsplitPos,
                                   *sntinnerOffset);

            }
            }
            else {
/// missed box
                iBranch = 0;
                iNode = 0; 
            }

            
        }
        __syncthreads();
/// update/restore Ncurrent
        scurrentNode[tidx] = (iBranch << 9) | iNode;
        __syncthreads();
	
/// count active rays, result stored in srayActive[0]
	    reduceMaxInBlock<NumThreads, int>(tidx, scurrentNode);
	
	    __syncthreads();
        
/// exit if no active rays
	    if(scurrentNode[0] < 1)
	        break;

    }
  
    if(px < c_renderRect.x || px >= c_renderRect.z) return;
    if(py < c_renderRect.y || py >= c_renderRect.w) return;
    
	pix[getImagePixelIdx(px, py)] = encodeRGB(128 + 127 * sshadingNormal[tidx].x,
                            128 + 127 * sshadingNormal[tidx].y,
                            128 + 127 * sshadingNormal[tidx].z);
	
}

