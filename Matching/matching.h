#ifndef MATCHING_H
#define MATCHING_H

#include "matching_global.h"
#include "matching_config.h"

namespace fvr
{

class MATCHINGSHARED_EXPORT Matching : public QObject
{
    Q_OBJECT

public:
    Matching(QObject *parent = nullptr);
    ~Matching();

    // API
    void start();

    int loadInput(const cv::Mat &, const cv::Mat &);
    int setMatcher(const MATCHER_TYPE type = BRUTEFORCE);
    int setBFMatcherParams(const bool crossCheck = true, const int normType = cv::NORM_L2);

private:
    cv::Ptr<cv::BFMatcher> bfMatcher;
    cv::Ptr<cv::FlannBasedMatcher> flannMatcher;

    INPUT inputDescriptors;

    MATCHER_TYPE matcherType;
    MATCHER_PARAMS bfMatcherParams;

    std::vector<cv::DMatch> matches;

private slots:
    void matchBruteForce();
    void matchFlannBased();
    void resetBFMatcherParams();
    void clearResults();
    void clearInput();
    void matcherError(const int);

signals:
    void matchingDoneSignal(std::vector<cv::DMatch>);
    void matcherErrorSignal(int);

};

}

#endif // MATCHING_H
