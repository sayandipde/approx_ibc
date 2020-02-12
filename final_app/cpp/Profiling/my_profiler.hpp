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
        // min.push_back( *min_element(iterations.begin(), iterations.end()) );
        // max.push_back( *max_element(iterations.begin(), iterations.end()) );
        // avg.push_back(  accumulate( iterations.begin(), iterations.end(), 0.0)/iterations.size() ); 
    }

    // std::vector<double> wc_tuple(2), avg_tuple(2), bc_tuple(2);
    // bc_tuple  = get_normal_dist_tuple(min);
    // avg_tuple = get_normal_dist_tuple(avg);
    // wc_tuple  = get_normal_dist_tuple(max);

    // return {bc_tuple, avg_tuple, wc_tuple};
    return {iterations};

}

/*void do_instr_benchmarking(std::function<void()> op){
    PROFILE_SCOPED()
    op();
    Profiler::dump();
}*/
