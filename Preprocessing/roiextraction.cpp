#include "roiextraction.h"

RoiExtraction::RoiExtraction(QObject *parent) : QObject(parent)
{

}

RoiExtraction::~RoiExtraction()
{
    this->clearResults();
}

cv::Mat RoiExtraction::findROI()
{
    this->findEdgeContours();
    this->findEndpoints();
    cv::Rect rect = cv::Rect(0, 0, 190, 80);

    if(this->endpoints.size() == 4)
    {
        this->findMinMaxXY();
        this->roi = this->original(cv::Range(this->minY, this->maxY),
                                   cv::Range(this->minX, this->maxX));

        if(this->roi.rows < 80 && this->roi.cols < 150)
        {
            if(this->original.cols > this->original.rows)
            {
                return this->original(cv::Rect(cv::Point(0, this->original.rows - 150), cv::Point(this->original.cols, this->original.rows - 80)));
            }
            else
            {
                return this->original(cv::Rect(cv::Point(this->original.cols - 150, 0), cv::Point(this->original.cols - 80, this->original.rows)));
            }
        }
    }
    else
    {
        this->roi = this->emergencyROI();

        if(this->original.cols > this->original.rows)
        {
            return this->original(cv::Rect(cv::Point(0, this->original.rows - 150), cv::Point(this->original.cols, this->original.rows - 80)));
        }
        else
        {
            return this->original(cv::Rect(cv::Point(this->original.cols - 150, 0), cv::Point(this->original.cols - 80, this->original.rows)));
        }
    }

    return this->roi;
}

cv::Mat RoiExtraction::emergencyROI()
{
    bool firstFound = false;
    bool secondFound = false;

    for(int i = 0; i < this->longestContour.size(); ++i)
    {
        cv::Point p = this->longestContour.at(i);

        if((p.x <= 10 || p.y <= 10) && !firstFound)
        {
            if(scanNeighborhood(this->longestContour, i))
            {
                this->endpoints.push_back(this->longestContour.at(i));
                firstFound = true;
            }
        }
        else if(firstFound)
            break;
    }

    for(int i = 0; i < this->secondLongestContour.size(); ++i)
    {
        cv::Point p = this->secondLongestContour.at(i);

        if((p.x >= this->cannyOutput.cols - 10 || p.y >= this->cannyOutput.rows - 10) && !secondFound)
        {
            if(scanNeighborhood(this->secondLongestContour, i))
            {
                this->endpoints.push_back(this->secondLongestContour.at(i));
                secondFound = true;
            }
        }
        else if(secondFound)
            break;
    }

    if(firstFound && secondFound)
    {
        return this->original(cv::Rect(this->endpoints.at(0), this->endpoints.at(1)));
    }
    else
    {
        if(firstFound)
        {
            if(this->endpoints.at(0).x <= 10)
            {
                if(this->endpoints.at(0).y < this->original.rows / 2)
                {
                    return this->original(cv::Rect(this->endpoints.at(0), cv::Point(this->original.cols, this->endpoints.at(0).y + 150)));
                }
                else
                {
                    return this->original(cv::Rect(this->endpoints.at(0), cv::Point(this->original.cols, this->endpoints.at(0).y - 150)));
                }
            }
            else if(this->endpoints.at(0).y <= 10)
            {
                if(this->endpoints.at(0).x < this->original.cols / 2)
                {
                    return this->original(cv::Rect(this->endpoints.at(0), cv::Point(this->endpoints.at(0).x + 150, this->original.rows)));
                }
                else
                {
                    return this->original(cv::Rect(this->endpoints.at(0), cv::Point(this->endpoints.at(0).x - 150, this->original.rows)));
                }
            }
        }
        else if(secondFound)
        {
            if(this->endpoints.at(0).x >= this->cannyOutput.cols - 10)
            {
                if(this->endpoints.at(0).y < this->original.rows)
                {
                    return this->original(cv::Rect(cv::Point(0, this->endpoints.at(0).y + 150), this->endpoints.at(0)));
                }
                else
                {
                    return this->original(cv::Rect(cv::Point(0, this->endpoints.at(0).y - 150), this->endpoints.at(0)));
                }
            }
            else if(this->endpoints.at(0).y >= this->cannyOutput.rows - 10)
            {
                if(this->endpoints.at(0).x < this->original.cols)
                {
                    return this->original(cv::Rect(cv::Point(this->endpoints.at(0).x + 150, 0), this->endpoints.at(0)));
                }
                else
                {
                    return this->original(cv::Rect(cv::Point(this->endpoints.at(0).x - 150, 0), this->endpoints.at(0)));
                }
            }
        }
        else
        {
            if(this->original.cols > this->original.rows)
            {
                return this->original(cv::Rect(cv::Point(0, this->original.rows - 150), cv::Point(this->original.cols, this->original.rows - 80)));
            }
            else
            {
                return this->original(cv::Rect(cv::Point(this->original.cols - 150, 0), cv::Point(this->original.cols - 80, this->original.rows)));
            }
        }
    }

}

