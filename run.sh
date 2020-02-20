#!/bin/sh
read -p "Please set correctly the paths: HALIDE_PATH, VREP_PATH, OPENCV_PATH and/or EIGEN_PATH in cpp_vrep_framework.pro, src/ReversiblePipeline/Makefile, and src/Profiling/demosaic-profiling/Makefile. Also set the path in /etc/environment or paths.sh. Press enter after setting the paths."
source /etc/environment
source paths.sh
qmake cpp_vrep_framework.pro
make
gnome-terminal -- sh -c 'cd externalApps/vrep; ./coppeliaSim.sh ../../vrep-scenes/EnterCurveTest.ttt; exec bash'
read -p "Press enter after the vrep scene has been opened"
./imacs 1

