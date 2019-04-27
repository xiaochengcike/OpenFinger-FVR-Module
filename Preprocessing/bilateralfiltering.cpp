#include "bilateralfiltering.h"

namespace fvr
{

BilateralFiltering::BilateralFiltering(QObject *parent) : QObject(parent)
{
    this->params.diameter = 10;
    this->params.sigmaColor = 10;
    this->params.sigmaSpace = 3;
    this->params.borderType = cv::BORDER_DEFAULT;
    this->params.numOfTimesApplied = 3;

    this->clearResults();
}

BilateralFiltering::~BilateralFiltering()
{
    this->clearResults();
    this->clearParams();
}

cv::Mat BilateralFiltering::applyFilter()
{
    if(this->params.numOfTimesApplied < 0)
    {
        // Invalid input
        this->bilateralFilterError(30);
    }

    if(this->params.numOfTimesApplied == 1)
    {
        cv::bilateralFilter(this->original, this->blurred, this->params.diameter,
                            this->params.sigmaColor, this->params.sigmaSpace,
                            this->params.borderType);

        return this->blurred;
    }
    else if(this->params.numOfTimesApplied > 1)
    {
        this->tmp1 = cv::Mat(this->original.rows, this->original.cols, this->original.type());
        this->tmp2 = cv::Mat(this->original.rows, this->original.cols, this->original.type());
        this->tmp1.setTo(0);
        this->tmp2.setTo(0);

        cv::bilateralFilter(this->original, this->tmp1, this->params.diameter,
                            this->params.sigmaColor, this->params.sigmaSpace,
                            this->params.borderType);

        for(int i = 0; i < this->params.numOfTimesApplied; ++i)
        {
            cv::bilateralFilter(this->tmp1, this->tmp2, this->params.diameter,
                                this->params.sigmaColor, this->params.sigmaSpace,
                                this->params.borderType);

            this->tmp1.setTo(0);
            this->tmp1 = this->tmp2.clone();
            this->tmp2.setTo(0);
        }

        this->blurred = this->tmp1.clone();
        return this->blurred;
    }
}

void BilateralFiltering::bilateralFilterError(const int errorCode)
{
    emit BilateralFilterErrorSignal(errorCode);
}

void BilateralFiltering::clearParams()
{
    this->params.diameter = 0;
    this->params.sigmaColor = 0;
    this->params.sigmaSpace = 0;
    this->params.borderType = cv::BORDER_DEFAULT;
    this->params.numOfTimesApplied = 1;
}

void BilateralFiltering::clearResults()
{
    this->tmp1.release();
    this->tmp2.release();
    this->blurred.release();
    this->original.release();
}

void BilateralFiltering::setOriginal(const cv::Mat &value)
{
    this->original = value;
}

void BilateralFiltering::setParams(const BILATERAL_FILTER_PARAMS params)
{
    this->params.diameter = params.diameter;
    this->params.sigmaColor = params.sigmaColor;
    this->params.sigmaSpace = params.sigmaSpace;
    this->params.borderType = cv::BORDER_DEFAULT;
    this->params.numOfTimesApplied = params.numOfTimesApplied;
}

}
