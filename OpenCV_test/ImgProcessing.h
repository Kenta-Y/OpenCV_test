//
// Created by KentaYamagishi on 2016/07/15.
//

#ifndef OPENCV_TEST_IMGPROCESSING_H
#define OPENCV_TEST_IMGPROCESSING_H

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <memory>

#include "DegreofCircle.h"

double Square(double a);
void Mat_cheaker(cv::Mat src_im);
void draw_Histogram(const cv::Mat &src_im, cv::Mat &hist_im);

void Integral_Image(cv::Mat src_img, cv::Mat &out_img);
void Ask_correlation(cv::Mat src_img, cv::Mat src_img2);
int Ask_Minimum_rad(cv::Point center, cv::Point img_size);
std::vector<cv::Point> Ask_Circle(cv::Mat &src_im, cv::Point center, cv::Mat &draw_im = *(cv::Mat *)nullptr);

class myCV{
private:
    static int cnt;             //インスタンス生成した回数
    const cv::Mat src_im;       //入力画像

    int thread_val;     //2値化の閾値
    int resize_val;     //画像リサイズの閾値[%]

    //各処理のON/OFFフラグ
    bool glayscale_flg;
    bool threshold_flg;
    bool edge_flg;
    bool negaposi_flg;
    bool flat_flg;
    bool integral_flg;

    struct Identifler {      //ボタンと実行時のオブジェクト(this)ポインタを格納するデータ
        Identifler(int arg, void *p) {
            ptr = p;
            no = arg;
        }
        void *ptr;
        int no;
    };
    std::vector<std::shared_ptr<Identifler>> tp;

    static void callbackButton(int state,void *ptr);
    void create_toolbar(int val);

public:
    myCV(cv::Mat _src_im);
    cv::Mat getImg();
    bool check_thread(){
        return glayscale_flg;
    }
    int get_thread(){
        return thread_val;
    }
};

////////Hist

class atsHistogram
{
public:
    cv::Mat DrawHistogram(cv::Mat src)
    {
        /// Separate the image in 3 places ( R, G and B )
        std::vector<cv::Mat> rgb_planes;
        cv::split( src, rgb_planes );

        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for R,G,B) )
        float range[] = { 0, 255 } ;
        const float* histRange = { range };

        bool uniform = true; bool accumulate = false;

        cv::Mat r_hist, g_hist, b_hist;

        /// Compute the histograms:
        calcHist( &rgb_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &rgb_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &rgb_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

        // Draw the histograms for R, G and B
        int hist_w = 768;
        int hist_h = 400;
        //int bin_w = cvRound( (double) hist_w/histSize );
        int bin_w = 3;

        cv::Mat histImage( hist_h+100, hist_w+10, CV_8UC3, cv::Scalar( 0,0,0) );

        /// Normalize the result to [ 0, histImage.rows ]
//        normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
//        normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
//        normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
        normalize(r_hist, r_hist, 0, hist_h, cv::NORM_MINMAX, -1, cv::Mat() );
        normalize(g_hist, g_hist, 0, hist_h, cv::NORM_MINMAX, -1, cv::Mat() );
        normalize(b_hist, b_hist, 0, hist_h, cv::NORM_MINMAX, -1, cv::Mat() );

        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1))+100 ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))+100 ),
                  cv::Scalar( 0, 0, 255), 2, 8, 0  );
            line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1))+100 ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))+100 ),
                  cv::Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1))+100 ) ,
                  cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))+100 ),
                  cv::Scalar( 255, 0, 0), 2, 8, 0  );
        }
        return histImage;
    }
private:
};

#endif //OPENCV_TEST_IMGPROCESSING_H

