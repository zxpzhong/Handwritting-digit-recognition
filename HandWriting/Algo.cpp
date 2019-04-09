#include "Algo.h"

//#define DEBUG
void debug_imshow(char * name, Mat img)
{
#ifdef DEBUG
    imshow(name, img);
    waitKey(0);
#endif
}
/*
算法类构造函数，读取所有预储存的图片，求类条件概率category_condition_probability | Algo class constructor will read all the training data and get the Conditional probability
*/
Algo::Algo()
{
    char img_path[80];
    //0. 读取图像 | read the image 
    for (int category = 0; category < CATEGORY; category++)   //10类 | 10 category
    {
        vector <double> probability;
        //1. 创建保存读取出的特征 | create and save the feature
        vector<vector<int>> summary;
        //2. 获取某类别下的$SAMPLE张图片特征，并保存 | get the $SAMPLE samples' feature in one of the category
        for (int sample = 0; sample < SAMPLE; sample++) 
        {
            sprintf(img_path, "./data/%d/%d-%d.bmp", category, category, sample+1);
            Mat src = cv::imread(img_path);
            vector <int> feature = get_feature(src);
            summary.push_back(feature);
            qDebug() << img_path;
        }
        //3. 遍历summary后，计算特征向量每个位置为1时，属于该类的概率 | Traversing the summary and cal the clasification probability when location of this feature is 1
        for (int f_index = 0; f_index < FEATURE_DIM; f_index++)   //100维特征向量 | 100 dim feature vector 
        {
            int sum = 0;
            for (int sample = 0; sample < SAMPLE; sample++)   //每类$SAMPLE张图片 | $SAMPLE images under every category
            {
                sum += summary[sample][f_index];
            }
            probability.push_back(double(sum+1)/float(SAMPLE + 2));
        }
        category_condition_probability.push_back(probability);
    }
}
vector<int> Algo::get_feature(Mat img)
{
    vector<int> feature;
    //0.0 对图像边界进行扩充 | expansion the img border
    Scalar value = Scalar(255,255, 255);
    cv::copyMakeBorder(img, img, 400, 400, 400, 400, cv::BORDER_CONSTANT, value);
    //0. 转灰度图 | convert it to gray
    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
    //1. 对图像二值化，确保是二值图像 | convert to bin img
    cv::threshold(img, img, 128, 255, cv::THRESH_BINARY);
    debug_imshow("bin", img);

    int min_col = img.cols;
    int max_col = 0;
    int max_row = 0;
    int min_row = img.rows;
    //2. 对图像最大外接长方形截取 | Intercept the external rect
    for (int col = 0; col < img.cols; col++)
    {
        for (int row = 0; row < img.rows; row++)
        {
            if (img.at<uchar>(row, col) < 250)
            {
                if (col < min_col)
                {
                    min_col = col;
                }
                if (col > max_col)
                {
                    max_col = col;
                }
                if (row < min_row)
                {
                    min_row = row;
                }
                if (row > max_row)
                {
                    max_row = row;
                }
            }
        }
    }
    qDebug(" max_col is %d", max_col);
    //3.1 求取最小区域中心坐标 | solve the center coor
    int middle_col = int((min_col + max_col) / 2);
    int middle_row = int((min_row + max_row) / 2);
    qDebug() << "max_col is" << max_col << "min_col is" << min_col << "max_row is" << max_row << "min_row is" << min_row << "img col is " << img.cols << "img row is "<< img.rows;
    if ((max_col - min_col == 0) || (abs(max_col - min_col) == img.cols) || (max_row - min_row == 0) || (abs(max_row - min_row) == img.rows))
    {
        return feature;
    }
    //3.1.0
    //3.2 判断是列方向宽还是行方向宽 | which direction is wider? col or row
    if ((max_col - min_col) >= (max_row - min_row))  //列宽 | col width
    {
        //从中心点，向两边扩增行 | expand the rows from the center point
        min_row = middle_row - int((max_col - min_col) / 2);
        max_row = middle_row + int((max_col - min_col) / 2);
    }
    else //行宽 | row height
    {
        //从中心点，向两边扩增列 | expand the cols from the center point
        min_col = middle_col - int((max_row - min_row) / 2);
        max_col = middle_col + int((max_row - min_row) / 2);
    }

    Rect rect(min_col, min_row, max_col - min_col, max_row - min_row);
    Mat image_roi = img(rect);
    debug_imshow("roi", image_roi);
    //4. 对截取后的图像分成10*10的区域，如果每个小区域有黑点，即为1，否则为0 | divide the intercepted img to 10x10 region and set the location of feature to 1 if this exist the number pixel

    //4.1 resize成固定大小 | resize to a constant size
    cv::resize(image_roi, image_roi, cv::Size(40, 40));
    debug_imshow("roi40", image_roi);
    
    Mat feature_img(Size(10,10), CV_8UC1);
    //4.2 遍历整幅图像，对每个小区域的进行判断是否存在像素 | Traversing the whole img
    for (int col = 0; col < 10; col++)
    {
        for (int row = 0; row < 10; row++)
        {
            int count = 0;
            for (int col_sub = 0; col_sub < 4; col_sub ++)
            {
                for (int row_sub = 0; row_sub < 4; row_sub ++)
                {
                    if (image_roi.at<uchar>(row*4 + row_sub, col*4 + col_sub) < 200 )  //如果小方格内有为1的 | if exist black pixel
                    {
                        count++;
                    }
                }
            }
            if (count > 1)
            {
                feature.push_back(1);
                feature_img.at<uchar>(row, col) = 0;
            }
            else
            {
                feature.push_back(0);
                feature_img.at<uchar>(row, col) = 255;
            }
        }
    }
    resize(feature_img, feature_img, Size(100, 100));
    debug_imshow("feature", feature_img);
    //5. 返回100维的特征向量
    return feature;
}

/*
输入为特征向量 | input:feature vector
返回值为类别 | return:clasification score
*/
vector<double> Algo::min_error_bayes(vector<int> feature)
{
    //category_condition_probability
    vector<double> probability;
    //0. 遍历100维feature，计算每个元素对应于每一类的概率 | Traversing the feature, cal the probability under every categrory
    
    for (int sample = 0; sample < CATEGORY; sample++)
    {
        float plus = 1; //乘法初始化为1 | init the plus to 1
        for (int f_index = 0; f_index < FEATURE_DIM; f_index++)   //100维特征向量 | 100 dim feature vector
        {
            //1. 将每一类的概率相乘，得到整个feature属于每一类的概率 | multiple the probability of every dim of feature and then get the the probability under every categrory
            if (feature[f_index] == 1)
            {
                plus = plus * category_condition_probability[sample][f_index];
            }
            else
            {
                plus = plus * (1-category_condition_probability[sample][f_index]);
            }
        }
        probability.push_back(plus);
    }
    //输出归一化 | normalize the output
    float sum = 0;
    for (int col = 0; col < CATEGORY; col++)
    {
        sum += probability[col];
    }
    for (int col = 0; col < CATEGORY; col++)
    {
        probability[col] = probability[col]/sum;
    }

    //2. 直接返回类别分数 | return clasification score
    return probability;
}