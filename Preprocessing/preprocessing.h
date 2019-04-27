#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "preprocessing_global.h"
#include "preprocessing_config.h"
#include "bilateralfiltering.h"
#include "edgeprocessing.h"
#include "roiextraction.h"

namespace fvr
{

typedef struct
{
    cv::Mat original;
    cv::Mat roi;
    cv::Mat roiContrastEnhanced;

}PREPROCESSING_RESULT;

typedef struct
{
    cv::Mat original;
    cv::Mat cropped;
    cv::Mat bilateralApplied;
    cv::Mat cannyOutput;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> endpoints;
    cv::Mat roi;
    cv::Mat roiContrastEnhanced;

}PREPROCESSING_ALL_RESULTS;

// All durations are in milliseconds
typedef struct
{
    int bilateralFilterApplication;
    int cannyEdgeDetection;
    int contourDetection;
    int roiExtraction;

}PREPROCESSING_DURATIONS;

class PREPROCESSINGSHARED_EXPORT Preprocessing: public QObject
{
    Q_OBJECT

public:
    Preprocessing(QObject *parent = nullptr);
    ~Preprocessing();

    // API
    void start();

    int loadInput(const cv::Mat);
    int loadInput(const QVector<cv::Mat>);
    int loadInput(const QString);

    int setPreprocessingParams(const int bilateralDiameter = 10, const int bilateralSigmaColor = 10, const int bilateralSigmaSpace = 3,
                               const int bilateralBorderType = cv::BORDER_DEFAULT, const int numOfBilateralFilterApplications = 3,
                               const int cannyThreshold1 = 0, const int cannyThreshold2 = 255, const int contoursMethod = cv::RETR_LIST,
                               const int contoursMode = cv::CHAIN_APPROX_NONE);
    int setAdvancedMode(bool useAdvancedMode = false);

private:
    // Key stages
    BilateralFiltering bilateralFilter;
    EdgeProcessing edgeProcessing;
    RoiExtraction roiExtraction;

    // Input
    INPUT input;

    // Parameters
    BILATERAL_FILTER_PARAMS bilateralFilterParams;
    CANNY_PARAMS cannyParams;
    CONTOUR_PARAMS contourParams;

    // Output
    PREPROCESSING_ALL_RESULTS allResults;
    QMap<QString, PREPROCESSING_RESULT> resultsMap;
    QMap<QString, PREPROCESSING_ALL_RESULTS> allResultsMap;
    PREPROCESSING_DURATIONS timeDurations;

    QTime timer;
    bool isRunning;
    bool isAdvancedMode;

private slots:
    void preprocessingError(const int);
    void startPreprocessing(const cv::Mat&);
    void preprocessingDone();
    void clearResults();
    void clearDurations();
    void clearInput();
    void errorCaught(const int);

signals:
    // Single result
    void PreprocessingDoneSignal(PREPROCESSING_ALL_RESULTS);
    void PreprocessingDoneSignal(PREPROCESSING_RESULT);

    // Set of results
    void PreprocessingSequenceDoneSignal(QMap<QString, PREPROCESSING_ALL_RESULTS>);
    void PreprocessingSequenceDoneSignal(QMap<QString, PREPROCESSING_RESULT>);

    void PreprocessingProgressSignal(int);
    void PreprocessingDurationSignal(PREPROCESSING_DURATIONS);

    void PreprocessingErrorSignal(int);
};

}

#endif // PREPROCESSING_H
