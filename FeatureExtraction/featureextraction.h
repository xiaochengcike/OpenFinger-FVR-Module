#ifndef FEATUREEXTRACTION_H
#define FEATUREEXTRACTION_H

#include "featureextraction_global.h"
#include "featureextraction_config.h"

namespace fvr
{

typedef struct
{
    int siftDetection;
    int siftDescriptorComputation;
    int surfDetection;
    int surfDescriptorComputation;

}EXTRACTION_DURATIONS;

typedef struct
{
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

}EXTRACTION_RESULTS;

class FEATUREEXTRACTIONSHARED_EXPORT FeatureExtraction : public QObject
{
    Q_OBJECT

public:
    FeatureExtraction(QObject *parent = nullptr);
    ~FeatureExtraction();

    // API
    int loadInput(const cv::Mat &, const cv::Mat &, const cv::Mat &);
    int loadInput(const PREPROCESSED_RESULT &);
    int loadInput(const QMap<QString, PREPROCESSED_RESULT> &);

    void start();

    int setExtractionParams(EXTRACTOR, SIFT_PARAMS, SURF_PARAMS);

private:
    // Feature extraction algorithms
    cv::Ptr<cv::xfeatures2d::SIFT> sift;
    cv::Ptr<cv::xfeatures2d::SURF> surf;

    // Input
    FEATURE_EXTRACTION_INPUT input;

    // Parameters
    EXTRACTOR extractor;
    SIFT_PARAMS siftParams;
    SURF_PARAMS surfParams;

    // Output
    EXTRACTION_DURATIONS timeDurations;
    EXTRACTION_RESULTS results;
    QMap<QString, EXTRACTION_RESULTS> resultsMap;

    QTime timer;
    bool isRunning;

private slots:
    void startExtraction(const PREPROCESSED_RESULT &);
    void clearInput();
    void clearDurations();
    void clearResults();
    void clearSequenceResults();
    void extractionError(const int);

signals:
    // Single result
    void extractionDoneSignal(EXTRACTION_RESULTS);

    // Set of results
    void extractionSequenceDoneSignal(QMap<QString, EXTRACTION_RESULTS>);

    void extractionProgressSignal(int);
    void extractionDurationsSignal(EXTRACTION_DURATIONS);

    void extractionErrorSignal(int);
};

}

#endif // FEATUREEXTRACTION_H
