////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Author: Jarret Shook
// 
// Timeperiod: 
// 
// 05-Mar-15: Version 1.0: Created 
// 05-Mar-15: Version 1.0: Last Updated 
// 
// Notes: 
//
// Atomic type built on the Compare and Swap hardware operation
//
// Please note that calling the Compare and Swap will flush the system cache and
// Lock the system bus. Like all locks, this is an expensive operation. Try to
// use it sparingly.
//
// Note, this lock would probably be used in place of an std::atomic type
// This implementation is roughly 15x faster then using std::atomic<__Type>
//
// The CAS operation requires hardware support, armv5 and below will not compile
// This code.
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __ATOMIC_HPP__
#define __ATOMIC_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename __Type> atomic
{
   private: // Private type definitions and constants

      enum lock_t { UNLOCKED, LOCKED };

   public:  // Constructor | Destructor

      atomic() { _ctor(); }
      ~atomic() { _dtor(); }

   public:  // Public Member Functions

      void increment() { _lock(); ++_m_value; _unlock(); }
      void decrement() { _lock(); --_m_value; _unlock(); }

   private: // Private Member Functions
   
      _ctor()
      {
         _m_lock = UNLOCKED;
      }

      _dtor()
      {
         _m_lock = LOCKED;
      }

      void _lock(int value)
      {
         #if __GNUC__

            while(!__sync_bool_compare_and_swap(&_m_lock, ));

         # elif _WIN32

            while(InterlockedCompareExchange(&_m_lock, UNLOCKED, LOCKED) != LOCKED);

         #endif

         // Fall out of the function with the lock
      }

      bool _try_lock()
      {
         #if __GNUC__

            return __sync_val_compare_and_swap(&_m_lock, UNLOCKED, LOCKED);

         # elif _WIN32

             return InterlockedCompareExchange(&_m_lock, UNLOCKED, LOCKED);

         #endif
      }

      void _unlock()
      {
         #if __GNUC__

            __sync_bool_compare_and_swap(&_m_lock, LOCKED, UNLOCKED);

         # elif _WIN32

            InterlockedCompareExchange(&_m_lock, LOCKED, UNLOCKED) == UNLOCKED;

         #endif
      }
      
   private: // Member Variables

      volatile lock_t _m_lock;
      volatile __Type _m_value;

}; // end of class(semaphore)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __SEMAPHORE_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file semaphore.hpp
////////////////////////////////////////////////////////////////////////////////

