#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "detect.h"

using namespace std;
using namespace cv; 

// 通道相减图像的预处理
void minus_dispose(const cv::Mat& m_frame, cv::Mat& m_dst){
    imageDispose minus_dispose;

    // 使用高斯函数平滑图像，减少噪声
    cv::Mat blurred, red_minus_blue, blue_minus_red, binary, open, close;
    blurred = minus_dispose.imageGaussion(m_frame);
    // 红蓝通道相减，强调红色区域
    red_minus_blue = minus_dispose.stressRed(blurred);

    // // 蓝红通道相减，强调蓝色区域
    // blue_minus_red = minus_dispose.stressBlue(blurred);

    // 对通道相减图像进行二值化处理
    binary = minus_dispose.imageThreshold(red_minus_blue, 100);

    // 对二值化后的图像进行开运算处理
    open = minus_dispose.imageOpen(binary);

    // 进行闭运算处理
    close = minus_dispose.imageClose(open);

    // 对闭运算后的图像进行膨胀处理
    m_dst = minus_dispose.imageDilate(close);
}


// 原图像的预处理
void image_dispose(const cv::Mat& m_frame, cv::Mat& m_binaryImage){
    imageDispose frame_dispose;

    // 使用高斯函数平滑图像，减少噪声
    cv::Mat blurred;
    blurred = frame_dispose.imageGaussion(m_frame);

    // 对原图像进行二值化处理
    m_binaryImage = frame_dispose.imageThreshold(blurred, 140);
}



int main(){
    //读取视频文件
    VideoCapture cap("/home/zjq/A&T/STUDY/MV-CS016-10UC+DA2849978/6.avi");
    if(!cap.isOpened()){
        cout << "视频加载失败" << endl;
    }


    Mat frame,      // 原图像
        binaryImage,    // 原图处理后的图像
        dst;     // 通道相减处理后的图像

    vector<vector<Point>> all_contours;     //未经筛选的轮廓
    vector<LightDescriptor> lights;       //筛选过宽高比的矩形
    vector<pair<LightDescriptor, LightDescriptor>> matching_lights;    //根据倾斜角等筛选出的配对灯条
    vector<pair<LightDescriptor, LightDescriptor>> foundArmor;      // 识别后的装甲板

  

    while(true){
    //读取每一帧
    cap >> frame;
    if(frame.empty()){
        break;
    }

    // 通道相减图像的二值化处理
    std::thread minusDispose(minus_dispose, std::ref(frame), std::ref(dst));

    // 原图像的二值化处理
    std::thread frameDispose(image_dispose, std::ref(frame), std::ref(binaryImage));

    // 等待线程执行完毕
    minusDispose.join();
    frameDispose.join();

    // cv::imshow("dst", dst);
    // cv::imshow("binaryImage", binaryImage);

    cv::Mat image;
    bitwise_and(dst, binaryImage, image);
    // imshow("image", image);

    // 寻找轮廓
    findContours(image, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //识别灯条
    findLightBar all_lightBar;
    lights = all_lightBar.Lights(all_contours);

    //匹配灯条
    matchingLightBar right_lightBar;

    //两两匹配灯条
    matching_lights = right_lightBar.matchLight(lights);

    // 识别装甲板
    findArmor armors;
    foundArmor = armors.find_Armor(matching_lights);

    // 匹配装甲板
    std::vector<cv::Point2f> armor;
    matchingArmor all_armors;
    frame = all_armors.matchingArmors(foundArmor, frame, armor);

    cutImg shear;
    shear.cut_image(armor, frame);

    // std::thread t(cut_image, std::ref(armor), std::ref(frame));
    // t.join();

    // cv::imshow("前哨站", frame);

    all_contours.clear();       // 清空上一帧的轮廓
    lights.clear();    // 清空上一帧筛选的矩形
    matching_lights.clear();    // 清空上一帧的配对灯条
    foundArmor.clear();     // 清空上一帧的装甲板
    armor.clear();     

    char c = cv::waitKey(cap.get(CAP_PROP_FPS));
    if(c == 'q' || c == 27){
        break;
    }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}