#include "reduceBox_implement.h"
#include <bvh_math.cu>

inline __device__ void copyVola(volatile Aabb * dst, const Aabb & src)
{
    dst->low.x = src.low.x;
    dst->low.y = src.low.y;
    dst->low.z = src.low.z;
    dst->high.x = src.high.x;
    dst->high.y = src.high.y;
    dst->high.z = src.high.z;
}

template <unsigned int blockSize, bool nIsPow2>
__global__ void reduceAabbByAabb_kernel(Aabb *g_idata, Aabb *g_odata, unsigned int n)
{
    extern __shared__ Aabb sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    Aabb mySum; resetAabb(mySum);

    // we reduce multiple elements per thread.  The number is determined by the 
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {         
        expandAabb(mySum, g_idata[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n) 
            expandAabb(mySum, g_idata[i+blockSize]);  
        i += gridSize;
    } 

    // each thread puts its local sum into shared memory 
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if (blockSize >= 512) { 
        if (tid < 256) { 
            expandAabb(mySum, sdata[tid + 256]);
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    if (blockSize >= 256) { 
        if (tid < 128) { 
            expandAabb(mySum, sdata[tid + 128]); 
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    if (blockSize >= 128) { 
        if (tid <  64) { 
            expandAabb(mySum, sdata[tid +  64]); 
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    
    {
        // now that we are using warp-synchronous programming (below)
        // we need to declare our shared memory volatile so that the compiler
        // doesn't reorder stores to it and induce incorrect behavior.
        volatile Aabb * smem = sdata;
        if (blockSize >=  64) {
            expandAabb(mySum, &smem[tid + 32]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=  32) { 
            expandAabb(mySum, &smem[tid + 16]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=  16) { 
            expandAabb(mySum, &smem[tid +  8]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   8) { 
            expandAabb(mySum, &smem[tid +  4]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   4) { 
            expandAabb(mySum, &smem[tid +  2]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   2) { 
            expandAabb(mySum, &smem[tid +  1]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
    }
    
    // write result for this block to global mem 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}

template <unsigned int blockSize, bool nIsPow2>
__global__ void reduceAabbByPoints_kernel(float3 *g_idata, Aabb *g_odata, unsigned int n, unsigned maxInd)
{
    extern __shared__ Aabb sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    if(i >= maxInd) return;
    unsigned int gridSize = blockSize*2*gridDim.x;
    
    Aabb mySum; resetAabb(mySum);

    // we reduce multiple elements per thread.  The number is determined by the 
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    // n <= maxInd
    while (i < n)
    {         
        expandAabb(mySum, g_idata[i]);
        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n) {
            expandAabb(mySum, g_idata[i+blockSize]); 
        }
        i += gridSize;
        if(i >= maxInd) break;
    } 

    // each thread puts its local sum into shared memory 
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if (blockSize >= 512) { 
        if (tid < 256) { 
            expandAabb(mySum, sdata[tid + 256]);
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    if (blockSize >= 256) { 
        if (tid < 128) { 
            expandAabb(mySum, sdata[tid + 128]); 
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    if (blockSize >= 128) { 
        if (tid <  64) { 
            expandAabb(mySum, sdata[tid +  64]); 
            sdata[tid] = mySum; 
        } 
        __syncthreads(); 
    }
    
    {
        // now that we are using warp-synchronous programming (below)
        // we need to declare our shared memory volatile so that the compiler
        // doesn't reorder stores to it and induce incorrect behavior.
        volatile Aabb * smem = sdata;
        if (blockSize >=  64) {
            expandAabb(mySum, &smem[tid + 32]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=  32) { 
            expandAabb(mySum, &smem[tid + 16]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=  16) { 
            expandAabb(mySum, &smem[tid +  8]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   8) { 
            expandAabb(mySum, &smem[tid +  4]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   4) { 
            expandAabb(mySum, &smem[tid +  2]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
        if (blockSize >=   2) { 
            expandAabb(mySum, &smem[tid +  1]);
            copyVola(&smem[tid], mySum);
            __syncthreads(); 
        }
    }
    
    // write result for this block to global mem 
    if (tid == 0) 
        g_odata[blockIdx.x] = sdata[0];
}

extern "C" void bvhReduceAabbByAabb(Aabb *dst, Aabb *src, unsigned numAabbs, unsigned numBlocks, unsigned numThreads)
{
	dim3 dimBlock(numThreads, 1, 1);
    dim3 dimGrid(numBlocks, 1, 1);
	int smemSize = (numThreads <= 2) ? 2 * numThreads * sizeof(Aabb) : numThreads * sizeof(Aabb);
	
	if (isPow2(numAabbs)) {
		switch (numThreads)
		{
		case 512:
			reduceAabbByAabb_kernel<512, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 256:
			reduceAabbByAabb_kernel<256, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 128:
			reduceAabbByAabb_kernel<128, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 64:
			reduceAabbByAabb_kernel<64, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 32:
			reduceAabbByAabb_kernel<32, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 16:
			reduceAabbByAabb_kernel<16, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  8:
			reduceAabbByAabb_kernel< 8, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  4:
			reduceAabbByAabb_kernel< 4, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  2:
			reduceAabbByAabb_kernel< 2, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  1:
			reduceAabbByAabb_kernel< 1, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		}
	}
	else {
		switch (numThreads)
		{
		case 512:
			reduceAabbByAabb_kernel<512, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 256:
			reduceAabbByAabb_kernel<256, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 128:
			reduceAabbByAabb_kernel<128, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 64:
			reduceAabbByAabb_kernel<64, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 32:
			reduceAabbByAabb_kernel<32, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case 16:
			reduceAabbByAabb_kernel<16, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  8:
			reduceAabbByAabb_kernel< 8, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  4:
			reduceAabbByAabb_kernel< 4, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  2:
			reduceAabbByAabb_kernel< 2, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		case  1:
			reduceAabbByAabb_kernel< 1, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numAabbs); break;
		}
	}
}

extern "C" void bvhReduceAabbByPoints(Aabb *dst, float3 *src, unsigned numPoints, unsigned numBlocks, unsigned numThreads, unsigned maxPInd)
{
	dim3 dimBlock(numThreads, 1, 1);
    dim3 dimGrid(numBlocks, 1, 1);
	int smemSize = (numThreads < 2) ? 2 * numThreads * sizeof(Aabb) : numThreads * sizeof(Aabb);
	
	if (isPow2(numPoints)) {
		switch (numThreads)
		{
		case 512:
			reduceAabbByPoints_kernel<512, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 256:
			reduceAabbByPoints_kernel<256, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 128:
			reduceAabbByPoints_kernel<128, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 64:
			reduceAabbByPoints_kernel<64, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 32:
			reduceAabbByPoints_kernel<32, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 16:
			reduceAabbByPoints_kernel<16, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  8:
			reduceAabbByPoints_kernel< 8, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  4:
			reduceAabbByPoints_kernel< 4, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  2:
			reduceAabbByPoints_kernel< 2, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  1:
			reduceAabbByPoints_kernel< 1, true><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		}
	}
	else {
		switch (numThreads)
		{
		case 512:
			reduceAabbByPoints_kernel<512, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 256:
			reduceAabbByPoints_kernel<256, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 128:
			reduceAabbByPoints_kernel<128, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 64:
			reduceAabbByPoints_kernel<64, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 32:
			reduceAabbByPoints_kernel<32, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case 16:
			reduceAabbByPoints_kernel<16, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  8:
			reduceAabbByPoints_kernel< 8, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  4:
			reduceAabbByPoints_kernel< 4, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  2:
			reduceAabbByPoints_kernel< 2, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		case  1:
			reduceAabbByPoints_kernel< 1, false><<< dimGrid, dimBlock, smemSize >>>(src, dst, numPoints, maxPInd); break;
		}
	}
}