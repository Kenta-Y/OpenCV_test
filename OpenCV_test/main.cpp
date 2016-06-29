//
//  main.cpp
//  exp_test
//
//  Created by KentaYamagishi on 2015/12/17.
//  Copyright © 2015年 KentaYamagishi. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

bool glayscale_flg = false;
bool threshold_flg = false;
bool edge_flg = false;
bool negaposi_flg = false;
bool flat_flg = false;

void callbackButton(int state,void *num){
    int tmp = *(int *)num;  //void*(汎用ポインタ)からint*(int型のポインタ？)に変換
    
    if(tmp == 0)
        glayscale_flg = !glayscale_flg;
    else if(tmp == 1)
        threshold_flg = !threshold_flg;
    else if(tmp == 2)
        edge_flg = !edge_flg;
    else if(tmp == 3)
        negaposi_flg = !negaposi_flg;
    else if(tmp == 4)
        flat_flg = !flat_flg;
}

int main(int argc, const char * argv[]) {
    
    int thread_val = 255;
    int resize_val = 100;
    int t[] = {0,1,2,3,4,5};
    
    cv::Mat input_img = cv::imread("/Users/KentaYamagishi/Pictures/a.jpg");
    
    cv::namedWindow("out_img", CV_WINDOW_AUTOSIZE);
    
    cv::createButton("GrayScalle",callbackButton,&t[0],CV_CHECKBOX,0);
    cv::createButton("Binarization",callbackButton,&t[1],CV_CHECKBOX,0);
    cv::createButton("NegaPosiRevers",callbackButton,&t[3],CV_CHECKBOX,0);
    cv::createButton("flat",callbackButton,&t[4],CV_CHECKBOX,0);
    cv::createButton("Edge(Canny)",callbackButton,&t[2],CV_CHECKBOX,0);
    cv::createTrackbar("Binarization", "", &thread_val, 255);
    cv::createTrackbar("resize [%]", "", &resize_val, 300);
    
    while(cv::waitKey(15) == -1){   //キー入力があるまでループ
        cv::Mat tmp;
        input_img.copyTo(tmp);

        if(negaposi_flg)
            tmp = ~tmp;
        if(glayscale_flg){
            cv::Mat gray;
            tmp.copyTo(gray);
            cv::cvtColor(gray, tmp, CV_RGB2GRAY); // グレースケールに変換する
        }
        if(threshold_flg){
            cv::Mat dst;
            tmp.copyTo(dst);
            cv::threshold(dst, tmp, thread_val, 255, cv::THRESH_BINARY);//二値化画像化
        }
        if(edge_flg){
            cv::Mat canny;
            tmp.copyTo(canny);
            Canny(canny, tmp, 1, 240);
        }
        if(flat_flg && glayscale_flg){
            cv::Mat flat;
            tmp.copyTo(flat);
            cv::equalizeHist(flat, tmp);  //ヒストグラムの平滑化
        }
        if(resize_val != 100){
            if(resize_val == 0) resize_val = 1;
            cv::Mat resize;
            tmp.copyTo(resize);
            cv::resize(resize, tmp, cv::Size(), (double)resize_val/100, (double)resize_val/100, CV_INTER_LINEAR);   //リサイズする倍率を指定
        }
        
        cv::imshow("out_img", tmp);
    }
    
    return 0;
}


void Ask_CenterofGravity(cv::Mat src_img){  //重心を求める
    int tmp = 0, sx = 0, sy = 0;
    int X_center = 0, Y_center = 0;
    
    for(int j = 0; j < src_img.rows; ++j){
        for(int i = 0; i < src_img.cols; ++i){
            if( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] != 255){
                tmp++;
                sx += i;
                sy += j;
            }
        }
    }
    
    X_center = (double)sx / tmp;
    Y_center = (double)sy / tmp;
    
    if(X_center > 0 && Y_center > 0){
        src_img.data[src_img.cols * Y_center + X_center] = 127;    //重心を白にする
    }
    
    std::cout << X_center << "，" << Y_center <<std::endl;     //重心座標
    std::cout << src_img.cols * Y_center + X_center << std::endl;
    
    //MEMO
    //rows  ->  縦のピクセル
    //cols  ->  横のピクセル
}

void Ask_correlation(cv::Mat src_img, cv::Mat src_img2){ //類似度を求める
    double T_bar = 0.0, I_bar = 0.0, I_vec = 0.0, T_vec = 0.0, den, Rzncc;
    
    //領域内の平均値
    for(int j = 0; j < src_img.rows; ++j){
        for(int i = 0; i < src_img.cols; ++i){
            I_bar += (int)src_img.data[ j * src_img.step + i * src_img.elemSize()];
            T_bar += (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()];
        }
    }
    
    I_bar = I_bar / (src_img.rows * src_img.cols);
    T_bar = T_bar / (src_img2.rows * src_img2.cols);
    
    //分母
    for(int j = 0; j < src_img.rows; ++j){
        for(int i = 0; i < src_img.cols; ++i){
            I_vec += ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar ) * ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar );
            T_vec += ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar ) * ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar );
        }
    }
    
    den = sqrt(I_vec * T_vec);
    
    //各ピクセル毎に計算
    for(int j = 0; j < src_img.rows; ++j){
        for(int i = 0; i < src_img.cols; ++i){
            Rzncc = ( ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar ) * ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar ) ) / den;
            std::cout << Rzncc << ",";
        }
        std::cout << std::endl;
    }
}
