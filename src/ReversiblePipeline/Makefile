
IMACSROOT = /home/sajid/Desktop/cppvrepimacs
HALIDE_PATH = /home/sajid/apps/Halide
OPENCV_PATH = /home/sajid/apps/opencv
PKG_CONFIG_PATH =/usr/lib/pkgconfig

INSTALLDIR  = ../bin

SRC_DIR := src
OBJ_DIR := obj
INCLUDE_DIR := include
#LIB_DIR2 directory to move the lib files. Use this for now
LIB_DIR := lib/
#DO NOT CHANGE THE TRAILING / for LIB_DIR

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX         = g++

CPPFLAGS := -I$(IMACSROOT)/include
CFLAGS   := -g -D_GLIBCXX_USE_CXX11_ABI=0
LDFLAGS  := -L /usr/local/lib -ljpeg
LDLIBS   := -fno-rtti -lpthread -ldl -ltinfo -lz -lm
HALIDE_FLAGS := -I$(HALIDE_PATH)/include -I$(HALIDE_PATH)/tools -L$(HALIDE_PATH)/bin -lHalide
OPENCV_FLAGS := -frtti -L$(OPENCV_PATH)/include -L$(OPENCV_PATH)/build/include -L$(OPENCV_PATH)/build/lib -L $(PKG_CONFIG_PATH)
PROFILER_FLAGS :=  -DPROFILEWITHCHRONO

