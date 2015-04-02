////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Someone who Implemented EyeLike
//
// Module: find_eye_center.hpp
//
// Timeperiod:
//
// unknown: Version 1.0: Created
// 21-Feb-15: Version 1.0: Refactored and changed includes
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "find_eye_center.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function: computeMatXGradient
//
// Notes: Calculates the x-gradient at each pixel location and returns the result
//
// Can be multi-threaded, sync in / sync out
//
////////////////////////////////////////////////////////////////////////////////

cv::Mat computeMatXGradient(const cv::Mat& mat)
{
   cv::Mat output(mat.rows, mat.cols, CV_64F);

   for (int column_index = 0; column_index < mat.rows; ++column_index)
   {
      const uchar* current_mat_row = mat.ptr<uchar>(column_index);
      double* output_row = output.ptr<double>(column_index);

      // Do not average the first index
      output_row[0] = current_mat_row[1] - current_mat_row[0];

      // Average each index
      for (int index = 1; index < mat.cols - 1; ++index)
      {
         output_row[index] = (current_mat_row[index + 1] - current_mat_row[index - 1]) / 2.0;
      }

      // Do not average the last index
      output_row[mat.cols - 1] = current_mat_row[mat.cols - 1] - current_mat_row[mat.cols - 2];
   }

   return output;
}

////////////////////////////////////////////////////////////////////////////////
// Function: findEyeCenter
//
// Notes: finds the eye center and returns a CV::Point at the location
// 
// computes gradients (can be threaded independently of each other for x, y)
// calculates magnitudes of grads
// compute gradient threshold
// normalize the gradients (this is the main loop of the function)
// create a weight matrix (use blur and inversion to give the pupil/iris a high weight
// test centers (use gradients to 'vote' at each potential center location
// post process (flood fill edges (?) and threshold again)
// choose the center location with the max number of 'votes'
//
// the 'outer loop' for threading is here.
//
////////////////////////////////////////////////////////////////////////////////

cv::Point findEyeCenter(cv::Mat& face, cv::Rect& eye, const std::string& debug_window)
{
   cv::Mat eye_roi;
   cv::Mat eye_roi_unscaled = face(eye);

   // We are prefering accuracy over speed at this point
   // Using threading to combat the performance loss
   scaleToFastSize(eye_roi_unscaled, eye_roi);

   // draw eye region
   // rectangle(face,eye,1234);

   // Find the gradient
   cv::Mat gradient_x = computeMatXGradient(eye_roi);
   cv::Mat gradient_y = computeMatXGradient(eye_roi.t()).t();

   // Normalize and threshold the gradient 
   // and compute all the magnitudes
   cv::Mat magnitudes = matrixMagnitude(gradient_x, gradient_y);

   // Compute the threshold
   double magnitude_threshhold = computeDynamicThreshold(magnitudes, kGradientThreshold);

   // row_index will start at row 0 to amount of rows (y direction)
   for (int row_index = 0; row_index < eye_roi.rows; ++row_index)
   {
      double* gradient_row = gradient_x.ptr<double>(row_index);
      double* gradient_column = gradient_y.ptr<double>(row_index);

      const double* magnitude_row = magnitudes.ptr<double>(row_index);

      for (int column_index = 0; column_index < eye_roi.cols; ++column_index)
      {
         register double gradient_x_at_column = gradient_row[column_index];
         register double gradient_y_at_column = gradient_column[column_index];

         register double magnitude = magnitude_row[column_index];

         if (magnitude > magnitude_threshhold)
         {
            gradient_row[column_index] = gradient_x_at_column / magnitude;
            gradient_column[column_index] = gradient_y_at_column / magnitude;
         }

         else
         {
            gradient_row[column_index] = 0.0;
            gradient_column[column_index] = 0.0;
         }
      }

   }

   // imshow(debug_window,gradient_x);

   // -- Create a blurred and inverted image for weighting
   cv::Mat weight;

   GaussianBlur(eye_roi, weight, cv::Size(kWeightBlurSize, kWeightBlurSize), 0, 0);

   // Invert at every index
   for (int row_index = 0; row_index < weight.rows; ++row_index)
   {
      unsigned char* current_row = weight.ptr<unsigned char>(row_index);

      for (int column_index = 0; column_index < weight.cols; ++column_index)
      {
         current_row[column_index] = (255 - current_row[column_index]);
      }

   }

   // imshow(debug_window, weight);

   // Run the algorithm!
   cv::Mat output_sum = cv::Mat::zeros(eye_roi.rows, eye_roi.cols, CV_64F);

   // For each possible gradient location
   // Note: these loops are reversed from the way the paper does them
   // it evaluates every possible center for each gradient location instead of
   // every possible gradient location for every center.

   // Only search in the inner 50% of the eye roi to find the eye center..

   const int column_start = weight.cols * 0.25;
   const int column_end = weight.cols * 0.75;

   const int row_start = weight.rows * 0.25;
   const int row_end = weight.rows * 0.75;

   // printf("Eye Size: %ix%i\n", outSum.cols, outSum.rows);

   // For every row
   for (int current_row_index = row_start; current_row_index < row_end; ++current_row_index)
   {
      // Get the Current Row
      const unsigned char* current_weight_row = weight.ptr<unsigned char>(current_row_index);

      const double* gradient_row_x_direction = gradient_x.ptr<double>(current_row_index);
      const double* gradient_row_y_direction = gradient_y.ptr<double>(current_row_index);

      // For every pixel inside the row
      for (int current_column_index = column_start; current_column_index < column_end; ++current_column_index)
      {
         double gradient_x = gradient_row_x_direction[current_column_index];
         double gradient_y = gradient_row_y_direction[current_column_index];

         if (gradient_x == 0.0 && gradient_y == 0.0)
         {
            continue;
         }

         testPossibleCentersFormula(current_column_index, current_row_index, current_weight_row[current_column_index], gradient_x, gradient_y, output_sum);
      }
   }

   // Scale all the values down, basically averaging them

   double numGradients = (weight.rows*weight.cols);

   cv::Mat output;
   output_sum.convertTo(output, CV_32F, 1.0 / numGradients);

   // imshow(debug_window, out);

   //-- Find the maximum point
   cv::Point max_p;
   double max_val;
   cv::minMaxLoc(output, NULL, &max_val, NULL, &max_p);

   //-- Flood fill the edges
   if (kEnablePostProcess)
   {
      cv::Mat floodClone;
      //double floodThresh = computeDynamicThreshold(out, 1.5);

      double floodThresh = max_val * kPostProcessThreshold;
      cv::threshold(output, floodClone, floodThresh, 0.0f, cv::THRESH_TOZERO);

      if (kPlotVectorField)
      {
         //plotVecField(gradientX, gradientY, floodClone);
         cv::imwrite("eyeFrame.png", eye_roi_unscaled);
      }

      cv::Mat mask = floodKillEdges(floodClone);

      //imshow(debugWindow + " Mask",mask);
      //imshow(debugWindow,out);
      // redo max

      cv::minMaxLoc(output, NULL, &max_val, NULL, &max_p, mask);

   }

   return unscalePoint(max_p, eye);

}

