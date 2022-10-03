#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif
// C standard
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// STL
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>
// C++11
#if (_MSC_VER>=1600/*VS2010*/) || (__cplusplus>199711L)
	#include <functional>
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
#endif
// windows/GCC
#if !defined(__GNUC__)&&(defined(_WIN32)||defined(_WIN64))
	#include <windows.h>
	#include <wchar.h>
#elif defined(__GNUC__)&&!defined(__forceinline)
	#define __forceinline inline __attribute__((__always_inline__))
#endif
// common macros
#ifndef PI
	#define PI 3.141592653589793f
#endif
#ifndef max
	#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
	#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef clamp
	#define clamp(value,vmin,vmax) (max(vmin,min(vmax,value)))
#endif
#ifndef isnan
	#define	isnan(x) (x!=x)
#endif

//*******************************************************************
// user types
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar;

// template constants
template <class T> struct precision { static const T value(){ return std::numeric_limits<T>::epsilon()*10; } };	// need to be 10x for robust practical test

//*******************************************************************
template <class T> struct tvec2
{
	union{ struct { T r, g; }; struct { T x, y; }; };

	// constructor/set
	tvec2(){ x=y=0; }
	tvec2( T a ){ x=y=a; }						inline void set( T a ){ x=y=a; }
	tvec2( T a, T b ){ x=a;y=b; }				inline void set( T a, T b ){ x=a;y=b; }
	tvec2( const tvec2& v ){ x=v.x;y=v.y; }		inline void set( const tvec2& v ){ x=v.x;y=v.y; }

	// assignment / compound assignment operators
	inline tvec2& operator=( T a ){ set(a); return *this; }
	inline tvec2& operator+=( const tvec2& v ){ x+=v.x; y+=v.y; return *this; }
	inline tvec2& operator-=( const tvec2& v ){ x-=v.x; y-=v.y; return *this; }
	inline tvec2& operator*=( const tvec2& v ){ x*=v.x; y*=v.y; return *this; }
	inline tvec2& operator/=( const tvec2& v ){ x/=v.x; y/=v.y; return *this; }
	inline tvec2& operator+=( T a ){ x+=a; y+=a; return *this; }
	inline tvec2& operator-=( T a ){ x-=a; y-=a; return *this; }
	inline tvec2& operator*=( T a ){ x*=a; y*=a; return *this; }
	inline tvec2& operator/=( T a ){ x/=a; y/=a; return *this; }

	// comparison operators
	inline bool operator==( const tvec2& v ) const { return std::abs(x-v.x)<=precision<T>::value()&&std::abs(y-v.y)<=precision<T>::value(); }
	inline bool operator!=( const tvec2& v ) const { return !operator==(v); }

	// casting operators
	inline operator T*(){ return &x; }
	inline operator const T*() const { return &x; }

	// array access operators
	inline T& operator[]( ptrdiff_t i ){ /* fill in here */ }
	inline const T& operator[]( ptrdiff_t i ) const { /* fill in here */ }
	inline T& at( ptrdiff_t i ){ /* fill in here */ }
	inline const T& at( ptrdiff_t i ) const { /* fill in here */ }

	// unary operators
	inline tvec2 operator+() const { /* fill in here */ }
	inline tvec2 operator-() const { /* fill in here */ }

	// binary operators
	inline tvec2 operator+( const tvec2& v ) const { /* fill in here */ }
	inline tvec2 operator-( const tvec2& v ) const { /* fill in here */ }
	inline tvec2 operator*( const tvec2& v ) const { /* fill in here */ }
	inline tvec2 operator/( const tvec2& v ) const { /* fill in here */ }
	inline tvec2 operator+( T a ) const { /* fill in here */ }
	inline tvec2 operator-( T a ) const { /* fill in here */ }
	inline tvec2 operator*( T a ) const { /* fill in here */ }
	inline tvec2 operator/( T a ) const { /* fill in here */ }

	// length, normalize, dot product
	inline T length() const { /* fill in here */ }
	inline T dot( const tvec2& v ) const { /* fill in here */ }
	inline tvec2 normalize() const { /* fill in here */ }
};

