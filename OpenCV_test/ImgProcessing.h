//
// Created by KentaYamagishi on 2016/07/15.
//

#ifndef OPENCV_TEST_IMGPROCESSING_H
#define OPENCV_TEST_IMGPROCESSING_H

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

double Square(double a);
void Mat_cheaker(cv::Mat src_im);

void Integral_Image(cv::Mat src_img, cv::Mat &out_img);
void Ask_CenterofGravity(cv::Mat src_img);
void Ask_CenterofGravity_kai(cv::Mat src_img);
void Ask_correlation(cv::Mat src_img, cv::Mat src_img2);

int Ask_Minimum_rad(cv::Point center, cv::Point img_size);
std::vector<cv::Point> Ask_Circle(cv::Mat &src_im, cv::Point center, cv::Mat &draw_im = *(cv::Mat *)nullptr);

#endif //OPENCV_TEST_IMGPROCESSING_H
