#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/calib3d.hpp>


const std::vector<std::string> PATTERN_IMAGES = {
        "chess-0.png", "chess-90r.png", "chess-r.png"
};
const cv::Size CORNERS_PATTERN(5, 4); // corners' number in row and column


int main(int argc, char* argv[])
{
    /*
     * Must be 2-dimensional array since the function #cv::calibrateCamera made for multiple patterns/views.
     * The array must be initially allocated with certain size (in this case 1) or error occurs.
     */
    std::vector<std::vector<cv::Point2f> > imagePoints; // image points
    cv::Size imageSize(0, 0);  // Size of the image used only to initialize the intrinsic camera matrix

    for (auto CHESSBOARD_IMAGE : PATTERN_IMAGES) {
        // get corners of different views
        cv::Mat chessboard = cv::imread(CHESSBOARD_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);
        std::vector<cv::Point2f> corners; // image points per pattern

        bool areFound = cv::findChessboardCorners(chessboard, CORNERS_PATTERN, corners);
        if (! areFound) {
            std::cerr << "No corner of view " << CHESSBOARD_IMAGE << " found." << std::endl;
            return EXIT_FAILURE;
        } else {
            std::cout << corners.size() << " corners found on view " << CHESSBOARD_IMAGE << std::endl;
        }
        // append to image points
        imagePoints.push_back(corners);

        // set image size to maximum of views
        imageSize.height = chessboard.size().height > imageSize.height ? chessboard.size().height : imageSize.height;
        imageSize.width = chessboard.size().width > imageSize.width ? chessboard.size().width : imageSize.width;
    }

    // construct object points; all views share the same array
    std::vector<cv::Point3f> corners3d;
    for (auto imagePoint : imagePoints[0]) {
        corners3d.push_back(cv::Point3f(imagePoint.x, imagePoint.y, 0.0f));
    }

    std::vector<std::vector<cv::Point3f> > objectPoints(imagePoints.size(), corners3d); // object points; constructed by repeat

    // allocate output matrices
    cv::Mat cameraMatrix(3, 3, CV_32F);
    cv::Mat distortCoefficients(8, 1, CV_32F);
    std::vector<cv::Mat> rotateVectors;
    std::vector<cv::Mat> translateVectors;

    // calibration
    cv::calibrateCamera(
                objectPoints,
                imagePoints,
                imageSize,
                cameraMatrix,
                distortCoefficients,
                rotateVectors,
                translateVectors);

    std::cout << "The camera intrinsic matrix is: " << std::endl << cameraMatrix << std::endl;

    std::cout << "The rotation vectors are: " << std::endl;
    for (auto vec : rotateVectors) {
        std::cout << vec << std::endl;
    }

    std::cout << "The translation vectors are: " << std::endl;
    for (auto vec : translateVectors) {
        std::cout << vec << std::endl;
    }

    std::cout << "The distortion coeffients are: " << std::endl << distortCoefficients << std::endl;

    return EXIT_SUCCESS;
}
