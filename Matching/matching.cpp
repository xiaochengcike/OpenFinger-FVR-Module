#include "matching.h"


Matching::Matching(QObject *parent) : QObject(parent)
{
    this->isRunning = false;
    this->threshold = 45;
    this->matcherParams.normType = cv::NORM_L2;
    this->matcherParams.crossCheck = false;

    this->clearResult();
}

Matching::~Matching()
{

}

void Matching::identify(const QVector<cv::Mat>& subject, const QMultiMap<QString, QVector<cv::Mat>>& database)
{
    if (this->isRunning)
    {
        // Matching already underway
        this->matcherError(10);
        return;
    }

    this->isRunning = true;
    this->mode = IDENTIFICATION;
    this->bfMatcher = cv::BFMatcher::create(this->matcherParams.normType, this->matcherParams.crossCheck);




}

void Matching::verify(const QVector<cv::Mat>& subject, const QVector<QVector<cv::Mat>>& database)
{
    if (this->isRunning)
    {
        // Matching already underway
        this->matcherError(10);
        return;
    }

    this->isRunning = true;
    this->mode = VERIFICATION;
    this->bfMatcher = cv::BFMatcher::create(this->matcherParams.normType, this->matcherParams.crossCheck);

}

void Matching::setMode(const MODE &value)
{
    this->mode = value;
}

void Matching::setParams(const int normType, bool crossCheck)
{
    this->matcherParams.normType = normType;
    this->matcherParams.crossCheck = crossCheck;
}

void Matching::clearResult()
{
    this->matchedDescriptors.clear();
    this->timeDurations.identificationDuration = 0;
    this->timeDurations.verificationDuration = 0;
}

void Matching::matcherError(const int errorCode)
{
    emit matcherErrorSignal(errorCode);
}
