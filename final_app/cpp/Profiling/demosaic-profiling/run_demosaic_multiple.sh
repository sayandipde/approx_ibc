#!/bin/bash

profilertype="$1"
echo $profilertype

if [ "$profilertype" != "chrono" ]; then
    echo "Error: incorrect profiler type!" >&2
    echo "... Run this script with \"/bin/bash run_demosaic_multiple.sh chrono\""
    exit 1
fi

if [ "$profilertype" = "chrono" ]; then
    echo "profiler type = chrono"
	# Compile the generator
	echo "#########################"
	echo "# Compile the generator #"
	echo "#########################"
	make gen_fwd

	# Run the generator
	echo "#########################"
	echo "#   run the generator   #"
	echo "#########################"
	./auto_scheduler_generate_fwd.o -o . -g auto_schedule_gen_fwd -f auto_schedule_dem_fwd -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16384,40
	#./auto_scheduler_generate_fwd.o -o . -g auto_schedule_gen_fwd -f auto_schedule_dem_fwd -e static_library,h,schedule target=host-profile auto_schedule=true machine_params=32,16384,40

	echo "#########################"
	echo "#   generator finished  #"
	echo "#########################"

	# Compile the runner
	echo "#########################"
	echo "#   Compile the runner  #"
	echo "#########################"
	make run_demosaic_multiple_chrono

	# run
	echo "#########################"
	echo "#          run          #"
	echo "#########################"
	./demosaic_multiple.o /home/sayandipde/Approx_IBC/final_app/cpp/Profiling/multiple-images/
fi


