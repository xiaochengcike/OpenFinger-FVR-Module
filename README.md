# OpenFinger's Finger Vein Recognition Module Guide

## Dependencies
- [OpenCV 4.0.1](https://github.com/opencv/opencv)
- [Qt5 / Qt Creator 4](https://www.qt.io/download)

## Getting Started: 
1. Build the libraries to generate .so (.dll / .lib) files.
2. Provide a valid path to each library and its header in your ```.pro``` file.
4. Include each library header in your application.

<br />  

The module itself consists of three lesser modules. Namely, each module represents a particular phase in  the recognition process. The modules and their APIs are further described as follows.

## Preprocessing Module

## API
**Required**
```cpp
int loadInput(const cv::Mat image);
int loadInput(const QVector<cv::Mat> images);
int loadInput(const QString inputPath);
  
void start();  
```
Usage:
<ol> 
  <li> First, you will need to load the data by using one of the overridden functions shown above. Your data can be:
<ul>
  
<li> a single image represented by cv::Mat 
<li> a vector of images, where each image is of type cv::Mat 
<li> a path to a single image or to a directory of images 
  
<b> Note that the input image/images can only be of type: jpg, .png, .bmp, .tif </b>
</ul>
<li> After a successful input load, you may call the start function.
</ol>

**Optional**  
```cpp
void setPreprocessingParams(const int bilateralDiameter = 10, const int bilateralSigmaColor = 10, 
                            const int bilateralSigmaSpace = 3, const int bilateralBorderType = cv::BORDER_DEFAULT, 
                            const int numOfBilateralFilterApplications = 3, const int cannyThreshold1 = 0, 
                            const int cannyThreshold2 = 255, const int contoursMethod = cv::RETR_LIST, 
                            const int contoursMode = cv::CHAIN_APPROX_NONE);  
  
int setAdvancedMode(bool useAdvancedMode = false);  
```
Usage:
- With ```setPreprocessingParams(...)``` you set the parameters required for image preprocessing. Note that the default parameters were acquired experimentally. For a more in - depth insight into the role of each parameter, please refer to the [OpenCV 4.0.1 documentation](https://docs.opencv.org/4.0.1/).
- With ```setAdvancedMode(...)``` you set the *advancedMode*, which gives you all the intermediate results acquired during the preprocessing phase.

<br />  

**SIGNALS:**
```cpp
PreprocessingDoneSignal(PREPROCESSING_ALL_RESULTS);

PreprocessingDoneSignal(PREPROCESSING_RESULT);

PreprocessingSequenceDoneSignal(QMap<QString, PREPROCESSING_ALL_RESULTS>);

PreprocessingSequenceDoneSignal(QMap<QString, PREPROCESSING_RESULT>);

PreprocessingProgressSignal(int);

PreprocessingDurationSignal(PREPROCESSING_DURATIONS);

PreprocessingErrorSignal(int);
```  
Few important notes:  
- If you load an image or an image path
  - You get ```preprocessingDoneSignal``` with <b>PREPROCESSING_ALL_RESULTS</b> if the *advancedMode* is <b>enabled</b>.  
  - You get ```preprocessingDoneSignal``` with <b>PREPROCESSING_RESULTS</b> if the *advancedMode* is <b>disabled</b>.
- If you load a vector of images or an input directory  
  - You get ```preprocessingSequenceDoneSignal``` with <b>PREPROCESSING_ALL_RESULTS</b> if the *advancedMode* is <b>enabled</b> . 
  - You get ```preprocessingSequenceDoneSignal``` with <b>PREPROCESSING_RESULTS</b> if *advancedMode* is <b>disabled </b> 
- You get ```preprocessingDurationSignal``` with duration values in *milliseconds* for each phase during preprocessing if it was finished successfully.
- You get ```preprocessingErrorSignal``` with the <b>error code</b> if an error occured during preprocessing.
<br />  
<br />  

