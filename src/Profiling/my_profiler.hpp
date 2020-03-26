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
 *  Function        :   run cpu profiling for benchmarking
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

#include <vector>
#include <numeric>
#include <algorithm> 
#include <cmath>
#include "halide_benchmark.h"
//#include "Profiler.h"

std::vector<double> get_normal_dist_tuple(std::vector<double> v){
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();

    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean));
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / v.size());
    return {mean, stdev};
}

std::vector<std::vector<double>> do_benchmarking(std::function<void()> op){

    std::vector<double> iterations;
    //std::vector<double> min, max, avg;
    double min_t_auto = 0.0;

    //for (int j = 0; j < 1; j++) {
    //    for (int i = 0; i < 1; i++){
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < 100; i++){
            min_t_auto = Halide::Tools::benchmark(1, 1, [&]() { op(); }); // min_t_auto is in seconds [SD] 
            iterations.push_back(min_t_auto * 1e3); // ms
        }
    }

    // return {bc_tuple, avg_tuple, wc_tuple};
    return {iterations};

}
