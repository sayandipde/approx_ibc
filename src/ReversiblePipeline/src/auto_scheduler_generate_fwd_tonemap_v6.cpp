/*
 *  Copyright (c) 2020 sayandipde
 *  Eindhoven University of Technology
 *  Eindhoven, The Netherlands
 *
 *  Name            :   image_signal_processing.cpp
 *
 *  Authors         :   Sayandip De (sayandip.de@tue.nl)
 *			Sajid Mohamed (s.mohamed@tue.nl)
 *
 *  Date            :   March 26, 2020
 *
 *  Function        :   approximate pipeline version 6
 *
 *  History         :
 *      26-03-20    :   Initial version.
 *			Code is modified from https://github.com/mbuckler/ReversiblePipeline [written by Mark Buckler]. 
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

#include "Halide.h"

namespace {

using namespace Halide;

///////////////////////// We will define a generator to auto-schedule.
class AutoScheduled : public Halide::Generator<AutoScheduled> {
public:
    Input<Buffer<uint8_t>>   in_patch   {"in_patch"   , 3};
    Input<Buffer<float>>   ctrl_pts_h {"ctrl_pts_h" , 2};      
    Input<Buffer<float>>   weights_h  {"weights_h"  , 2};       
    Input<Buffer<float>>   rev_tone_h {"rev_tone_h" , 2};       
    Input<Buffer<float>>   TsTw_tran_h{"TsTw_tran_h", 2};       
    Input<Buffer<float>>   coefs_h    {"coefs_h"    , 2};       

    Output<Buffer<uint8_t>> processed {"processed"  , 3};

    void generate() {      
        Func scale("scale");
        scale(x,y,c) = cast<float>(in_patch(x,y,c))/256;

        Func rbf_biases("rbf_biases");
        rbf_biases(x,y,c) = scale(x,y,c);

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

HALIDE_REGISTER_GENERATOR(AutoScheduled, auto_schedule_gen_fwd_v6)
