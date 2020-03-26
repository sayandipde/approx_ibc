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
 *  Function        :   run denoise-cpu profiling with single image workloads
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

#ifdef PROFILEWITHVALGRIND
#include "callgrind.h" 
#endif

#include "nl_means.h"

#include "halide_benchmark.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

#include <fstream>

using namespace std;
using namespace Halide::Runtime;
using namespace Halide::Tools;

#ifdef PROFILEWITHCHRONO
	template<class Container>
	std::ostream& write_container(const Container& c, std::ostream& out, string pipeversion, char delimiter = '\n')
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
    if (argc < 6) {
        printf("Usage: ./denoise_single.o input.png patch_size search_area sigma output.png\n"
               "e.g.: ./denoise_single.o input.png 7 7 0.12 output.png\n");
        return 0;
    }

    int patch_size = atoi(argv[2]);
    int search_area = atoi(argv[3]);
    float sigma = atof(argv[4]);

    Buffer<float> input;
    vector<vector<double>> wc_avg_bc_tuples;
    string out_string;

    input = load_and_convert_image(argv[1]);
    Buffer<float> output(input.width(), input.height(), 3);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Timing code
    // statistical
	
	#ifdef PROFILEWITHCHRONO
     wc_avg_bc_tuples = do_benchmarking( [&]() {
        nl_means(input, patch_size, search_area, sigma, output);
    } );
      out_string = "chrono/runtime_denoise_single.csv";
      std::ofstream outfile(out_string);
      write_container(wc_avg_bc_tuples[0], outfile, "v0");

    convert_and_save_image(output, argv[5]);
	#endif
	
	#ifdef PROFILEWITHVALGRIND
	CALLGRIND_TOGGLE_COLLECT;
	nl_means(input, patch_size, search_area, sigma, output);
	CALLGRIND_TOGGLE_COLLECT;
	CALLGRIND_DUMP_STATS;
	#endif
	
    return 0;
}
