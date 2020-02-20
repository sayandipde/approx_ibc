#include <opencv2/opencv.hpp>
#include "config.hpp"

using namespace std;
using namespace cv;

int main(){
	Mat input, output;
	input = imread("1.png");
	imageSignalProcessing ISP;
	output = ISP.approximate_pipeline(input, 0);

	imwrite("output.png", output);
	return 0;
}