//*******************************************************************
template <class T> struct tvec3
{
	union { struct { T r, g, b; }; struct { T x, y, z; }; };

	// constructor/set
	tvec3(){ x=y=z=0; }
	tvec3( T a ){ x=y=z=a; }								inline void set( T a ){ x=y=z=a; }
	tvec3( T a, T b, T c ){ x=a;y=b;z=c; }					inline void set( T a, T b, T c ){ x=a;y=b;z=c; }
	tvec3( const tvec3& v ){ x=v.x;y=v.y;z=v.z; }			inline void set( const tvec3& v ){ x=v.x;y=v.y;z=v.z; }
	tvec3( const tvec2<T>& v, T c ){ x=v.x;y=v.y;z=c; }		inline void set( const tvec2<T>& v, T c ){ x=v.x;y=v.y;z=c; }
	tvec3( T a, const tvec2<T>& v ){ x=a;y=v.x;z=v.y; }		inline void set( T a, const tvec2<T>& v ){ x=a;y=v.x;z=v.y; }

	// assignment / compound assignment operators
	inline tvec3& operator=( T a ){ set(a); return *this; }
	inline tvec3& operator+=( const tvec3& v ){ x+=v.x; y+=v.y; z+=v.z; return *this; }
	inline tvec3& operator-=( const tvec3& v ){ x-=v.x; y-=v.y; z-=v.z; return *this; }
	inline tvec3& operator*=( const tvec3& v ){ x*=v.x; y*=v.y; z*=v.z; return *this; }
	inline tvec3& operator/=( const tvec3& v ){ x/=v.x; y/=v.y; z/=v.z; return *this; }
	inline tvec3& operator+=( T a ){ x+=a; y+=a; z+=a; return *this; }
	inline tvec3& operator-=( T a ){ x-=a; y-=a; z-=a; return *this; }
	inline tvec3& operator*=( T a ){ x*=a; y*=a; z*=a; return *this; }
	inline tvec3& operator/=( T a ){ x/=a; y/=a; z/=a; return *this; }

	// comparison operators
	inline bool operator==( const tvec3& v ) const { return std::abs(x-v.x)<=precision<T>::value()&&std::abs(y-v.y)<=precision<T>::value()&&std::abs(z-v.z)<=precision<T>::value(); }
	inline bool operator!=( const tvec3& v ) const { return !operator==(v); }

	// casting operators
	inline operator T*(){ return &x; }
	inline operator const T*(){ return &x; }

	// array access operators
	inline T& operator[]( ptrdiff_t i ){ /* fill in here */ }
	inline const T& operator[]( ptrdiff_t i ) const { /* fill in here */ }
	inline T& at( ptrdiff_t i ){ /* fill in here */ }
	inline const T& at( ptrdiff_t i ) const { /* fill in here */ }
	
	// unary operators
	inline tvec3 operator+() const { /* fill in here */ }
	inline tvec3 operator-() const { /* fill in here */ }

	// binary operators
	inline tvec3 operator+( const tvec3& v ) const { /* fill in here */ }
	inline tvec3 operator-( const tvec3& v ) const { /* fill in here */ }
	inline tvec3 operator*( const tvec3& v ) const { /* fill in here */ }
	inline tvec3 operator/( const tvec3& v ) const { /* fill in here */ }
	inline tvec3 operator+( T a ) const { /* fill in here */ }
	inline tvec3 operator-( T a ) const { /* fill in here */ }
	inline tvec3 operator*( T a ) const { /* fill in here */ }
	inline tvec3 operator/( T a ) const { /* fill in here */ }
		
	// length, normalize, dot product
	inline T length() const { /* fill in here */ }
	inline tvec3 normalize() const { /* fill in here */ }
	inline T dot( const tvec3& v ) const { /* fill in here */ }

	// tvec3 only: cross product
	inline tvec3 operator^( const tvec3& v ) const { /* fill in here */ }
	inline tvec3 cross( const tvec3& v ) const { /* fill in here */ }
};

