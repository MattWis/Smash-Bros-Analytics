#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <iostream>

void rgb2r_gray(cv::Mat& src, cv::Mat dst) {
  CV_Assert(src.type() == CV_8UC3);

  for (int i = 0; i < src.rows; ++i) {
    for (int j = 0; j < src.cols; ++j) {
      cv::Vec3b point = src.at<cv::Vec3b>(i,j);
      float red = point[2];

      cv::Vec3b red_point;
      red_point[0] = red;
      red_point[1] = red;
      red_point[2] = red;

      dst.at<cv::Vec3b>(i,j) = red_point;
    }
  }
}

int main()
{
  cv::Mat full_color = cv::imread("00300.tiff");
  if (!full_color.data)
    return -1;

  cv::Mat gray = full_color.clone();
  rgb2r_gray(full_color, gray);

  cv::imwrite("preprocessed.tiff", gray);
  gray = gray > 200;

  cv::Mat filtered;
  cv::cvtColor(gray, filtered, CV_BGR2GRAY);

  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(filtered.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  for (int i = 0; i < contours.size(); i++)
  {
    std::cout << cv::contourArea(contours[i]) << std::endl;
    cv::drawContours(filtered, contours, i, cv::Scalar(0), -1);
  }

  cv::imwrite("processed.tiff", filtered);

  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
  tess.SetVariable("tessedit_char_whitelist", "0123456789");
  tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tess.SetImage((uchar*)filtered.data, filtered.cols, filtered.rows, 1, filtered.cols);

  char* out = tess.GetUTF8Text();
  std::cout << out << std::endl;
  std::cout << "oh hai" << std::endl;

  return 0;
}
