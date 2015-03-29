////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Some EyeLike Person
//
// Modifiers: Andrew Blanchard, Jarret Shook
//
// Module: constants.hpp
//
// Modifications:
//
// unkown: Version 1.0: Created
// 16-Feb-15: Version 2.0: Changed a few constants
// 21-Feb-15: Version 2.0: Refactoring and adding comments
//
// Notes:
//
// Some global constants used by the EyeLike code.
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Debugging
const bool kPlotVectorField = false;

// Size constants
const int kEyePercentTop = 25;
const int kEyePercentSide = 13;
const int kEyePercentHeight = 30;
const int kEyePercentWidth = 35;

// Preprocessing
const bool kSmoothFaceImage = false;
const float kSmoothFaceFactor = 0.005;

// Algorithm Parameters
const int kFastEyeWidth = 45;
const int kWeightBlurSize = 23;
const bool kEnableWeight = false;
const float kWeightDivisor = 150.0;
const double kGradientThreshold = 50.0;

// Postprocessing
const bool kEnablePostProcess = false;
const float kPostProcessThreshold = 0.97;

// Eye Corner
const bool kEnableEyeCorner = false;

//screen adjustment parameters
const float cameraDistance = 12;
const int eyeRadius = 1;
const float spatialResolution = 60; //THIS SHOULD NOT BE CONSTANT. IT SHOULD BE ADJUSTED BASED ON RESOLUTION, BUT RIGHT NOW WE ONLY RUN IN 640
const float adjustmentPixelsToInches = cameraDistance / spatialResolution;
const float monitorResolution = 1920; //THIS SHOULD ALSO NOT BE CONSTANT. CUZ. REASONS.
const float montiorWidth = 20.625;  //...THIS SHOULD ALSO NOT BE CONSTANT. CUZ. REASONS.
const float monitorInchesToPixels = monitorResolution / montiorWidth;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CONSTANTS_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


