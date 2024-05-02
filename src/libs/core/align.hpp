//
//  align.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/5/2.
//

#ifndef align_hpp
#define align_hpp

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

namespace qrcode {
namespace detect {
class Align {
  public:
    Align();

    cv::Mat crop(const cv::Mat &inputImg, const cv::Rect &srcRect, const float paddingW, const float paddingH,
                 const int minPadding, cv::Rect &crop_roi);

    cv::Mat crop(const cv::Mat &inputImg, const cv::Mat &srcPts, const float paddingW, const float paddingH,
                 const int minPadding, cv::Rect &crop_roi);

    void setRotate90(bool v) { rotate90_ = v; }

  private:
    cv::Mat crop(const cv::Mat &inputImg, const int width, const int height);
    cv::Mat M_;
    cv::Mat M_inv_;

    int crop_x_;
    int crop_y_;
    bool rotate90_;
};
};  // namespace detect
};  // namespace qrcode

#endif /* align_hpp */
