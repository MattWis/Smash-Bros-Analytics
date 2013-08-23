#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <iostream>

cv::Vec3b blackOrWhite (bool white) {
  if(white)
    return cv::Vec3b(255, 255, 255);
  return cv::Vec3b(0, 0, 0);
}

bool isRed(cv::Vec3b color) {
  return (color[2] > 200 && (color[0] + color[1] < 120));
}

bool isWhite(cv::Vec3b color) {
#define THRESH 150
  return ((color[0] > THRESH) && (color[1] > THRESH) && (color[2] > THRESH));
}

void smartGrayscale(cv::Mat& src, cv::Mat dst) {
  CV_Assert(src.type() == CV_8UC3);

  for (int i = 0; i < src.rows; ++i) {
    for (int j = 0; j < src.cols; ++j) {
      cv::Vec3b point = src.at<cv::Vec3b>(i,j);

      dst.at<cv::Vec3b>(i,j) = blackOrWhite(isWhite(point) || isRed(point));
    }
  }
}

char* readNumFromImage(cv::Mat image) {
  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
  tess.SetVariable("tessedit_char_whitelist", "0123456789");
  tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tess.SetImage((uchar*)image.data, image.cols, image.rows, 1, image.cols);

  return tess.GetUTF8Text();
}

void getPercentageFromImage(const char *filename) {
  std::string x = filename;
  std::cout << filename << std::endl;

  cv::Mat full_color = cv::imread(filename);
  if (!full_color.data)
    return;

  cv::Mat gray = full_color.clone();
  smartGrayscale(full_color, gray);

  cv::imwrite("preprocessed" + x, gray);

  cv::Mat prefiltered;
  cv::cvtColor(gray, prefiltered, CV_BGR2GRAY);
  cv::Mat noise = prefiltered.clone();
  cv::Mat filtered;

  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(noise.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  for (int i = 0; i < contours.size(); i++)
  {
    if(cv::contourArea(contours[i]) > 20)
      cv::drawContours(noise, contours, i, cv::Scalar(0), -1);
  }

  filtered = prefiltered & (noise < 100);

  cv::imwrite("prefiltered" + x, prefiltered);
  cv::imwrite("noise" + x, noise);
  cv::imwrite("not_noise" + x, (noise < 100));
  cv::imwrite("filtered" + x, filtered);

  char* out = readNumFromImage(filtered);
  std::cout << out << std::endl;
}

int main() {
  getPercentageFromImage("00100.tiff");
  std::cout << std::endl;
  getPercentageFromImage("00300.tiff");

  return 0;
}

