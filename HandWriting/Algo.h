#pragma once
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include <string>
using namespace cv;
using namespace std;

#define FEATURE_DIM 100
#define CATEGORY 10
#define SAMPLE 20

class Algo {
public:
    Algo();
    vector<int> get_feature(Mat img);
    vector<vector<double>> category_condition_probability; //10*100   每一类下，每张图片对应的，每一个特征属于该类的概率 | category Conditional probability under every categrory
    vector<double> min_error_bayes(vector<int> feature);
};