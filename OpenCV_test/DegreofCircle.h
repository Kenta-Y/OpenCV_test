//
// Created by KentaYamagishi on 2016/07/15.
//

#ifndef OPENCV_TEST_DEGREOFCIRCLE_H
#define OPENCV_TEST_DEGREOFCIRCLE_H

#include <opencv2/opencv.hpp>
#include <stdio.h>

cv::Point2i RasterScan(cv::Mat src_im, int target_val);
double Ask_Perimeter(cv::Mat tmp, int i, cv::Point2i start_pos, int mode, cv::Mat &src_im = *(cv::Mat *)nullptr);
void Paint2label(cv::Mat src_im, cv::Mat &out_im, int nlabels);
cv::Point2i Ask_DegreofCircle(cv::Mat src_im, cv::Mat &draw_im = *(cv::Mat *)nullptr, cv::Mat &paint_im = *(cv::Mat *)nullptr);

#endif //OPENCV_TEST_DEGREOFCIRCLE_H
