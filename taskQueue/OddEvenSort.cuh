/*
 *  odd-even sort
 0:  67   67   67   25   25   25   25   25   25   18
 1:  138  138  25   67   52   52   52   52   18   25
 2:  52   25   138  52   67   67   67   18   52   52
 3:  25   52   52   138  133  133  18   67   67   67
 4:  133  133  133  133  138  18   133  133  133  122
 5:  207  207  151  151  18   138  138  138  122  133
 6:  158  151  207  18   151  151  151  122  138  138
 7:  151  158  18   207  152  152  122  151  144  144
 8:  159  18   158  152  207  122  152  144  151  151
 9:  18   159  152  158  122  207  144  152  152  152
 10:  242 152  159  122  158  144  207  158  158  158
 11:  152 242  122  159  144  158  158  207  159  159
 12:  122 122  242  144  159  159  159  159  207  207
 13:  220 144  144  242  220  220  220  220  220  220
 14:  144 220  220  220  242  242  242  242  242  242
odd/even  0    1    0    1    0    1    0    1    0   1 
sorted    0    0    0    0    0    0    0    0    0   1
step      0         1         2         3         4
 *
 */

#ifndef ODDEVENSORT_CUH
#define ODDEVENSORT_CUH

#include <cuda_runtime_api.h>

struct OddEvenSortTask {
    template <typename QueueType>
    __device__ int execute(QueueType * q, int * smem,
                            uint * idata,
                            int2 * nodes)
    {
        int & sWorkPerBlock = smem[0];
        int & sSorted = smem[1];
        int2 root;
        int headToSecond, spawn;
    
        if(threadIdx.x == 0) {
            sWorkPerBlock = q->dequeue();
        }
                
        __syncthreads();
        
        if(sWorkPerBlock < 0) return 0;
        
        root = nodes[sWorkPerBlock];
        if((root.y - root.x + 1) <= 256*2) {
            sort_oddEven(idata,
                       root.x,
                       root.y,
                       sSorted);
        }
        else { 
            sort_batch(idata,
                    root.x,
                       root.y,
                        sSorted);
        }
         
        __syncthreads();
        
        if(threadIdx.x == 0) {
            if(sWorkPerBlock > -1) { 
                
                int sbatch = quickSort_batchSize(root.y - root.x + 1);
                int nbatch = (root.y - root.x + 1) / sbatch;
                if(nbatch&1) nbatch++;
                headToSecond = root.x + sbatch * nbatch / 2;
                
                if((root.y - root.x + 1) > 256*2) {               
                        spawn = q->enqueue();
                        nodes[spawn].x = root.x;
                        nodes[spawn].y = headToSecond - 1;

                        spawn = q->enqueue();
                        nodes[spawn].x = headToSecond;
                        nodes[spawn].y = root.y;
                }
                    
                q->setWorkDone();
                    
                    //__threadfence_block();
            }
    }
    
    return 1;
  } 
  
  
  __device__ int quickSort_batchSize(int length)
{
    int rounded = length>>8;
    if((length & 255)>0) rounded = (length+255)>>8;
    return rounded;
}

__device__ void sort_swap(uint & a, uint & b)
{
    uint intermediate = a;
    a = b;
    b = intermediate;
}

__device__ void sort_batch(uint * data,
                            int low, int high,
                            int & sorted)
{
    int batchSize = quickSort_batchSize(high - low + 1);
    
    int batchBegin = low + threadIdx.x * batchSize;
    int batchEnd = batchBegin + batchSize - 1;
    if(batchEnd > high) batchEnd = high;

    int i;
    
    if(threadIdx.x ==0)
        sorted = 0;
        
    __syncthreads();
    
    while(sorted<1) {
               
// begin as lowest in batch     
        i= batchBegin + 1;
        for(;i<= batchEnd;i++) {
            if(data[batchBegin] > data[i])
                sort_swap(data[batchBegin], data[i]);
        }
    
// end as highest in batch    
        i= batchBegin;
        for(;i< batchEnd;i++) {
            if(data[batchEnd] < data[i])
                sort_swap(data[batchEnd], data[i]);
        }
        
        __syncthreads();
        
        if(threadIdx.x < 1) {
            sorted = 1;
        }
        
        __syncthreads();

// swap at batch begin        
        if(threadIdx.x > 0) {
            if(data[batchBegin-1] > data[batchBegin]) {
                sort_swap(data[batchBegin-1], data[batchBegin]);
                sorted = 0;
            }
        }
        
        __syncthreads();
    }
}

__device__ void sort_oddEven(uint * data,
                            int low, int high, 
                            int & sorted)
{
    if(threadIdx.x < 1) {
        sorted = 0;
    }
    
    __syncthreads();
    
    int left, right;
    while(sorted<1) {
        
        if(threadIdx.x < 1) {
            sorted = 1;
        }
        
        __syncthreads();
        
        left = low + threadIdx.x * 2;
        right = left + 1;
        if(left < high) {
            if(data[right] < data[left]) {
                sort_swap(data[left], data[right]);
                sorted = 0;
            }
        }
        
        __syncthreads();
        
        left = low + threadIdx.x * 2 + 1;
        right = left + 1;
        if(left < high) {
            if(data[right] < data[left]) {
                sort_swap(data[left], data[right]);
                sorted = 0;
            }
        }
        
        __syncthreads();
    }
}
};
#endif        //  #ifndef ODDEVENSORT_CUH

