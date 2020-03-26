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
 *  Function        :   main file to run different pipelines
 *
 *  History         :
 *      26-03-20    :   Initial version.
 *						Code is modified from https://github.com/mbuckler/ReversiblePipeline [written by Mark Buckler]. 
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
 
#include "auto_schedule_true_rev.h"
#include "auto_schedule_true_fwd_v0.h"
#include "auto_schedule_true_fwd_v1.h"
#include "auto_schedule_true_fwd_v2.h"
#include "auto_schedule_true_fwd_v3.h"
#include "auto_schedule_true_fwd_v4.h"
#include "auto_schedule_true_fwd_v5.h"
#include "auto_schedule_true_fwd_v6.h"

#include "Halide.h"
#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "halide_image.h"

#include "LoadCamModel.h"
#include "MatrixOps.h"
#include <stdio.h>
#include <math.h>
#include <iostream>


// Function prototypes
int run_pipeline(bool direction);


int main(int argc, char **argv) {
  using namespace std;

  printf("Starting script. Note that processing may take several minutes for large images\n");

  //Run backward pipeline
  printf("Running backward pipeline...\n");
  run_pipeline(false);
  printf("Backward pipeline complete\n");
  
  // Run forward pipeline
  printf("Running forward pipeline...\n");
  run_pipeline(true);
  printf("Forward pipeline complete\n");



  printf("Success!\n");
  return 0;
}


