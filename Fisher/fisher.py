import numpy as np
import cv2 as cv
root_dir = 'E:\data'
import os

# 1. get img list
img_list = []
for i in range(10):
    img = []
    for num in range(1,21):
        a = cv.imread(os.path.join(root_dir,str(i),'{}-{}.bmp'.format(i,num)),0)
        img.append(a)
    img_list.append(img)


# all feature is 10*20*100     10 class 20 samples 100 dim features

features = np.zeros([10,20,100])

# 1. feature extracture 10*10
for class_idx,class_img in enumerate(img_list):
    for img_idx,single_img in enumerate(class_img):
        ## 1.1 divide to 10*10 parts of every img
        for i in range(10):
            for j in range(10):
                ## 1.2 for every single parts, is there any pixels < 100
                for rows in range(24*i,24*i+23+1):
                    for cols in range(24*j,24*j+23+1):
                        if single_img[rows,cols] < 100:
                            features[class_idx,img_idx,i*10+j] = 1

# 2. take this misson to 10 bin classification misson, which need 10 fisher linear classifer
W_stars = []
W0s = []
for classifer_idx in range(10):
    ## 3.1 Ui
    ## features[classifer_idx] vs np.delete(features,classifer_idx, axis = 0)
    features_this = features[classifer_idx]
    features_other = np.delete(features,classifer_idx, axis = 0)
    features_other = np.resize(features_other,[20*9,100])
    mean_this = np.mean(features_this,0)
    mean_other = np.mean(features_other, 0)
    ## 3.2 类内离散矩阵Sw0
    feature_submean = features_this - mean_this
    Sw0 = np.zeros([100,100])
    for idx in range(feature_submean.shape[0]):
        a = feature_submean[idx]
        tmp = np.dot(a[:,np.newaxis],a[np.newaxis,:])
        Sw0  = Sw0 + tmp
    Sw0 = Sw0/feature_submean.shape[0]

    ## 3.3 类内离散矩阵Sw1
    feature_submean = features_other - mean_other
    Sw1 = np.zeros([100,100])
    for idx in range(feature_submean.shape[0]):
        a = feature_submean[idx]
        tmp = np.dot(a[:,np.newaxis],a[np.newaxis,:])
        Sw1  = Sw1 + tmp
    Sw1 = Sw1/feature_submean.shape[0]
    ## 3.4 总类内离散矩阵 Sw
    Sw = Sw0 + Sw1
    ## 3.5 计算总类内离散矩阵的逆矩阵Sw-1
    ## ?????使用了广义逆矩阵pinv，由于Sw过于稀疏，行列式为0，无法直接求逆矩阵inv
    Sw_trans = np.linalg.pinv(Sw)
    ## 3.6 求投影向量
    W_star = np.dot(Sw_trans,mean_this-mean_other)
    ## 3.7 求分割阈值，第一种阈值为
    W0 = (np.dot(W_star.T,mean_this)+np.dot(W_star.T,mean_other))/2

    ## 3.8 保存投影向量和阈值
    W_stars.append(W_star)
    W0s.append(W0)
# 4. 分割函数y = np.dot(W_star.T,x)
for class_classifer in range(10):
    print('-----------------Handle class_classifer {}'.format(class_classifer))
    for class_idx,class_img in enumerate(img_list):
        print('Handle Number {}'.format(class_idx))
        ans = ''
        for img_idx,single_img in enumerate(class_img):
            y = np.dot(W_stars[class_classifer].T,features[class_idx][img_idx])
            if y >= W0s[class_classifer]:
                ans+='Y'
            else:
                ans+='N'
        print('{} is {}'.format(class_idx,ans))
# 5. 读取测试图片
for t in range(10):
    test = cv.imread('E:/data/{}.bmp'.format(t),0)
    test_feature = np.zeros([100])
    for i in range(10):
        for j in range(10):
            ## 1.2 for every single parts, is there any pixels < 100
            for rows in range(24 * i, 24 * i + 23 + 1):
                for cols in range(24 * j, 24 * j + 23 + 1):
                    if test[rows, cols] < 100:
                        test_feature[i*10+j] = 1
    ans = ''
    for class_classifer in range(10):
        print('-----------------Handle class_classifer {}'.format(class_classifer))
        y = np.dot(W_stars[class_classifer].T,test_feature)
        if y >= W0s[class_classifer]:
            ans+='Y'
        else:
            ans+='N'
    print('ans is {}'.format(ans))


# for i in range(10):
#     ans = ''
#     for j in range(100):
#         ans += str(W_stars[i][j])+', '
#     print(ans)
#
# b = np.zeros(100)
# for i in range(100):
#     b[i] = int(a[i])