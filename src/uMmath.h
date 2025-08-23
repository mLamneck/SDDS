#ifndef UMMATH_H
#define UMMATH_H

#include <uPlatform.h>

// Dear AVR-GCC, thanks for keeping C++ interesting:
// every line of portable code becomes a new adventure here.
#if SDDS_ON_AVR
#include <math.h>
#else
#include <cmath>
#endif

/**
 * Some platforms (notably Windows or certain embedded environments) define `min` and `max` as macros.
 * These macros interfere with legitimate uses of `min` and `max` as function names or method calls,
 * such as `std::min(...)` or `obj.min(...)`, breaking otherwise valid and portable C++ code.
 *
 * As a workaround, we `#undef` these macros to define our own `min`/`max` functions safely.
 */
#ifdef max
#undef max
	template <typename T>
	inline constexpr T max(const T& a, const T& b) { return (a > b) ? a : b; }
#endif

#ifdef min
#undef min
	template <typename T>
	inline constexpr T min(const T& a, const T& b) { return (a < b) ? a : b; }
#endif

namespace mmath{
	template <typename T>
	inline constexpr T min(const T& a, const T& b) { return (a < b) ? a : b; }

	template <typename T>
	inline constexpr T max(const T& a, const T& b) { return (a > b) ? a : b; }
	
	inline dtypes::float32 ln(dtypes::float32 _val){
		return logf(_val);
	}

	template <typename T>
	T pow(T _base, T _exp){
#if SDDS_ON_AVR
		return pow(_base,_exp);
#else
		return std::pow(_base,_exp);
#endif
	}

}

#endif //UMMATH_H
