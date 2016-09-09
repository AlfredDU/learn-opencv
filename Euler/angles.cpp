#include <cstdlib>
#include <vector>
#include <iostream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


const std::string ORIGINAL_IMAGE = "perpetual-motion.jpg";

const float f = 1000.0;
const float nutation = float(M_PI) / 180 * 0.0f;
const float precession = float(M_PI) / 180 * 0.0f;
const float spin = float(M_PI) / 180 * 0.0f;


cv::Point2f weakPerspective(const cv::Vec<float, 3>& vec, float focal) {
    cv::Vec<float, 4> vec_homogeneous(vec[0], vec[1], vec[2], focal);

    cv::Matx<float, 4, 4> t(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 1.0f / focal, 0.0
    );

    cv::Vec<float, 4> transformed = t * vec_homogeneous;
    return cv::Point2f(transformed(0) / transformed(3), transformed(1) / transformed(3));
};

int main(int argc, char* argv[]){

    cv::Mat src = cv::imread(ORIGINAL_IMAGE, CV_LOAD_IMAGE_UNCHANGED);

    const float x = static_cast<float>(src.cols);
    const float y = static_cast<float>(src.rows);
    const float z = f;

    cv::Vec<float, 3> point_x00, point_x01, point_x10, point_x11;
    point_x00 << 0.0, 0.0, z;
    point_x01 << 0.0, y, z;
    point_x10 << x, 0.0, z;
    point_x11 << x, y, z;
    
    cv::Matx<float, 3, 3> M_Nutation(
        1.0, 0.0, 0.0,
        0.0, std::cos(nutation), - std::sin(nutation),
        0.0, std::sin(nutation), std::cos(nutation)
    );
    cv::Matx<float, 3, 3> M_Precession(
        std::cos(precession), 0.0, std::sin(precession),
        0.0, 1.0, 0.0,
        - std::sin(precession), 0.0, std::cos(precession)
    );
    cv::Matx<float, 3, 3> M_Spin(
        std::cos(spin), - std::sin(spin), 0.0,
        std::sin(spin), std::cos(spin), 0.0,
        0.0, 0.0, 1.0
    );

    cv::Vec<float, 3> transformed_x00 = M_Spin * ( M_Precession * ( M_Nutation * point_x00 ));
    cv::Vec<float, 3> transformed_x01 = M_Spin * ( M_Precession * ( M_Nutation * point_x01 ));
    cv::Vec<float, 3> transformed_x10 = M_Spin * ( M_Precession * ( M_Nutation * point_x10 ));
    cv::Vec<float, 3> transformed_x11 = M_Spin * ( M_Precession * ( M_Nutation * point_x11 ));

    cv::Point2f srcCtrl[4] = {
        weakPerspective(point_x00, f),
        weakPerspective(point_x01, f),
        weakPerspective(point_x10, f),
        weakPerspective(point_x11, f)
    };
    cv::Point2f dstCtrl[4] = {
        weakPerspective(transformed_x00, f),
        weakPerspective(transformed_x01, f),
        weakPerspective(transformed_x10, f),
        weakPerspective(transformed_x11, f)
    };

    cv::Mat t = cv::getPerspectiveTransform(srcCtrl, dstCtrl);

    cv::Mat dst(src.rows, src.cols, src.depth());
    cv::warpPerspective(src, dst, t, src.size());


    cv::namedWindow("src");
    cv::imshow("src", src);

    cv::namedWindow("dst");
    cv::imshow("dst", dst);

    cv::waitKey(0);
    return EXIT_SUCCESS;
}
