/*
 *  sort node count and range
 *
 *  1    0,13
 *  2    0,6  7,13
 *  4    0,3  4,6  7,10  11,13
 *  8    0,1  2,3  4,5   5,6   7,8  9,10  11,12  13,13
 *
 *  each node consumes one spawns two or one or zero
 *  level n needs max 2^n nodes
 *  each level has node count and range updated by atomic
 *  
 *  taskIn  1  0,13 |
 *  taskOut 3  0,13 | 0,6  7,13 
 *
 *  taskIn  3  0,13   0,6  7,13 |
 *  taskOut 7  0,13   0,6  7,13 | 0,3   4,6  7,10  11,13
 *
 *  taskIn  7  0,13   0,6  7,13   0,3   4,6  7,10  11,13 |
 *  taskOut 15 0,13   0,6  7,13   0,3   4,6  7,10  11,13 | 0,1  2,3  4,5   5,6   7,8  9,10  11,12  13,13
 *
 *  when taskIn.tbid > taskOut.qtail means no more work is available
 *  no more work is available doesn't mean the job is done
 *  still could be more task added, need workDoneCounter
 *  once workDoneCounter > taskOut.qtail, there is nothing left to do
 *  intially task out is a copy of task in
 *  new tasks will be added to end
 *  each time a task is done first enqueue() of needed, 
 *  then check if work is done, if not then dequeue()
 *  
 */
 
#include "quickSort.cuh"
#include "OddEvenSort.cuh"
#include "bvh_common.h"
#include "CudaBase.h"

extern "C" {
void cu_testQuickSort(void * q,
                    uint * idata,
                    uint * nodes, 
                    int * elements,
                    SimpleQueueInterface * qi,
                    uint numElements,
                    uint maxNumBlocks,
                    uint * workBlocks,
                    uint * loopbuf,
                    int * headtailperloop)
{
    //cudaDeviceSynchronize();
    
    simpleQueue::SimpleQueue * queue = (simpleQueue::SimpleQueue *)q;
    simpleQueue::init_kernel<<< 1,32 >>>(queue, elements);
    
    const int tpb = 256;
    dim3 block(tpb, 1, 1);
    const unsigned nblk = maxNumBlocks;
    dim3 grid(nblk, 1, 1);
    
    OddEvenSortTask oes;
    
    quickSort_checkQ_kernel<simpleQueue::SimpleQueue, OddEvenSortTask, 2, 127, 24><<<grid, block, 16320>>>(queue,
                                oes,
                                qi,
                                idata,
                                (int2 *)nodes,
                                workBlocks,
                                loopbuf,
                                (int4 *)headtailperloop);
    
    CudaBase::CheckCudaError("q sort");
}

}

