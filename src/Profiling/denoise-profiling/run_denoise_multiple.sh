#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_denoise_single.sh chrono\""
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
	make run_denoise_multiple_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./denoise_multiple.o /home/sayandipde/Approx_IBC/final_app/cpp/Profiling/multiple-images/ 3 3 0.12
fi

