//
//  main.cpp
//  exp_test
//
//  Created by KentaYamagishi on 2015/12/17.
//  Copyright © 2015年 KentaYamagishi. All rights reserved.
//

#include <iostream>
#include <opencv.hpp>
#include <core.hpp>
#include <highgui.hpp>

#define mov
//#define edge

cv::Mat ImgProcessing(cv::Mat, int*);
cv::Mat ImgProcessing2(cv::Mat, cv::Mat, int*);

int test = 0;

int main(int argc, const char * argv[]) {
    
    //////////////////////////////////*
    //  変数
    //////////////////////////////////*/
    int val = 255, val2 = 255;
    cv::Mat output, output2;
    
    
    
    //////////////////////////////////*
    //  入力画像
    //////////////////////////////////*/
    cv::Mat rgb_img = cv::imread("/Users/KentaYamagishi/Projects/DATA/test21.jpg");         //RGB画像を読み込む
    cv::Mat ir_img = cv::imread("/Users/KentaYamagishi/Projects/DATA/test22.jpg");          //IR画像読み込み
    
    cv::VideoCapture rgb_mov("/Users/KentaYamagishi/Projects/DATA/test21.mov");               //RGB動画読み込み
    cv::VideoCapture ir_mov("/Users/KentaYamagishi/Projects/DATA/test22.mov");               //IR動画読み込み
    
    
    //////////////////////////////////*
    //  画像処理
    //////////////////////////////////*/
    int COM1[] = {1, 0};
    output = ImgProcessing(rgb_img, COM1);
    output2 = ImgProcessing(ir_img, COM1);
    
    //int COM2[] = {2, 0};
    //ImgProcessing2(output, output2, COM2);
    
    
    
    //////////////////////////////////*
    //  表示(GUI)
    //////////////////////////////////*/
    cv::namedWindow("out_img", CV_WINDOW_AUTOSIZE);
    cv::createTrackbar("Value1", "out_img", &val, 255);
    cv::createTrackbar("Value2", "out_img", &val2, 255);
    
    cv::namedWindow("mov");
    
    
    
    //////////////////////////////////*
    //  無限ループ(トラックバーによる変更を可視化する為    ->  キー入力で終了
    //////////////////////////////////*/
    
    while(cv::waitKey(15) == -1){   //キー入力があるまでループ
        //cv::threshold(diff, dst, th, 255, cv::THRESH_TOZERO_INV);//二値化画像化
        //cv::threshold(gray_img, dst, th, 255, cv::THRESH_BINARY);//二値化画像化
        //diff = ir_src;
        
        //cv::imshow("Inputimg", output);
        
        //cv::Mat canny_img;
        //Canny(dst, canny_img, 1, 240);
        //cv::imshow("Canny", canny_img);
        
#ifdef edge
        cv::Mat canny_img, dst_img;
        cv::threshold(output, dst_img, val, 255, cv::THRESH_BINARY);//二値化画像化
        Canny(dst_img, canny_img, 1, 240);
        cv::imshow("out_img", canny_img);
#endif
        
        
#ifdef mov
        cv::Mat frame, frame2, frame3, frame4, frame5;
        ir_mov >> frame;
        if(frame.empty() || cv::waitKey(30) >= 0 || ir_mov.get(CV_CAP_PROP_POS_AVI_RATIO) == 1){
            break;
        }
        cv::cvtColor(frame, frame4, CV_RGB2GRAY); // グレースケールに変換する
        cv::threshold(frame4, frame2, 20, 255, cv::THRESH_BINARY);//二値化画像化
        int COM3[] = {7, 5, 0};
        frame3 = ImgProcessing(frame2, COM3);
        
        rgb_mov >> frame5;
        //frame3.data[test*3] = 0;
        //frame3.data[test*3+1] = 255;
        //frame3.data[test*3+2] = 255;
        for(int j = 0; j < frame3.rows; ++j){
            for(int i = 0; i < frame3.cols; ++i){
                if( (int)frame3.data[j * frame3.step + i * frame3.elemSize()] == 255){
                    frame5.data[(j * frame3.step + i * frame3.elemSize())*3] = 0;
                    frame5.data[(j * frame3.step + i * frame3.elemSize())*3+1] = 0;
                    frame5.data[(j * frame3.step + i * frame3.elemSize())*3+2] = 255;
                }
            }
        }
        cv::imshow("mov", frame5);
#endif
        
    }
    
    return 0;
}






