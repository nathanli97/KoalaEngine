//Copyright 2024 Li Xingru
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
//associated documentation files (the “Software”), to deal in the Software without restriction,
//including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial
//portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <cstddef>
#include <cstdint>

// forceinline definitions
#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#if defined( __GNUC__ ) || defined( __MINGW__ ) || defined ( __clang__ )
  #define FORCEINLINE __attribute__((always_inline)) inline
  #ifndef likely
    #define likely(x)       __builtin_expect((x),1) // NOLINT
  #endif

  #ifndef unlikely
    #define unlikely(x)     __builtin_expect((x),0) // NOLINT
  #endif
#elif defined( _MSC_VER )

#define FORCEINLINE __forceinline
  #ifndef likely
    #define likely(x) x
  #endif
  #ifndef unlikely
    #define unlikely(x) x
  #endif
#else
  #define FORCEINLINE inline
  #ifndef likely
    #define likely(x) x
  #endif
  #ifndef unlikely
    #define unlikely(x) x
  #endif
#endif
#if !defined(NDEBUG)
  #include <cassert>
  #define ASSERT(X) assert(X)
  #define ASSERTS(X, MSG) ASSERT(X)
#elif defined(RUNTIME_ASSERT)
  #define ASSERT_(X) if (!(X)) {LOG_FATAL("Condition " #X
  #define ASSERT(X) ASSERT_(X) " Failed");}
  #define ASSERTS_(X, MSG) if (!(X)) {LOG_FATAL("Condition " #X
  #define ASSERTS__(X, MSG) ASSERTS_(X, MSG) "Failed: " #MSG
  #define ASSERTS(X, MSG) ASSERTS__(X, MSG));}
#else
  #define ASSERT_(X)
  #define ASSERT(X)
  #define ASSERTS_(X, MSG)
  #define ASSERTS(X, MSG)
#endif

#if __cplusplus >= 201402L || defined(_MSC_VER) && _MSC_VER >= 1929L
  #define NODISCARD [[nodiscard]]
  #define DEPRECATED [[deprecated]]
  #define DEPRECATED_MSG(X) [[deprecated(X)]]
#else
  #define NODISCARD
  #define DEPRECATED(X)
#endif

#define NON_CONST_MEMBER_CALL_CONST_RET(Func) \
typedef typename std::remove_reference<decltype(*this)>::type ThisNonConstType; \
typedef decltype(Func) RetvalType; \
return const_cast<RetvalType>((const_cast<ThisNonConstType const&>(*this).Func));