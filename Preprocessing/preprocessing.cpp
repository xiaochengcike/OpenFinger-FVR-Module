#include "preprocessing.h"

#include <QDebug>


Preprocessing::Preprocessing(QObject *parent) : QObject(parent)
{
    this->isRunning = false;

    this->input.mode = IMAGE;
    this->input.isLoaded = false;
    this->input.counter = 0;

    // Bilateral Filter params
    this->bilateralFilterParams.numOfTimesApplied = 3;
    this->bilateralFilterParams.diameter = 10;
    this->bilateralFilterParams.sigmaColor = 10;
    this->bilateralFilterParams.sigmaSpace = 3;
    this->bilateralFilterParams.borderType = cv::BORDER_DEFAULT;

    // Canny Edge Detector params
    this->cannyParams.t1 = 0;
    this->cannyParams.t2 = 255;

    // Find Contours params
    this->contourParams.method = cv::RETR_LIST;
    this->contourParams.mode = cv::CHAIN_APPROX_NONE;

    // Connections
    connect(&this->bilateralFilter, SIGNAL(BilateralFilterErrorSignal(int)), this, SLOT(errorCaught(int)));
    connect(&this->edgeProcessing, SIGNAL(EdgeProcessingErrorSignal(int)), this, SLOT(errorCaught(int)));
    connect(&this->roiExtraction, SIGNAL(RoiExtractionErrorSignal(int)), this, SLOT(errorCaught(int)));
}

Preprocessing::~Preprocessing()
{
    this->clearInput();
    this->clearDurations();
    this->clearResults();
}

void Preprocessing::start()
{
    if(!this->isRunning)
    {
        if(this->input.isLoaded)
        {
            this->clearResults();
            this->clearDurations();
            this->isRunning = true;

            if(this->input.mode == IMAGE || this->input.mode == IMAGE_PATH)
            {
                this->allResults.original = this->input.image.clone();
                this->startPreprocessing(this->allResults.original.clone());
            }
            else if(!this->input.images.isEmpty())
            {
                this->allResults.original = this->input.images.at(0);
                this->startPreprocessing(this->allResults.original);
            }
        }
        else
        {
            // Invalid input file
            this->preprocessingError(20);
        }
    }
    else
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
    }
}

void Preprocessing::startPreprocessing(const cv::Mat& image)
{
    // Crop to remove additional side noise.
    // Note that the values presented as input in rectangle were acquired experimentally.
    cv::Rect r = cv::Rect(20, 30, 220, 200);
    this->allResults.cropped = image(r);

    // Bilateral filter application
    this->bilateralFilter.setParams(this->bilateralFilterParams);
    this->bilateralFilter.setOriginal(this->allResults.cropped);
    this->timer.start();
    this->allResults.bilateralApplied = this->bilateralFilter.applyFilter().clone();
    this->timeDurations.bilateralFilterApplication += this->timer.elapsed();

    // Edge processing
    this->edgeProcessing.setOriginal(this->allResults.bilateralApplied);
    this->edgeProcessing.setParams(this->cannyParams, this->contourParams);

    // 1. Canny
    this->timer.start();
    this->allResults.cannyOutput = this->edgeProcessing.cannyEdgeDetection().clone();
    this->timeDurations.cannyEdgeDetection += this->timer.elapsed();

    // 2. Contour detection
    this->timer.start();
    this->allResults.contours = this->edgeProcessing.contourDetection();
    this->timeDurations.contourDetection = this->timer.elapsed();

    // ROI Extraction
    this->roiExtraction.setOriginal(this->allResults.cropped.clone());
    this->roiExtraction.setCannyOutput(this->allResults.cannyOutput.clone());
    this->roiExtraction.setContours(this->allResults.contours);

    this->timer.start();
    this->allResults.roi = this->roiExtraction.findROI().clone();
    this->timeDurations.roiExtraction += this->timer.elapsed();
    this->allResults.endpoints = this->roiExtraction.getEndpoints();

    // ROI enhancement
    cv::equalizeHist(this->allResults.roi, this->allResults.roiContrastEnhanced);

    this->preprocessingDone();
}

void Preprocessing::preprocessingDone()
{
    if(this->input.mode == IMAGE || this->input.mode == IMAGE_PATH)
    {
        this->isRunning = false;

        if(this->isAdvancedMode)
        {
            emit PreprocessingDoneSignal(this->allResults);
        }
        else
        {
            PREPROCESSING_RESULT result =
            {
                this->allResults.original,
                this->allResults.roi,
                this->allResults.roiContrastEnhanced
            };

            emit PreprocessingDoneSignal(result);
        }

        emit PreprocessingDurationSignal(this->timeDurations);

        this->clearResults();
    }
    else
    {
        if(this->isAdvancedMode)
        {
            this->allResultsMap.insert(this->input.imageNames[this->input.counter], this->allResults);
        }
        else
        {
            PREPROCESSING_RESULT result =
            {
                this->allResults.original,
                this->allResults.roi,
                this->allResults.roiContrastEnhanced
            };

            this->resultsMap.insert(this->input.imageNames[this->input.counter], result);
        }

        emit PreprocessingProgressSignal((int) (this->input.counter * 1.0 / (this->input.images.size() - 1) * 100));

        if(this->input.counter == (this->input.images.size() - 1))
        {
            this->isRunning = false;

            if(this->isAdvancedMode)
            {
                emit this->PreprocessingSequenceDoneSignal(this->allResultsMap);
            }
            else
            {
                emit PreprocessingSequenceDoneSignal(this->resultsMap);
            }

            emit PreprocessingDurationSignal(this->timeDurations);

            this->clearResults();
        }
        else
        {
            this->allResults.original = this->input.images.at(++(this->input.counter));
            this->startPreprocessing(this->allResults.original);
        }
    }
}


