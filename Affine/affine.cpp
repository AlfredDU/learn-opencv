#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


std::string SRC_IMAGE = "chess.png";

cv::Mat affine_no_culling(cv::Mat& src, cv::Matx<float, 3, 3>& affine_matrix) {

    cv::Matx<float, 3, 4> src_vertices(
        0, src.cols, src.cols, 0,
        0, 0,        src.rows, src.rows,
        1, 1,        1,        1
    );

    cv::Matx<float, 3, 4> dst_vertices = affine_matrix * src_vertices;

    double xMax, xMin, yMax, yMin;
    cv::minMaxIdx(dst_vertices.row(0), & xMin, & xMax);
    cv::minMaxIdx(dst_vertices.row(1), & yMin, & yMax);

    xMin = std::min(xMin, 0.0);
    yMin = std::min(yMin, 0.0);
    xMax = std::max(xMax, static_cast<double>(src.cols));
    yMax = std::max(yMax, static_cast<double>(src.rows));

    cv::Size dst_size(static_cast<int>(xMax - xMin), static_cast<int>(yMax - yMin));
    cv::Mat dst(dst_size, src.type());

    // cv::Mat dst_rect = (cv::Mat_<float>(3, 4) << 
    //     xMin, xMax, xMax, xMin,   
    //     yMin, yMin, yMax, yMax,
    //     1,    1,   1,  1
    // );
    // cv::Mat src_rect = m_inv * dst_rect;
    // cv::minMaxIdx(src_rect.row(0), & xMin, & xMax);
    // cv::minMaxIdx(src_rect.row(1), & yMin, & yMax);

    // cv::Size src_ext_size(xMax - xMin, yMax - yMin);
    // cv::Mat src_ext(src_ext_size, src.type());

    cv::Matx<float, 3, 3> panning(
        1.0, 0.0, static_cast<float>(- xMin),
        0.0, 1.0, static_cast<float>(- yMin),
        0.0, 0.0, 1.0
    );

    cv::Mat affine_with_panning(panning * affine_matrix); // use type Mat instead of Matx for convenient slicing
    cv::warpAffine(
    	src, 
    	dst, 
        cv::Mat(affine_with_panning, cv::Rect(0, 0, 3, 2)),
    	dst_size
    );

    cv::Matx<float, 3, 4> src_vertices_shadow = panning * src_vertices;
    cv::Point2f left_top(
        src_vertices_shadow(0, 0), src_vertices_shadow(1, 0));
    cv::Point2f right_bottom(
        src_vertices_shadow(0, 2), src_vertices_shadow(1, 2));
    cv::rectangle(dst, left_top, right_bottom, CV_RGB(0, 0, 255), 8);

    return dst;
};

cv::Mat translate(cv::Mat& src, float xOffset, float yOffset) {

    cv::Matx<float, 3, 3> translate_matrix(
        1.0, 0.0, xOffset, 
        0.0, 1.0, yOffset, 
        0.0, 0.0, 1.0
    );
    
    return affine_no_culling(src, translate_matrix);
};

cv::Mat rotate(cv::Mat& src, float angle, cv::Point2i center=cv::Point2i(0, 0)) {

	cv::Matx<float, 3, 3> shift(
        1.0, 0.0, - center.x,
        0.0, 1.0, - center.y,
        0.0, 0.0, 1.0
    );

    cv::Matx<float, 3, 3> rotate_matrix( // use std::sin instead of sin for saving numerical cast
        std::cos(angle),   - std::sin(angle), 0.0,
        std::sin(angle),   std::cos(angle),   0.0,
        0.0,               0.0,               1.0
    );

    cv::Matx<float, 3, 3> shift_inv(
        1.0, 0.0, center.x,
        0.0, 1.0, center.y,
        0.0, 0.0, 1.0
    );
    
    cv::Matx<float, 3, 3> M = shift_inv * rotate_matrix * shift;

    return affine_no_culling(src, M);
};

cv::Mat scale(cv::Mat& src, float xScale, float yScale, cv::Point2i center=cv::Point2i(0, 0)) {

    cv::Matx<float, 3, 3> shift(
        1.0, 0.0, - center.x,
        0.0, 1.0, - center.y,
        0.0, 0.0, 1.0
    );

    cv::Matx<float, 3, 3> scale_matrix(
        xScale, 0.0,    0.0, 
        0.0,    yScale, 0.0, 
        0.0,    0.0,    1.0
    );
    
    cv::Matx<float, 3, 3> shift_inv(
        1.0, 0.0, center.x,
        0.0, 1.0, center.y,
        0.0, 0.0, 1.0
    );
    
    cv::Matx<float, 3, 3> M = shift_inv * scale_matrix * shift;

    return affine_no_culling(src, M);
};


int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread(SRC_IMAGE, CV_LOAD_IMAGE_COLOR);
    cv::Mat dst = rotate(src, 3.14f / 6, cv::Point2i(src.cols / 2, src.rows / 2));
//     cv::Mat dst = translate(src, - 50, 80);
//    cv::Mat dst = scale(src, - 2.0f, 1.5f, cv::Point2i(src.cols / 3, src.rows / 2));
   
    cv::imshow("original", src);
    cv::imshow("transformed", dst);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}
