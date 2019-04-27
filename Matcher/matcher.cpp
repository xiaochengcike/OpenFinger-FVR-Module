#include "matcher.h"

namespace fvr
{

Matcher::Matcher(QObject *parent) : QObject(parent)
{
    this->matcherType = BRUTEFORCE;
    this->bfMatcherParams.crossCheck = true;
    this->bfMatcherParams.normType = cv::NORM_L2;
    this->bfMatcher = cv::BFMatcher::create(this->bfMatcherParams.normType, this->bfMatcherParams.crossCheck);

    this->inputDescriptors.inputLoaded = false;
}

Matcher::~Matcher()
{
    this->bfMatcher.release();
    this->flannMatcher.release();
}

void Matcher::start()
{
    if(this->inputDescriptors.inputLoaded)
    {
        if(this->matcherType == BRUTEFORCE)
        {
            this->matchBruteForce();
        }
        else if(this->matcherType == FLANN)
        {
            this->flannMatcher = cv::FlannBasedMatcher::create();
            this->matchFlannBased();
        }

        this->clearInput();
        this->clearResults();
    }
    else
    {
        this->matcherError(400);
    }
}

int Matcher::loadInput(const cv::Mat& descriptors1, const cv::Mat& descriptors2)
{
    this->inputDescriptors.descriptors1 = descriptors1;
    this->inputDescriptors.descriptors2 = descriptors2;
    this->inputDescriptors.inputLoaded = true;
}

int Matcher::setMatcher(const MATCHER_TYPE type)
{
    this->matcherType = type;
}

int Matcher::setBFMatcherParams(const bool crossCheck, const int normType)
{
    this->bfMatcherParams.crossCheck = crossCheck;
    this->bfMatcherParams.normType = normType;
}

void Matcher::matchBruteForce()
{
    this->bfMatcher->match(this->inputDescriptors.descriptors1, this->inputDescriptors.descriptors2, this->matches);

    emit this->matchingDoneSignal(this->matches);
}

void Matcher::matchFlannBased()
{
    this->flannMatcher->match(this->inputDescriptors.descriptors1, this->inputDescriptors.descriptors2, this->matches);

    emit this->matchingDoneSignal(this->matches);
}

void Matcher::resetBFMatcherParams()
{
    this->bfMatcherParams.normType = cv::NORM_L2;
    this->bfMatcherParams.crossCheck = true;
}

void Matcher::clearResults()
{
    this->matches.clear();
}

void Matcher::clearInput()
{
    this->inputDescriptors.descriptors1.release();
    this->inputDescriptors.descriptors2.release();
    this->inputDescriptors.inputLoaded = false;
}

void Matcher::matcherError(const int errorCode)
{
    emit matcherErrorSignal(errorCode);
}

}
