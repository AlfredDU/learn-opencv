#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


const int HBLOCKS = 6;
const int VBLOCKS = 5;
const int BLOCK_EDGEWIDTH = 40;

int main(int argc, char* argv[])
{
    int cols = BLOCK_EDGEWIDTH * HBLOCKS;
    int rows = BLOCK_EDGEWIDTH * VBLOCKS;

    cv::Mat chess(rows, cols, CV_8UC1);

    int x_thBlock, y_thBlock;
    unsigned char c;

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            x_thBlock = i / BLOCK_EDGEWIDTH;
            y_thBlock = j / BLOCK_EDGEWIDTH;
            c = static_cast<unsigned char>((x_thBlock + (- 1) ^ y_thBlock) % 2 == 0 ? 0 : 255);

            chess.at<uchar>(j, i) = c; // Note: the fist index is row-index, while the second is col-index.
        }
    }

    cv::imwrite("chess.png", chess);

    cv::imshow("Chess", chess);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}