//*******************************************************************
template <class T> struct tvec4
{
	union { struct { T r, g, b, a; }; struct { T x, y, z, w; }; };

	// constructor/set
	tvec4(){ x=y=z=w=0; }
	tvec4( T a ){ x=y=z=w=a; }										inline void set( T a ){ x=y=z=w=a; }
	tvec4( T a, T b, T c, T d ){ x=a;y=b;z=c;w=d; }					inline void set( T a, T b, T c, T d ){ x=a;y=b;z=c;w=d; }
	tvec4( const tvec4& v ){ x=v.x;y=v.y;z=v.z;w=v.w; }				inline void set( const tvec4& v ){ x=v.x;y=v.y;z=v.z;w=v.w; }
	tvec4( const tvec2<T>& v, T c, T d ){ x=v.x;y=v.y;z=c;w=d; }	inline void set( const tvec2<T>& v, T c, T d ){ x=v.x;y=v.y;z=c;w=d; }
	tvec4( T a, T b, const tvec2<T>& v ){ x=a;y=b;z=v.x;w=v.y; }	inline void set( T a, T b, const tvec2<T>& v ){ x=a;y=b;z=v.x;w=v.y; }	
	tvec4( const tvec3<T>& v, T d ){ x=v.x;y=v.y;z=v.z;w=d; }		inline void set( const tvec3<T>& v, T d ){ x=v.x;y=v.y;z=v.z;w=d; }
	tvec4( T a, const tvec3<T>& v ){ x=a;y=v.x;z=v.y;w=v.z; }		inline void set( T a, const tvec3<T>& v ){ x=a;y=v.x;z=v.y;w=v.z; }
	tvec4( const tvec2<T>& v1, const tvec2<T>& v2 ){ x=v1.x;y=v1.y;z=v2.x;w=v2.y; }
	inline void set( const tvec2<T>& v1, const tvec2<T>& v2 ){ x=v1.x;y=v1.y;z=v2.x;w=v2.y; }

	// assignment / compound assignment operators
	inline tvec4& operator=( T a ){ set(a); return *this; }
	inline tvec4& operator+=( const tvec4& v ){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
	inline tvec4& operator-=( const tvec4& v ){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
	inline tvec4& operator*=( const tvec4& v ){ x*=v.x; y*=v.y; z*=v.z; w*=v.w; return *this; }
	inline tvec4& operator/=( const tvec4& v ){ x/=v.x; y/=v.y; z/=v.z; w/=v.w; return *this; }
	inline tvec4& operator+=( T a ){ x+=a; y+=a; z+=a; w+=a; return *this; }
	inline tvec4& operator-=( T a ){ x-=a; y-=a; z-=a; w-=a; return *this; }
	inline tvec4& operator*=( T a ){ x*=a; y*=a; z*=a; w*=a; return *this; }
	inline tvec4& operator/=( T a ){ x/=a; y/=a; z/=a; w/=a; return *this; }
	
	// comparison operators
	inline bool operator==( const tvec4& v ) const { return std::abs(x-v.x)<=precision<T>::value()&&std::abs(y-v.y)<=precision<T>::value()&&std::abs(z-v.z)<=precision<T>::value()&&std::abs(w-v.w)<=precision<T>::value(); }
	inline bool operator!=( const tvec4& v ) const { return !operator==(v); }
	
	// casting operators
	inline operator T*(){ return &x; }
	inline operator const T*(){ return &x; }

	// array access operators
	inline T& operator[]( ptrdiff_t i ){ /* fill in here */ }
	inline const T& operator[]( ptrdiff_t i ) const { /* fill in here */ }
	inline T& at( ptrdiff_t i ){ /* fill in here */ }
	inline const T& at( ptrdiff_t i ) const { /* fill in here */ }

	// unary operators
	inline tvec4 operator+() const { /* fill in here */ }
	inline tvec4 operator-() const { /* fill in here */ }

	// binary operators
	inline tvec4 operator+( const tvec4& v ) const { /* fill in here */ }
	inline tvec4 operator-( const tvec4& v ) const { /* fill in here */ }
	inline tvec4 operator*( const tvec4& v ) const { /* fill in here */ }
	inline tvec4 operator/( const tvec4& v ) const { /* fill in here */ }
	inline tvec4 operator+( T a ) const { /* fill in here */ }
	inline tvec4 operator-( T a ) const { /* fill in here */ }
	inline tvec4 operator*( T a ) const { /* fill in here */ }
	inline tvec4 operator/( T a ) const { /* fill in here */ }

	// length, normalize, dot product
	inline T length() const { /* fill in here */ }
	inline tvec4 normalize() const { /* fill in here */ }
	inline T dot( const tvec4& v ) const { /* fill in here */ }
};

//*******************************************************************
// type definitions
using vec2	= tvec2<float>;		using vec3	= tvec3<float>;		using vec4	= tvec4<float>;
using ivec2 = tvec2<int>;		using ivec3 = tvec3<int>;		using ivec4 = tvec4<int>;
using uvec2 = tvec2<uint>;		using uvec3 = tvec3<uint>;		using uvec4 = tvec4<uint>;
using dvec2 = tvec2<double>;	using dvec3 = tvec3<double>;	using dvec4 = tvec4<double>;

//*******************************************************************
// matrix 4x4: uses a standard row-major notation
struct mat4
{
	union { float a[16]; struct {float _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; };

	mat4(){ _12=_13=_14=_21=_23=_24=_31=_32=_34=_41=_42=_43=0.0f;_11=_22=_33=_44=1.0f; }
	mat4( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 ){_11=f11;_12=f12;_13=f13;_14=f14;_21=f21;_22=f22;_23=f23;_24=f24;_31=f31;_32=f32;_33=f33;_34=f34;_41=f41;_42=f42;_43=f43;_44=f44;}

	// comparison operators
	inline bool operator==( const mat4& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	inline bool operator!=( const mat4& m ) const { return !operator==(m); }

	// casting
	inline operator float*(){ return a; }
	inline operator const float*() const { return a; }

	// array access operator
	inline float& operator[]( ptrdiff_t i ){ /* fill in here */ }
	inline const float& operator[]( ptrdiff_t i ) const { /* fill in here */ }
	inline float& at( ptrdiff_t i ){ /* fill in here */ }
	inline const float& at( ptrdiff_t i ) const { /* fill in here */ }
	
	// identity and transpose
	static mat4 identity(){ return mat4(); }
	inline mat4& setIdentity(){ /* fill in here */ }
	inline mat4 transpose() const { /* fill in here */ }

	// binary operators
	inline vec4 operator*( const vec4& v ) const { /* fill in here */ }
	inline mat4 operator*( mat4& m ) const { /* fill in here */ }

	// determinant and inverse: see below for implemenations
	inline float determinant() const;
	inline mat4 inverse() const;
};

inline float mat4::determinant() const
{
	/* fill in here */
}

inline mat4 mat4::inverse() const
{
	float det=determinant();
	if( det==0 ) printf( "mat4::inverse() might be singular.\n" );
	
	/* fill in here */
}
