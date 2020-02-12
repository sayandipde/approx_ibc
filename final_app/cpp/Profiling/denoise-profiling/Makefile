all:


gen:
	g++  nl_means_generator.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L ~/apps/Halide/bin -lHalide \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	-fno-rtti -lpthread -ldl -lm \
	-o nl_means_generator.o -std=c++11
	
run_denoise_multiple_chrono:
	g++ denoise_multiple.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	nl_means.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o denoise_multiple.o -std=c++11

run_denoise_single_chrono:
	g++ denoise_single.cpp \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	nl_means.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHCHRONO \
	-o denoise_single.o -std=c++11
	
run_denoise_single_valgrind:
	g++ denoise_single.cpp \
	-I /usr/include/valgrind \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-I ~/anaconda3/include \
	-L ~/apps/Halide/bin -lHalide \
	-L ~/anaconda3/lib \
	nl_means.a \
	-L /usr/local/lib -ljpeg \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -DPROFILEWITHVALGRIND \
	-o denoise_single.o -std=c++11

clean:
	rm -f *.o nl_means.*
