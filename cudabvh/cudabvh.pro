macx:CONFIG -= app_bundle
CONFIG += release
message(QMAKE with CUDA)
INCLUDEPATH += ./ ../shared ../radixsort
HEADERS       = glWidget.h \
                window.h \
                BvhTriangleMesh.h \
                BvhSolver.h \
                CudaLinearBvh.h \
                CudaParticleSystem.h \
                simpleMesh.h \
                rayTest.h \
                bvh_common.h \
bvh_dbg.h \
				app_define.h \
                plane_implement.h \
                createBvh_implement.h \
                traverseBvh_implement.h \
        reduce_common.h \
		reduceBox_implement.h \
		reduceRange_implement.h \
		particleSystem_implement.h \                                                                                 
		../radixsort/radixsort_implement.h \
                ../shared/CudaBase.h \
                ../shared/Base3DView.h \
                ../shared/BaseBuffer.h \
                ../shared/CUDABuffer.h \
                ../shared/BaseSolverThread.h \
                CudaDynamicSystem.h  \
		CollisionQuery.h \
		CollisionObject.h \
		TetrahedronSystem.h \
		CudaTetrahedronSystem.h \
		DrawBvh.h \
		CudaBroadphase.h \
		broadphase_implement.h \
		scan_implement.h
SOURCES       = main.cpp \
                glWidget.cpp \
                window.cpp \
                BvhTriangleMesh.cpp \
                BvhSolver.cpp \
                CudaLinearBvh.cpp \
                CudaParticleSystem.cpp \
                simpleMesh.cpp \
                rayTest.cpp \
                ../shared/CudaBase.cpp \
                ../shared/Base3DView.cpp \
                ../shared/BaseBuffer.cpp \
                ../shared/CUDABuffer.cpp \
                ../shared/BaseSolverThread.cpp \
                CudaDynamicSystem.cpp \
		CollisionQuery.cpp \
		 CollisionObject.cpp \
		 TetrahedronSystem.cpp \
		 CudaTetrahedronSystem.cpp \
		 DrawBvh.cpp \
		 CudaBroadphase.cpp

win32 {
    HEADERS += ../shared/gExtension.h
    SOURCES += ../shared/gExtension.cpp
    INCLUDEPATH += D:/usr/boost_1_51_0 /usr/OpenEXR/include
    LIBS += -LD:/usr/openEXR/lib -LD:/usr/boost_1_51_0/stage/lib
    CONFIG += console
}
macx {
    INCLUDEPATH += ../../Library/boost_1_55_0
        LIBS += -L../../Library/boost_1_55_0/stage/lib -lboost_date_time -lboost_system -lboost_thread -lboost_filesystem
}
QT           += opengl
LIBS += -L../lib -laphid -lIlmImf -lHalf
DESTDIR = ./
OBJECTS_DIR = release/obj
MOC_DIR = release/moc
RCC_DIR = release/rcc
UI_DIR = release/ui

CUSOURCES = bvh_math.cu plane.cu createBvh.cu traverseBvh.cu reduceBox.cu reduceRange.cu \
            ../radixsort/radixsort.cu \
            particleSystem.cu \
            broadphase.cu \
            scan.cu
macx {
CUDA_CC = /usr/local/cuda/bin/nvcc
CUDA_DIR = /usr/local/cuda
CUDA_DEV = "/Developer/GPU Computing/C"
CUDA_COMMON = $$CUDA_DEV/common
CUDA_INC_PATH = $$CUDA_DIR/include
CUDA_SHARED = "/Developer/GPU Computing/shared"
message("nvcc resides in :" $$CUDA_CC)
INCLUDEPATH += $$CUDA_INC_PATH \
    $$CUDA_COMMON/inc \
    $$CUDA_SHARED/inc
LIBS += -lcuda \
    -lcudart \
    -lcutil_i386 \
    -lshrutil_i386
QMAKE_LIBDIR += $$CUDA_DIR/lib \
    $$CUDA_COMMON/lib/darwin \
    $$CUDA_DEV/lib \
    $$CUDA_SHARED/lib
CUDA_MACHINE_FLAG = -m32
}
win32 {
CUDA_CC =D:/usr/cuda4/v4.0/bin/nvcc.exe
CUDA_DIR = D:/usr/cuda4/v4.0
CUDA_DEV = "D:/usr/cuda4_sdk/C"

## CUDA_CC =C:/CUDA/bin64/nvcc.exe
## CUDA_DIR = C:/CUDA
## CUDA_DEV = "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/C"

CUDA_COMMON = $$CUDA_DEV/common
CUDA_INC_PATH = $$CUDA_DIR/include
message("nvcc resides in :" $$CUDA_CC)
INCLUDEPATH += $$CUDA_INC_PATH \
                $$CUDA_COMMON/inc

QMAKE_LIBDIR += $$CUDA_DIR/lib/x64 \
                $$CUDA_COMMON/lib/x64
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libcmt  /NODEFAULTLIB:libcpmt
LIBS += -lcuda -lcudart
CUDA_MACHINE_FLAG = -m64
}

cuda.name = CUDA
cuda.input = CUSOURCES
cuda.dependency_type = TYPE_C
cuda.variable_out = OBJECTS
cuda.output = ${QMAKE_FILE_IN}$$QMAKE_EXT_OBJ
cuda.commands = $$CUDA_CC \
    -c \
    --ptxas-options=-v \
    $$CUDA_MACHINE_FLAG \
    -arch sm_11 \
    $$join(INCLUDEPATH,'" -I "','-I "','"') \
    ${QMAKE_FILE_IN} \
    -o \
    ${QMAKE_FILE_OUT} # Note the -O0
QMAKE_EXTRA_COMPILERS += cuda
## DEFINES += CUDA_V3

