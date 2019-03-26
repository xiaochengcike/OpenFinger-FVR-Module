#ifndef PREPROCESSING_CONFIG_H
#define PREPROCESSING_CONFIG_H

// Qt
#include <QObject>
#include <QVector>
#include <QMap>
#include <QTime>
#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QListIterator>
#include <QVectorIterator>

// std
#include <iostream>
#include <stdio.h>
#include <vector>
#include <limits>

// OpenCV
#include "opencv4/opencv2/opencv.hpp"

enum INPUT_MODE { IMAGE, IMAGES, IMAGE_PATH, IMAGE_DIR };

typedef struct
{
    int diameter;
    double sigmaColor;
    double sigmaSpace;
    int borderType;
    int numOfTimesApplied;

}BILATERAL_FILTER_PARAMS;

typedef struct
{
    double t1;
    double t2;

}CANNY_PARAMS;

typedef struct
{
    int mode;
    int method;

}CONTOUR_PARAMS;

typedef struct
{
    cv::Mat image;
    QVector<cv::Mat> images;
    QVector<QString> imageNames;
    QString inputPath;
    bool isLoaded;
    int counter;
    INPUT_MODE mode;

}INPUT;


#endif // PREPROCESSING_CONFIG_H
