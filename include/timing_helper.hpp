////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Author: Jarret Shook
// 
// Module: timing_helper.hpp
//
// Timeperiod: 
// 
// 01-Mar-15: Version 1.0: Created 
// 01-Mar-15: Version 1.0: Last Updated 
// 
// Notes: 
//
// Helper to time functions
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __TIMING_HELPER_HPP__
#define __TIMING_HELPER_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum time_quantity_t { NANOSECOND, MICROSECOND, MILLISECOND, SECOND };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<time_quantity_t __TimeQuantity> class timing_helper
{
   private: // Constructor | Destructor

      timing_helper() { }
      ~timing_helper() { }

   public: // Member Functions

      template<typename __Function, typename ... __Arguments> double static time(__Function& function, __Arguments ... arguments)
      { 
         // Start the HR Clock
         auto start = std::chrono::high_resolution_clock::now();
         
         // Pass all arugments to the function to run
         function(arguments ...);

         // End the HR Clock
         auto end = std::chrono::high_resolution_clock::now();

         std::chrono::duration<double> elapsed_time = end - start;

         double return_value = 0;

         switch(__TimeQuantity)
         {
            case NANOSECOND:
            {
               std::chrono::nanoseconds nano_seconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);

               return_value = nano_seconds.count();
               break;
            }

            case MICROSECOND:
            {
               std::chrono::microseconds mc_seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);

               return_value = mc_seconds.count();
               break;
            }

            case MILLISECOND:
            {
               std::chrono::milliseconds ms_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time);

               return_value = ms_seconds.count();
               break;
            }

            case SECOND:
            {
               return_value = elapsed_time.count();
               break;
            }
         }

         return return_value;

      }

}; // end of class(time_helper)

 ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __TIMING_HELPER_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file timing_helper.hpp
////////////////////////////////////////////////////////////////////////////////

