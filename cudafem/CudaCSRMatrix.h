#ifndef CUDACSRMATRIX_H
#define CUDACSRMATRIX_H

#include "CSRMatrix.h"

class CUDABuffer;
class BaseLog;
class CudaCSRMatrix : public CSRMatrix
{
public:
    CudaCSRMatrix();
    virtual ~CudaCSRMatrix();
    
    void initOnDevice();
    CUDABuffer * valueBuf();
    CUDABuffer * rowPtrBuf();
    CUDABuffer * colIndBuf();
    void * deviceValue();
    void * deviceRowPtr();
    void * deviceColInd();
    void print(BaseLog * lg);
protected:

private:
    CUDABuffer * m_deviceValue;
    CUDABuffer * m_deviceRowPtr;
    CUDABuffer * m_deviceColInd;
};

#endif        //  #ifndef CUDACSRMATRIX_H

