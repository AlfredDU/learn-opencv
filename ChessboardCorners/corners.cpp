#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/calib3d.hpp>


const std::string CHESSBOARD_IMAGE = "chess.png";
const cv::Size CORNERS_PATTERN(5, 4); // corners' number in row and column
const int CORNER_MARK_SIZE = 4;

int main(int argc, char* argv[])
{
    cv::Mat chessboard = cv::imread(CHESSBOARD_IMAGE, CV_LOAD_IMAGE_COLOR);
    std::vector<cv::Point2i> corners;

    cv::findChessboardCorners(chessboard, CORNERS_PATTERN, corners);
    std::cout << corners.size() << " corners found." << std::endl;

    for (auto corner : corners) {
        cv::circle(chessboard, corner, CORNER_MARK_SIZE, CV_RGB(255, 0, 0)); // draw circles at the corners
    }

    cv::imshow("Marked", chessboard);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}
