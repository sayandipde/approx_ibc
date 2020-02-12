/////////////////////////////////////////////////////////////////////////////////////////
// Image Processing Pipeline
//
// This is a Halide implementation of a pre-learned image 
// processing model. A description of the model can be found in
// "A New In-Camera Imaging Model for Color Computer Vision 
// and its Application" by Seon Joo Kim, Hai Ting Lin, 
// Michael Brown, et al. Code for learning a new model can 
// be found at the original project page. This particular 
// implementation was written by Mark Buckler.
//
// Original Project Page:
// http://www.comp.nus.edu.sg/~brown/radiometric_calibration/
//
// Model Format Readme:
// http://www.comp.nus.edu.sg/~brown/radiometric_calibration/datasets/Model_param/readme.pdf
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "auto_schedule_true_rev.h"
#include "auto_schedule_true_fwd_v0.h"
#include "auto_schedule_true_fwd_v1.h"
#include "auto_schedule_true_fwd_v2.h"
#include "auto_schedule_true_fwd_v3.h"
#include "auto_schedule_true_fwd_v4.h"
#include "auto_schedule_true_fwd_v5.h"
#include "auto_schedule_true_fwd_v6.h"
#include "auto_schedule_dem_fwd.h"

// Halide
#include "Halide.h"
#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "halide_image.h"

#include <opencv2/opencv.hpp>
#include "config.hpp"

// #######################
#include "LoadCamModel.h"
#include "MatrixOps.h"
#include <stdio.h>
#include <math.h>


using namespace cv;
using namespace Halide;
using namespace Halide::Tools;

imageSignalProcessing::imageSignalProcessing(){}
imageSignalProcessing::~imageSignalProcessing(){}

Mat imageSignalProcessing::approximate_pipeline(Mat the_img_vrep, int the_pipe_version) { 
  using namespace std;

  Mat img_rev, img_fwd, v0_rev, v0_fwd;

  //Run pipeline v0
  v0_rev = run_pipeline(false, the_img_vrep, 0);
  v0_fwd = run_pipeline(true, v0_rev, 0);

  if (the_pipe_version != 0){
    //Run backward pipeline
    img_rev = run_pipeline(false, v0_fwd, the_pipe_version);
    if (the_pipe_version == 7) return img_rev;

    // Run forward pipeline
    img_fwd = run_pipeline(true, img_rev, the_pipe_version);
  } else {
    img_rev = v0_rev;
    img_fwd = v0_fwd;
  }

  // save_images
  string out_string = "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_vrep_api/out_imgs";
  imwrite(out_string+"/output_rev.png", img_rev);
  imwrite(out_string+"/output_fwd_v"+to_string(the_pipe_version)+".png", img_fwd);

  return img_fwd;
}

