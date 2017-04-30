#ifndef CUDABASE_H
#define CUDABASE_H
#include <cuda_runtime_api.h>
#include <string>

namespace aphid {

class CudaBase
{
public:
    CudaBase();
    virtual ~CudaBase();
    
    static char CheckCUDevice();
    static void SetDevice();
    
    static int MaxThreadPerBlock;
    static int MaxRegisterPerBlock;
    static int MaxSharedMemoryPerBlock;
	static int WarpSize;
	static int RuntimeVersion;
	static bool HasDevice;
    static int LimitNThreadPerBlock(int regPT, int memPT);
	static void CheckCudaError(cudaError_t err, const char * info);
    static void CheckCudaError(const char * info);
    static cudaError_t Synchronize();
    static int MemoryUsed;
	static std::string BreakInfo;
private:
};

}
#endif        //  #ifndef CUDABASE_H

