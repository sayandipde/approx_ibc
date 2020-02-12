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
    // int timing_iterations = atoi(argv[5]);
    //int timing_iterations = 1;

    Buffer<float> input;
    vector<vector<double>> wc_avg_bc_tuples;
    string out_string;
    //for (int version = 0; version <= 0; version++){
    //cout << "- profiling version: " << version << endl;
    //for (int i = 1; i < 2; i++){
    //  cout << i << endl;
    //  in_string = "../multiple-images/images/v"+to_string(version)+"/"+to_string(i)+".png";

    input = load_and_convert_image(argv[1]);
    Buffer<float> output(input.width(), input.height(), 3);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Timing code
    // statistical
	
	#ifdef PROFILEWITHCHRONO
     wc_avg_bc_tuples = do_benchmarking( [&]() {
        nl_means(input, patch_size, search_area, sigma, output);
    } );
    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
  // std::ofstream outfile("results.csv");
  // write_container(wc_avg_bc_tuples[0], outfile);
      out_string = "chrono/runtime_denoise_single.csv";
      std::ofstream outfile(out_string);
      write_container(wc_avg_bc_tuples[0], outfile, "v0");
//}}
    // // sampling 
    // BenchmarkConfig config;
    // config.accuracy = 0.001;
    // BenchmarkResult result{0, 0, 0};
    // result = Halide::Tools::benchmark( [&](){ nl_means(input, patch_size, search_area, sigma, output); }, config );
    // cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
    // cout << "Number of samples used for measurement                    : " << result.samples << endl;
    // cout << "Total number of iterations across all samples             : " << result.iterations << endl;
    // cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

    // // instruction
    // cout << "\n";
    // do_instr_benchmarking([&](){nl_means(input, patch_size, search_area, sigma, output);});

    // // Manually-tuned version
    // double min_t_manual = benchmark(timing_iterations, 1, [&]() {
    //     nl_means(input, patch_size, search_area, sigma, output);
    // });
    // printf("Manually-tuned time: %gms\n", min_t_manual * 1e3);
    // double min_t1_auto = benchmark(timing_iterations, 1, [&]() {
    //     nl_means(input, patch_size, search_area, sigma, output);
    // });
    // printf("Auto-scheduled time: %gms\n", min_t1_auto * 1e3);

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
