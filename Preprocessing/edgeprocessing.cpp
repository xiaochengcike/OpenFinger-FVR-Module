#include "edgeprocessing.h"

namespace fvr
{

EdgeProcessing::EdgeProcessing(QObject *parent) : QObject(parent)
{
    this->cannyParams.t1 = 0;
    this->cannyParams.t2 = 255;
    this->contourParams.method = cv::RETR_LIST;
    this->contourParams.mode = cv::CHAIN_APPROX_NONE;

    this->clearResults();
}

EdgeProcessing::~EdgeProcessing()
{
    this->clearParams();
    this->clearResults();
}

cv::Mat EdgeProcessing::cannyEdgeDetection()
{
    this->cannyOutput = cv::Mat(this->original.rows, this->original.cols, CV_8UC1);
    this->cannyOutput.setTo(0);

    cv::Canny(this->original, this->cannyOutput, this->cannyParams.t1, this->cannyParams.t2);

    return this->cannyOutput;
}

std::vector<std::vector<cv::Point>> EdgeProcessing::contourDetection()
{
    cv::findContours(this->cannyOutput, this->contours, this->contourParams.mode, this->contourParams.method);

    if(this->contours.empty())
    {
        // No contours found
        this->edgeProcessingError(31);
    }

    return this->contours;
}

void EdgeProcessing::setParams(const CANNY_PARAMS cannyParams, const CONTOUR_PARAMS contourParams)
{
    this->cannyParams.t1 = cannyParams.t1;
    this->cannyParams.t2 = cannyParams.t2;
    this->contourParams.method = contourParams.method;
    this->contourParams.mode = contourParams.mode;
}

void EdgeProcessing::setOriginal(const cv::Mat &value)
{
    this->original = value;
}

void EdgeProcessing::edgeProcessingError(const int errorCode)
{
    emit EdgeProcessingErrorSignal(errorCode);
}

void EdgeProcessing::clearParams()
{
    this->cannyParams.t1 = 0;
    this->cannyParams.t2 = 0;
    this->contourParams.method = cv::RETR_LIST;
    this->contourParams.mode = cv::CHAIN_APPROX_NONE;
}

void EdgeProcessing::clearResults()
{
    this->original.release();
    this->cannyOutput.release();
    this->contours.clear();
}

}
