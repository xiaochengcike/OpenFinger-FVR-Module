#ifndef BILATERALFILTERING_H
#define BILATERALFILTERING_H

#include "preprocessing_config.h"

class BilateralFiltering : public QObject
{
    Q_OBJECT

public:
    BilateralFiltering(QObject *parent = nullptr);
    ~BilateralFiltering();

    void setOriginal(const cv::Mat &);
    void setParams(const BILATERAL_FILTER_PARAMS);
    cv::Mat applyFilter();

signals:
    void BilateralFilterErrorSignal(int);

private:
    cv::Mat original;
    cv::Mat blurred;
    cv::Mat tmp1;
    cv::Mat tmp2;

    BILATERAL_FILTER_PARAMS params;

    void bilateralFilterError(const int);
    void clearParams();
    void clearResults();
};

#endif // BILATERALFILTERING_H
