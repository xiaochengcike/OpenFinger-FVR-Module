#ifndef EDGEPROCESSING_H
#define EDGEPROCESSING_H

#include "preprocessing_config.h"

namespace fvr
{

class EdgeProcessing : public QObject
{
    Q_OBJECT
public:
    EdgeProcessing(QObject *parent = nullptr);
    ~EdgeProcessing();

    cv::Mat cannyEdgeDetection();
    std::vector<std::vector<cv::Point>> contourDetection();

    void setParams(const CANNY_PARAMS, const CONTOUR_PARAMS);
    void setOriginal(const cv::Mat &value);

signals:
    void EdgeProcessingErrorSignal(int);

private:
    cv::Mat original;
    cv::Mat cannyOutput;
    std::vector<std::vector<cv::Point>> contours;

    CANNY_PARAMS cannyParams;
    CONTOUR_PARAMS contourParams;

    void edgeProcessingError(const int);
    void clearParams();
    void clearResults();
};

}

#endif // EDGEPROCESSING_H
