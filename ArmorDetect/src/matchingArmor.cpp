#include "detect.h"

double distance(const cv::Point2f& p1, const cv::Point2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

cv::Mat matchingArmor:: matchingArmors(const std::vector<std::pair<LightDescriptor, LightDescriptor>>& m_lights, cv::Mat& m_frame, std::vector<cv::Point2f>& m_armor){
    // 如果没有灯条，则直接返回原始图像
    if (m_lights.empty()) {
        return m_frame;
    }

    double min_armorArea = 500.0;

    //绘制装甲板区域
    for(const auto& m_light : m_lights){

        const auto& leftLight = m_light.first;
        const auto& rightLight = m_light.second;

        // 提取两个旋转矩形的顶点
        // 顶点的顺序是按矩形的边排列，依次为左上、右上、右下、左下（以逆时针顺序为准）。
        cv::Point2f vertices1[4], vertices2[4];
        for(int i = 0; i < 4; i++){
            vertices1[i] = leftLight.point[i];
        }
        for(int i = 0; i < 4; i++){
            vertices2[i] = rightLight.point[i];
        }
 
        // 合并所有顶点
        std::vector<cv::Point2f> allPoints(vertices1, vertices1 + 4);
        allPoints.insert(allPoints.end(), vertices2, vertices2 + 4);

        // 转换为整型点
        std::vector<cv::Point> intPoints;
        for (const auto& pt : allPoints) {
            intPoints.emplace_back(cv::Point(cvRound(pt.x), cvRound(pt.y)));
        }

        // 使用cv::convexHull计算凸包
        std::vector<cv::Point> hull;
        convexHull(intPoints, hull);

        // 计算凸包面积（面积太小的装甲板不要）
        double armorArea = contourArea(hull);

        if(armorArea < min_armorArea){
            continue;
        } 

        // cout << armorArea << endl;

        // // 绘制凸包
        // for (size_t i = 0; i < hull.size(); i++) {
        //     cv::line(m_frame, hull[i], hull[(i+1) % hull.size()], cv::Scalar(255, 255, 255), 2);
        // }

        cv::RotatedRect armor = minAreaRect(hull);
        cv::Point2f vertices[4];
        armor.points(vertices);
        std::vector<cv::Point2f> m_armor_points;
      
        for(int i = 0; i < 4; ++i){
            m_armor_points.push_back(vertices[i]);
        }
        
        // 按左上. 左下. 右下. 右上的顺序存入m_armor
        // 按x从左到右排序
        sort(m_armor_points.begin(), m_armor_points.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
            return a.x < b.x;
        });

        std::vector<cv::Point2f> armor_lights;

        // 输入左上左下两个点
        if(m_armor_points[0].y < m_armor_points[1].y){
            armor_lights.emplace_back(m_armor_points[0]);
            armor_lights.emplace_back(m_armor_points[1]);
        }else{
            armor_lights.emplace_back(m_armor_points[1]);
            armor_lights.emplace_back(m_armor_points[0]);
        }   
    
        // 输入右下右上两个点
        if(m_armor_points[2].y < m_armor_points[3].y){
            armor_lights.emplace_back(m_armor_points[3]);
            armor_lights.emplace_back(m_armor_points[2]);
        }else{
            armor_lights.emplace_back(m_armor_points[2]);
            armor_lights.emplace_back(m_armor_points[3]);
        }

        // 0左上  1左下  2右下  3右上
        // 灯条的高
        double light_hight = (distance(armor_lights[0], armor_lights[1]) + distance(armor_lights[2], armor_lights[3])) / 2;

        for (int i = 0; i < 4; i++) {
            if (i == 0 || i == 3) {
                // 减小 y 坐标
                armor_lights[i].y -= light_hight / 2;
            }else {
                // 增大 y 坐标
                armor_lights[i].y += light_hight / 2;
            }

            m_armor.push_back(armor_lights[i]);
        }

        cv::Mat frame_clone = m_frame.clone();

        // 绘制装甲板
        for (size_t i = 0; i < m_armor.size(); i++) {
            cv::line(frame_clone, m_armor[i], m_armor[(i+1) % m_armor.size()], cv::Scalar(255, 255, 255), 2);
        }

        cv::imshow("前哨站", frame_clone);

    }
    return m_frame;
}
