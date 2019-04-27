#ifndef MATCHER_CONFIG_H
#define MATCHER_CONFIG_H

// Qt
#include <QObject>
#include <QVector>
#include <QString>
#include <QMultiMap>
#include <QMap>
#include <QTime>

// OpenCV
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/xfeatures2d.hpp>

namespace fvr
{

enum MATCHER_TYPE { BRUTEFORCE, FLANN };

typedef struct
{
    bool crossCheck;
    int normType;

}MATCHER_PARAMS;

typedef struct
{
    cv::Mat descriptors1;
    cv::Mat descriptors2;
    bool inputLoaded;

}INPUT;

}

#endif // MATCHER_CONFIG_H
