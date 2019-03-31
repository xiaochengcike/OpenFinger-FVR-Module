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

    int setMode(const RECOGNITION_MODE &);
    int setParams(const int normType = cv::NORM_L2, bool crossCheck = false);

    int setDatabaseParams(const int, const int);
    void testDatabase(const QMap<QString, QVector<cv::Mat>> &);

    void loadInput(const QMap<QString, QPair<std::vector<int>, cv::Mat>>&);

private:
    cv::Ptr<cv::BFMatcher> bfMatcher;
    BF_MATCHER_PARAMS matcherParams;

    RECOGNITION_MODE mode;

    int threshold;
    QMultiMap<int, std::vector<cv::DMatch>> matchedDescriptors;

    QVector<QPair<cv::Mat, cv::Mat>> bfMatcherTemplates;

    QVector<FINGER_VEIN_PAIR> fingerveinPairs;
    QMap<QString, QString> fvAlternativeNames;

    DB_PARAMS databaseTestParams;
    DB_TEST_RESULTS databaseTestResults;

    bool isRunning;
    QTime timer;
    MATCHING_DURATIONS timeDurations;

    void generateFVPairs();
    void generateGenuineFVPairs();
    void generateImpostorFVPairs();
    void match();

    int findEntryWithHighestScore();
    double computeEER();
    void computeROC();

    void clearDatabaseTestResults();
    void clearDatabaseTestParams();
    void clearResult();

private slots:
    void matcherError(const int);

signals:
    void identificationDoneSignal(bool, QString, double);
    void verificationDoneSignal(bool);
    void matchingDone(QMultiMap<int, std::vector<cv::DMatch>>);

    void matcherProgressSignal(int);
    void matcherDuration(MATCHING_DURATIONS);
    void matcherErrorSignal(int);
};

#endif // MATCHING_H
