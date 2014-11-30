////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Authors: Andrew Blanchard
//        : Jarret Shook
//
// Module: constants.hpp
//
// Timeperiod:
//
// 29-Nov-14: Version 1.0: Last updated
//
// Notes:
//
// Constants for finding the eye
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __EYE_CONSTANTS_HPP__
#define __EYE_CONSTANTS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Debugging
////////////////////////////////////////////////////////////////////////////////

extern const bool kPlotVectorField = false;

////////////////////////////////////////////////////////////////////////////////
// Size Constraints
////////////////////////////////////////////////////////////////////////////////

const int kEyePercentTop = 25;
const int kEyePercentSide = 13;
const int kEyePercentHeight = 30;
const int kEyePercentWidth = 35;

////////////////////////////////////////////////////////////////////////////////
// Preprocessing
////////////////////////////////////////////////////////////////////////////////

const bool kSmoothFaceImage = false;
const float kSmoothFaceFactor = 0.005f;

////////////////////////////////////////////////////////////////////////////////
// Algorithm Parameters
////////////////////////////////////////////////////////////////////////////////

const int kFastEyeWidth = 50;
const int kWeightBlurSize = 5;
const bool kEnableWeight = false;
const float kWeightDivisor = 150.0;
const double kGradientThreshold = 50.0;

////////////////////////////////////////////////////////////////////////////////
// Postprocessing
////////////////////////////////////////////////////////////////////////////////

const bool kEnablePostProcess = true;
const float kPostProcessThreshold = 0.97f;

////////////////////////////////////////////////////////////////////////////////
// Eye Corner
////////////////////////////////////////////////////////////////////////////////

const bool kEnableEyeCorner = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __EYE_CONSTANTS_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////