// Halide tutorial lesson 21: Auto-Scheduler

#include "Halide.h"

namespace {

using namespace Halide;

///////////////////////// We will define a generator to auto-schedule.
class AutoScheduled : public Halide::Generator<AutoScheduled> {
public:
    Input<Buffer<uint8_t>> in_patch   {"in_patch"   , 3};
    Input<Buffer<float>>   ctrl_pts_h {"ctrl_pts_h" , 2};      
    Input<Buffer<float>>   weights_h  {"weights_h"  , 2};       
    Input<Buffer<float>>   rev_tone_h {"rev_tone_h" , 2};       
    Input<Buffer<float>>   TsTw_tran_h{"TsTw_tran_h", 2};       
    Input<Buffer<float>>   coefs_h    {"coefs_h"    , 2};       

    Output<Buffer<uint8_t>> processed {"processed"  , 3};

    void generate() {      
        Func scale("scale");
        scale(x,y,c) = cast<float>(in_patch(x,y,c))/256;

        // Color map and white balance transform
        Func transform("transform");
            transform(x,y,c) = select(
                // Perform matrix multiplication, set min of 0
                c == 0, scale(x,y,0)      * TsTw_tran_h(0,0)
                      + scale(x,y,1)      * TsTw_tran_h(1,0)
                      + scale(x,y,2)      * TsTw_tran_h(2,0),
                c == 1, scale(x,y,0)      * TsTw_tran_h(0,1)
                      + scale(x,y,1)      * TsTw_tran_h(1,1)
                      + scale(x,y,2)      * TsTw_tran_h(2,1),
                        scale(x,y,0)      * TsTw_tran_h(0,2)
                      + scale(x,y,1)      * TsTw_tran_h(1,2)
                      + scale(x,y,2)      * TsTw_tran_h(2,2) );

        // // Weighted radial basis function for gamut mapping
        // Func rbf_ctrl_pts("rbf_ctrl_pts");
        //     // Initialization with all zero
        //     rbf_ctrl_pts(x,y,c) = cast<float>(0);
        //     // Index to iterate with
        //     RDom idx(0,num_ctrl_pts);
        //     // Loop code
        //     // Subtract the vectors 
        //     Expr red_sub   = transform(x,y,0) - ctrl_pts_h(0,idx);
        //     Expr green_sub = transform(x,y,1) - ctrl_pts_h(1,idx);
        //     Expr blue_sub  = transform(x,y,2) - ctrl_pts_h(2,idx);
        //     // Take the L2 norm to get the distance
        //     Expr dist      = sqrt( red_sub*red_sub + green_sub*green_sub + blue_sub*blue_sub );
        //     // Update persistant loop variables
        //     rbf_ctrl_pts(x,y,c) = select( c == 0, rbf_ctrl_pts(x,y,c) + (weights_h(0,idx) * dist),
        //                                   c == 1, rbf_ctrl_pts(x,y,c) + (weights_h(1,idx) * dist),
        //                                           rbf_ctrl_pts(x,y,c) + (weights_h(2,idx) * dist));

        // // Add on the biases for the RBF
        // Func rbf_biases("rbf_biases");
        //     rbf_biases(x,y,c) = max( select( 
        //         c == 0, rbf_ctrl_pts(x,y,0) + coefs_h(0,0)                  + coefs_h(1,0)*transform(x,y,0) +
        //                                       coefs_h(2,0)*transform(x,y,1) + coefs_h(3,0)*transform(x,y,2),
        //         c == 1, rbf_ctrl_pts(x,y,1) + coefs_h(0,1)                  + coefs_h(1,1)*transform(x,y,0) +
        //                                       coefs_h(2,1)*transform(x,y,1) + coefs_h(3,1)*transform(x,y,2),
        //                 rbf_ctrl_pts(x,y,2) + coefs_h(0,2)                  + coefs_h(1,2)*transform(x,y,0) +
        //                                       coefs_h(2,2)*transform(x,y,1) + coefs_h(3,2)*transform(x,y,2))
        //                             , 0);

        Func rbf_biases("rbf_biases");
        rbf_biases(x,y,c) = transform(x,y,c); 

        // Forward tone mapping
        Func tonemap("tonemap");
            RDom idx2(0,256);
            // Theres a lot in this one line! Functionality wise it finds the entry in 
            // the reverse tone mapping function which is closest to the value found by
            // gamut mapping. The output is then cast to uint8 for output. Effectively 
            // it reverses the reverse tone mapping function.

            tonemap(x,y,c) = cast<uint8_t>(argmin( abs( rev_tone_h(c,idx2) - rbf_biases(x,y,c) ) )[0]);
        
        // output processed image
        processed(x, y, c) = tonemap(x, y, c);   
    }

