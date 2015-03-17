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
// Simple lock built on the Compare and Swap hardware operation
//
// Please note that calling the Compare and Swap will flush the system cache and
// Lock the system bus.  Like all locks, this is an expensive operation. Try to
// use it sparingly.
//
// Note, this lock would probably be used in place of an std::atomic type
// This implementation is roughly 15x faster then using std::atomic<bool>
//
// The CAS operation requires hardware support, armv5 and below will not compile
// This code.
// 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef __CAS_LOCK_HPP__
#define __CAS_LOCK_HPP__

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace ev10 {
namespace eIIe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class cas_lock
{
   public: // Type definitions and Constants

      enum locked_t { UNLOCKED, LOCKED };

   public:  // Constructor | Destructor

      cas_lock() { _ctor(); }
      ~cas_lock() { _dtor(); }

   public:  // Public Member Functions

      void lock() { _lock(); }
      bool try_lock() { return _try_lock(); }
      void unlock() { _unlock(); }

   private: // Private Member Functions
   
      void _ctor()
      {
         _m_lock = UNLOCKED;
      }

      void _dtor()
      {
         _m_lock = UNLOCKED;
      }

      void _lock()
      {
         #if __GNUC__

            while(!__sync_bool_compare_and_swap(&_m_lock, UNLOCKED, LOCKED));

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

      volatile bool _m_lock;

}; // end of class(cas_lock)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // end of namespace(eIIe)
} // end of namespace(ev10)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CAS_LOCK_HPP__

////////////////////////////////////////////////////////////////////////////////
// End of file cas_lock.hpp
////////////////////////////////////////////////////////////////////////////////