// Reversible pipeline function
int run_pipeline(bool direction) {
    using namespace std;  

  ///////////////////////////////////////////////////////////////
  // Input Image Parameters
  ///////////////////////////////////////////////////////////////

  // Demosaiced raw input (to forward pipeline)
  char demosaiced_image[] = 
  //"../../imgs/NikonD7000FL/DSC_0916.NEF.demos_3C.png";
  "./imgs/output_rev.png";


  // Jpeg input to backward pipeline. Must be converted to png
  char jpg_image[] =
  //"/home/kbimpisi/Approx_IBC_offline/ReversiblePipeline/DSC_0799.NEF.dcraw.png";
  //"../../imgs/NikonD7000FL/DSC_0916.png";
  "./imgs/1.png";

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format model data: cpp -> halide format

  // Declare model parameters
  vector<vector<float>> Ts, Tw, TsTw;
  vector<vector<float>> ctrl_pts, weights, coefs;
  vector<vector<float>> rev_tone;

  // Load model parameters from file
  // NOTE: Ts, Tw, and TsTw read only forward data
  // ctrl_pts, weights, and coefs are either forward or backward
  // tone mapping is always backward
  // This is due to the the camera model format
  ///////////////////////////////////////////////////////////////
  // Camera Model Parameters
  ///////////////////////////////////////////////////////////////

  // Path to the camera model to be used
  char cam_model_path[] =
  "./camera_models/NikonD7000/";

  // White balance index (select white balance from transform file)
  // The first white balance in the file has a wb_index of 1
  // For more information on model format see the readme
  int wb_index = 3 ; // Actual choice = Given no. + 1
  //4; //6 [SD]

  // Number of control points
  const int num_ctrl_pts = 
  3702;

  Ts        = get_Ts       (cam_model_path);

  /*for (int i = 0; i < Ts.size(); i++){
    for (int j = 0; j < Ts[i].size(); j++){
      cout << Ts[i][j] << ' ';
    }
    cout << '\n';
  }*/

  Tw        = get_Tw       (cam_model_path, wb_index);

  /*cout <<'\n';
  for (int i = 0; i < Tw.size(); i++){
    for (int j = 0; j < Tw[i].size(); j++){
      cout << Tw[i][j] << ' ';
    }
    cout << '\n';
  }*/

  TsTw      = get_TsTw     (cam_model_path, wb_index);

  /*cout <<'\n';
  for (int i = 0; i < TsTw.size(); i++){
    for (int j = 0; j < TsTw[i].size(); j++){
      cout << TsTw[i][j] << ' ';
    }
    cout << '\n';
  }*/

  ctrl_pts  = get_ctrl_pts (cam_model_path, num_ctrl_pts, direction);

  /*cout <<'\n';
  for (int i = 0; i < ctrl_pts.size(); i++){
    for (int j = 0; j < ctrl_pts[i].size(); j++){
      cout << ctrl_pts[i][j] << ' ';
    }
    cout << '\n';
  }*/

  weights   = get_weights  (cam_model_path, num_ctrl_pts, direction);

  /*cout <<'\n';
  for (int i = 0; i < weights.size(); i++){
    for (int j = 0; j < weights[i].size(); j++){
      cout << weights[i][j] << ' ';
    }
    cout << '\n';
  }*/

  coefs     = get_coefs    (cam_model_path, num_ctrl_pts, direction);

  /*cout <<'\n';
  for (int i = 0; i < coefs.size(); i++){
    for (int j = 0; j < coefs[i].size(); j++){
      cout << coefs[i][j] << ' ';
    }
    cout << '\n';
  }*/

  rev_tone  = get_rev_tone (cam_model_path);

  // Take the transpose of the color map and white balance transform for later use
  vector<vector<float>> TsTw_tran = transpose_mat (TsTw);

  // If we are performing a backward implementation of the pipeline, 
  // take the inverse of TsTw_tran
  if (direction == 0) { 
    TsTw_tran = inv_3x3mat(TsTw_tran);
  }

  using namespace Halide;
  using namespace Halide::Tools;

  // Convert control points to a Halide image
  int width  = ctrl_pts[0].size();
  int length = ctrl_pts.size();

  // cout << "\n- ctrl_pts_h(width,length): ";
  // cout << width << ", " << length << endl;

  Halide::Runtime::Buffer<float> ctrl_pts_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      ctrl_pts_h(x,y) = ctrl_pts[y][x];
    }
  }
  // Convert weights to a Halide image
  width  = weights[0].size();
  length = weights.size();

  // cout << "\n- weights_h(width,length): ";
  // cout << width << ", " << length << endl;

  Halide::Runtime::Buffer<float> weights_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      weights_h(x,y) = weights[y][x];
    }
  }
  // Convert the reverse tone mapping function to a Halide image
  width  = 3;
  length = 256;
  Halide::Runtime::Buffer<float> rev_tone_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      rev_tone_h(x,y) = rev_tone[y][x];
    }
  }
 

  // Convert the TsTw_tran to a Halide image
  width  = 3;
  length = 3;
  Halide::Runtime::Buffer<float> TsTw_tran_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      TsTw_tran_h(x,y) = TsTw_tran[x][y];
    }
  }

  // Convert the coefs to a Halide image
  width  = 4;
  length = 3;
  Halide::Runtime::Buffer<float> coefs_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      coefs_h(x,y) = coefs[x][y];
      //cout << coefs_h(x, y) << ' ';
    }
    //cout<<'\n'; 
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format input image

  // Declare image handle variables
  Var x, y, c;

  Halide::Runtime::Buffer<uint8_t> input;
  // Load input image 
  if (direction == 1) {
    // If using forward pipeline
    input = load_and_convert_image(demosaiced_image); // convert to the format of LHS [SD]
  } else {
    // If using backward pipeline
    input = load_image(jpg_image);
  }

  ////////////////////////////////////////////////////////////////////////
  // run the generator

  Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());
  int cam_pipe_version = 0;
  int samples          = 1; //2
  int iterations       = 1; //5
  double auto_schedule_on = 0.0;
  if (direction == 1) {
    for (cam_pipe_version = 0; cam_pipe_version < 7; cam_pipe_version++){
      cout << "\n- cam pipe version           : " << cam_pipe_version << "\n";
      switch(cam_pipe_version){
        case 0: // full rev + fwd                   skip 0 stages
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v0(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_transform_gamut_tonemap: %gms\n", auto_schedule_on * 1e3);
          break;
        case 1: // rev + fwd_transform_tonemap      skip 1 stage
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v1(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_transform_tonemap      : %gms\n", auto_schedule_on * 1e3);
          break;      
        case 2: // rev + fwd_transform_gamut        skip 1 stage
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v2(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_transform_gamut        : %gms\n", auto_schedule_on * 1e3);
          break;
        case 3: // rev + fwd_gamut_tonemap          skip 1 stage
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v3(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_gamut_tonemap          : %gms\n", auto_schedule_on * 1e3);
          break;
        case 4: // rev + fwd_transform              skip 2 stages
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v4(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_transform              : %gms\n", auto_schedule_on * 1e3);
          break;      
        case 5: // rev + fwd_gamut                  skip 2 stages
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v5(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_gamut                  : %gms\n", auto_schedule_on * 1e3);
          break;      
        case 6: // rev + fwd_tonemap                skip 2 stages
          auto_schedule_on = Halide::Tools::benchmark(samples, iterations, [&]() {
              auto_schedule_true_fwd_v6(input, ctrl_pts_h, weights_h, rev_tone_h, 
                                        TsTw_tran_h, coefs_h, output);
          });
          printf("- fwd_tonemap                : %gms\n", auto_schedule_on * 1e3);
          break;
        case 7: // rev only                         skip 3 stages
          break;
        default: // should not happen
          cout << "Default pipe branch taken, pls check\n";
          break;
      }      
      save_image(output, "imgs/output_fwd_v"+to_string(cam_pipe_version)+".png"); 
    }


  } else {
    double auto_schedule_on = Halide::Tools::benchmark(2, 5, [&]() {
        auto_schedule_true_rev(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
    });
    printf("- Auto schedule rev: %gms\n", auto_schedule_on * 1e3);  
    //save_image(output, "output_rev.png");  
  }

  ////////////////////////////////////////////////////////////////////////
  // Save the output

  if (direction == 0) {
    //save_image(output, "imgs/output_fwd_v"+to_string(cam_pipe_version)+".png"); 
    ////cout << "- output(c,x,y) fwd: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  //} else {
    save_image(output, "imgs/output_rev.png");
    //cout << "- output(c,x,y) rev: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  }

  return 0;
}
