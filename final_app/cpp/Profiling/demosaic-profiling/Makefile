all:


gen_fwd:
	g++  auto_scheduler_generate_fwd.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L ~/apps/Halide/bin -lHalide \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	-fno-rtti -lpthread -ldl -lm \
	-o auto_scheduler_generate_fwd.o -std=c++11

run_demosaic_multiple_chrono:
	g++ demosaic_multiple.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o demosaic_multiple.o -std=c++11

run_demosaic_single_chrono:
	g++ demosaic_single.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o demosaic_single.o -std=c++11	
	
run_demosaic_single_valgrind:
	g++ demosaic_single.cpp \
	-I /usr/include/valgrind \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	auto_schedule_dem_fwd.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRIND \
	-o demosaic_single.o -std=c++11	

clean:
	rm -f *.o auto_schedule_dem_fwd.*
