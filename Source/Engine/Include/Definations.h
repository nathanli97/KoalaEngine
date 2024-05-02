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
#include <type_traits>
#include "Core/KoalaLogger.h"

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

#ifdef _NDEBUG
  #define FORCEINLINE_DEBUGABLE FORCEINLINE
#else
  #define FORCEINLINE_DEBUGABLE inline
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace Koala
{
    template<typename T,
        typename std::enable_if<!std::is_class<T>::value, bool>::type = true
    >
    FORCEINLINE void DestructElement(T &element) {}
    template<typename T,
        typename std::enable_if<std::is_class<T>::value, bool>::type = true
    >
    FORCEINLINE void DestructElement(T &element) {
        element.~T();
    }
    template<typename T,
        typename ValueType,
        typename std::enable_if<std::is_rvalue_reference<ValueType>::value, bool>::type = true
    >
    FORCEINLINE void ConstructElement(T &element, ValueType value) {
        element = std::move(value);
    }
    template<typename T,
        typename ValueType,
        typename std::enable_if<std::is_scalar<T>::value, bool>::type = true
    >
    FORCEINLINE void ConstructElement(T &element, ValueType &&value) {
        element = value;
    }
    template<bool...> struct bool_pack;
    template<bool... bs>
    using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;
    template<typename T,
        typename... ValueType,
        typename std::enable_if<std::is_class<T>::value &&
        all_true<(!std::is_reference_v<ValueType>)...>::value
        , bool>::type = true
    >
    FORCEINLINE void ConstructElement(T &element, ValueType... values) {
        new(&element) T(values...);
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/**
 * Calculate how many memory pad size needed for alignment
 * @param alignedTo Aligned to this size. Must be power of 2.
 * @param currSize Current size
 * @return Pad size in bytes. If = 0, then {currSize} is already aligned with {alignedTo}.
 * Return 0 if {aligned_to} <= 1
 */
template <typename T>
FORCEINLINE constexpr uint16_t CalculateAlignedPadSize(uint16_t alignedTo, T currSize) {
    if (alignedTo <= 1) {
        return 0;
    }
    uint16_t t = currSize % alignedTo;
    return (t == alignedTo) ? 0 : alignedTo - t;
}
/**
 * Calculate how many memory size needed for alignment
 * @param alignedTo Aligned to this size. Must be power of 2.
 * @param currSize Current size
 * @return Aligned size. If memory is already aligned with {alignedTo}, return {currSize}. Otherwise, return (enlarged) aligned memory size.
 * Return {currSize} if {alignedTo} <= 1
 */
template <typename T>
FORCEINLINE constexpr T CalculateAlignedSize(uint16_t alignedTo, T currSize) {
    T pad = CalculateAlignedPadSize(alignedTo, currSize);
    return currSize + pad;
}

#if __cplusplus >= 201402L || defined(_MSC_VER) && _MSC_VER >= 1929L
  #define NODISCARD [[nodiscard]]
  #define DEPRECATED(MSG) [[deprecated(MSG)]]
#else
  #define NODISCARD
  #define DEPRECATED(X)
#endif

#define NON_CONST_MEMBER_CALL_CONST_RET(Func) \
typedef typename std::remove_reference<decltype(*this)>::type ThisNonConstType; \
typedef decltype(Func) RetvalType; \
return const_cast<RetvalType>((const_cast<ThisNonConstType const&>(*this).Func));