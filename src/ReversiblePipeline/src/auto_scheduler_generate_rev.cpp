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

        // Backward tone mapping
        Func rev_tonemap("rev_tonemap");
        Expr rev_tone_idx = cast<uint8_t>(scale(x,y,c) * 256.0f);
        rev_tonemap(x,y,c) = rev_tone_h(c,rev_tone_idx) ;

        // Weighted radial basis function for gamut mapping
        Func rev_rbf_ctrl_pts("rev_rbf_ctrl_pts");
        // Initialization with all zero
        rev_rbf_ctrl_pts(x,y,c) = cast<float>(0);
        // Index to iterate with
        RDom revidx(0,num_ctrl_pts);
        // Loop code
        // Subtract the vectors 
        Expr revred_sub   = rev_tonemap(x,y,0) - ctrl_pts_h(0,revidx);
        Expr revgreen_sub = rev_tonemap(x,y,1) - ctrl_pts_h(1,revidx);
        Expr revblue_sub  = rev_tonemap(x,y,2) - ctrl_pts_h(2,revidx);
        // Take the L2 norm to get the distance
        Expr revdist      = sqrt( revred_sub*revred_sub + 
                                  revgreen_sub*revgreen_sub + 
                                  revblue_sub*revblue_sub );
        // Update persistant loop variables
        rev_rbf_ctrl_pts(x,y,c) = select( c == 0, rev_rbf_ctrl_pts(x,y,c) + (weights_h(0,revidx) * revdist),
                                          c == 1, rev_rbf_ctrl_pts(x,y,c) + (weights_h(1,revidx) * revdist),
                                                  rev_rbf_ctrl_pts(x,y,c) + (weights_h(2,revidx) * revdist));

        // Add on the biases for the RBF
        Func rev_rbf_biases("rev_rbf_biases");
        rev_rbf_biases(x,y,c) = max( select( 
          c == 0, rev_rbf_ctrl_pts(x,y,0) + coefs_h(0,0)                    + coefs_h(1,0)*rev_tonemap(x,y,0) +
                                            coefs_h(2,0)*rev_tonemap(x,y,1) + coefs_h(3,0)*rev_tonemap(x,y,2),
          c == 1, rev_rbf_ctrl_pts(x,y,1) + coefs_h(0,1)                    + coefs_h(1,1)*rev_tonemap(x,y,0) +
                                            coefs_h(2,1)*rev_tonemap(x,y,1) + coefs_h(3,1)*rev_tonemap(x,y,2),
                  rev_rbf_ctrl_pts(x,y,2) + coefs_h(0,2)                    + coefs_h(1,2)*rev_tonemap(x,y,0) +
                                            coefs_h(2,2)*rev_tonemap(x,y,1) + coefs_h(3,2)*rev_tonemap(x,y,2))
                                , 0);

        // Reverse color map and white balance transform
        Func rev_transform("rev_transform");
        rev_transform(x,y,c) = cast<uint8_t>( 256.0f * max( select(
          // Perform matrix multiplication, set min of 0
          c == 0, rev_rbf_biases(x,y,0)*TsTw_tran_h(0,0)
                + rev_rbf_biases(x,y,1)*TsTw_tran_h(1,0)
                + rev_rbf_biases(x,y,2)*TsTw_tran_h(2,0),
          c == 1, rev_rbf_biases(x,y,0)*TsTw_tran_h(0,1)
                + rev_rbf_biases(x,y,1)*TsTw_tran_h(1,1)
                + rev_rbf_biases(x,y,2)*TsTw_tran_h(2,1),
                  rev_rbf_biases(x,y,0)*TsTw_tran_h(0,2)
                + rev_rbf_biases(x,y,1)*TsTw_tran_h(1,2)
                + rev_rbf_biases(x,y,2)*TsTw_tran_h(2,2))
                                                            , 0) );
        
        // output processed image
        processed(x, y, c) = rev_transform(x, y, c);   
    }

    void schedule() {
        if(!auto_schedule){
            // Target: x86-64-linux-avx-avx2-f16c-fma-sse41
            // MachineParams: 4,8000000,40

            // Delete this line if not using Generator
            Pipeline pipeline = get_pipeline();

            Var x_vi("x_vi");
            Var x_vo("x_vo");
            Var y_i("y_i");
            Var y_o("y_o");

            Func processed = pipeline.get_func(11);
            Func rev_rbf_ctrl_pts = pipeline.get_func(8);
            Func rev_tonemap = pipeline.get_func(6);

            {
                Var x = processed.args()[0];
                Var y = processed.args()[1];
                processed
                    .compute_root()
                    .split(y, y_o, y_i, 32)
                    .split(x, x_vo, x_vi, 32)
                    .reorder(c, x_vi,y_i,x_vo, y_o)
                    .bound(c,0,3)
                    .unroll(c)
                    .parallel(x_vo)
                    .vectorize(x_vi)
                    .parallel(y_o);
            }
            {
                Var x = rev_rbf_ctrl_pts.args()[0];
                Var y = rev_rbf_ctrl_pts.args()[1];
                Var c = rev_rbf_ctrl_pts.args()[2];
                RVar revidx$x(rev_rbf_ctrl_pts.update(0).get_schedule().rvars()[0].var);
                rev_rbf_ctrl_pts
                    .compute_at(processed, y_i)
                    .store_at(processed, y_i)
                    .reorder(c,x, y)
                    .unroll(c)
                    .vectorize(x);
                rev_rbf_ctrl_pts.update(0)
                    .reorder(c,x, revidx$x, y)
                    .unroll(c)
                    .vectorize(x);
            }
            {
                Var x = rev_tonemap.args()[0];
                Var y = rev_tonemap.args()[1];
                Var c = rev_tonemap.args()[2];
                rev_tonemap
                    .compute_at(processed, y_i)
                    .store_at(processed, y_i)
                    .reorder(c,x, y)
                    .unroll(c)
                    .vectorize(x);
            }
        } else {
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

            processed
                .estimate(c, 0, 3)
                .estimate(x, 0, 4992)
                .estimate(y, 0, 3280);
        } // if   
    } // schedule()
private:
        // Declare image handle variables
        Var x, y, c;
        const int num_ctrl_pts = 3702;
};

} // namespace

HALIDE_REGISTER_GENERATOR(AutoScheduled, auto_schedule_gen_rev)