cv::Mat ImgProcessing(cv::Mat src_img, int *command){
    cv::Mat output_img, tmp_img = src_img;
    
    for(int i = 0; command[i] != 0; i++){
        switch (command[i]) {
            case 1: //グレースケール
            {
                cv::Mat gray_img;
                cv::cvtColor(tmp_img, gray_img, CV_RGB2GRAY); // グレースケールに変換する
                tmp_img = gray_img;
            }
                break;
                
            case 3: //2値化
            {
                int th = 40;
                cv::Mat dst;
                cv::threshold(tmp_img, dst, th, 255, cv::THRESH_BINARY);//二値化画像化
                tmp_img = dst;
            }
                break;
                
            case 4: //ヒストグラムの平滑化
            {
                cv::Mat flat_img;
                cv::equalizeHist(tmp_img, flat_img);  //ヒストグラムの平滑化
                tmp_img = flat_img;
            }
                break;
                
            case 5: //エッジ検出 (Canny
            {
                cv::Mat canny_img;
                Canny(tmp_img, canny_img, 1, 240);
                tmp_img = canny_img;
            }
                break;
                
            case 6: //エッジ検出(Laplacian
            {
                cv::Mat lap_img;
                cv::Laplacian(tmp_img, lap_img, CV_32F, 3);
                cv::convertScaleAbs(lap_img, tmp_img, 2,50);
                //tmp_img = lap_img;
            }
                break;
                
            case 7: //重心から中心を求める
            {
                int tmp = 0, sx = 0, sy = 0;
                int X_center = 0, Y_center = 0;
                src_img = tmp_img;
                 
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
                    tmp_img.data[src_img.cols * Y_center + X_center] = 127;    //重心を白にする
                    test = src_img.cols * Y_center + X_center;
                }
                
                std::cout << X_center << "，" << Y_center <<std::endl;     //重心座標
                std::cout << src_img.cols * Y_center + X_center << std::endl;
                
                //MEMO
                //rows  ->  縦のピクセル
                //cols  ->  横のピクセル
            }
                break;
                
            default:
                std::cout << "Error";
                break;
        }
    }
    return tmp_img;
}

cv::Mat ImgProcessing2(cv::Mat src_img, cv::Mat src_img2, int *command){
    cv::Mat output_img, tmp_img = src_img, tmp_img2 = src_img2, den;
    
    for(int i = 0; command[i] != 0; i++){
        switch (command[i]) {
            case 1: //背景差分
            {
                cv::Mat diff;
                cv::absdiff(tmp_img, tmp_img2, diff);
                output_img = diff;
            }
                break;
                
            case 2: //類似度
            {
                double T_bar = 0.0, I_bar = 0.0, I_vec = 0.0, T_vec = 0.0, den, Rzncc;
                
                //領域内の平均値
                for(int j = 0; j < tmp_img.rows; ++j){
                    for(int i = 0; i < tmp_img.cols; ++i){
                        I_bar += (int)src_img.data[ j * src_img.step + i * src_img.elemSize()];
                        T_bar += (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()];
                    }
                }
                
                I_bar = I_bar / (src_img.rows * src_img.cols);
                T_bar = T_bar / (src_img2.rows * src_img2.cols);

                //分母
                for(int j = 0; j < tmp_img.rows; ++j){
                    for(int i = 0; i < tmp_img.cols; ++i){
                        I_vec += ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar ) * ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar );
                        T_vec += ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar ) * ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar );
                    }
                }
                
                den = sqrt(I_vec * T_vec);
                
                //各ピクセル毎に計算
                for(int j = 0; j < tmp_img.rows; ++j){
                    for(int i = 0; i < tmp_img.cols; ++i){
                        Rzncc = ( ( (int)src_img.data[ j * src_img.step + i * src_img.elemSize()] - I_bar ) * ( (int)src_img2.data[ j * src_img2.step + i * src_img2.elemSize()] - T_bar ) ) / den;
                        std::cout << Rzncc << ",";
                    }
                    std::cout << std::endl;
                }
                
            }
                break;
                
            default:
                std::cout << "Error";
                break;
        }
    }
    return output_img;
}
