 #include "matching.h"


Matching::Matching(QObject *parent) : QObject(parent)
{
    this->isRunning = false;
    this->threshold = 45;
    this->matcherParams.normType = cv::NORM_L2;
    this->matcherParams.crossCheck = true;

    this->bfMatcher = cv::BFMatcher::create(this->matcherParams.normType, this->matcherParams.crossCheck);

    this->databaseTestParams.imgsPerSubject = 0;
    this->databaseTestParams.numOfSubjects = 0;
    this->databaseTestResults.fvrPlotParams = {0, 100, 1};

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

int Matching::setMode(const RECOGNITION_MODE &value)
{
    if (this->isRunning)
    {
        // Matching already underway
        this->matcherError(10);
        return -1;
    }

    this->mode = value;
}

int Matching::setParams(const int normType, bool crossCheck)
{
    if (this->isRunning)
    {
        // Matching already underway
        this->matcherError(10);
        return -1;
    }

    this->matcherParams.normType = normType;
    this->matcherParams.crossCheck = crossCheck;
}

int Matching::setDatabaseParams(const int numOfSubjects, const int imgsPerSubject)
{
    if (this->isRunning)
    {
        // Matching already underway
        this->matcherError(10);
        return -1;
    }

    this->databaseTestParams.numOfSubjects = numOfSubjects;
    this->databaseTestParams.imgsPerSubject = imgsPerSubject;
}

void Matching::testDatabase(const QMap<QString, QVector<cv::Mat>>& database)
{
    /*if (this->isRunning)
    {
        this->matcherError(10);
        return;
    }

    this->isRunning = true;

    this->bfMatcherTemplates.clear();

    for (auto i = database.begin(); i != database.end(); ++i)
    {
        this->databaseTestParams.keys.push_back(i.key());
        this->bfMatcherTemplates.insert(i.key(), i.value());
    }

    this->generateGenuineFVPairs();
    this->generateImpostorFVPairs();*/

    // TODO:
    // this->match();
}

void Matching::loadInput(const QMap<QString, QPair<std::vector<int>, cv::Mat>>& input)
{
    for(int i = 1; i <= input.keys().size(); ++i)
    {
        if(i % 2 == 1)
        {
            this->bfMatcherTemplates[i-1].first = input.values().at(i - 1).second;
        }
        else
        {
            this->bfMatcherTemplates[i].second = input.values().at(i).second;
        }
    }
}

void Matching::match()
{
    for(int i = 0; this->bfMatcherTemplates.size(); ++i)
    {
        std::vector<cv::DMatch> matches;
        this->bfMatcher->match(this->bfMatcherTemplates[i].first, this->bfMatcherTemplates[i].second, matches);
        this->matchedDescriptors.insert(matches.size(), matches);
    }

    emit matchingDone(this->matchedDescriptors);

    this->clearResult();
}

void Matching::generateFVPairs()
{
    this->fingerveinPairs.clear();

    for (auto i = this->fvAlternativeNames.begin(); i != this->fvAlternativeNames.end(); ++i)
    {
        this->fingerveinPairs.push_back(FINGER_VEIN_PAIR{"Subject", i.key(), 0});
    }
}

void Matching::generateGenuineFVPairs()
{
    this->databaseTestParams.fvrGenuinePairs.clear();


}

void Matching::generateImpostorFVPairs()
{
    this->databaseTestParams.fvrImpostorPairs.clear();


}

int Matching::findEntryWithHighestScore()
{
    int index = 0;
    double maxScore = this->fingerveinPairs.at(0).score;

    for(int i = 0; i < this->fingerveinPairs.size(); ++i)
    {
        if(this->fingerveinPairs.at(i).score > maxScore)
        {
            maxScore = this->fingerveinPairs.at(i).score;
            index = i;
        }
    }

    return index;
}

double Matching::computeEER()
{
    QVector<double> absDiff;

    for(int i=0; i < this->databaseTestResults.farY.size(); ++i)
    {
        absDiff.push_back(qAbs(this->databaseTestResults.farY.at(i) - this->databaseTestResults.frrY.at(i)));
    }

    double smallestDiff = *std::min_element(absDiff.begin(), absDiff.end());

    for(int i=0; i < absDiff.size(); ++i)
    {
        if(absDiff.at(i) == smallestDiff)
        {
            return (this->databaseTestResults.farY.at(i) + this->databaseTestResults.frrY.at(i)) / 2.0;
        }
    }

    return 0;
}

void Matching::computeROC()
{
    for(int i = 0; i < this->databaseTestResults.frrY.size(); ++i)
    {
        this->databaseTestResults.rocY.push_back(1 - this->databaseTestResults.frrY.at(i) / 100.0);
    }

    for(int i = 0; i < this->databaseTestResults.farY.size(); ++i)
    {
        this->databaseTestResults.rocX.push_back(this->databaseTestResults.farY.at(i) / 100.0);
    }
}

void Matching::clearDatabaseTestResults()
{
    this->databaseTestResults.eer = -1;
    this->databaseTestResults.farX.clear();
    this->databaseTestResults.farY.clear();
    this->databaseTestResults.frrX.clear();
    this->databaseTestResults.frrY.clear();
    this->databaseTestResults.rocX.clear();
    this->databaseTestResults.rocY.clear();
    this->databaseTestResults.fvrPlotParams.max = 0;
    this->databaseTestResults.fvrPlotParams.min = 0;
    this->databaseTestResults.fvrPlotParams.sensitivity = 0;
}

void Matching::clearDatabaseTestParams()
{
    this->databaseTestParams.database.clear();
    this->databaseTestParams.fvrGenuinePairs.clear();
    this->databaseTestParams.fvrImpostorPairs.clear();
    this->databaseTestParams.keys.clear();
    this->databaseTestParams.imgsPerSubject = 0;
    this->databaseTestParams.numOfSubjects = 0;
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
