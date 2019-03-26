#ifndef MATCHING_CONFIG_H
#define MATCHING_CONFIG_H

// Qt
#include <QObject>
#include <QVector>
#include <QString>
#include <QMultiMap>
#include <QTime>

// OpenCV
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/xfeatures2d.hpp>

enum MODE{IDENTIFICATION, VERIFICATION};

typedef struct
{
    int normType;
    bool crossCheck;

}BF_MATCHER_PARAMS;

typedef struct
{
   int identificationDuration;
   int verificationDuration;

}MATCHING_DURATIONS;


#endif // MATCHING_CONFIG_H
