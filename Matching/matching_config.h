#ifndef MATCHING_CONFIG_H
#define MATCHING_CONFIG_H

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

#pragma once
enum RECOGNITION_MODE { IDENTIFICATION, VERIFICATION };

typedef struct
{
    QString leftFinger;
    QString rightFinger;
    double score;

}FINGER_VEIN_PAIR;

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

typedef struct
{
    double sensitivity;
    int min;
    int max;

}FVR_PLOT_PARAMS;

typedef struct
{
    QVector<double> farX;
    QVector<double> farY;
    QVector<double> frrX;
    QVector<double> frrY;
    QVector<double> rocX;
    QVector<double> rocY;

    double eer;
    FVR_PLOT_PARAMS fvrPlotParams;

}DB_TEST_RESULTS;

typedef struct
{
    QMap<QString, QVector<cv::Mat>> database;
    QVector<QString> keys;
    QVector<FINGER_VEIN_PAIR> fvrGenuinePairs;
    QVector<FINGER_VEIN_PAIR> fvrImpostorPairs;
    int numOfSubjects;
    int imgsPerSubject;
    // bool genuineTestDone

}DB_PARAMS;


#endif // MATCHING_CONFIG_H
