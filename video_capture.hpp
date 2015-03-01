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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define ESC_KEY 27

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if 1

#define FPS_TIMING 

#endif

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
			bool width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
         bool height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

			//TODO - find why (even though the set function returns true) the get value returns the default.
			double setWidth = _m_capture.get(CV_CAP_PROP_FRAME_WIDTH);	
			std::cout << "set width: " << setWidth << std::endl;

         if (width && height)
         {
				//TODO - change this: I am reasonably sure that checking if the camera is open like this will reset the default settings (640x480)
            if (!_m_capture.open(0))
            {
               std::cerr << error_message << std::endl;
            }

            return;
         }

         std::cout << _m_capture.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;

         // Try 720p
         width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
         height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 768);

         if (width && height)
         {
            if (!_m_capture.open(0))
            {
               std::cerr << error_message << std::endl;
            }

            return;
         }

         width = _m_capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
         height = _m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

         if (width && height)
         {
            if (!_m_capture.open(-1))
            {
               std::cerr << error_message << std::endl;
            }

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
         cv::Mat frame;
      
			//define done bool
         bool done = false;

#ifdef FPS_TIMING
			//define timing vars
			std::chrono::time_point<std::chrono::system_clock> start, end;

			//make queue for framerate (average of last 10 frames?)
			std::queue<double> time_between_frames;

			double timeInverse = 0;
			double runningSum = 0;

			//initilize the queue to 20 items
			int queueLength = 20;
			for (int i = 0; i < queueLength; ++i)
			{
				time_between_frames.push(timeInverse);
			}
#endif

			//process frames until the user exits
         while (!done)
         {

#ifdef FPS_TIMING
				//get start time
				start = std::chrono::system_clock::now();
#endif 

				// Get the frame
            _m_capture >> frame;
            if (!frame.empty())
            {
               // Start processing
               done = __ProcessingFunction(frame);
            
               if (__Gui)
               {
                  done = _show_default_gui(frame);
               
               }
            }

#ifdef FPS_TIMING
				//get end time
				end = std::chrono::system_clock::now();

				//calculate frame rate
				std::chrono::duration<double> elapsed_seconds = end - start;
				timeInverse = 1 / elapsed_seconds.count();

				//add new value to queue
				time_between_frames.push(timeInverse);
				//add to runningSum
				runningSum += timeInverse;
				//subtract last value in queue from runningSum
				runningSum -= time_between_frames.front();
				//pop the subtracted value
				time_between_frames.pop();
				//display framerate as the average of the queue (=runningSum / 10)
				std::cout << "fps: " << (int)runningSum / queueLength << "\r";
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
