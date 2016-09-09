#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>


std::string LEFT_VIEW_IMAGE = "chess-l.png";
std::string RIGHT_VIEW_IMAGE = "chess-r.png";

const cv::Size CORNERS_PATTERN(5, 4); // corners' number in row and column

int main()
{
    cv::Mat lView =cv::imread(LEFT_VIEW_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat rView =cv::imread(RIGHT_VIEW_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);

    std::vector<cv::Point2f> lCorners;
    std::vector<cv::Point2f> rCorners;

    if (cv::findChessboardCorners(lView, CORNERS_PATTERN, lCorners)) {
        std::cout << lCorners.size() << " corners of the left view found." << std::endl;
    } else {
        std::cerr << "No corners found." << std::endl;
        return EXIT_FAILURE;
    }

    if (cv::findChessboardCorners(rView, CORNERS_PATTERN, rCorners)) {
        std::cout << rCorners.size() << " corners of the right view found." << std::endl;
    } else {
        std::cerr << "No corners found." << std::endl;
        return EXIT_FAILURE;
    }

   cv::Mat fundamental = cv::findFundamentalMat(lCorners, rCorners);

   std::cout << "Fundamental matrix: " << std::endl << fundamental << std::endl;

    return EXIT_SUCCESS;
}

