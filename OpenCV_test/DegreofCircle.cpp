//
// Created by KentaYamagishi on 2016/07/15.
//

#include "DegreofCircle.h"

cv::Point2i RasterScan(cv::Mat src_im, int target_val){
    for(int y = 0 ;y < src_im.rows; y++){
        for(int x = 0; x < src_im.cols; x++){
            if(src_im.at<int>(y*src_im.cols + x) == target_val) return cv::Point2i(x,y);
        }
    }
    return cv::Point2i(-1, -1);
}

cv::RotatedRect outline_detect(cv::Mat src_im, cv::Point min_pos, cv::Point max_pos){
    cv::Mat cols_scan, rows_scan;
    src_im.copyTo(cols_scan);
    src_im.copyTo(rows_scan);

    //横スキャン
    for (int j = min_pos.y; j <= max_pos.y; ++j) {
        int left_pos = -1, right_pos = -1;
        for (int i = min_pos.x; i <= max_pos.x; ++i) {
            if ((int) cols_scan.data[j * cols_scan.step + i * cols_scan.elemSize()] == 255) {
                if (left_pos == -1)
                    left_pos = i;
                else if (right_pos == -1)
                    right_pos = i;
                    //else if(i - right_pos == 1)
                    //    right_pos = i;
                else {
                    cols_scan.data[j * cols_scan.step + right_pos * cols_scan.elemSize()] = 0;
                    right_pos = i;
                }
            }
        }
    }

    std::vector<cv::Point> iris_age;

    //縦スキャン
    for (int i = min_pos.x; i <= max_pos.x; ++i) {
        int top_pos = -1, under_pos = -1;
        for (int j = min_pos.y; j <= max_pos.y; ++j) {
            if ((int) rows_scan.data[j * rows_scan.step + i * rows_scan.elemSize()] == 255) {
                if (top_pos == -1)
                    top_pos = j;
                else if (under_pos == -1)
                    under_pos = j;
                    //else if(j - under_pos == 1)
                    //    under_pos = j;
                else {
                    rows_scan.data[under_pos * rows_scan.step + i * rows_scan.elemSize()] = 0;
                    under_pos = j;
                }
            }
        }
        if ((int) cols_scan.data[top_pos * rows_scan.step + i * rows_scan.elemSize()] == 255)
            iris_age.push_back(cv::Point(i, top_pos));
        if ((int) cols_scan.data[under_pos * rows_scan.step + i * rows_scan.elemSize()] == 255)
            iris_age.push_back(cv::Point(i, under_pos));
    }

    return cv::fitEllipse(iris_age);
}

double Ask_Perimeter(cv::Mat tmp, int i, cv::Point2i start_pos, int mode, cv::Mat &src_im){
    // mode0 ->  周囲長を求める
    // mode1 ->  周囲長の描画
    // mode4 ->  周囲長の描画改

    int x = start_pos.x, y = start_pos.y;
    int nextVec = 0;
    double length = 0.0;
    //cv::Vec3b Color(0,0,200);

    std::vector<cv::Point> pupilelement;
    cv::Mat outlines = cv::Mat::zeros(tmp.rows, tmp.cols, CV_8UC1);
    cv::Point min_p = cv::Point(tmp.cols, tmp.rows), max_p = cv::Point(0,0);

    while(1){
        bool midstream_flg = false;
        switch (nextVec){
            case 0: // 左斜め下
                if(x != 0 && y != tmp.rows)
                if(tmp.at<int>(y+1, x-1) == i){
                    // 座標の移動
                    y++;
                    x--;
                    // 次の探索開始方向
                    nextVec = (0 + 6) % 8; //6
                    break;
                }
            case 1: // 下
                if(y != tmp.rows)
                if(tmp.at<int>(y+1, x) == i){
                    y++;
                    nextVec = (1 + 6) % 8; //7
                    break;
                }
            case 2: // 右斜め下
                if(x != tmp.cols && y != tmp.rows)
                if(tmp.at<int>(y+1, x+1) == i){
                    y++;
                    x++;
                    nextVec = (2 + 6) % 8; //0
                    break;
                }
            case 3: // 右
                if(x != tmp.cols)
                if(tmp.at<int>(y, x+1) == i){
                    x++;
                    nextVec = (3 + 6) % 8; //1
                    break;
                }
            case 4: // 右斜め上
                if(x != tmp.cols && y != 0)
                if(tmp.at<int>(y-1, x+1) == i){
                    y--;
                    x++;
                    nextVec = (4 + 6) % 8; //2
                    break;
                }
            case 5: // 上
                if(y != 0)
                if(tmp.at<int>(y-1, x) == i){
                    y--;
                    nextVec = (5 + 6) % 8; //3
                    break;
                }
            case 6: // 左斜め上
                if(x != 0 && y != 0)
                if(tmp.at<int>(y-1, x-1) == i){
                    y--;
                    x--;
                    nextVec = (6 + 6) % 8; //4
                    break;
                }
            case 7: // 左
                if(x != 0)
                if(tmp.at<int>(y, x-1) == i){
                    x--;
                    nextVec = (7 + 6) % 8; //5
                    break;
                }
            default:
                nextVec = 0;
                midstream_flg = true;
                break;
        }

        if(!midstream_flg){
            if(mode == 0){
                if(nextVec % 2 == 1)
                    length += 1;
                else
                    length += sqrt(2);
            }else if(mode == 1){
                //src_im.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,200);
                pupilelement.push_back(cv::Point(x, y));
            }else if(mode == 2){
                if(src_im.channels() == 3)
                    src_im.at<cv::Vec3b>(y, x) = cv::Vec3b(200,200,200);
                else
                    src_im.data[y * src_im.cols + x] = 200;
            }else if(mode == 4){
                if(y < min_p.y) min_p.y = y;
                if(x < min_p.x) min_p.x = x;
                if(y > max_p.y) max_p.y = y;
                if(x > max_p.x) max_p.x = x;

                pupilelement.push_back(cv::Point(x, y));
                outlines.data[y * outlines.step + x * outlines.elemSize()] = 255;
            }
        }

        if(cv::Point(x,y) == start_pos){
            if(pupilelement.size() > 5 && mode == 1){
                cv::RotatedRect pupilbox = cv::fitEllipse(pupilelement);
                cv::ellipse(src_im, pupilbox, cv::Scalar(0,255,0), 2, 8);   //楕円フィッティング！
            }else if(pupilelement.size() > 5 && mode == 4){
                cv::RotatedRect pupilbox = outline_detect(outlines, min_p, max_p);
                cv::ellipse(src_im, pupilbox, cv::Scalar(0,255,0), 2, 8);   //楕円フィッティング！
                //p_c = pupilbox.center;
            }
            break;
        }
    }
    return length;
}
void Paint2label(cv::Mat src_im, cv::Mat &out_im, int nlabels){

    static std::vector<cv::Vec3b> colors;
    if(colors.size() == 0)
        colors.push_back(cv::Vec3b(0,0,0));

    if(nlabels > colors.size()){
        for(int i = (int)colors.size(); i < nlabels; i++){
            colors.push_back(cv::Vec3b((rand()&255), (rand()&255), (rand()&255)));
        }
    }

    // 3層の画像
    cv::Mat dst(src_im.size(), CV_8UC3);
    for(int y = 0; y < dst.rows; ++y){
        for(int x = 0; x < dst.cols; ++x){
            int label_num = src_im.at<int>(y, x);
            dst.at<cv::Vec3b>(y, x) = colors[label_num];
        }
    }

    dst.copyTo(out_im);
}

