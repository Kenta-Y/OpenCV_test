#include <iostream>
#include <opencv2/opencv.hpp>

#include "ImgProcessing.h"
#include "DegreofCircle.h"

using namespace cv;

bool glayscale_flg = false;
bool threshold_flg = false;
bool edge_flg = false;
bool negaposi_flg = false;
bool flat_flg = false;
bool integral_flg = false;
bool DoC_flg = false;
bool fast_flg = false;
bool test_flg = false;

void callbackButton(int state,void *num);
void open_gnuplot();
void exit_gnuplot();
void draw_histgram(const cv::Mat& src_im);

cv::Mat org;
bool gnuplot_flg = false;
int value = 100, old_value = 0;
FILE *gp;

int main(int argc, const char * argv[]) {

    int thread_val = 255, resize_val = 100;
    int t[] = {0,1,2,3,4,5,6,7};

    cv::Mat input_img = cv::imread("/Users/KentaYamagishi/Desktop/a.jpg");

    cv::namedWindow("out_img", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("out_img2", CV_WINDOW_AUTOSIZE);
    cv::createButton("GrayScalle  ",callbackButton,&t[0],CV_CHECKBOX,0);
    cv::createButton("Binarization  ",callbackButton,&t[1],CV_CHECKBOX,0);
    cv::createButton("NegaPosiRevers  ",callbackButton,&t[3],CV_CHECKBOX,0);
    cv::createButton("flat  ",callbackButton,&t[4],CV_CHECKBOX,0);
    cv::createButton("Integral  ",callbackButton,&t[5],CV_CHECKBOX,0);
    cv::createButton("DoC  ",callbackButton,&t[6],CV_CHECKBOX,0);
    cv::createButton("fast  ",callbackButton,&t[7],CV_CHECKBOX,0);
    cv::createButton("Edge(Canny)  ",callbackButton,&t[2],CV_CHECKBOX,0);
    cv::createTrackbar("Binarization", "", &thread_val, 255);
    cv::createTrackbar("resize [%]", "", &resize_val, 300);
    cv::createTrackbar("value", "", &value, 255);

    while(cv::waitKey(15) == -1) {   //キー入力があるまでループ
        cv::Mat tmp;
        input_img.copyto(tmp);

        if (negaposi_flg)
            tmp = ~tmp;
        if (glayscale_flg) {
            cv::Mat gray;
            tmp.copyTo(gray);
            cv::cvtColor(gray, tmp, CV_RGB2GRAY); // グレースケールに変換する
            //draw_histgram(input_img);
        }
        if (threshold_flg) {
            cv::Mat dst;
            tmp.copyTo(dst);
            cv::threshold(dst, tmp, thread_val, 255, cv::THRESH_BINARY);//二値化画像化
            cv::Mat testes;
            cv::erode(tmp, testes, cv::Mat());
            tmp = testes;
        }
        if (edge_flg) {
            cv::Mat canny;
            tmp.copyTo(canny);
            Canny(canny, tmp, 1, 240);
        }
        if (flat_flg && glayscale_flg) {
            cv::Mat flat;
            tmp.copyTo(flat);
            cv::equalizeHist(flat, tmp);  //ヒストグラムの平滑化
        }
        if (integral_flg && glayscale_flg) {
            cv::Mat integral;
            tmp.copyTo(integral);
            Integral_Image(integral, tmp);
        }
        if (resize_val != 100) {
            if (resize_val == 0) resize_val = 1;
            cv::Mat resize;
            tmp.copyTo(resize);
            cv::resize(resize, tmp, cv::Size(), (double) resize_val / 100, (double) resize_val / 100,
                       CV_INTER_LINEAR);   //リサイズする倍率を指定
        }

        if (fast_flg) {
            std::vector<cv::KeyPoint> fast_keypoints;
            cv::FAST(tmp, fast_keypoints, value);
            for (int i = 0; i < (int) fast_keypoints.size(); i++)
                cv::circle(org, fast_keypoints[i].pt, (int) fast_keypoints[i].size / 2, CV_RGB(0, 200, 200));
        }

        cv::Point Center_Px;

        if (DoC_flg && glayscale_flg && threshold_flg) {
            cv::namedWindow("org_img", CV_WINDOW_AUTOSIZE);
            input_img.copyTo(org);
            cv::Mat labeling;
            Center_Px = Ask_DegreofCircle(tmp, org, labeling);   // 円形度の高いものを摘出
            labeling.copyTo(tmp);
            cv::imshow("org_img", labeling);
            //std::cout << Center_Px << std::endl;
        }

        //cx = 555, cy = 460

        if (threshold_flg && test_flg) {
            std::vector<cv::Point> circle_element = Ask_Circle(tmp, cv::Point(555, 460), org);
//            for(int i = 0; i < circle_element.size(); i++){
//                cv::circle(org, circle_element[i], 4, CV_RGB(0,200,200));
//            }
        }

        cv::imshow("out_img", tmp);
        cv::imshow("out_img2", org);
        //cv::imshow("out_img", tmp);
    }

    return 0;
}




void open_gnuplot(){
    gp = popen("gnuplot","w");
    fprintf(gp, "set terminal aqua\n");
    fprintf(gp, "set xrange[0:255]\n");
}

void exit_gnuplot(){
    fflush(gp); // バッファに格納されているデータを吐き出す（必須）
    fprintf(gp, "exit\n"); // gnuplotの終了
    //fprintf(gp, "pkill -KILL -f AquaTerm\n"); // AquaTermの終了
    pclose(gp);
}

void draw_histgram(const cv::Mat& src_im){
    static cv::Mat old_im;

    if(!gnuplot_flg){
        open_gnuplot();
        atexit(exit_gnuplot);
        gnuplot_flg = true;
    }

    // (2)allocate Mat to draw a histogram image
    //const int ch_width = 260;
    const int sch = src_im.channels();
    //Mat hist_img(Size(ch_width * sch, 200), CV_8UC3, Scalar::all(255));

    std::vector<MatND> hist(3);
    const int hist_size = 256;
    const int hdims[] = {hist_size};
    const float hranges[] = {0, 256};
    const float *ranges[] = {hranges};
    double max_val = .0;

    if (sch == 1) {
        // (3a)if the source image has single-channel, calculate its histogram
        calcHist(&src_im, 1, 0, Mat(), hist[0], 1, hdims, ranges, true, false);
        minMaxLoc(hist[0], 0, &max_val);
    } else {
        // (3b)if the souce image has multi-channel, calculate histogram of each plane
        for (int i = 0; i < sch; ++i) {
            calcHist(&src_im, 1, &i, Mat(), hist[i], 1, hdims, ranges, true, false);
            double tmp_val;
            minMaxLoc(hist[i], 0, &tmp_val);
            max_val = max_val < tmp_val ? tmp_val : max_val;
        }
    }

    if(value != old_value) {
        fprintf(gp, "plot '-' with lines lc 'blue', '-' with lines lc 'green', '-' with lines lc 'red' linetype 1\n");

        for (int i = 0; i < 256; ++i)
            fprintf(gp, "%d\n", hist[0].at<int>(i));
        fprintf(gp, "e\n");

        for (int i = 0; i < 256; ++i)
            fprintf(gp, "%d\n", hist[1].at<int>(i));
        fprintf(gp, "e\n");

        for (int i = 0; i < 256; ++i)
            fprintf(gp, "%d\n", hist[2].at<int>(i));
        fprintf(gp, "e\n");

        old_value = value;
    }
}


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
    else if(tmp == 5)
        integral_flg = !integral_flg;
    else if(tmp == 6)
        DoC_flg = !DoC_flg;
    else if(tmp == 7)
        fast_flg = !fast_flg;
}