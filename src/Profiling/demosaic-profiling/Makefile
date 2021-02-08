
IMACSROOT = /home/sajid/Desktop/cppvrepimacs
HALIDE_PATH = /home/sajid/apps/Halide
OPENCV_PATH = /home/sajid/apps/opencv
PKG_CONFIG_PATH = /usr/lib/pkgconfig

# Specify the target file and the install directory
OUTPUTFILE  = auto_schedule_dem_fwd.a
INSTALLDIR  = ../bin

SRC_DIR := src
OBJ_DIR := $(IMACSROOT)/obj
LIB_DIR := $(IMACSROOT)/lib
INCLUDE_DIR:= $(IMACSROOT)/include

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX         = g++

CPPFLAGS := -I$(IMACSROOT)/include
CFLAGS   := -g -D_GLIBCXX_USE_CXX11_ABI=0
LDFLAGS  := -L /usr/local/lib -ljpeg
LDLIBS   := -lm -lpthread -ldl
HALIDE_FLAGS := -I$(HALIDE_PATH)/include -I$(HALIDE_PATH)/tools -L$(HALIDE_PATH)/bin -lHalide
OPENCV_FLAGS := -frtti -L$(OPENCV_PATH)/include -L$(OPENCV_PATH)/build/include -L$(OPENCV_PATH)/build/lib -L $(PKG_CONFIG_PATH)
PROFILER_FLAGS :=  -DPROFILEWITHCHRONO
# Default target
.PHONY: all
#all: $(LIB_DIR)/$(OUTPUTFILE) DEMOSAIC
all: $(LIB_DIR)/$(OUTPUTFILE) 

#$(EXE): $(OBJ)
 #   $(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
#    $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR):
#	mkdir $@
    
$(LIB_DIR)/$(OUTPUTFILE): $(OBJ_DIR)/auto_scheduler_generate_fwd.o | $(LIB_DIR)
	#ar r $@ $^
	#ranlib $@
	$^ -o . -g auto_schedule_gen_fwd -f auto_schedule_dem_fwd -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	mv auto_schedule_dem_fwd.schedule auto_schedule_dem_fwd.a $(LIB_DIR)	
	mv auto_schedule_dem_fwd.h $(INCLUDE_DIR)
	#################### successfully completed ########################
	
$(LIB_DIR):
	mkdir $@

DEMOSAIC: $(OBJ_DIR)/demosaic_multiple.o $(OBJ_DIR)/demosaic_single.o
.PHONY: install
install:
	mkdir -p $(INSTALLDIR)
	cp -p $(LIB_DIR)/$(OUTPUTFILE) $(INSTALLDIR)

.PHONY: clean 
clean:
	rm -f $(OBJ_DIR)/auto_scheduler_generate_fwd.o $(LIB_DIR)/$(OUTPUTFILE)

# Indicate dependencies of .ccp files on .hpp files
$(OBJ_DIR)/auto_scheduler_generate_fwd.o: src/auto_scheduler_generate_fwd.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp | $(OBJ_DIR)
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(HALIDE_PATH)/bin
	$(CXX) $^ $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` $(LDLIBS) -fno-rtti -o $@ -std=c++11
$(OBJ_DIR):
	mkdir $@  
$(OBJ_DIR)/demosaic_multiple.o: src/demosaic_multiple.cpp src/auto_schedule_dem_fwd.h lib/auto_schedule_dem_fwd.a
	$(CXX) src/demosaic_multiple.cpp $(CPPFLAGS) -Iinclude $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS) lib/auto_schedule_dem_fwd.a $(OPENCV_FLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) $(PROFILER_FLAGS) -o $@ -std=c++11
$(OBJ_DIR)/demosaic_single.o: src/demosaic_single.cpp src/auto_schedule_dem_fwd.h lib/auto_schedule_dem_fwd.a
	$(CXX) src/demosaic_single.cpp $(CPPFLAGS) -Iinclude $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS) lib/auto_schedule_dem_fwd.a $(OPENCV_FLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) $(PROFILER_FLAGS) -o $@ -std=c++11