cv::Point2i Ask_DegreofCircle(cv::Mat src_im, cv::Mat &draw_im, cv::Mat &paint_im){
    //src_im    ->  2値画像
    //draw_im   ->  src_imの最も円形度が高い円の周辺+重心を描画(省略可)
    //paint_im  ->  src_imにラベルごとに色を塗ったもの(省略可)
    //return    ->  重心座標(Pixel)

    // ラベリング
    cv::Mat label, state, centroids;
    int nLabels = cv::connectedComponentsWithStats(src_im, label, state, centroids, 8);

    // 周囲長を求める
    int high_circle = -1;       //円形度が最も高いラベル
    double tmp_circle = 10.0;   //円形度の数値保持用

    for(int i = 1; i < nLabels; i++){

        // 500px以下の領域はノイズとして除外する
        while(1){
            if(state.at<int>(i, cv::CC_STAT_AREA) > 500) break;
            i++;
        }
        if(i >= nLabels) break;

        // ラスタスキャンを行い，ラベルの(X, Y)が最も小さい位置(優先順位：Y > X)
        cv::Point2i point = RasterScan(label, i);

        // 円形度を求め，最高のものを保持
        double length = Ask_Perimeter(label, i, point, 0); // 周囲長の計算
        double Deg_circle = 1 - (4 * CV_PI * state.at<int>(i, cv::CC_STAT_AREA)) / (length*length); // 円形度の計算
        if(Deg_circle < 0) Deg_circle *= -1;    // 絶対値

        if(Deg_circle < tmp_circle){
            high_circle = i;
            tmp_circle = Deg_circle;
        }

    }

    // 円形度が最高の領域の周囲辺，重心の描画
    if(&draw_im != (cv::Mat *)nullptr){
        cv::Point2i point = RasterScan(label, high_circle);
        if(nLabels != 1 && high_circle != -1){
            Ask_Perimeter(label, high_circle, point, 4, draw_im);   //周囲辺を描画
            draw_im.at<cv::Vec3b>((int)centroids.at<double>(high_circle*2+1), (int)centroids.at<double>(high_circle*2)) = cv::Vec3b(0, 200, 200);   // 重心の描画
            //draw_im.at<cv::Vec3b>(p_c.y, p_c.x) = cv::Vec3b(200, 200, 0);   // 重心の描画
            //Ask_Perimeter(label, high_circle, point, 4, draw_im);   //周囲辺を描画
        }
    }

    // ラベリング色
    if(&paint_im != (cv::Mat *)nullptr){
        Paint2label(label, paint_im, nLabels);
    }

    return cv::Point2d((int)centroids.at<double>(high_circle*2), (int)centroids.at<double>(high_circle*2+1));
}