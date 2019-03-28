#include "featureextraction.h"


FeatureExtraction::FeatureExtraction(QObject *parent)
{
    this->isRunning = false;

    this->input.isLoaded = false;
    this->input.isSequence = false;
    this->input.counter = 0;

    this->extractor = SIFT;

    this->siftParams.numberOfFeatures = 45;
    this->siftParams.numberOfOctaveLayers = 3;
    this->siftParams.contrastThreshold = 0.01;
    this->siftParams.edgeThreshold = 5;
    this->siftParams.sigma = 1.6;

    this->surfParams.hessianThreshold = 170;
    this->surfParams.numberOfOctaves = 4;
    this->surfParams.numberOfOctaveLayers = 3;
    this->surfParams.extended = false;
    this->surfParams.upright = false;
}

FeatureExtraction::~FeatureExtraction()
{
    this->clearInput();
    this->clearResults();
    this->clearSequenceResults();
    this->clearDurations();
}

int FeatureExtraction::loadInput(const cv::Mat& original, const cv::Mat& roi, const cv::Mat& roiContrastEnhanced)
{
    if(this->isRunning)
    {
        // Feature extraction underway
        this->extractionError(10);
        return -1;
    }

    this->input.singleInput.original = original.clone();
    this->input.singleInput.roi = roi.clone();
    this->input.singleInput.roiContrastEnhanced = roiContrastEnhanced.clone();
    this->input.isLoaded = true;

    if(this->input.singleInput.original.channels() != 1)
        cv::cvtColor(this->input.singleInput.original, this->input.singleInput.original, cv::COLOR_BGR2GRAY);

    if(this->input.singleInput.roi.channels() != 1)
        cv::cvtColor(this->input.singleInput.roi, this->input.singleInput.roi, cv::COLOR_BGR2GRAY);

    if(this->input.singleInput.roiContrastEnhanced.channels() != 1)
        cv::cvtColor(this->input.singleInput.roiContrastEnhanced, this->input.singleInput.roiContrastEnhanced, cv::COLOR_BGR2GRAY);

    return 1;
}

int FeatureExtraction::loadInput(const EXTRACTION_PREPROCESSING_RESULT& preprocessingResult)
{
    if(this->isRunning)
    {
        // Feature extraction underway
        this->extractionError(10);
        return -1;
    }

    this->input.singleInput = preprocessingResult;
    this->input.isLoaded = true;

    if(this->input.singleInput.original.channels() != 1)
        cv::cvtColor(this->input.singleInput.original, this->input.singleInput.original, cv::COLOR_BGR2GRAY);

    if(this->input.singleInput.roi.channels() != 1)
        cv::cvtColor(this->input.singleInput.roi, this->input.singleInput.roi, cv::COLOR_BGR2GRAY);

    if(this->input.singleInput.roiContrastEnhanced.channels() != 1)
        cv::cvtColor(this->input.singleInput.roiContrastEnhanced, this->input.singleInput.roiContrastEnhanced, cv::COLOR_BGR2GRAY);

    return 1;
}

int FeatureExtraction::loadInput(const QMap<QString, EXTRACTION_PREPROCESSING_RESULT>& preprocessingResults)
{
    if(this->isRunning)
    {
        // Feature extraction underway
        this->extractionError(10);
        return -1;
    }

    this->input.sequenceInput = preprocessingResults;
    this->input.isSequence = true;
    this->input.isLoaded = true;

    this->input.keys.clear();

    foreach (QString key, this->input.sequenceInput.keys())
    {
        this->input.keys.push_back(key);

        if(this->input.sequenceInput.value(key).original.channels() != 1)
            cv::cvtColor(this->input.sequenceInput.value(key).original,
                         this->input.sequenceInput.value(key).original, cv::COLOR_BGR2GRAY);

        if(this->input.sequenceInput.value(key).roi.channels() != 1)
            cv::cvtColor(this->input.sequenceInput.value(key).roi,
                         this->input.sequenceInput.value(key).roi, cv::COLOR_BGR2GRAY);

        if(this->input.sequenceInput.value(key).roiContrastEnhanced.channels() != 1)
            cv::cvtColor(this->input.sequenceInput.value(key).roiContrastEnhanced,
                         this->input.sequenceInput.value(key).roiContrastEnhanced, cv::COLOR_BGR2GRAY);
    }

    return 1;
}

