# Handwritting digit recognition

----------

基于**最小错误率**和**Fisher线性分类器**的QT/C++手写字符识别系统

QT/C++ implement of Handwritting digit recognition using minimum error rate bayes & Fisher linear classifier

## Support input
 - Hand write(By mouse)
 - Video(USB driverless camera)

## Enviroment
 - `PC`:WIN10 64bit
 - `IDE`:Visual Studio 15
 - `COMPLIER`:msvc2015_64
 - `QT`:QT5.9.3
 - `OpenCV`:3.4.0

## File structure
 - `HandWriting.sln`: project of VS2015
 - `HandWriting`:src dir
     - `data dir`:training data
     - `Algo.cpp`:Algorithm source
     - `Algo.h`:Algorithm header
     - `debug.log`:debug output file
     - `HandWriting.cpp`:QT framework source
     - `HandWriting.h`:QT framework header
     - `HandWriting.ui`:QT GUI file
     - `main.cpp`:QT enter point of main process
 - `Fisher`:
     - `fisher.py`:fisher linear classifier in python

