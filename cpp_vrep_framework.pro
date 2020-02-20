QT -= core
QT -= gui

TARGET = imacs
TEMPLATE = app

IMACSROOT = $(PWD)
OPENCV_PATH = /home/sajid/apps/opencv
HALIDE_PATH = /home/sajid/apps/Halide
VREP_PATH   = $$IMACSROOT/externalApps/vrep
EIGEN_PATH  = $$IMACSROOT/externalApps/eigen

DEFINES -= UNICODE
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += NON_MATLAB_PARSING
DEFINES += MAX_EXT_API_CONNECTIONS=255
DEFINES += DO_NOT_USE_SHARED_MEMORY
DEFINES += HALIDE_NO_JPEG
DEFINES += _GLIBCXX_USE_CXX11_ABI=0



*-msvc* {
    QMAKE_CXXFLAGS += -W3
}
*-g++* {
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings
    QMAKE_CXXFLAGS += -g
    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing
    QMAKE_CXXFLAGS += -std=c++11 

    QMAKE_CFLAGS += -Wall
    QMAKE_CFLAGS += -Wno-strict-aliasing
    QMAKE_CFLAGS += -Wno-unused-parameter
    QMAKE_CFLAGS += -Wno-unused-but-set-variable
    QMAKE_CFLAGS += -Wno-unused-local-typedefs  
	 
}


win32 {
    LIBS += -lwinmm
    LIBS += -lWs2_32
}

macx {
}


unix:!macx {
    LIBS += -lrt
    LIBS += -ldl
    LIBS += -lm
    LIBS += `pkg-config opencv --cflags --libs`
    LIBS += -L/usr/lib
    LIBS += -L/usr/lib/pkgconfig
    LIBS += -L$$OPENCV_PATH -ljpeg
    LIBS += -L$$HALIDE_PATH/bin -lHalide
    LIBS += `libpng-config --cflags --ldflags`
    LIBS += -lpthread
    LIBS += $$IMACSROOT/lib/auto_schedule_true_rev.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v0.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v1.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v2.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v3.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v4.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v5.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_true_fwd_v6.a -ldl
    LIBS += $$IMACSROOT/lib/auto_schedule_dem_fwd.a -ldl
    OBJECTS_DIR = $$IMACSROOT/obj
    extraclean.commands = rm -f $(wildcard ./lib/*.schedule) $(wildcard ./lib/*.a) $(wildcard ./obj/*.o) $(wildcard ./include/auto_schedule*) $(wildcard ./src/ReversiblePipeline/lib/*.a)  $(wildcard ./src/ReversiblePipeline/lib/*.schedule) $(wildcard ./src/ReversiblePipeline/obj/*.o) $(wildcard ./src/ReversiblePipeline/include/auto_schedule_true_*)
    distclean.depends = extraclean
    QMAKE_EXTRA_TARGETS += distclean extraclean
}


INCLUDEPATH += "$$VREP_PATH/programming/include"
INCLUDEPATH += "$$VREP_PATH/programming/remoteApi"
INCLUDEPATH += "$$EIGEN_PATH"
INCLUDEPATH += "$$IMACSROOT/src/cpp_vrep_api"
INCLUDEPATH += "$$IMACSROOT/include"
INCLUDEPATH += "$$HALIDE_PATH/include"
INCLUDEPATH += "/usr/lib/pkgconfig"
INCLUDEPATH += "$$HALIDE_PATH/tools"
INCLUDEPATH += "$$IMACSROOT/src/ReversiblePipeline/src"
INCLUDEPATH += "$$IMACSROOT/src/LaneDetection_and_Control"
INCLUDEPATH += "$$IMACSROOT/src/Profiling/demosaic-profiling/src"

SOURCES += \
	$$IMACSROOT/src/cpp_vrep_api/cpp_vrep_framework.cpp \
	$$IMACSROOT/src/cpp_vrep_api/my_vrep_api.cpp \
	$$IMACSROOT/src/LaneDetection_and_Control/lane_detection.cpp \
	$$IMACSROOT/src/LaneDetection_and_Control/lateralcontrol_multiple.cpp \  
    $$IMACSROOT/src/cpp_vrep_api/image_signal_processing.cpp \
	$$IMACSROOT/src/cpp_vrep_api/utils.cpp \
    $$IMACSROOT/src/ReversiblePipeline/src/LoadCamModel.cpp \
    $$IMACSROOT/src/ReversiblePipeline/src/MatrixOps.cpp \
    $$VREP_PATH/programming/remoteApi/extApi.c \
    $$VREP_PATH/programming/remoteApi/extApiPlatform.c \
    $$VREP_PATH/programming/common/shared_memory.c

HEADERS +=\
    $$VREP_PATH/programming/remoteApi/extApi.h \
    $$VREP_PATH/programming/remoteApi/extApiPlatform.h \
    $$VREP_PATH/programming/include/shared_memory.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
!exists( $$IMACSROOT/lib/auto_schedule_true_rev.a ) {
	message("Make sure the paths IMACSROOT, HALIDE_PATH and OPENCV_PATH are set correctly in the Makefile in $$IMACSROOT/src/ReversiblePipeline." )
	system(cd src/ReversiblePipeline && make)
	
}
!exists( $$IMACSROOT/lib/auto_schedule_dem_fwd.a ) {	
    message("Make sure the paths IMACSROOT, HALIDE_PATH and OPENCV_PATH are set correctly in the Makefile $$IMACSROOT/src/Profiling/demosaic-profiling." )
    system(cd src/Profiling/demosaic-profiling && make)
}
