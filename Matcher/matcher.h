#ifndef MATCHER_H
#define MATCHER_H

#include "matcher_global.h"
#include "matcher_config.h"

namespace fvr
{

class MATCHERSHARED_EXPORT Matcher : public QObject
{
    Q_OBJECT

public:
    Matcher(QObject *parent = nullptr);
    ~Matcher();

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

#endif // MATCHER_H
