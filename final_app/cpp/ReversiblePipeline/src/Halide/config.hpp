#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include "Halide.h"

class imageSignalProcessing {
private:
    // class private methods
    cv::Mat run_pipeline(bool direction, cv::Mat the_img_in, int the_pipe_version);
    Halide::Runtime::Buffer<uint8_t> Mat2Image(cv::Mat *InMat);
    cv::Mat Image2Mat(Halide::Runtime::Buffer<uint8_t> *InImage);
public:
    // constructor
    imageSignalProcessing();
    // destructor
    ~imageSignalProcessing();
    // class public methods
    cv::Mat approximate_pipeline(cv::Mat the_img_vrep, int the_pipe_version);
};


// ------------ function declarations -------------//
template<class Container>
std::ostream& write_container(const Container& c,
                     std::ostream& out,
                     char delimiter = '\n')
{
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

void write_yL_2_file(std::vector<long double> yL_container);
std::string get_timestamp();



