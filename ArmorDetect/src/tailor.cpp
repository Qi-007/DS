#include "detect.h"

// 记录保存帧数
int armor_count = 148;

void cutImg::cut_image(const std::vector<cv::Point2f>& m_armor, cv::Mat& frame){

    // 图像尺寸
    int width = 200;
    int height = 200;

    // 创建一张纯黑色图像（灰度图）
    cv::Mat black_image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    if (m_armor.size() != 4) {
        // std::cerr << "Error: m_armor must contain exactly 4 points." << std::endl;
        return;
    }

    // 定义目标矩形的顶点（目标点）
    std::vector<cv::Point2f> dst_points = {
        cv::Point2f(0, 0),       // 左上
        cv::Point2f(0, 200),     // 左下
        cv::Point2f(200, 200),   // 右下
        cv::Point2f(200, 0)      // 右上
    };

    // 计算透视变换矩阵
    cv::Mat perspective_matrix = cv::getPerspectiveTransform(m_armor, dst_points);

    // 应用透视变换
    cv::Mat warped_image;
    cv::warpPerspective(frame, warped_image, perspective_matrix, cv::Size(200, 200));

    // 保存图片的文件夹路径
    std::string output_dir = "/home/zjq/data/test/class6/";

    // 保存当前装甲板为图片
    std::ostringstream filename;
    // filename << output_dir << std::setw(5) << std::setfill('0') << armor_count << ".jpg";
    filename << output_dir << armor_count << ".jpg";

    cv::imwrite(filename.str(), warped_image);

    armor_count++;

}