#!/bin/bash

# Compile the generator
echo "#########################"
echo "# Compile the generator #"
echo "#########################"
make gen_rev 
make gen_fwd

# Run the generator
echo "#########################"
echo "#   run the generator   #"
echo "#########################"
echo "rev"
./auto_scheduler_generate_rev.o    -o . -g auto_schedule_gen_rev -f auto_schedule_true_rev -e static_library,h,schedule target=host auto_schedule=false machine_params=32,16384,40
echo "v0"
./auto_scheduler_generate_fwd_v0.o -o . -g auto_schedule_gen_fwd_v0 -f auto_schedule_true_fwd_v0 -e static_library,h,schedule target=host-no_runtime auto_schedule=false machine_params=32,16384,40
echo "v1"
./auto_scheduler_generate_fwd_v1.o -o . -g auto_schedule_gen_fwd_v1 -f auto_schedule_true_fwd_v1 -e static_library,h,schedule target=host-no_runtime auto_schedule=true machine_params=32,16384,40
echo "v2"
./auto_scheduler_generate_fwd_v2.o -o . -g auto_schedule_gen_fwd_v2 -f auto_schedule_true_fwd_v2 -e static_library,h,schedule target=host-no_runtime auto_schedule=false machine_params=32,16384,40
echo "v3"
./auto_scheduler_generate_fwd_v3.o -o . -g auto_schedule_gen_fwd_v3 -f auto_schedule_true_fwd_v3 -e static_library,h,schedule target=host-no_runtime auto_schedule=false machine_params=32,16384,40
echo "v4"
./auto_scheduler_generate_fwd_v4.o -o . -g auto_schedule_gen_fwd_v4 -f auto_schedule_true_fwd_v4 -e static_library,h,schedule target=host-no_runtime auto_schedule=true machine_params=32,16384,40
echo "v5"
./auto_scheduler_generate_fwd_v5.o -o . -g auto_schedule_gen_fwd_v5 -f auto_schedule_true_fwd_v5 -e static_library,h,schedule target=host-no_runtime auto_schedule=false machine_params=32,16384,40
echo "v6"
./auto_scheduler_generate_fwd_v6.o -o . -g auto_schedule_gen_fwd_v6 -f auto_schedule_true_fwd_v6 -e static_library,h,schedule target=host-no_runtime auto_schedule=true machine_params=32,16384,40
echo "#########################"
echo "#   generator finished  #"
echo "#########################"

# Compile the runner
echo "#########################"
echo "#   Compile the runner  #"
echo "#########################"
make run

# run
echo "#########################"
echo "#          run          #"
echo "#########################"
./auto_scheduler_run.o
