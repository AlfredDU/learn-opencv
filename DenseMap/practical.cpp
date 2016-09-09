#include <cmath>
#include <cstdlib>
#include <array>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// four corners of a quad
typedef std::array<cv::Point2f, 4> quad;
#define COLOR_WITH_ALPHA CV_8UC4

// generate corners of a rectangle
quad rectangle(int width, int height) {
    return {
        cv::Point2f(0.0f, 0.0f),
        cv::Point2f(static_cast<float>(width), 0.0f),
        cv::Point2f(static_cast<float>(width), static_cast<float>(height)),
        cv::Point2f(0.0f, static_cast<float>(height))
    };
};

// generate grid for a rectangle
cv::Mat grid(int width, int height, int horizontal, int vertical, int stroke = 2) {
    cv::Scalar TRANSPARENT(0, 0, 0, 255), RED(0, 0, 255, 0);

    cv::Mat rect(width, height, COLOR_WITH_ALPHA, TRANSPARENT);

    float vSpace = static_cast<float>(width) / (vertical + 1);
    float hSpace = static_cast<float>(height) / (horizontal + 1);

    for (int i = 1; i <= vertical; ++i) {
        cv::line(rect, cv::Point2f(vSpace * i, 0.0f), cv::Point2f(vSpace * i, static_cast<float>(height)), RED, stroke);
    }
    for (int i = 1; i <= horizontal; ++i) {
        cv::line(rect, cv::Point2f(0.0f, hSpace * i), cv::Point2f(static_cast<float>(width), hSpace * i), RED, stroke);
    }

    return rect;
};


// any two quads implies a perspective transformation
cv::Mat perspectiveMatrix(quad src, quad dst){
    return cv::getPerspectiveTransform(src.data(), dst.data());
}


int main(int argc, char* argv[]) {
    int w = 512, h = 512;

    // before transform
    cv::Mat preMorph = grid(w, h, 7, 7);

    // corners before transform
    quad preMorphCorners = rectangle(w, h);
    // set the quad corners that transform to
    quad morphedCorners = {
        cv::Point2f(0.0f, 0.0f), cv::Point2f(static_cast<float>(w), 0.0f),
        cv::Point2f(400.0f, 400.0f), cv::Point2f(0, static_cast<float>(h))
    };

    // transformed quad width & height
    int wt = 0, ht = 0;
    for(auto corner : morphedCorners) {
        wt = corner.x > wt ? static_cast<int>(corner.x) : wt;
        ht = corner.y > ht ? static_cast<int>(corner.y) : ht;
    }

    // calculate perspective matrix
    cv::Mat matrix = perspectiveMatrix(preMorphCorners, morphedCorners);
    std::cout << "The perspective matrix is: " << std::endl << matrix << std::endl;

    // perform transformation
    cv::Mat morphed = cv::Mat(ht, wt, COLOR_WITH_ALPHA);
    cv::warpPerspective(preMorph, morphed, matrix, morphed.size());

    // dense map, with same shape before transformation, for convenient mapping
    cv::Mat denseMap_t(preMorph.size(), preMorph.type());

    // determinant of perspective transformation matrix; use double to ensure precision
    double det = cv::determinant(matrix);
    // homogeneous coefficients of perspective transformation matrix
    double c0 = matrix.at<double>(2, 0);
    double c1 = matrix.at<double>(2, 1);
    double c2 = matrix.at<double>(2, 2);

    /* calculate dense mapping value: it is the quotient of perspective determinant and
     * cubic square of homogeneous multiplier  */
    double denseRatio; // a 0-1 decimal to illustrate pixels' density
    unsigned char greyscale; // proportionally attenuation white color

    for (int i = 0; i < denseMap_t.rows; i++){
        for (int j = 0; j < denseMap_t.cols; j++){

            denseRatio = det / pow((c0 * j + c1 * i + c2), 3);
            greyscale = static_cast<unsigned char>(255 * denseRatio);
            denseMap_t.at<cv::Vec4b>(i, j) = cv::Vec4b(greyscale, greyscale, greyscale, 0);
        }
    }

    // dense map, with same shape after transformation
    cv::Mat denseMap(morphed.size(), morphed.type());
    cv::warpPerspective(denseMap_t, denseMap, matrix, denseMap.size());

    cv::imshow("Dense map", denseMap_t);
    cv::waitKey();

    return EXIT_SUCCESS;
}