void RoiExtraction::findEdgeContours()
{
    this->longestContour.clear();
    this->secondLongestContour.clear();

    int longestSize = 0;
    int secondLongestSize = 0;

    if(this->contours.empty())
    {
        // No contours
        this->roiExtractionError(50);
    }

    for(size_t i = 0; i < this->contours.size(); ++i)
    {
        if(longestSize < contours.at(i).size())
        {
            longestSize = contours.at(i).size();
            this->longestContour = contours.at(i);
        }
        else if((secondLongestSize <= this->contours.at(i).size()) && (longestSize >= this->contours.at(i).size()))
        {
            secondLongestSize = contours.at(i).size();
            this->secondLongestContour = contours.at(i);
        }
    }
}

void RoiExtraction::findEndpoints()
{
    if(!this->longestContour.empty() && !this->secondLongestContour.empty())
    {
        this->endpoints.clear();
        this->findEndpointsInLongestContour();
        this->findEndpointsInSecondLongestContour();
    }
    else
    {
        // No contours found
        this->roiExtractionError(50);
    }
}

void RoiExtraction::findEndpointsInLongestContour()
{
    bool firstFound = false;
    bool secondFound = false;

    for(int i = 0; i < this->longestContour.size(); ++i)
    {
        cv::Point p = this->longestContour.at(i);

        if((p.x <= 5 || p.y <= 5) && !firstFound)
        {
            if(scanNeighborhood(this->longestContour, i))
            {
                this->endpoints.push_back(this->longestContour.at(i));
                firstFound = true;
            }
        }
        else if(firstFound)
            break;
    }

    for(int i = 0; i < this->longestContour.size(); ++i)
    {
        cv::Point p = this->longestContour.at(i);

        if((p.x >= this->cannyOutput.cols - 5 || p.y >= this->cannyOutput.rows - 5) && !secondFound)
        {
            if(scanNeighborhood(this->longestContour, i))
            {
                this->endpoints.push_back(this->longestContour.at(i));
                secondFound = true;
            }
        }
        else if(secondFound)
            break;
    }
}

void RoiExtraction::findEndpointsInSecondLongestContour()
{
    bool firstFound = false;
    bool secondFound = false;

    for(int i = 0; i < this->secondLongestContour.size(); ++i)
    {
        cv::Point p = this->secondLongestContour.at(i);
        if((p.x <= 5 || p.y <= 5) && !firstFound)
        {
            if(scanNeighborhood(this->secondLongestContour, i))
            {
                this->endpoints.push_back(this->secondLongestContour.at(i));
                firstFound = true;
            }
        }
        else if(firstFound)
            break;
    }

    for(int i = 0; i < this->secondLongestContour.size(); ++i)
    {
        cv::Point p = this->secondLongestContour.at(i);
        if((p.x >= this->cannyOutput.cols - 5 || p.y >= this->cannyOutput.rows - 5) && !secondFound)
        {
            if(scanNeighborhood(this->secondLongestContour, i))
            {
                this->endpoints.push_back(this->secondLongestContour.at(i));
                secondFound = true;
            }
        }
        else if(secondFound)
            break;
    }
}

