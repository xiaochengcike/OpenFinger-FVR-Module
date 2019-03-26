#ifndef MATCHING_H
#define MATCHING_H

#include "matching_global.h"
#include "matching_config.h"

class MATCHINGSHARED_EXPORT Matching : public QObject
{
    Q_OBJECT

public:
    Matching(QObject *parent = nullptr);
    ~Matching();

    // API
    void identify(const QVector<cv::Mat> &, const QMultiMap<QString, QVector<cv::Mat>> &);
    void verify(const QVector<cv::Mat> &, const QVector<QVector<cv::Mat>> &);

    void setMode(const MODE &);
    void setParams(const int normType = cv::NORM_L2, bool crossCheck = false);

private:
    cv::Ptr<cv::BFMatcher> bfMatcher;
    BF_MATCHER_PARAMS matcherParams;

    MODE mode;

    int threshold;
    std::vector<cv::DMatch> matchedDescriptors;

    bool isRunning;
    QTime timer;
    MATCHING_DURATIONS timeDurations;

private slots:
    void clearResult();
    void matcherError(const int);

signals:
    void identificationDoneSignal(bool, QString, double);
    void verificationDoneSignal(bool);

    void matcherProgressSignal(int);
    void matcherDuration(MATCHING_DURATIONS);
    void matcherErrorSignal(int);
};

#endif // MATCHING_H
