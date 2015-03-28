////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Author: Jarret Shook
// 
// Timeperiod: 
// 
// 16-Mar-15: Version 1.0: Created 
// 16-Mar-15: Version 1.0: Last Updated 
// 
// Notes: 
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "thread_dispatch.hpp"
#include "test.hpp"

#include <exception>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main()
{
   ev9::test rw_test(0);

   rw_test.add_test(test_multiple_readers);
   rw_test.add_test(test_multiple_reader_writer);
}

////////////////////////////////////////////////////////////////////////////////
// End of file rw_lock_test.cpp
////////////////////////////////////////////////////////////////////////////////