    void schedule() {
        // The auto-scheduler requires estimates on all the input/output
        // sizes and parameter values in order to compare different
        // alternatives and decide on a good schedule.

        // To provide estimates (min and extent values) for each dimension
        // of the input images ('input', 'filter', and 'bias'), we use the
        // set_bounds_estimate() method. set_bounds_estimate() takes in
        // (min, extent) of the corresponding dimension as arguments.
        in_patch.dim(0).set_bounds_estimate(0, 4992); 
        in_patch.dim(1).set_bounds_estimate(0, 3280); 
        in_patch.dim(2).set_bounds_estimate(0, 3);

        ctrl_pts_h.dim(0).set_bounds_estimate(0, 3);      
        ctrl_pts_h.dim(1).set_bounds_estimate(0, 3702);

        weights_h.dim(0).set_bounds_estimate(0, 3);         
        weights_h.dim(1).set_bounds_estimate(0, 3702);

        rev_tone_h.dim(0).set_bounds_estimate(0, 3);       
        rev_tone_h.dim(1).set_bounds_estimate(0, 256);       
 
        TsTw_tran_h.dim(0).set_bounds_estimate(0, 3); 
        TsTw_tran_h.dim(1).set_bounds_estimate(0, 3);  
        
        coefs_h.dim(0).set_bounds_estimate(0, 4); 
        coefs_h.dim(1).set_bounds_estimate(0, 4);

        // To provide estimates on the parameter values, we use the
        // set_estimate() method.

        // To provide estimates (min and extent values) for each dimension
        // of pipeline outputs, we use the estimate() method. estimate()
        // takes in (dim_name, min, extent) as arguments.
        processed
            .estimate(c, 0, 3)
            .estimate(x, 0, 4992)
            .estimate(y, 0, 3280);
    }
private:
        // Declare image handle variables
        Var x, y, c;
        const int num_ctrl_pts = 3702;
};

} // namespace

// As in lesson 15, we register our generator and then compile this
// file along with tools/GenGen.cpp.
HALIDE_REGISTER_GENERATOR(AutoScheduled, auto_schedule_gen_fwd_v1)



        // Technically, the estimate values can be anything, but the closer
        // they are to the actual use-case values, the better the generated
        // schedule will be.

        // To auto-schedule the the pipeline, we don't have to do anything else:
        // every Generator implicitly has a GeneratorParam named "auto_schedule";
        // if this is set to true, Halide will call auto_schedule() on all of
        // our pipeline's outputs automatically.

        // Every Generator also implicitly has a GeneratorParams named "machine_params",
        // which allows you to specify characteristics of the machine architecture
        // for the auto-scheduler; it's generally specified in your Makefile.
        // If none is specified, the default machine parameters for a generic CPU
        // architecture will be used by the auto-scheduler.

        // Let's see some arbitrary but plausible values for the machine parameters.
        //
        //      const int kParallelism = 32;
        //      const int kLastLevelCacheSize = 16 * 1024 * 1024;
        //      const int kBalance = 40;
        //      MachineParams machine_params(kParallelism, kLastLevelCacheSize, kBalance);
        //      DEFAULT machine_params=32,16777216,40
        //  
        // The arguments to MachineParams are the maximum level of parallelism
        // available, the size of the last-level cache (in KB), and the ratio
        // between the cost of a miss at the last level cache and the cost
        // of arithmetic on the target architecture, in that order.

        // Note that when using the auto-scheduler, no schedule should have
        // been applied to the pipeline; otherwise, the auto-scheduler will
        // throw an error. The current auto-scheduler cannot handle a
        // partially-scheduled pipeline.

        // If HL_DEBUG_CODEGEN is set to 3 or greater, the schedule will be dumped
        // to stdout (along with much other information); a more useful way is
        // to add "schedule" to the -e flag to the Generator. (In CMake and Bazel,
        // this is done using the "extra_outputs" flag.)

        // The generated schedule that is dumped to file is an actual
        // Halide C++ source, which is readily copy-pasteable back into
        // this very same source file with few modifications. Programmers
        // can use this as a starting schedule and iteratively improve the
        // schedule. Note that the current auto-scheduler is only able to
        // generate CPU schedules and only does tiling, simple vectorization
        // and parallelization. It doesn't deal with line buffering, storage
        // reordering, or factoring reductions.