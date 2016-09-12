#include <iostream>
#include <opencv2/opencv.hpp>

#include "ImgProcessing.h"
#include "DegreofCircle.h"

cv::Mat org;

int main(int argc, const char * argv[]) {

    //ウィンドウ生成
    cv::namedWindow("out_img", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("out_img2", CV_WINDOW_AUTOSIZE);

    //画像入力
    cv::Mat input1 = cv::imread("/Users/KentaYamagishi/Pictures/a.jpg");
    cv::Mat input2 = cv::imread("/Users/KentaYamagishi/Pictures/b.jpg");

    //コンストラクト
    myCV *mycv = new myCV(input1);
    myCV *mycv2 = new myCV(input2);

    while(cv::waitKey(15) == -1) {      //キー入力があるまでループ
        //myCVの出力
        cv::Mat dst = mycv->getImg();
        cv::Mat dst2 = mycv2->getImg();

        //描画
        cv::imshow("out_img", dst);
        cv::imshow("out_img2", dst2);
    }

    //デストラクト
    delete mycv;
    delete mycv2;

    return 0;
}



