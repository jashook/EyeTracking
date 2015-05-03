////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: video_capture.hpp
//
// Timeperiod:
//
// 29-Oct-14: Version 1.0: Created
// 29-Oct-14: Version 1.0: Last updated
//
// Notes:
//
// Uses opencv
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEO_CAPTURE_HPP__
#define __VIDEO_CAPTURE_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#if _WIN32

// Windows

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>


#else

// Unix Based System

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#endif

#include "ring_buffer.hpp"
#include "timing_helper.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define ESC_KEY 27
#define FPS_TIMING

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<bool(*__ProcessingFunction)(cv::Mat&), bool __Gui = false, size_t __Threads = 1> class video_capture
{
   private: // Member Variables

      cv::VideoCapture _m_capture;

   public: // Constructor | Destructor

      video_capture()
         {
         const std::string error_message = "Unable to capture on a connected device.  Please make sure everything is connected correctly and try again.";

         //open camera
         _m_capture.open(0);

         //initilize window for video feed
         cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);

         // Set resolution

         // Try 1080p
         /*bool width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
         bool height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

         //TODO - find why (even though the set function returns true) the get value returns the default.
         double setWidth = _m_capture.get(CV_CAP_PROP_FRAME_WIDTH);	
         std::cout << "set width: " << setWidth << std::endl;

               if (width && height)
               {
                  std::cout << "Frame Width: " << _m_capture.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
                  return;
               }
         */


         // Try 720p
         /*bool width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
         bool height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 768);

         if (width && height)
            {
               std::cout << "Frame Width: " << _m_capture.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
               return;
            }*/

         bool width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
         bool height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

         if (width && height)
            {
               std::cout << "Frame Width: " << _m_capture.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
               return;
            }

         std::cerr << "Unable to set the frame width and height, defaulting to the standard resolution" << std::endl;
         }

      ~video_capture()
         {
         // Clean up everything

         _m_capture.release();
         cvDestroyWindow("Video");
         }

   public: // Member Functions

      void capture_sync()
         {
#ifdef FPS_TIMING
         static double time_inverse = 0;
         static double running_sum = 0;
         static const size_t queue_length = 20;

         // Frame rate queue (averages of last n frames)
         // Start it at the last index (queue_length)
         ev10::eIIe::ring_buffer<double, queue_length> time_between_frames(queue_length);

#endif

         cv::Mat* frame = new cv::Mat();

         //define done bool
         bool done = false;

         // Process frames until the user exits
         while (!done)
         {
            static auto processing_function = [this](cv::VideoCapture& capture, cv::Mat* frame, bool& done)
            {
               // Get the frame
               capture >> *frame;
               if (!frame->empty())
               {
                  // Start processing
                  done = __ProcessingFunction(*frame);

                  if (__Gui)
                  {
                     done = _show_default_gui(*frame);
                  }
               }
            };

#ifdef FPS_TIMING

            // Time this function
            double time_passed = ev10::eIIe::timing_helper<ev10::eIIe::SECOND>::time(processing_function, _m_capture, frame, done);

            time_inverse = 1 / time_passed;

            // Add new value to queue
            time_between_frames.push(time_inverse);

            // Add to runningSum
            running_sum += time_inverse;

            // Subtract last value in queue from runningSum
            running_sum -= time_between_frames.pop();

            // Display framerate as the average of the queue (=runningSum / queue_size)
            std::cout << "fps: " << (int)running_sum / queue_length << "\r" << std::flush;

#else

            _capture_sync_helper(_m_capture, frame, done);

#endif
            }
         }

   private: // Private Member functions

      bool _show_default_gui(cv::Mat& frame)
         {
         cv::imshow("Video", frame);

         // Wait 10 ms for a key to be pressed
         int character = cvWaitKey(10);

         // Break if the esc key is pressed
         if (character == ESC_KEY) return true;

         return false;

         }

   }; // end of class (video_capture)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __VIDEO_CAPTURE_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
