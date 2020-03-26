/*
 *  Copyright (c) 2020 sayandipde
 *  Eindhoven University of Technology
 *  Eindhoven, The Netherlands
 *
 *  Name            :   image_signal_processing.cpp
 *
 *  Authors         :   Sayandip De (sayandip.de@tue.nl)
 *						Sajid Mohamed (s.mohamed@tue.nl)
 *
 *  Date            :   March 26, 2020
 *
 *  Function        :   run denoise-cpu profiling with multiple image workloads
 *
 *  History         :
 *      26-03-20    :   Initial version. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <cstdio>
//#include <chrono>
#include <iostream> 
#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

#include "nl_means.h"
// #include "nl_means_auto_schedule.h"

#include "halide_benchmark.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

#include <fstream>

using namespace std;
using namespace Halide::Runtime;
using namespace Halide::Tools;					
#ifdef PROFILEWITHCHRONO
template<class Container>
std::ostream& write_container(const Container& c, std::ostream& out, string pipeversion, char delimiter = ',')
									   
										   
{
    out << pipeversion;
    out << delimiter;
    bool write_sep = false;
    for (const auto& e: c) {
        if (write_sep)
            out << delimiter;
        else
            write_sep = true;
        out << e;
    }
    return out;
}
#endif




int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: ./denoise_multiple.o input.png patch_size search_area sigma\n"
               "e.g.: ./denoise_multiple.o input.png 7 7 0.12\n");
        return 0;
    }
	
	const char * img_path = argv[1];

    int patch_size = atoi(argv[2]);
    int search_area = atoi(argv[3]);
    float sigma = atof(argv[4]);
    // int timing_iterations = atoi(argv[5]);
    // int timing_iterations = 1;

    Buffer<float> input;
    vector<vector<double>> wc_avg_bc_tuples;
    string in_string, out_string;
    
	for (int version = 0; version <= 7; version++){
		cout << "- profiling version: " << version << endl;
		out_string = "chrono/runtime_denoise_multiple_v" + to_string(version) + "_.csv";
		std::ofstream outfile(out_string);
		for (int i = 0; i < 200; i++){
			cout << i << endl;
			in_string = std::string(img_path)+"v"+to_string(version)+"/demosaic/img_dem_"+to_string(i)+".png";
	
			input = load_and_convert_image(in_string);
			Buffer<float> output(input.width(), input.height(), 3);

			//////////////////////////////////////////////////////////////////////////////////////////////////
			// Timing code
			// statistical
			#ifdef PROFILEWITHCHRONO			  
			 wc_avg_bc_tuples = do_benchmarking( [&]() {
				nl_means(input, patch_size, search_area, sigma, output);
			} );

			write_container(wc_avg_bc_tuples[0], outfile, "img_"+to_string(i));
			outfile << "\n";
			
			// Save the output
			convert_and_save_image(output, (std::string(img_path)+"v"+to_string(version)+"/denoise/img_den_"+to_string(i)+".png").c_str());
			#endif
		}
	}
    return 0;
}
