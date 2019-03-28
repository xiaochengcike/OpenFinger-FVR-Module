 #include "matching.h"


Matching::Matching(QObject *parent) : QObject(parent)
{
    this->isRunning = false;
    this->threshold = 45;
    this->matcherParams.normType = cv::NORM_L2;
    this->matcherParams.crossCheck = false;

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
    if (this->isRunning)
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
    this->generateImpostorFVPairs();

    // TODO:
    // this->match();
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

    for (int subject = 0; subject < this->databaseTestParams.numOfSubjects; ++subject)
    {
        for(int image1 = subject * this->databaseTestParams.imgsPerSubject; image1 < subject * this->databaseTestParams.imgsPerSubject + this->databaseTestParams.imgsPerSubject; ++image1)
        {
            for(int image2 = image1 + 1; image2 < subject * this->databaseTestParams.imgsPerSubject + this->databaseTestParams.imgsPerSubject; ++image2)
            {

                this->databaseTestParams.fvrGenuinePairs.push_back(FINGER_VEIN_PAIR{this->databaseTestParams.keys.at(image1), this->databaseTestParams.keys.at(image2), 0});

            }
        }
    }
}

void Matching::generateImpostorFVPairs()
{
    this->databaseTestParams.fvrImpostorPairs.clear();

    for (int image1 = 0; image1 < (this->databaseTestParams.numOfSubjects - 1) * this->databaseTestParams.imgsPerSubject; image1 += this->databaseTestParams.imgsPerSubject)
    {
        for (int image2 = image1 + this->databaseTestParams.imgsPerSubject; image2 < this->databaseTestParams.numOfSubjects * this->databaseTestParams.imgsPerSubject; image2 += this->databaseTestParams.imgsPerSubject)
        {
            this->databaseTestParams.fvrImpostorPairs.push_back(FINGER_VEIN_PAIR{this->databaseTestParams.keys.at(image1), this->databaseTestParams.keys.at(image2), 0});
        }
    }
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