int Preprocessing::loadInput(const cv::Mat image)
{
    if(this->isRunning)
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
        return -1;
    }

    this->clearInput();

    this->input.image = image.clone();
    this->input.isLoaded = true;
    this->input.mode = IMAGE;

    if(this->input.image.channels() != 1)
        cv::cvtColor(this->input.image, this->input.image, cv::COLOR_BGR2GRAY);
}


int Preprocessing::loadInput(const QVector<cv::Mat> images)
{
    if(this->isRunning)
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
        return -1;
    }

    this->clearInput();

    this->input.images = images;
    this->input.isLoaded = true;
    this->input.mode = IMAGES;

    for(int i = 0; i < this->input.images.size(); ++i)
    {
        cv::Mat tmp;
        tmp.setTo(0);
        if(this->input.images.at(i).channels() != 1)
        {
            cv::cvtColor(this->input.images.at(i), tmp, cv::COLOR_BGR2GRAY);
            this->input.images.replace(i, tmp);
            this->input.imageNames.push_back(QString::number(i + 1));
        }
    }

    return 1;
}

int Preprocessing::loadInput(const QString inputPath)
{
    if(this->isRunning)
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
        return -1;
    }

    this->clearInput();

    this->input.inputPath = inputPath;

    QFileInfo pathInfo(this->input.inputPath);

    if(pathInfo.isDir())
    {
        QDir dirImages(this->input.inputPath, "*.png *.jpg *.bmp *.tif");
        QListIterator<QFileInfo> it(dirImages.entryInfoList());

        while(it.hasNext())
        {
            this->input.imageNames.push_back((it.peekNext()).baseName());
            this->input.images.push_back(cv::imread((it.next()).absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE));
        }

        this->input.mode = IMAGE_DIR;
    }
    else if(pathInfo.isFile())
    {
        QStringList fileTypes;
        fileTypes << "png" << "jpg" << "bmp" << "tif";

        if(fileTypes.contains(pathInfo.completeSuffix().toLower()))
        {
            this->input.image = cv::imread(pathInfo.absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE);
        }
        else
        {
            this->input.isLoaded = false;

            // Invalid file type
            this->preprocessingError(22);
        }

        this->input.mode = IMAGE_PATH;
    }
    else
    {
        this->input.isLoaded = false;

        // Invalid input path
        this->preprocessingError(21);
    }

    this->input.isLoaded = true;

    return 1;
}

int Preprocessing::setPreprocessingParams(const int bilateralDiameter, const int bilateralSigmaColor,
                                          const int bilateralSigmaSpace, const int bilateralBorderType,
                                          const int numOfBilateralFilterApplications, const int cannyThreshold1,
                                          const int cannyThreshold2, const int contoursMethod,
                                          const int contoursMode)
{
    if(this->isRunning)
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
        return -1;
    }

    this->bilateralFilterParams.numOfTimesApplied = numOfBilateralFilterApplications;
    this->bilateralFilterParams.diameter = bilateralDiameter;
    this->bilateralFilterParams.sigmaColor = bilateralSigmaColor;
    this->bilateralFilterParams.sigmaSpace = bilateralSigmaSpace;
    this->bilateralFilterParams.borderType = bilateralBorderType;
    this->cannyParams.t1 = cannyThreshold1;
    this->cannyParams.t2 = cannyThreshold2;
    this->contourParams.method = contoursMethod;
    this->contourParams.mode = contoursMode;

    return 1;
}


int Preprocessing::setAdvancedMode(bool useAdvancedMode)
{
    if(this->isRunning)
    {
        // Preprocessing is already underway
        this->preprocessingError(10);
        return -1;
    }

    this->isAdvancedMode = useAdvancedMode;

    return 1;
}

void Preprocessing::clearResults()
{
    this->allResults.original.release();
    this->allResults.cropped.release();
    this->allResults.bilateralApplied.release();
    this->allResults.cannyOutput.release();
    this->allResults.contours.clear();
    this->allResults.endpoints.clear();
    this->allResults.roi.release();
    this->allResults.roiContrastEnhanced.release();

    this->resultsMap.clear();
    this->allResultsMap.clear();
}

void Preprocessing::clearDurations()
{
    this->timeDurations.bilateralFilterApplication = 0;
    this->timeDurations.cannyEdgeDetection = 0;
    this->timeDurations.contourDetection = 0;
    this->timeDurations.roiExtraction = 0;
}

void Preprocessing::clearInput()
{
    this->input.image.release();
    this->input.images.clear();
    this->input.isLoaded = false;
    this->input.imageNames.clear();
    this->input.mode = IMAGE;
    this->input.inputPath.clear();
    this->input.counter = 0;
}

void Preprocessing::preprocessingError(const int errorCode)
{
    emit PreprocessingErrorSignal(errorCode);
}

void Preprocessing::errorCaught(const int errorCode)
{
    std::cout << "Error occurred! Error code: " << errorCode << "\n";
}
