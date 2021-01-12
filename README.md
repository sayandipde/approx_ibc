# Setting up IMACS Approx_IBC in Linux
Approximation Trade Offs in an Image Based Control System.
Tested using the following versions. If you would like to use latest versions, you might have to edit the syntax of some functions.

If you are using this simulator in any form in your work, please cite:
```
S. De, S. Mohamed, K. Bimpisidis, D. Goswami, T. Basten and H. Corporaal, "Approximation Trade Offs in an Image-Based Control System," 2020 Design, Automation & Test in Europe Conference & Exhibition (DATE), Grenoble, France, 2020, pp. 1680-1685, doi: 10.23919/DATE48585.2020.9116552.

S. De, S. Mohamed, D. Goswami and H. Corporaal, "Approximation-Aware Design of an Image-Based Control System," in IEEE Access, vol. 8, pp. 174568-174586, 2020, doi: 10.1109/ACCESS.2020.3023047.
```

Tested with the following versions:
* OS: Ubuntu 18.04
* g++ (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0
* llvm: clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
* Halide: checkout branch 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c

1. Install dependent libraries
2. Install Halide
3. Install OpenCV
4. Install Eigen
5. Install VREP
6. Run IMACS

Initially, clone this repository:
```
git clone https://github.com/sayandipde/approx_ibc.git
cd approx_ibc
pwd
```
For brevity, `$(root)=pwd`, i.e. the path to `approx_ibc` is called as `$(root)` or `$(IMACSROOT)`. 

# 1-5 Dependencies

## 1. Dependent libraries
The following libraries might be needed for successful execution.
```
sudo apt-get install libtinfo-dev
sudo apt-get install libjpeg-dev
sudo apt-get install libtiff5-dev
sudo apt-get install libpng-dev
sudo apt-get install jasper
sudo apt-get install libgtk-3-dev
sudo apt-get install libopenblas-dev
sudo apt-get install -y libavcodec-dev
sudo apt-get install -y libavformat-dev
sudo apt-get install libavutil-dev
sudo apt-get install libswscale-dev
sudo apt-get install valgrind
# For openCV installation
sudo apt-get install cmake
# LibRAW for reversible pipeline
sudo apt-get install -y libraw-dev
## qt4
sudo apt-get install qt4-qmake
sudo apt-get install libqt4-dev
```

## 2. Installing Halide

If already installed, only export the paths of `Halide/bin` and `Halide/distrib/bin` to `LD_LIBRARY_PATH`. 

Note that for our framework, we use a specific checkout version `git checkout 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c`. If you are using the latest version, you may have to adapt the syntax in the source files.

For more details, https://github.com/halide/Halide/blob/master/README.md.

If not already installed, follow the steps below.

Building Halide
===============

### 2.1 Dependencies for Halide

1. Install CLANG + LLVM
2. Make in HALIDE_PATH
3. Install Halide

#### 2.1.1 CLANG + LLVM
Have clang+llvm-6.0.0 installed. If already installed, you only need to export the paths CLANG and LLVM_CONFIG.
1. Download the Pre-Built Binary corresponding to your OS from http://releases.llvm.org/download.html.
Recommended version: LLVM 6.0.0
2. Extract the downloaded file into the folder `llvm` in `$(root)/externalApps`. If this folder does not exist `mkdir externalApps`.
```
cd $(root)/externalApps
mkdir llvm
tar -xvf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz -C llvm --strip-components=1
#export CLANG=<path to clang>/bin/clang
export CLANG=$(root)/externalApps/llvm/bin/clang
#export LLVM_CONFIG=<path to llvm>/bin/llvm-config
export LLVM_CONFIG=$(root)/externalApps/llvm/bin/llvm-config
```
Alternately, these paths can be added to the `/etc/environment` file.
```
sudo gedit /etc/environment
```
Add these lines to the end of this file, save and close the file. As an example,
```
# root = /home/sajid/Desktop/IMACS_cppVrep
CLANG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/clang
LLVM_CONFIG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/llvm-config
```
source the paths for this to take effect.
```
source /etc/environment
```

If you do not have `sudo` rights, please follow the steps in `FAQ 4`.
#### 2.1.2 Building Halide with make

With `LLVM_CONFIG` set (or `llvm-config` in your path), you should be
able to just run `make` in the root directory of the Halide source tree.
We checkout a specific version as the syntax of codes in Approx_IBC is based on this version.
```
cd $(root)/externalApps
git clone https://github.com/halide/Halide.git
cd Halide
git checkout 810a14b1cef3eb99c4051a2a7ca0c50a9872c37c
gedit Makefile
```
Append ` -D_GLIBCXX_USE_CXX11_ABI=0` to lines 95 and 222 (there is a space in front) to the `Makefile`. This appends the flag `-D_GLIBCXX_USE_CXX11_ABI=0` to `LLVM_CXX_FLAGS` and `TEST_CXX_FLAGS`. Save and close the `Makefile`.
The lines will then look like the following: line 95- `LLVM_CXX_FLAGS += -DLLVM_VERSION=$(LLVM_VERSION_TIMES_10) -D_GLIBCXX_USE_CXX11_ABI=0`; line 222- `TEST_CXX_FLAGS ?= $(TUTORIAL_CXX_FLAGS) $(CXX_WARNING_FLAGS) -march=native -D_GLIBCXX_USE_CXX11_ABI=0`.

Save and then Run the `Makefile`. This should not result in any errors.
```
make
```
A successful installation means all the tests are passed.

#### 2.1.3 Installing Halide
Halide does not have a `make install`. To install we use the following command.
```
make distrib
```
This will copy files to the `distrib` folder. 
Export the Halide bin to the library path.
```
export LD_LIBRARY_PATH=$(ROOT)/externalApps/Halide/distrib/bin:$(ROOT)/externalApps/Halide/bin:$LD_LIBRARY_PATH
```

## 3. Installing OpenCV

If opencv is already installed, make sure `opencv.pc` file is in the `/usr/lib/pkgconfig` or change the `PKG_CONFIG_PATH` to point to this file.

If not installed, follow the OpenCV installation steps from https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html.

Required Packages:
* GCC 4.4.x or later
* CMake 2.8.7 or higher
* Git
* GTK+2.x or higher, including headers (libgtk2.0-dev)
* pkg-config
* Python 2.6 or later and Numpy 1.5 or later with developer packages (python-dev, python-numpy)
* ffmpeg or libav development packages: libavcodec-dev, libavformat-dev, libswscale-dev
* [optional] libtbb2 libtbb-dev
* [optional] libdc1394 2.x
* [optional] libjpeg-dev, libpng-dev, libtiff-dev, libjasper-dev, libdc1394-22-dev
* [optional] CUDA Toolkit 6.5 or higher

### 3.1 Install ccache
```
sudo apt install -y ccache
sudo /usr/sbin/update-ccache-symlinks
echo 'export PATH="/usr/lib/ccache:$PATH"' | tee -a ~/.bashrc
source ~/.bashrc && echo $PATH
```
### 3.2 Install java
If not already installed, you can follow the steps from here: https://www.digitalocean.com/community/tutorials/how-to-install-java-with-apt-on-ubuntu-18-04.

In case there is an error: unable to locate `*.jar`
```
sudo apt install default-jre
sudo apt install default-jdk
sudo update-alternatives --config javac
sudo update-alternatives --config java
```
Choose the correct `jdk` version. `jre` does not have `tools.jar`
Update the JAVA_HOME path
```
sudo gedit /etc/environment
```
At the end of this file, add the following line, making sure to replace the path with your own copied path:
```
JAVA_HOME="/usr/lib/jvm/java-11-openjdk-amd64/bin/"
```
Modifying this file will set the JAVA_HOME path for all users on your system.
Save the file and exit the editor.

Now reload this file to apply the changes to your current session:
```
source /etc/environment
```
### 3.3 Getting OpenCV Source Code
```
cd $(root)/externalApps
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
```
### 3.4 Building OpenCV from Source Using CMake
```
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. -DOPENCV_GENERATE_PKGCONFIG=ON
make
sudo make install
```

## 4. Install eigen
```
cd $(root)/externalApps
git clone https://gitlab.com/libeigen/eigen.git
```
## 5. Install Vrep
```
cd $(root)/externalApps
```
Download the linux version from https://coppeliarobotics.com/downloads.
```
mkdir vrep
tar -xvf CoppeliaSim_Edu_V4_0_0_Ubuntu18_04.tar.xz -C vrep --strip-components=1
```
Generally for local tar file:
```
tar -xvf FILENAME -C FOLDER --strip-components=1
```
The `--strip-components` flag is used when a tar file would naturally expand itself into a folder, let say, like github where it examples to `repo-name-master` folder. Of course you wouldn’t need the first level folder generated here so `--strip-components` set to 1 would automatically remove that first folder for you. The larger the number is set the deeper nested folders are removed.

# 6. Running IMACS

## 6.1 Change hardcoded paths
Please change paths to your own system in the following files
```
$(IMACSROOT)/src/LaneDetection_and_Control/lane_detection.cpp
$(IMACSROOT)/src/cpp_vrep_api/image_signal_processing.cpp
$(IMACSROOT)/src/Profiling/demosaic-profiling/Makefile
$(IMACSROOT)/src/ReversiblePipeline/Makefile
$(IMACSROOT)/cpp_vrep_framework.pro
```
1. Change the `IMACSROOT`, `HALIDE_PATH` and `OPENCV_PATH` in both `Makefile`s to the corresponding PATH in your system. 
2. If the external applications Halide, vrep, eigen or opencv are installed to another directory than the `$(IMACSROOT)/externalApps`, change the corresponding `HALIDE_PATH`, `OPENCV_PATH`, `EIGEN_PATH`, and `VREP_PATH` in `cpp_vrep_framework.pro` to the corresponding PATH in your system. 

To obtain the path, you could run `pwd` in a terminal opened from the corresponding folder.
3. Change line 89 of `lane_detection.cpp` to `string out_string = "$(IMACSROOT)/out_imgs";` where $(IMACSROOT) is your actual path.
4. Change line 143 of `image_signal_processing.cpp` to `"$(IMACSROOT)/src/ReversiblePipeline/camera_models/NikonD7000/";` where $(IMACSROOT) is your actual path.
5. Change line 77 of `image_signal_processing.cpp` to `string out_string = "$(IMACSROOT)/out_imgs";` where $(IMACSROOT) is your actual path.

## 6.2 Make IMACS
Generate the `Makefile` using `qmake`.
```
cd $(IMACSROOT)
qmake cpp_vrep_framework.pro
make
```

When you want to run `make` in `$(IMACSROOT)` always run `qmake cpp_vrep_framework.pro` first. 
`make clean` cleans the file generated by this make.
`make distclean` cleans all the generated files by IMACS.

## 6.3 Start a vrep scene
Open a vrep-scene. Vrep needs to be started before running IMACS framework.
```
cd $VREP_PATH
./coppeliaSim.sh ../../vrep-scenes/SmallBias_report.ttt
```
If you are using an older version of vrep (`coppeliaSim.sh is not found`), please use the following command:
```
./vrep.sh ../../vrep-scenes/EnterCurveTest.ttt
```
## 6.4 Run IMACS (in a new terminal)
```
cd $(IMACSROOT)
# Usage: ./imacs {pipeline_version} {simstep}
# Minimum one argument needed. (default values) ./imacs 1 0.005
./imacs 1
```
There might be some dialog windows opening in `VREP` if you are running it for the first time. This is normal.

You do not have to always close vrep to run imacs again. Just `stop simulation` in vrep and wait for sometime before executing `./imacs 1 0.005`.

In case you encounter some other errors, you can debug using the following command:
```
valgrind ./imacs 1
```
## 6.2 - 6.4 Alternatively: run bash script
```
cd $(IMACSROOT)
bash run.sh
```
You can change the commands in this file to run automatically.

# FAQs
## 1. If you encounter the following errors: i) `error: libHalide.so cannot be found`; ii) `-lopencv cannot be found`.
Make sure all the export paths are present in `/etc/environment`. Then,
```
source /etc/environment
echo $LD_LIBRARY_PATH
```
The Halide path should be shown. If not shown, close the current terminal and open a new terminal and source path again.
## 2. When running `make` in `$(IMACSROOT)`: the following warnings (many) are also observed by us and is normal. We are working on improving this in a later version.
```
matrix.hpp [-Wreorder]
GivensQR.hpp [-Wreorder]
halide_benchmark.h [-Wmissing-field-initializers]
Halide.h [-Wmissing-field-initializers]
extApiPlatform.c [-Wunused-result]
```
## If you receive the following error, wait for some time before running the command `./imacs 1` If that does not work, close and reopen vrep.
```
imacs: /home/sajid/Desktop/cppvrepimacs/src/cpp_vrep_api/my_vrep_api.cpp:59: vrepAPI::vrepAPI(): Assertion `m_clientID != -1 && "V-REP must be started: ./vrep.sh ../../vrep-scenes/EnterCurveTest.ttt"' failed.
```
## 3. Exporting paths permanently with sudo rights
To set paths globally if you have sudo rights:
```
sudo gedit /etc/environment
```
At the end of this file, add the following line, making sure to replace the path with your own copied path:
```
CLANG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/clang
LLVM_CONFIG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/llvm-config
PKG_CONFIG_PATH=/usr/lib/pkgconfig/
LD_LIBRARY_PATH=/usr/lib:/home/sajid/Desktop/cppvrepimacs/externalApps/Halide/bin:/home/sajid/Desktop/cppvrepimacs/externalApps/Halide/distrib/bin
```
If there were already variables present with the same name, append the above variables with a `:`.
Modifying this file will set the variables for all users on your system.
Save the file and exit the editor.
Now reload this file to apply the changes to your current session:
```
source /etc/environment
```
## 4. Exporting paths without sudo rights
If you want to export paths permanently,
Open a terminal window using `Ctrl+Alt+T`
Run the command 
```
gedit ~/.profile
```
Add the line(s) to the bottom of this file and save.
```
#export CLANG=<path to llvm>/bin/clang
#export LLVM_CONFIG=<path to llvm>/bin/llvm-config
export CLANG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/clang
export LLVM_CONFIG=/home/sajid/Desktop/cppvrepimacs/externalApps/llvm/bin/llvm-config
export PKG_CONFIG_PATH=/usr/lib/pkgconfig/
export LD_LIBRARY_PATH=/usr/lib:/home/sajid/Desktop/cppvrepimacs/externalApps/Halide/bin:/home/sajid/Desktop/cppvrepimacs/externalApps/Halide/distrib/bin:$LD_LIBRARY_PATH
```
Log out and log in again.
A safer way is to use quotes. Doing so is necessary if one or more directories in the original PATH contain spaces. E.g.:
```
export CLANG="$(root)/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang"
```
Alternatively, you can set the paths in `paths.sh` and then source paths
```
cd $(IMACSROOT)
source paths.sh
```
## 5. The simulation time is hardcoded to 15 in src/cpp_vrep_api/cpp_vrep_framework.cpp. This may result in erroneous behaviour after the simulation exceeds this time.
Change line 11 in `src/cpp_vrep_api/cpp_vrep_framework.cpp` if you want to simulate for a longer time, i.e. `int simulation_time = 100;` if you want to logically simulate for `100 sec`.

# Contact
[Sayandip De](mailto:sayandip.de@tue.nl) <br/>
[Sajid Mohamed](mailto:s.mohamed@tue.nl)
