#ifndef FEATUREEXTRACTION_CONFIG_H
#define FEATUREEXTRACTION_CONFIG_H

// Qt
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QTime>

// std
#include <vector>

// OpenCV
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/xfeatures2d.hpp>

namespace fvr
{

enum EXTRACTOR {SIFT, SURF};

typedef struct
{
    cv::Mat original;
    cv::Mat roi;
    cv::Mat roiContrastEnhanced;

}PREPROCESSED_RESULT;

typedef struct
{
    bool isSequence;
    bool isLoaded;
    PREPROCESSED_RESULT singleInput;
    QMap<QString, PREPROCESSED_RESULT> sequenceInput;
    QVector<QString> keys;
    int counter;

}FEATURE_EXTRACTION_INPUT;

typedef struct
{
    int numberOfFeatures;
    int numberOfOctaveLayers;
    double contrastThreshold;
    double edgeThreshold;
    double sigma;

}SIFT_PARAMS;

typedef struct
{
    double hessianThreshold;
    int numberOfOctaves;
    int numberOfOctaveLayers;
    bool extended;
    bool upright;

}SURF_PARAMS;

}

#endif // FEATUREEXTRACTION_CONFIG_H
