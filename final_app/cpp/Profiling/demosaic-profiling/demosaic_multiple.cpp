// Halide
#include "auto_schedule_dem_fwd.h"
#include "Halide.h"
//#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "halide_image.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
//#include <chrono> 
#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

// Pipeline V79
// 
// Test type: 
// Debug
// 
// Stages:
// Remosaic, DemosInterp
using namespace cv;
using namespace std;
//using namespace std::chrono; 
using namespace Halide;
using namespace Halide::Tools;		   
#ifdef PROFILEWITHCHRONO
template<class Container>
std::ostream& write_container(const Container& c, std::ostream& out, string pipeversion, char delimiter = ',')
									   
										   
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



// fucntion prototypes
void OpenCV_remosaic (Mat *InMat );
Halide::Runtime::Buffer<float> Mat2Image( Mat *InMat );
Mat Image2Mat( Buffer<float> *InImage );
Func make_scale( Buffer<uint8_t> *in_img );

int main(int argc, char **argv){
  // Inform user of usage method
  if ( argc != 2 )
  {
      printf("usage: \n./convert path/to/image/dir\n");
      return -1;
  }

  // Input image (path and name)
  const char * img_path = argv[1];

  // Output image (just path)
  // const char * out_path    = argv[2];

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format input image

  // Declare image handle variables
  Var x, y, c;
  Image<uint8_t> input;
  int width, height;
  Func scale;
  Buffer<float> opencv_in_image;
  Mat opencv_in_mat;
  Halide::Runtime::Buffer<float> opencv_out;
  vector<vector<double>> wc_avg_bc_tuples;

  string in_string, out_string;
  for (int version = 0; version <= 7; version++){
    cout << "- profiling version: " << version << endl;
	out_string = "chrono/runtime_demos_multiple_v" + to_string(version) + "_.csv";
	std::ofstream outfile(out_string);
    for (int i = 0; i < 200; i++){
      cout << i << endl;
      in_string = std::string(img_path)+"v"+to_string(version)+"/vrep/img_vrep_"+to_string(i)+".png";

      // Load input image 
      input = load_image(in_string);

      ///////////////////////////////////////////////////////////////////////////////////////
      // Reverse Camera pipeline

       width  = input.width();
       height = input.height();

      // Scale to 0-1 range and represent in floating point
      scale              = make_scale        ( &input);

      opencv_in_image = scale.realize(width, height, 3);

      opencv_in_mat = Image2Mat(&opencv_in_image);

      // open cv
      OpenCV_remosaic(&opencv_in_mat);

      opencv_out = Mat2Image(&opencv_in_mat);


      ///////////////////////////////////////////////////////////////////////////////////////
      // Forward Camera pipeline 
      // Use AOT compilation
      Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());

      //////////////////////////////////////////////////////////////////////////////////////////////////
      // Timing code statistical
	  #ifdef PROFILEWITHCHRONO					
      wc_avg_bc_tuples = do_benchmarking( [&]() { auto_schedule_dem_fwd(opencv_out, output);} );
      // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
      // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
      // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
      
      // std::ofstream outfile("results.csv");
      // write_container(wc_avg_bc_tuples[0], outfile);
      
      write_container(wc_avg_bc_tuples[0], outfile, "img_"+to_string(i));
	  outfile << "\n";
	  
	  // Save the output
	  save_image(output, (std::string(img_path)+"v"+to_string(version)+"/demosaic/img_dem_"+to_string(i)+".png").c_str());

    }
  }

  // // sampling 
  // BenchmarkConfig config;
  // config.accuracy = 0.001;
  // BenchmarkResult result{0, 0, 0};
  // result = Halide::Tools::benchmark( [&](){ auto_schedule_true_fwd(opencv_out, output); }, config );
  // cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
  // cout << "Number of samples used for measurement                    : " << result.samples << endl;
  // cout << "Total number of iterations across all samples             : " << result.iterations << endl;
  // cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

  // // instruction
  // cout << "\n";
  // do_instr_benchmarking([&](){auto_schedule_true_fwd(opencv_out, output);});

////////////////////////////////////////////////////////////////////
  
  #endif				

  return 0;
}

Mat Image2Mat( Buffer<float> *InImage ) {
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

Halide::Runtime::Buffer<float> Mat2Image( Mat *InMat ) {
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

void OpenCV_remosaic (Mat *InMat ) {

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


Func make_scale( Buffer<uint8_t> *in_img ) {
  Var x, y, c;
  // Cast input to float and scale from 8 bit 0-255 range to 0-1 range
  Func scale("scale");
    scale(x,y,c) = cast<float>( (*in_img)(x,y,c) ) / 255;
  return scale;
}