bool RoiExtraction::scanNeighborhood(std::vector<cv::Point>& contour, const int pointIndex)
{
    cv::Point p = contour.at(pointIndex);
    cv::Point startingPoint;
    int neighbors = 0;
    bool hasNeighbor = false;

    if(p.x <= 5)
    {
        startingPoint = cv::Point(p.x, p.y);
        neighbors = 0;

        while(startingPoint.x < this->cannyOutput.cols)
        {
            if(neighbors > 30)
                return true;

            if(this->cannyOutput.at<uchar>(startingPoint.y, startingPoint.x + 1) == 255)
            {
                ++neighbors;
                startingPoint.x += 1;
                hasNeighbor = true;
            }

            if(this->cannyOutput.at<uchar>(startingPoint.y - 1, startingPoint.x + 1) == 255)
            {
                ++neighbors;
                startingPoint.x += 1;
                startingPoint.y -= 1;
                hasNeighbor = true;
            }

            if(this->cannyOutput.at<uchar>(startingPoint.y + 1, startingPoint.x + 1) == 255)
            {
                ++neighbors;
                startingPoint.x += 1;
                startingPoint.y += 1;
                hasNeighbor = true;
            }

            if(!hasNeighbor)
                break;

            hasNeighbor = false;
        }
    }
    else if(p.x >= this->cannyOutput.cols - 5)
    {
        startingPoint = cv::Point(p.x, p.y);
        neighbors = 0;
        hasNeighbor = false;

        while(startingPoint.x >= 0)
        {
            if(neighbors > 20)
                return true;

            if(this->cannyOutput.at<uchar>(startingPoint.y, startingPoint.x - 1) == 255)
            {
                ++neighbors;
                startingPoint.x -= 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y - 1, startingPoint.x - 1) == 255)
            {
                ++neighbors;
                startingPoint.x -= 1;
                startingPoint.y -= 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y + 1, startingPoint.x - 1) == 255)
            {
                ++neighbors;
                startingPoint.x -= 1;
                startingPoint.y += 1;
                hasNeighbor = true;
            }

            if(!hasNeighbor)
                break;

            hasNeighbor = false;
        }
    }
    else if(p.y <= 5)
    {
        startingPoint = cv::Point(p.x, p.y);
        neighbors = 0;
        hasNeighbor = false;

        while(startingPoint.y < this->cannyOutput.rows)
        {
            if(neighbors > 50)
                return true;

            if(this->cannyOutput.at<uchar>(startingPoint.y + 1, startingPoint.x) == 255)
            {
                ++neighbors;
                startingPoint.y += 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y + 1, startingPoint.x - 1) == 255)
            {
                ++neighbors;
                startingPoint.x -= 1;
                startingPoint.y += 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y + 1, startingPoint.x + 1) == 255)
            {
                ++neighbors;
                startingPoint.x += 1;
                startingPoint.y += 1;
                hasNeighbor = true;
            }

            if(!hasNeighbor)
                break;

            hasNeighbor = false;
        }
    }
    else if(p.y >= this->cannyOutput.rows - 5)
    {
        startingPoint = cv::Point(p.x, p.y);
        neighbors = 0;
        hasNeighbor = false;

        while(startingPoint.y >= 0)
        {
            if(neighbors > 50)
                return true;

            if(this->cannyOutput.at<uchar>(startingPoint.y - 1, startingPoint.x) == 255)
            {
                ++neighbors;
                startingPoint.y -= 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y - 1, startingPoint.x - 1) == 255)
            {
                ++neighbors;
                startingPoint.x -= 1;
                startingPoint.y -= 1;
                hasNeighbor = true;
            }
            else if(this->cannyOutput.at<uchar>(startingPoint.y - 1, startingPoint.x + 1) == 255)
            {
                ++neighbors;
                startingPoint.x += 1;
                startingPoint.y -= 1;
                hasNeighbor = true;
            }

            if(!hasNeighbor)
                break;

            hasNeighbor = false;
        }
    }

    return false;
}

void RoiExtraction::findMinMaxXY()
{
    this->minX = UCHAR_MAX;
    this->minY = UCHAR_MAX;
    this->maxX = 0;
    this->maxY = 0;

    for(int i = 0; i < this->endpoints.size(); ++i)
    {
        if(this->endpoints.at(i).x < this->minX)
            this->minX = this->endpoints.at(i).x;

        if(this->endpoints.at(i).y < this->minY)
            this->minY = this->endpoints.at(i).y;

        if(this->endpoints.at(i).x > this->maxX)
            this->maxX = this->endpoints.at(i).x;

        if(this->endpoints.at(i).y > this->maxY)
            this->maxY = this->endpoints.at(i).y;
    }
}

void RoiExtraction::clearResults()
{
    this->original.release();
    this->roi.release();
    this->contours.clear();
    this->longestContour.clear();
    this->secondLongestContour.clear();
    this->endpoints.clear();
    this->minX = 0;
    this->maxX = 0;
    this->minY = 0;
    this->maxY = 0;
}

void RoiExtraction::roiExtractionError(const int errorCode)
{
    emit RoiExtractionErrorSignal(errorCode);
}

void RoiExtraction::setContours(const std::vector<std::vector<cv::Point> > &value)
{
    this->contours = value;
}

void RoiExtraction::setCannyOutput(const cv::Mat &value)
{
    this->cannyOutput = value;
}

void RoiExtraction::setOriginal(const cv::Mat &value)
{
    this->original = value;
}

std::vector<cv::Point> RoiExtraction::getEndpoints() const
{
    return this->endpoints;
}


