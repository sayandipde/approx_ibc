all:
	./run_generator.sh

gen_fwd:
	g++  auto_scheduler_generate_fwd_v0.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -ltinfo  -lz\
	 -o auto_scheduler_generate_fwd_v0.o -std=c++11

	g++ auto_scheduler_generate_fwd_transform_tonemap_v1.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -lm -ltinfo -lz\
	 -o auto_scheduler_generate_fwd_v1.o -std=c++11

	g++ auto_scheduler_generate_fwd_transform_gamut_v2.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -lm -ltinfo -lz\
	 -o auto_scheduler_generate_fwd_v2.o -std=c++11

	g++ auto_scheduler_generate_fwd_gamut_tonemap_v3.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -lm -ltinfo -lz \
	 -o auto_scheduler_generate_fwd_v3.o -std=c++11

	g++ auto_scheduler_generate_fwd_transform_v4.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -lm -ltinfo -lz \
	-o auto_scheduler_generate_fwd_v4.o -std=c++11

	g++ auto_scheduler_generate_fwd_gamut_v5.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -ltinfo -lm -lz\
 	 -o auto_scheduler_generate_fwd_v5.o -std=c++11

	g++ auto_scheduler_generate_fwd_tonemap_v6.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L /usr/local/lib -ljpeg \
	-L ~/apps/Halide/bin -lHalide \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -ltinfo -lm -lz\
	 -o auto_scheduler_generate_fwd_v6.o -std=c++11


gen_rev:
	g++  auto_scheduler_generate_rev.cpp ~/apps/Halide/tools/GenGen.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	-L ~/apps/Halide/bin -lHalide \
	-L /usr/local/lib -ljpeg \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -fno-rtti -lpthread -ldl -lz -ltinfo \
	-o auto_scheduler_generate_rev.o -std=c++11

run:
	g++  auto_scheduler_run.cpp LoadCamModel.cpp MatrixOps.cpp -g \
	-I ~/apps/Halide/include \
	-I ~/apps/Halide/tools \
	auto_schedule_true_rev.a auto_schedule_true_fwd_v0.a \
	auto_schedule_true_fwd_v1.a auto_schedule_true_fwd_v2.a auto_schedule_true_fwd_v3.a \
	auto_schedule_true_fwd_v4.a auto_schedule_true_fwd_v5.a auto_schedule_true_fwd_v6.a \
	-L ~/apps/Halide/bin -lHalide \
	-L /usr/local/lib -ljpeg \
	-DHALIDE_NO_JPEG \
	`libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -lz -ltinfo \
	-o auto_scheduler_run.o -std=c++11

main:
	g++ main.cpp image_signal_processing.cpp  LoadCamModel.cpp MatrixOps.cpp -g \
	-I ~/apps/include \
	-I ~/apps/Halide/bin \
	-I ~/apps/Halide/tools \
	-I /home/sayandipde/Approx_IBC/eigen \
	auto_schedule_true_rev.a auto_schedule_true_fwd_v0.a \
	auto_schedule_true_fwd_v1.a auto_schedule_true_fwd_v2.a auto_schedule_true_fwd_v3.a \
	auto_schedule_true_fwd_v4.a auto_schedule_true_fwd_v5.a auto_schedule_true_fwd_v6.a \
	-L ~/apps/Halide/lib \
	-L ~/apps/Halide/bin \
	-L /usr/local/lib -ljpeg \
	-lHalide `libpng-config --cflags --ldflags` -D_GLIBCXX_USE_CXX11_ABI=0 \
	`pkg-config opencv --cflags --libs` -lpthread -ldl -lm -ltinfo -lz \
	-o main.o -std=c++11	
		
clean:
	rm -f *.o
	rm auto_schedule_true_fwd_v0.a
	rm auto_schedule_true_fwd_v0.schedule
	rm auto_schedule_true_fwd_v0.h
	rm auto_schedule_true_fwd_v1.a
	rm auto_schedule_true_fwd_v1.schedule
	rm auto_schedule_true_fwd_v1.h
	rm auto_schedule_true_fwd_v2.a
	rm auto_schedule_true_fwd_v2.schedule
	rm auto_schedule_true_fwd_v2.h
	rm auto_schedule_true_fwd_v3.a
	rm auto_schedule_true_fwd_v3.schedule
	rm auto_schedule_true_fwd_v3.h
	rm auto_schedule_true_fwd_v4.a
	rm auto_schedule_true_fwd_v4.schedule
	rm auto_schedule_true_fwd_v4.h
	rm auto_schedule_true_fwd_v5.a
	rm auto_schedule_true_fwd_v5.schedule
	rm auto_schedule_true_fwd_v5.h
	rm auto_schedule_true_fwd_v6.a
	rm auto_schedule_true_fwd_v6.schedule
	rm auto_schedule_true_fwd_v6.h
	rm auto_schedule_true_rev.a
	rm auto_schedule_true_rev.schedule
	rm auto_schedule_true_rev.h
