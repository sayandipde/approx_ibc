QT -= core
QT -= gui

TARGET = cpp_vrep_main
TEMPLATE = app

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
    LIBS += -L/home/sayandipde/anaconda3/lib -ljpeg
    LIBS += -L/home/sayandipde/apps/Halide/bin -lHalide
    LIBS += `libpng-config --cflags --ldflags`
    LIBS += -lpthread
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_rev.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v0.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v1.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v2.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v3.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v4.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v5.a -ldl
    LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/auto_schedule_true_fwd_v6.a -ldl
	LIBS += /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/Profiling/demosaic-profiling/auto_schedule_dem_fwd.a -ldl
}


INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/include"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/remoteApi"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/eigen"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_vrep_api/other-sources"
INCLUDEPATH += "/home/sayandipde/apps/Halide/include"
INCLUDEPATH += "/home/sayandipde/anaconda3/bin"
INCLUDEPATH += "/home/sayandipde/anaconda3/include"
INCLUDEPATH += "/home/sayandipde/apps/Halide/tools"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/LaneDetection_and_Control/other-sources"
INCLUDEPATH += "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/Profiling/demosaic-profiling"

SOURCES += \
    cpp_vrep_framework.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_vrep_api/other-sources/my_vrep_api.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/LaneDetection_and_Control/lane_detection.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/LaneDetection_and_Control/lateralcontrol_multiple.cpp \  
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_vrep_api/other-sources/image_signal_processing.cpp \
	/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_vrep_api/other-sources/utils.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/LoadCamModel.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/MatrixOps.cpp \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/remoteApi/extApi.c \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/remoteApi/extApiPlatform.c \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/common/shared_memory.c

HEADERS +=\
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/remoteApi/extApi.h \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/remoteApi/extApiPlatform.h \
    /home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/vrep/programming/common/shared_memory.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