COMPILE_COMMAND := $(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11
LIB_COMMAND := -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
LIB_EXTS := h a schedule
OBJECTS := $(OBJ_DIR)/auto_scheduler_generate_fwd_v0.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v1.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v2.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v3.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v4.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v5.o $(OBJ_DIR)/auto_scheduler_generate_fwd_v6.o $(OBJ_DIR)/auto_scheduler_generate_rev.o

LIBRARY := $(LIB_DIR)auto_schedule_true_fwd_v0.a $(LIB_DIR)auto_schedule_true_fwd_v1.a $(LIB_DIR)auto_schedule_true_fwd_v2.a $(LIB_DIR)auto_schedule_true_fwd_v3.a $(LIB_DIR)auto_schedule_true_fwd_v4.a $(LIB_DIR)auto_schedule_true_fwd_v5.a $(LIB_DIR)auto_schedule_true_fwd_v6.a $(LIB_DIR)auto_schedule_true_rev.a

# Default target
.PHONY: all
all: $(OBJECTS) $(LIBRARY) $(OBJ_DIR)/auto_scheduler_run.o 

#$(EXE): $(OBJ)
 #   $(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
#    $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR):
#	mkdir $@
    
$(LIB_DIR)auto_schedule_true_rev.a: $(OBJ_DIR)/auto_scheduler_generate_rev.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_rev -f auto_schedule_true_rev $(LIB_COMMAND)
	mv auto_schedule_true_rev.schedule auto_schedule_true_rev.a $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_rev.schedule $(LIB_DIR)auto_schedule_true_rev.a $(IMACSROOT)/lib	
	cp auto_schedule_true_rev.h $(IMACSROOT)/include
	mv auto_schedule_true_rev.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v0.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v0.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v0 -f auto_schedule_true_fwd_v0 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v0.a auto_schedule_true_fwd_v0.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v0.schedule $(LIB_DIR)auto_schedule_true_fwd_v0.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v0.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v0.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v1.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v1.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v1 -f auto_schedule_true_fwd_v1 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v1.a auto_schedule_true_fwd_v1.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v1.schedule $(LIB_DIR)auto_schedule_true_fwd_v1.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v1.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v1.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v2.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v2.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v2 -f auto_schedule_true_fwd_v2 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v2.a auto_schedule_true_fwd_v2.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v2.schedule $(LIB_DIR)auto_schedule_true_fwd_v2.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v2.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v2.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v3.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v3.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v3 -f auto_schedule_true_fwd_v3 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v3.a auto_schedule_true_fwd_v3.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v3.schedule $(LIB_DIR)auto_schedule_true_fwd_v3.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v3.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v3.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v4.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v4.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v4 -f auto_schedule_true_fwd_v4 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v4.a auto_schedule_true_fwd_v4.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v4.schedule $(LIB_DIR)auto_schedule_true_fwd_v4.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v4.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v4.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v5.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v5.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v5 -f auto_schedule_true_fwd_v5 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v5.a auto_schedule_true_fwd_v5.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v5.schedule $(LIB_DIR)auto_schedule_true_fwd_v5.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v5.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v5.h $(INCLUDE_DIR)
	
$(LIB_DIR)auto_schedule_true_fwd_v6.a: $(OBJ_DIR)/auto_scheduler_generate_fwd_v6.o | $(LIB_DIR)
	$(info ************ Compiling $@ **********)
	$^ -o . -g auto_schedule_gen_fwd_v6 -f auto_schedule_true_fwd_v6 $(LIB_COMMAND)
	mv auto_schedule_true_fwd_v6.a auto_schedule_true_fwd_v6.schedule $(LIB_DIR)
	cp $(LIB_DIR)auto_schedule_true_fwd_v6.schedule $(LIB_DIR)auto_schedule_true_fwd_v6.a $(IMACSROOT)/lib	
	cp auto_schedule_true_fwd_v6.h $(IMACSROOT)/include
	mv auto_schedule_true_fwd_v6.h $(INCLUDE_DIR)
	
$(LIB_DIR):
	mkdir $@
.PHONY: install
install: 
	$(info ************ Setting up IMACS **********)
	#cp $(wildcard $(LIB_DIR)*.a) $(wildcard $(LIB_DIR)*.schedule) $(IMACSROOT)/lib
	#cp $(wildcard $(INCLUDE_DIR)/*.h) $(IMACSROOT)/include
	#cp $(wildcard $(OBJ_DIR)/*.o) $(IMACSROOT)/obj
	################IMACS install success############################

.PHONY: clean 
clean:
	rm -f $(OBJECTS)
	rm -f $(wildcard $(LIB_DIR)auto_schedule_true_*.a) 
	rm -f $(wildcard $(INCLUDE_DIR)/auto_schedule_true_*.h)
	rm -f $(wildcard $(LIB_DIR)*.schedule)  
	rm -f $(wildcard $(IMACSROOT)/lib/auto_schedule_true_*.a)
	rm -f $(wildcard $(IMACSROOT)/lib/auto_schedule_true_*.schedule)
	rm -f $(wildcard $(IMACSROOT)/include/auto_schedule_true_*.h)
	rm -f $(wildcard $(IMACSROOT)/obj/auto_scheduler_generate_fwd_*.o)	
	rm -f $(wildcard $(IMACSROOT)/obj/auto_scheduler_generate_rev.o)
	rm -f $(OBJ_DIR)/auto_scheduler_run.o
	rm -f $(IMACSROOT)/obj/auto_scheduler_run.o

$(OBJ_DIR)/auto_scheduler_generate_fwd_v0.o: src/auto_scheduler_generate_fwd_v0.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp | $(OBJ_DIR)
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR):
	mkdir $@  
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v1.o: src/auto_scheduler_generate_fwd_transform_tonemap_v1.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v2.o: src/auto_scheduler_generate_fwd_transform_gamut_v2.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v3.o: src/auto_scheduler_generate_fwd_gamut_tonemap_v3.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v4.o: src/auto_scheduler_generate_fwd_transform_v4.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v5.o: src/auto_scheduler_generate_fwd_gamut_v5.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_fwd_v6.o: src/auto_scheduler_generate_fwd_tonemap_v6.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_generate_rev.o: src/auto_scheduler_generate_rev.cpp $(HALIDE_PATH)/include/Halide.h $(HALIDE_PATH)/tools/GenGen.cpp
	$(info ************ Compiling $@ **********)
	$(CXX) $^ $(HALIDE_PATH)/tools/GenGen.cpp $(CPPFLAGS)  $(HALIDE_FLAGS) $(CFLAGS) $(LDFLAGS)  `libpng-config --cflags --ldflags` `pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	
$(OBJ_DIR)/auto_scheduler_run.o: src/auto_scheduler_run.cpp src/LoadCamModel.cpp src/MatrixOps.cpp $(LIB_DIR)auto_schedule_true_rev.a $(LIB_DIR)auto_schedule_true_fwd_v0.a $(LIB_DIR)auto_schedule_true_fwd_v1.a $(LIB_DIR)auto_schedule_true_fwd_v2.a $(LIB_DIR)auto_schedule_true_fwd_v3.a $(LIB_DIR)auto_schedule_true_fwd_v4.a $(LIB_DIR)auto_schedule_true_fwd_v5.a $(LIB_DIR)auto_schedule_true_fwd_v6.a $(HALIDE_PATH)/tools/halide_benchmark.h $(HALIDE_PATH)/tools/halide_image_io.h $(HALIDE_PATH)/tools/halide_image.h $(HALIDE_PATH)/include/Halide.h include/LoadCamModel.h include/MatrixOps.h
	
	$(info ************ Compiling $@ **********)
	$(CXX)  src/auto_scheduler_run.cpp src/LoadCamModel.cpp src/MatrixOps.cpp -g \
	-I$(LIB_DIR) -I$(IMACSROOT)/include -I$(IMACSROOT)/lib -I$(IMACSROOT)/src/ReversiblePipeline/include -I$(IMACSROOT)/src/ReversiblePipeline -L$(IMACSROOT)/src/ReversiblePipeline $(HALIDE_FLAGS) \
	$(LIB_DIR)auto_schedule_true_rev.a $(LIB_DIR)auto_schedule_true_fwd_v0.a \
	$(LIB_DIR)auto_schedule_true_fwd_v1.a $(LIB_DIR)auto_schedule_true_fwd_v2.a $(LIB_DIR)auto_schedule_true_fwd_v3.a \
	$(LIB_DIR)auto_schedule_true_fwd_v4.a $(LIB_DIR)auto_schedule_true_fwd_v5.a $(LIB_DIR)auto_schedule_true_fwd_v6.a \
	$(CFLAGS) $(LDFLAGS) -DHALIDE_NO_JPEG `libpng-config --cflags --ldflags` \
	`pkg-config opencv --cflags --libs` $(LDLIBS) -o $@ -std=c++11	
	cp $@ $(IMACSROOT)/obj
	################  IMACS install success  ############################
	
	
