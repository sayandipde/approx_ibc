#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ] && [ "$profilertype" != "valgrind" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono/valgrind\""
    exit 1
fi

if [ "$profilertype" = "chrono" ]; then
    echo "profiler type = chrono"
	# Compile the generator
	echo "#########################"
	echo "# Compile the generator #"
	echo "#########################"
	make gen

	# Run the generator
	echo "#########################"
	echo "#   run the generator   #"
	echo "#########################"
	./nl_means_generator.o -o . -g nl_means -f nl_means -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40

	echo "#########################"
	echo "#   generator finished  #"
	echo "#########################"

	# Compile the runner
	echo "#########################"
	echo "#   Compile the runner  #"
	echo "#########################"
	make run_denoise_single_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./denoise_single.o imgs/dem.output.png 3 3 0.12 imgs/den.output.png
fi

if [ "$profilertype" = "valgrind" ]; then
	echo "profiler type = valgrind"
	# Compile the generator
	echo "#########################"
	echo "# Compile the generator #"
	echo "#########################"
	make gen

	# Run the generator
	echo "#########################"
	echo "#   run the generator   #"
	echo "#########################"
	./nl_means_generator.o -o . -g nl_means -f nl_means -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	#./auto_scheduler_generate_fwd.o -o . -g auto_schedule_gen_fwd -f auto_schedule_dem_fwd -e static_library,h,schedule target=host-profile auto_schedule=true machine_params=32,16384,40

	echo "#########################"
	echo "#   generator finished  #"
	echo "#########################"

	# Compile the runner
	echo "#########################"
	echo "#   Compile the runner  #"
	echo "#########################"
	make run_denoise_single_valgrind

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	valgrind --tool=callgrind --demangle=yes --callgrind-out-file=valgrind/profile.callgrind --collect-atstart=no --dump-instr=yes --fair-sched=try ./denoise_single.o imgs/dem.output.png 3 3 0.12 imgs/den.output.png
	callgrind_annotate --show-percs=yes --inclusive=no --context=1 --threshold=90 valgrind/profile.callgrind.1 | tee valgrind/readable.profile.callgrind
	callgrind_annotate --show-percs=yes --inclusive=yes --context=1 --threshold=90 valgrind/profile.callgrind.1 | tee valgrind/readable.profile.callgrind.inclusive
fi

