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
 *  Function        :   run different approximate ISP versions
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
 
#include "auto_schedule_true_rev.h"
#include "auto_schedule_true_fwd_v0.h"
#include "auto_schedule_true_fwd_v1.h"
#include "auto_schedule_true_fwd_v2.h"
#include "auto_schedule_true_fwd_v3.h"
#include "auto_schedule_true_fwd_v4.h"
#include "auto_schedule_true_fwd_v5.h"
#include "auto_schedule_true_fwd_v6.h"

// Halide
#include "Halide.h"
#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "halide_image.h"

#include <opencv2/opencv.hpp>
#include "config.hpp"

#include "LoadCamModel.h"
#include "MatrixOps.h"
#include <stdio.h>
#include <math.h>

using namespace cv;

imageSignalProcessing::imageSignalProcessing(){}
imageSignalProcessing::~imageSignalProcessing(){}

Mat imageSignalProcessing::approximate_pipeline(Mat the_img_vrep, int the_pipe_version) { 
  using namespace std;

  Mat img_rev, img_fwd;
  printf("Starting script. Note that processing may take several minutes for large images\n");

  //Run backward pipeline
  printf("Running backward pipeline...\n");
  img_rev = run_pipeline(false, the_img_vrep, the_pipe_version);
  printf("Backward pipeline complete\n");
  
  // Run forward pipeline
  printf("Running forward pipeline...\n");
  img_fwd = run_pipeline(true, img_rev, the_pipe_version);
  printf("Forward pipeline complete\n");


  printf("Success!\n");
  return img_fwd;
}

// Reversible pipeline function
Mat imageSignalProcessing::run_pipeline(bool direction, Mat the_img_in, int the_pipe_version) {
  using namespace std;  

  ///////////////////////////////////////////////////////////////
  // Input Image Parameters
  ///////////////////////////////////////////////////////////////

  // convert input Mat to Image
  Halide::Runtime::Buffer<uint8_t> input;
  input = Mat2Image(&the_img_in);

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
  char cam_model_path[] = "./camera_models/NikonD7000/";

  // White balance index (select white balance from transform file)
  // The first white balance in the file has a wb_index of 1
  // For more information on model format see the readme
  int wb_index = 6;

  // Number of control points
  const int num_ctrl_pts = 3702;
  // Colour space transform "raw2jpg_transform.txt"
  Ts        = get_Ts       (cam_model_path);
  // White balance transform "raw2jpg_transform.txt"
  Tw        = get_Tw       (cam_model_path, wb_index);
  // Combined transforms of color space and white balance for checking "raw2jpg_transform.txt"
  TsTw      = get_TsTw     (cam_model_path, wb_index);
  // get control points "raw2jpg_ctrlPoints.txt" and "jpg2raw_ctrlPoints.txt"
  ctrl_pts  = get_ctrl_pts (cam_model_path, num_ctrl_pts, direction);
  // get weights "raw2jpg_coefs.txt"(1) and "jpg2raw_coefs.txt"(0)
  weights   = get_weights  (cam_model_path, num_ctrl_pts, direction);
  // get coefficients "raw2jpg_coefs.txt"(1) and "jpg2raw_coefs.txt"(0)
  coefs     = get_coefs    (cam_model_path, num_ctrl_pts, direction);
  // get reverse tone mapping "jpg2raw_respFcns.txt"
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

  Halide::Runtime::Buffer<float> ctrl_pts_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      ctrl_pts_h(x,y) = ctrl_pts[y][x];
    }
  }
  // Convert weights to a Halide image
  width  = weights[0].size();
  length = weights.size();

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
  length = 4;
  Halide::Runtime::Buffer<float> coefs_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      coefs_h(x,y) = coefs[x][y];
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format input image

  // Declare image handle variables
  Var x, y, c;

  // Halide::Runtime::Buffer<uint8_t> input;
  // // Load input image 
  // if (direction == 1) {
  //   // If using forward pipeline
  //   input = load_and_convert_image(demosaiced_image);
  // } else {
  //   // If using backward pipeline
  //   input = load_image(jpg_image);
  // }

  ////////////////////////////////////////////////////////////////////////
  // run the generator

  Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());
  int samples          = 1; //2
  int iterations       = 1; //5
  double auto_schedule_on = 0.0;

  if (direction == 1) {
    cout << "- cam pipe version           : " << the_pipe_version << "\n";

    switch(the_pipe_version){
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
    }// switch      


  } else {
    double auto_schedule_on = Halide::Tools::benchmark(2, 5, [&]() {
        auto_schedule_true_rev(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
    });
    printf("- Auto schedule rev: %gms\n", auto_schedule_on * 1e3);    
  }

  ////////////////////////////////////////////////////////////////////////
  // Save the output


  if (direction == 1) {
    save_image(output, "imgs/output_fwd_test.png"); 
    //cout << "- output(c,x,y) fwd: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  } else {
    save_image(output, "imgs/output_rev_test.png");
    //cout << "- output(c,x,y) rev: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  }

  // convert Image to Mat and return
  Mat img_out;
  img_out = Image2Mat(&output);
  return img_out;
}

Mat imageSignalProcessing::Image2Mat( Halide::Runtime::Buffer<uint8_t> *InImage ) {
  int height = (*InImage).height();
  int width  = (*InImage).width();
  Mat OutMat(height,width,CV_8UC3);
  vector<Mat> three_channels;
  split(OutMat, three_channels);

  // Convert from planar RGB memory storage to interleaved BGR memory storage
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      // Blue channel
      three_channels[0].at<uint8_t>(y,x) = (*InImage)(x,y,2);
      // Green channel
      three_channels[1].at<uint8_t>(y,x) = (*InImage)(x,y,1);
      // Red channel
      three_channels[2].at<uint8_t>(y,x) = (*InImage)(x,y,0);
    }
  }

  merge(three_channels, OutMat);

  return OutMat;
}

Halide::Runtime::Buffer<uint8_t> imageSignalProcessing::Mat2Image( Mat *InMat ) {
  int height = (*InMat).rows;
  int width  = (*InMat).cols;
  Halide::Runtime::Buffer<uint8_t> OutImage(width,height,3);
  vector<Mat> three_channels;
  split((*InMat), three_channels);

  // Convert from interleaved BGR memory storage to planar RGB memory storage
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      // Blue channel
      OutImage(x,y,2) = three_channels[0].at<uint8_t>(y,x);
      // Green channel
      OutImage(x,y,1) = three_channels[1].at<uint8_t>(y,x);
      // Red channel
      OutImage(x,y,0) = three_channels[2].at<uint8_t>(y,x);
    }
  }

  return OutImage;
}
