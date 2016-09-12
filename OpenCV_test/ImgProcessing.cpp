//
// Created by KentaYamagishi on 2016/07/15.
//

#include "ImgProcessing.h"
#include "DegreofCircle.h"
#include "opencv-histlib/src/histLib.h"
#include "histLib.h"

extern cv::Mat org;
int myCV::cnt;

double Square(double a){    //  二乗の計算
    return a * a;
}

void draw_Histogram(const cv::Mat &src_im, cv::Mat &hist_im){
    //use filitchp made libraly
    //github : "https://github.com/filitchp/opencv-histlib"
    //
    //引数 -> 入力画像，ヒストグラム用のMat型

    cv::Mat dst;
    if(src_im.channels() == 1) cv::cvtColor(src_im, dst, CV_GRAY2BGR);
    else src_im.copyTo(dst);

    CHistLib *Histogram = new CHistLib;
    cv::MatND HistB, HistG, HistR;

    Histogram->ComputeHistogramBGR(dst, HistB, HistG, HistR);
    Histogram->DrawHistogramBGR(HistB, HistG, HistR, hist_im);

    delete Histogram;
}

void Mat_cheaker(cv::Mat src_im){
    std::cout << "rows: "   << src_im.rows << std::endl;
    std::cout << "cols: "   << src_im.cols << std::endl;
    std::cout << "chanel: " << src_im.channels() << std::endl;
    std::cout << "step: "   << src_im.step1() << std::endl;
    std::cout << "dims: "   << src_im.dims << std::endl;
    std::cout << "elemesize: " << src_im.elemSize1() << std::endl;
}

