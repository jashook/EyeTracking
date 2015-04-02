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

#include "rw_lock.hpp"
#include "test.hpp"

#include <exception>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void test_multiple_readers()
{
   ev10::eIIe::rw_lock rw_lock;

   rw_lock.lock<ev10::eIIe::READER>();
   rw_lock.lock<ev10::eIIe::READER>();

   rw_lock.unlock<ev10::eIIe::READER>();
   rw_lock.unlock<ev10::eIIe::READER>();
}

void test_multiple_reader_writer()
{
   int shared_value = 0;
   ev10::eIIe::rw_lock rw_lock;

   rw_lock.lock<ev10::eIIe::READER>();

   auto launching_function = [&shared_value, &rw_lock] (){
      // Get a writer lock and spin for a little

      rw_lock.lock<ev10::eIIe::WRITER>();

      for (std::size_t count = 0; count < 1000000000; ++count);

      shared_value = 100;

      rw_lock.unlock<ev10::eIIe::WRITER>();
   };

   std::thread launching_thread(launching_function);

   rw_lock.unlock<ev10::eIIe::READER>();

   // Do nothing for a little
   for (std::size_t count = 0; count < 10000000; ++count);

   //rw_lock.lock<ev10::eIIe::READER>();

   if (shared_value != 100)
   {
      launching_thread.join();

      return;
   }

   launching_thread.join();

   throw std::runtime_error("Incorrect value");

}

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