// Reversible pipeline function
Mat imageSignalProcessing::run_pipeline(bool direction, Mat the_img_in, int the_pipe_version) {
  using namespace std;  

  ///////////////////////////////////////////////////////////////
  // Input Image Parameters
  ///////////////////////////////////////////////////////////////
   Halide::Buffer<uint8_t> input1;
   Halide::Runtime::Buffer<uint8_t> input;
  // convert input Mat to Image
  if (the_pipe_version == 8 && direction){
	input1 = Mat2ImageB(&the_img_in);
	Mat mat_input, mat_output;
	int width  = input1.width();
	int height = input1.height();
	// cout << "here3.1" << endl;
	Func scale              = make_scale        ( &input1);
	// cout << "here3.2" << endl;
	Buffer<float> opencv_in_image = scale.realize(width, height, 3);
	// cout << "here3.3" << endl;
	Mat opencv_in_mat = Image2Matf(&opencv_in_image);
	// cout << "here3.4" << endl;
	OpenCV_remosaic(&opencv_in_mat);
	// cout << "here3.5" << endl;
	Halide::Runtime::Buffer<float> opencv_out = Mat2Imagef(&opencv_in_mat);
	// cout << "here3.6" << endl;
	Halide::Runtime::Buffer<uint8_t> output_n(input1.width(), input1.height(), input1.channels());
	// cout << "here3.7" << endl;
	auto_schedule_dem_fwd(opencv_out, output_n);
	// cout << "here3.8" << endl;
	mat_output = Image2Mat(&output_n);
	imwrite("out_imgs/img_v8.png", mat_output);
	mat_input = imread("out_imgs/img_v8.png", IMREAD_COLOR);
	input = Mat2Image(&mat_input); 
  }
  else{
	input = Mat2Image(&the_img_in);  
  }

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
  "/home/sayandipde/yingkai-huang3/Approx_IBC_sim/final_app/cpp/ReversiblePipeline/src/Halide/camera_models/NikonD7000/";

  // White balance index (select white balance from transform file)
  // The first white balance in the file has a wb_index of 1
  // For more information on model format see the readme
  int wb_index = 3; // Actual choice = Given no. + 1

  // Number of control points
  const int num_ctrl_pts = 3702;

  Ts        = get_Ts       (cam_model_path);
  Tw        = get_Tw       (cam_model_path, wb_index);
  TsTw      = get_TsTw     (cam_model_path, wb_index);
  ctrl_pts  = get_ctrl_pts (cam_model_path, num_ctrl_pts, direction);
  weights   = get_weights  (cam_model_path, num_ctrl_pts, direction);
  coefs     = get_coefs    (cam_model_path, num_ctrl_pts, direction);
  rev_tone  = get_rev_tone (cam_model_path);
  
  // Take the transpose of the color map and white balance transform for later use
  vector<vector<float>> TsTw_tran = transpose_mat (TsTw); // source of segmentation error

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
  length = 3; //4 [SD]
  Halide::Runtime::Buffer<float> coefs_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      coefs_h(x,y) = coefs[x][y];
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // Declare image handle variables
  Var x, y, c;

  ////////////////////////////////////////////////////////////////////////
  // run the generator

  Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());

  if (direction == 1) {
    switch(the_pipe_version){
      case 0: // full rev + fwd                   skip 0 stages
        auto_schedule_true_fwd_v0(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;
      case 1: // rev + fwd_transform_tonemap      skip 1 stage
        auto_schedule_true_fwd_v1(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;      
      case 2: // rev + fwd_transform_gamut        skip 1 stage
       auto_schedule_true_fwd_v2(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;
      case 3: // rev + fwd_gamut_tonemap          skip 1 stage
        auto_schedule_true_fwd_v3(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;
      case 4: // rev + fwd_transform              skip 2 stages
        auto_schedule_true_fwd_v4(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;      
      case 5: // rev + fwd_gamut                  skip 2 stages
        auto_schedule_true_fwd_v5(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;      
      case 6: // rev + fwd_tonemap                skip 2 stages
        auto_schedule_true_fwd_v6(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
        break;
	  case 8: // skip denoise
	  {	auto_schedule_true_fwd_v0(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
		// cout << "here3.9" << endl;
	    break; }
      default: // should not happen
        cout << "Default pipe branch taken, pls check\n";
        break;
    }// switch      
  } else {
	 auto_schedule_true_rev(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output); 
  }

  ///////////////////////////////////////////////////////////////////////
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

Mat imageSignalProcessing::Image2Matf( Buffer<float> *InImage ) {
  int height = (*InImage).height();
  int width  = (*InImage).width();
  Mat OutMat(height,width,CV_32FC3);
  vector<Mat> three_channels;
  split(OutMat, three_channels);

  // Convert from planar RGB memory storage to interleaved BGR memory storage
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      // Blue channel
      three_channels[0].at<float>(y,x) = (*InImage)(x,y,2);
      // Green channel
      three_channels[1].at<float>(y,x) = (*InImage)(x,y,1);
      // Red channel
      three_channels[2].at<float>(y,x) = (*InImage)(x,y,0);
    }
  }

  merge(three_channels, OutMat);

  return OutMat;
}

Halide::Runtime::Buffer<float> imageSignalProcessing::Mat2Imagef( Mat *InMat ) {
  int height = (*InMat).rows;
  int width  = (*InMat).cols;
  Halide::Runtime::Buffer<float> OutImage(width,height,3);
  vector<Mat> three_channels;
  split((*InMat), three_channels);

  // Convert from interleaved BGR memory storage to planar RGB memory storage
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      // Blue channel
      OutImage(x,y,2) = three_channels[0].at<float>(y,x);
      // Green channel
      OutImage(x,y,1) = three_channels[1].at<float>(y,x);
      // Red channel
      OutImage(x,y,0) = three_channels[2].at<float>(y,x);
    }
  }

  return OutImage;
}

Halide::Buffer<uint8_t> imageSignalProcessing::Mat2ImageB( Mat *InMat ) {
  int height = (*InMat).rows;
  int width  = (*InMat).cols;
  Halide::Buffer<uint8_t> OutImage(width,height,3);
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

Halide::Func imageSignalProcessing::make_scale( Buffer<uint8_t> *in_img ) {
  Var x, y, c;
  // Cast input to float and scale from 8 bit 0-255 range to 0-1 range
  Func scale("scale");
    scale(x,y,c) = cast<float>( (*in_img)(x,y,c) ) / 255;
  return scale;
}

void imageSignalProcessing::OpenCV_remosaic (Mat *InMat ) {

  vector<Mat> three_channels;
  cv::split((*InMat), three_channels);

  // Re-mosaic aka re-bayer the image
  // B G
  // G R

  // Note: OpenCV stores as BGR not RGB
  for (int y=0; y<(*InMat).rows; y++) {
    for (int x=0; x<(*InMat).cols; x++) {
      // If an even row
      if ( y%2 == 0 ) {
        // If an even column
        if ( x%2 == 0 ) {
          // Green pixel, remove blue and red
          three_channels[0].at<float>(y,x) = 0; 
          three_channels[2].at<float>(y,x) = 0;
          // Also divide the green by half to account
          // for interpolation reversal
          three_channels[1].at<float>(y,x) = 
            three_channels[1].at<float>(y,x) / 2;
        }
        // If an odd column
        else {
          // Red pixel, remove blue and green
          three_channels[0].at<float>(y,x) = 0;
          three_channels[1].at<float>(y,x) = 0;
        }
      }
      // If an odd row
      else {
        // If an even column
        if ( x%2 == 0 ) {
          // Blue pixel, remove red and green
          three_channels[2].at<float>(y,x) = 0;
          three_channels[1].at<float>(y,x) = 0;
        }
        // If an odd column
        else {
          // Green pixel, remove blue and red
          three_channels[0].at<float>(y,x) = 0;
          three_channels[2].at<float>(y,x) = 0;
          // Also divide the green by half to account
          // for interpolation reversal
          three_channels[1].at<float>(y,x) = 
            three_channels[1].at<float>(y,x) / 2;
        }
      }
    }
  }
  cv::merge(three_channels, *InMat);

}