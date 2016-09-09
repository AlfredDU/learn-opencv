#include <cstdlib>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, const char * argv[]) {

    cv::Mat lenna = cv::imread("Lenna.png");
    cv::imshow("Lenna", lenna);

    cv::waitKey();

    return EXIT_SUCCESS;
}
