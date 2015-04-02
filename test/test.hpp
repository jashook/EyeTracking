////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Author: Jarret Shook
//
// Module: test.hpp
//
// Time-period:
//
// Dec 11, 2014: Version 1.0: Created
// Dec 11, 2014: Version 1.0: Last Updated
//
// Notes:
//
// Requirements: c++11
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __TEST_HPP__
#define __TEST_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "tester.hpp"

#include <functional>
#include <string>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define STR(num) #num

#define TEST_INFORMATION std::string(__func__) + std::string(":") + std::string(std::to_string(__LINE__)) + std::string(" -- ")

#define PRINT_TEST_INFORMATION printf("%s:%s:%d --", __FILE__, __func__, __LINE__);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev9 {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class test : public tester
{
   public:  // Constructor | Destructor
    
      test(int threads) : tester(threads) { }
   
      virtual ~test() { }
   
   public:  // Member functions
   
      void add_test(const std::function<void()>& function) { _add_test(function); }
   
   private: // Private member functions
   
      void _add_test(const std::function<void()>& function)
      {
         run(function);
      }
    
}; // end of class(tester_tester)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(ev9)
    
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __TEST_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
