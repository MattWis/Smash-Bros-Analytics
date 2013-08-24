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
  return (color[2] > 200 && (abs(color[0] - color[1]) < 20));
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

int readNumFromImage(cv::Mat image) {
  tesseract::TessBaseAPI tess;
  tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
  tess.SetVariable("tessedit_char_whitelist", "0123456789");
  tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tess.SetImage((uchar*)image.data, image.cols, image.rows, 1, image.cols);

  int image_num = -1;

  char *imageText = tess.GetUTF8Text();
  if (imageText != "")
    sscanf(imageText, "%d", &image_num);

  return image_num;
}

int getPercentageFromImage(std::string filename) {
  cv::Mat full_color = cv::imread(filename);
  if (!full_color.data) {
    fprintf(stderr, "File not found\n");
    return -1;
  }

  cv::Mat gray = full_color.clone();
  smartGrayscale(full_color, gray);

  cv::Mat prefiltered;
  cv::cvtColor(gray, prefiltered, CV_BGR2GRAY);
  cv::Mat noise = prefiltered.clone();
  cv::Mat filtered;

  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(noise.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

  for (int i = 0; i < contours.size(); i++) {
    if(cv::contourArea(contours[i]) > 20)
      cv::drawContours(noise, contours, i, cv::Scalar(0), -1);
  }

  filtered = prefiltered & (noise < 100);

  return readNumFromImage(filtered);
}

int main() {
  std::string filename;
  getline(std::cin, filename);
  int percent;

  while (filename != "") {
    percent = getPercentageFromImage(filename);
    std::cout << filename << ", " << percent << std::endl;
    getline(std::cin, filename);
  }

  return 0;
}