void Integral_Image(cv::Mat src_img, cv::Mat &out_img){   //画像の積分？
    cv::Mat tmpim;
    src_img.copyTo(tmpim);

    int height = (int)src_img.step1(), width = src_img.channels();

    for(int j = 0; j < src_img.rows; j++){               //縦
        for(int i = 0; i < src_img.cols; i++){           //横
            for(int k = 0; k < src_img.channels(); k++){ //チャンネル数

                int present_pos = j * (int)src_img.step1() + i * src_img.channels() + k;

//                if(j == 0)
//                    tmpim.data[present_pos] += src_img.data[present_pos + height];
//                else if(j == src_img.rows -1)
//                    tmpim.data[present_pos] += src_img.data[present_pos - height];
//                else
//                    tmpim.data[present_pos] += src_img.data[present_pos + height] + src_img.data[present_pos - height];

                if(i == 0)
                    tmpim.data[present_pos] += src_img.data[present_pos + width];
                else if(i == src_img.cols - 1)
                    tmpim.data[present_pos] += src_img.data[present_pos - width];
                else
                    tmpim.data[present_pos] += src_img.data[present_pos + width] + src_img.data[present_pos - width];
            }
        }
    }

    tmpim.copyTo(out_img);  //出力画像
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

int Ask_Minimum_rad(cv::Point center, cv::Point img_size){
    int minimum_rad = img_size.x;
    if(center.x < minimum_rad) minimum_rad = center.x;
    if(center.y < minimum_rad) minimum_rad = center.y;
    if(img_size.x - center.x < minimum_rad) minimum_rad = img_size.x - center.x;
    if(img_size.y - center.y < minimum_rad) minimum_rad = img_size.y - center.y;

    return minimum_rad;
}

std::vector<cv::Point> Ask_Circle(cv::Mat &src_im, cv::Point center, cv::Mat &draw_im){
    //src_im    ->  入力画像(2値価画像
    //center    ->  求める円の中心
    //draw_im   ->  描画用の画像

    // ラベリング
    cv::Mat label, state, centroids;
    int nLabels = cv::connectedComponentsWithStats(~src_im, label, state, centroids, 8);

    // 中心座標を含んだ外周を求める
    cv::Mat dst = cv::Mat::zeros(src_im.rows, src_im.cols, CV_8UC1);

    int a = 0;

    for(int i = 1; i < nLabels; i++){
        int l = state.at<int>(i, cv::CC_STAT_LEFT);
        int r = l + state.at<int>(i, cv::CC_STAT_WIDTH);
        int t = state.at<int>(i, cv::CC_STAT_TOP);
        int u = t + state.at<int>(i, cv::CC_STAT_HEIGHT);

        if(l < center.x && center.x < r && t < center.y && center.y < u){
            a++;
            if(a > 0) {
                cv::Point point = RasterScan(label, i);
                Ask_Perimeter(label, i, point, 2, dst);   //周囲辺を描画
                break;
            }
        }
    }

    cv::namedWindow("testdao", CV_WINDOW_AUTOSIZE);
    cv::imshow("testdao", org);

    // スターバースト(もどき)
    std::vector<cv::Point> iris_age;
    int min_rad = Ask_Minimum_rad(center, cv::Point(src_im.cols, src_im.rows));

    for(int theta = -50; theta <= 50; theta++){
        bool l_flg = false, r_flg = false;
        int  l_rad = 0, r_rad = 0;

        for(int rad = 1; rad < min_rad; rad++){
            int x = (int)(center.x + rad * cos(theta * (CV_PI/180)));
            int y = (int)(center.y + rad * sin(theta * (CV_PI/180)));
            int x2 = (int)(center.x - rad * cos(theta * (CV_PI/180)));
            int y2 = (int)(center.y - rad * sin(theta * (CV_PI/180)));

            if(dst.data[ y * dst.step + x * dst.elemSize()] == 200 && !r_flg){
                r_rad = rad;
                r_flg = true;
            }

            if(dst.data[ y2 * dst.step + x2 * dst.elemSize()] == 200 && !l_flg){
                l_rad = rad;
                l_flg = true;
            }

            if(r_flg && l_flg){
                //if(abs(l_rad-r_rad) < 15){
                if(r_rad > 115 && l_rad > 115){
                    x = (int)(center.x + r_rad * cos(theta * (CV_PI/180)));
                    y = (int)(center.y + r_rad * sin(theta * (CV_PI/180)));
                    x2 = (int)(center.x - l_rad * cos(theta * (CV_PI/180)));
                    y2 = (int)(center.y - l_rad * sin(theta * (CV_PI/180)));
                    iris_age.push_back(cv::Point(x,y));
                    iris_age.push_back(cv::Point(x2,y2));
                    if(&draw_im != (cv::Mat *)nullptr){
                        //cv::line(draw_im, center, cv::Point(x,y), CV_RGB(200, 0, 0));
                        //cv::line(draw_im, center, cv::Point(x2,y2), CV_RGB(200, 0, 0));
                    }
                //std::cout << r_rad << ", " << l_rad << std::endl;
                }
                break;
            }
        }
    }

    //cv::RotatedRect box = cv::fitEllipse(iris_age);
    //cv::ellipse(org, box,cv::Scalar(255,0,0),2,8);

    std::vector<cv::Point> circle_element;

    if ((int)iris_age.size() < 2) return circle_element;

    // RANSACを用いて最適解の選定
    int all_element = (int)iris_age.size();
    int sample_element = 4;
    int repeat = 100;

    int best_rad = -1;
    int min_error = 10000000;

    for(int i = 0; i < repeat; i++){
        // サンプル作成
        int sample_rad = 0;
        for(int j = 0; j < sample_element; j++){
            int sample_num = rand()%all_element;
            sample_rad += sqrt(Square(center.x-iris_age[sample_num].x) + Square(center.y-iris_age[sample_num].y));
        }

        // サンプルの平均半径を算出
        sample_rad /= sample_element;

        // 実際のデータとサンプルを比較
        int total_error = 0;
        for(int k = 0; k < all_element; k++){
            int element_rad = (int)sqrt(Square(center.x-iris_age[k].x) + Square(center.y-iris_age[k].y));
            total_error += abs(element_rad - sample_rad);
        }

        // 誤差が今までで最小なら保持
        if(total_error < min_error){
            min_error = total_error;
            best_rad = sample_rad;
        }
    }

    // 円の点群(72点)を取得
    for(int theta = 0; theta < 359; theta += 5){
        int x = (int)(center.x + best_rad * cos(theta * (CV_PI/180)));
        int y = (int)(center.y + best_rad * sin(theta * (CV_PI/180)));
        circle_element.push_back(cv::Point(x,y));
    }

    // 円の描画
    if(best_rad > 0 && &draw_im != (cv::Mat *)nullptr){
        //cv::circle(draw_im, center, best_rad, CV_RGB(0,200,0));
    }

    return circle_element;
}

myCV::myCV(cv::Mat _src_im) : src_im(_src_im){
        //インスタンス作成数のカウント
        cnt++;

        //変数init
        glayscale_flg = false;
        threshold_flg = false;
        edge_flg      = false;
        negaposi_flg  = false;
        flat_flg      = false;
        integral_flg  = false;

        thread_val = 255;
        resize_val = 100;

        //OpenCVのコントロールパネル生成
        create_toolbar(cnt);
}

void myCV::callbackButton(int state,void *ptr){
    Identifler* data = reinterpret_cast<Identifler*>(ptr);       //void*にぶち込んだ構造体のパス
    myCV* this_p = reinterpret_cast<myCV*>(data->ptr); //コールバック登録したクラスのパス

    if     (data->no == 0) this_p->glayscale_flg = !this_p->glayscale_flg;
    else if(data->no == 1) this_p->threshold_flg = !this_p->threshold_flg;
    else if(data->no == 2) this_p->edge_flg      = !this_p->edge_flg;
    else if(data->no == 3) this_p->negaposi_flg  = !this_p->negaposi_flg;
    else if(data->no == 4) this_p->flat_flg      = !this_p->flat_flg;
    else if(data->no == 5) this_p->integral_flg  = !this_p->integral_flg;
}

void myCV::create_toolbar(int val){
    //チェックボックス
    for(int i = 0;i < 8;i++)
        tp.push_back(std::shared_ptr<Identifler>(new Identifler(i, this)));

    cv::createButton("GrayScalle  ",     callbackButton, tp[0].get(), CV_CHECKBOX, 0);
    cv::createButton("Binarization  ",   callbackButton, tp[1].get(), CV_CHECKBOX, 0);
    cv::createButton("NegaPosiRevers  ", callbackButton, tp[3].get(), CV_CHECKBOX, 0);
    cv::createButton("flat  ",           callbackButton, tp[4].get(), CV_CHECKBOX, 0);
    cv::createButton("Integral  ",       callbackButton, tp[5].get(), CV_CHECKBOX, 0);
    cv::createButton("Edge(Canny)  ",    callbackButton, tp[2].get(), CV_CHECKBOX, 0);

    //スライダー
    std::string str = "binary" + std::to_string(val);
    cv::createTrackbar(str, "", &thread_val, 255);

    str = "resize" + std::to_string(val);
    cv::createTrackbar(str, "", &resize_val, 300);

    //str = "value" + std::to_string(val);
    //cv::createTrackbar(str, "", &value, 255);
}

cv::Mat myCV::getImg(){
    cv::Mat dst;
    src_im.copyTo(dst);

    if (negaposi_flg)   //ネガポジ反転
        dst = ~dst;
    if (glayscale_flg) {    //グレースケールに変換
        cv::Mat gray;
        dst.copyTo(gray);
        cv::cvtColor(gray, dst, CV_RGB2GRAY);
    }
    if (threshold_flg) {    //画像の2値化
        cv::Mat binary;
        dst.copyTo(binary);
        cv::threshold(binary, dst, thread_val, 255, cv::THRESH_BINARY);
    }
    if (edge_flg) {         //エッジ検出
        cv::Mat canny;
        dst.copyTo(canny);
        Canny(canny, dst, 1, 240);
    }
    if (flat_flg && glayscale_flg) {    //ヒストグラムの平滑化
        cv::Mat flat;
        dst.copyTo(flat);
        cv::equalizeHist(flat, dst);
    }
    if (integral_flg && glayscale_flg) {
        cv::Mat integral;
        dst.copyTo(integral);
        Integral_Image(integral, dst);
    }
    if (resize_val != 100) {
        if (resize_val == 0) resize_val = 1;
        cv::Mat resize;
        dst.copyTo(resize);
        cv::resize(resize, dst, cv::Size(), (double) resize_val / 100, (double) resize_val / 100, CV_INTER_LINEAR);   //リサイズする倍率を指定
    }

    return dst;
}