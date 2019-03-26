#ifndef ROIEXTRACTION_H
#define ROIEXTRACTION_H

#include "preprocessing_config.h"

class RoiExtraction : public QObject
{
    Q_OBJECT

public:
    RoiExtraction(QObject *parent = nullptr);
    ~RoiExtraction();

    void setOriginal(const cv::Mat &value);
    void setContours(const std::vector<std::vector<cv::Point> > &value);
    void setCannyOutput(const cv::Mat &value);
    std::vector<cv::Point> getEndpoints() const;

    cv::Mat findROI();
    cv::Mat emergencyROI();

signals:
    void RoiExtractionErrorSignal(int);

private:
    cv::Mat original;
    cv::Mat roi;
    cv::Mat cannyOutput;

    int minX, minY;
    int maxX, maxY;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> longestContour;
    std::vector<cv::Point> secondLongestContour;
    std::vector<cv::Point> endpoints;

    void findEdgeContours();
    void findEndpoints();
    void findEndpointsInLongestContour();
    void findEndpointsInSecondLongestContour();
    void findMinMaxXY();
    bool scanNeighborhood(std::vector<cv::Point>&, const int);

    void clearResults();
    void roiExtractionError(const int);
};

#endif // ROIEXTRACTION_H