void FeatureExtraction::start()
{
    if (!this->isRunning)
    {
        this->isRunning = true;
        this->clearResults();
        this->clearSequenceResults();
        this->clearDurations();

        if(this->extractor == SIFT)
        {
            this->sift = cv::xfeatures2d::SIFT::create(this->siftParams.numberOfFeatures, this->siftParams.numberOfOctaveLayers,
                                                       this->siftParams.contrastThreshold, this->siftParams.edgeThreshold,
                                                       this->siftParams.sigma);
        }
        else if(this->extractor == SURF)
        {
            this->surf = cv::xfeatures2d::SURF::create(this->surfParams.hessianThreshold, this->surfParams.numberOfOctaves,
                                                       this->surfParams.numberOfOctaveLayers, this->surfParams.extended,
                                                       this->surfParams.upright);
        }

        if (!this->input.isSequence)
        {
            this->startExtraction(this->input.singleInput);
        }
        else if (!this->input.sequenceInput.empty())
        {
            this->startExtraction(this->input.sequenceInput.value(this->input.keys.at(0)));
        }
   }
   else
   {
      this->extractionError(10);
   }
}

void FeatureExtraction::startExtraction(const EXTRACTION_PREPROCESSING_RESULT& result)
{
    if(this->extractor == SIFT)
    {
        this->timer.start();
        this->sift->detect(this->input.singleInput.roiContrastEnhanced, this->results.keypoints);
        this->timeDurations.siftDetection += this->timer.elapsed();

        this->timer.start();
        this->sift->compute(result.roiContrastEnhanced, this->results.keypoints, this->results.descriptors);
        this->timeDurations.siftDescriptorComputation += this->timer.elapsed();
    }
    else if(this->extractor == SURF)
    {
        this->timer.start();
        this->surf->detect(result.roiContrastEnhanced, this->results.keypoints);
        this->timeDurations.surfDetection += this->timer.elapsed();

        this->timer.start();
        this->surf->compute(result.roiContrastEnhanced, this->results.keypoints, this->results.descriptors);
        this->timeDurations.surfDescriptorComputation += this->timer.elapsed();
    }

    if(!this->input.isSequence)
    {
        emit extractionDoneSignal(this->results);
        emit extractionDurationsSignal(this->timeDurations);

        this->isRunning = false;
    }
    else
    {
        this->resultsMap.insert(this->input.keys.at(this->input.counter), this->results);
        this->clearResults();

        emit extractionProgressSignal((int) (this->input.counter * 1.0 / (this->input.sequenceInput.size() - 1) * 100));

        if(this->input.counter == this->input.sequenceInput.size() - 1)
        {
            this->isRunning = false;

            emit extractionSequenceDoneSignal(this->resultsMap);

            this->clearSequenceResults();
        }
        else
        {
            this->startExtraction(this->input.sequenceInput.value(this->input.keys.at(++this->input.counter)));
        }
    }

}

int FeatureExtraction::setExtractionParams(EXTRACTOR extractor, SIFT_PARAMS siftParams, SURF_PARAMS surfParams)
{
    if(this->isRunning)
    {
        this->extractionError(10);
        return -1;
    }

    this->extractor = extractor;

    this->siftParams.numberOfFeatures = siftParams.numberOfFeatures;
    this->siftParams.numberOfOctaveLayers = siftParams.numberOfOctaveLayers;
    this->siftParams.contrastThreshold = siftParams.contrastThreshold;
    this->siftParams.edgeThreshold = siftParams.edgeThreshold;
    this->siftParams.sigma = siftParams.sigma;

    this->surfParams.hessianThreshold = surfParams.hessianThreshold;
    this->surfParams.numberOfOctaves = surfParams.numberOfOctaves;
    this->surfParams.numberOfOctaveLayers = surfParams.numberOfOctaveLayers;
    this->surfParams.extended = surfParams.extended;
    this->surfParams.upright = surfParams.upright;

    return 1;
}

void FeatureExtraction::clearInput()
{
    this->input.isLoaded = false;
    this->input.isSequence = false;
    this->input.counter = 0;
    this->input.singleInput = {};
    this->input.sequenceInput.clear();
    this->input.keys.clear();
}

void FeatureExtraction::clearDurations()
{
    this->timeDurations.siftDetection = 0;
    this->timeDurations.siftDescriptorComputation = 0;
    this->timeDurations.surfDetection = 0;
    this->timeDurations.surfDescriptorComputation = 0;
}

void FeatureExtraction::clearResults()
{
    this->results.keypoints.clear();
    this->results.descriptors.release();
}

void FeatureExtraction::clearSequenceResults()
{
    this->resultsMap.clear();
}

void FeatureExtraction::extractionError(const int errorCode)
{
    emit extractionErrorSignal(errorCode);
}
