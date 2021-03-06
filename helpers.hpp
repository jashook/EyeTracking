////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Someone who Implemented EyeLike
//
// Module: helpers.hpp
//
// Timeperiod:
//
// unknown: Version 1.0: Created
// 24-Feb-15: Version 1.0: Refactored and added functions.
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __HELPERS_HPP__
#define __HELPERS_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <iostream>
#include <queue>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "constants.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double computeDynamicThreshold(const cv::Mat& mat, double stdDevFactor);
bool inMat(cv::Point& p, int rows, int cols);
cv::Mat matrixMagnitude(const cv::Mat& matX, const cv::Mat& matY);
bool rectInImage(cv::Rect& rect, cv::Mat& image);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __HELPERS_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