bool floodShouldPushPoint(cv::Point& np, cv::Mat &mat)
{
   return inMat(np, mat.rows, mat.cols);
}

// returns a mask
cv::Mat floodKillEdges(cv::Mat &mat)
{
   rectangle(mat, cv::Rect(0, 0, mat.cols, mat.rows), 255);

   cv::Mat mask(mat.rows, mat.cols, CV_8U, 255);
   std::queue<cv::Point> toDo;
   toDo.push(cv::Point(0, 0));
   while (!toDo.empty())
   {
      cv::Point p = toDo.front();
      toDo.pop();
      if (mat.at<float>(p) == 0.0f)
      {
         continue;
      }
      // add in every direction
      cv::Point np(p.x + 1, p.y); // right
      if (floodShouldPushPoint(np, mat))
         toDo.push(np);
      np.x = p.x - 1; np.y = p.y; // left
      if (floodShouldPushPoint(np, mat))
         toDo.push(np);
      np.x = p.x; np.y = p.y + 1; // down
      if (floodShouldPushPoint(np, mat))
         toDo.push(np);
      np.x = p.x; np.y = p.y - 1; // up
      if (floodShouldPushPoint(np, mat))
         toDo.push(np);
      // kill it
      mat.at<float>(p) = 0.0f;
      mask.at<uchar>(p) = 0;
   }
   return mask;

}

/*void plotVecField(const cv::Mat &gradientX, const cv::Mat &gradientY, const cv::Mat &img) {
  mglData* xData = matToData<double>(gradientX);
  mglData* yData = matToData<double>(gradientY);
  mglData* imgData = matToData<float>(img);

  mglGraph gr(0,gradientX.cols * 20, gradientY.rows * 20);
  gr.Vect(*xData, *yData);
  gr.Mesh(*imgData);
  gr.WriteFrame("vecField.png");

  delete xData;
  delete yData;
  delete imgData;

  }*/

void testPossibleCentersFormula(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out)
{
   ////gridsize
   int gs = 25;
   // for all possible centers within the grid
   for (int cy = y - gs; cy < y + gs; ++cy)
   {
      if (cy < 0 || cy > out.rows - 1)
         continue;
      double *Or = out.ptr<double>(cy);
      for (int cx = x - gs; cx < x + gs; ++cx)
      {
         if (cx < 0 || cx > out.cols - 1)
            continue;

         //try with original for loop structure from eyelike (no gridsize - loop over entire image)
         //for all possible centers
         //for (int cy = 0; cy < out.rows; ++cy)
         //{
         //	double *Or = out.ptr<double>(cy);
         //	for (int cx = 0; cx < out.cols; ++cx)
         //	{
         if (x == cx && y == cy)
         {
            continue;
         }
         // create a vector from the possible center to the gradient origin
         double dx = x - cx;
         double dy = y - cy;
         // normalize d
         double magnitude = sqrt((dx * dx) + (dy * dy));
         dx = dx / magnitude;
         dy = dy / magnitude;
         double dotProduct = dx*gx + dy*gy;
         dotProduct = std::max(0.0, dotProduct);
         // square and multiply by the weight
         if (kEnableWeight)
         {
            Or[cx] += dotProduct * dotProduct * (weight / kWeightDivisor);
         }
         else
         {
            Or[cx] += dotProduct * dotProduct;
         }
      }
   }
}

cv::Point unscalePoint(cv::Point p, cv::Rect origSize)
{
   /*float ratio = (((float)kFastEyeWidth)/origSize.width);
   int x = round(p.x / ratio);
   int y = round(p.y / ratio);*/
   int x = p.x;
   int y = p.y;
   return cv::Point(x, y);

}

void scaleToFastSize(const cv::Mat &src, cv::Mat &dst)
{
   //cv::resize(src, dst, cv::Size(kFastEyeWidth,(((float)kFastEyeWidth)/src.cols) * src.rows));
   src.copyTo(dst);

}



