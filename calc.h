#ifndef CALC_H
#define CALC_H


#include <cmath>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <cstring>
#include <string>

namespace calc
{

////
// Constants.
////

constexpr float pi = 3.1415926535897932384626f;
constexpr float eps = 16 * std::numeric_limits<float>::epsilon();

////
// Mat class.
////

template<typename ValType, int NumRows, int NumCols>
class Mat;

template<typename T>
struct is_Mat {
	static constexpr bool value = false;
};

template<typename ValType, int NumRows, int NumCols>
struct is_Mat<Mat<ValType, NumRows, NumCols>> {
	static constexpr bool value = true;
};

template<typename ValType>
class Mat<ValType, 2, 1> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 2;
	constexpr static int num_cols = 1;

	ValType x;
	ValType y;

	const ValType& operator[](const int i) const { return (&x)[i]; }
	ValType& operator[](const int i) { return (&x)[i]; }
};

using Vec2 = Mat<float, 2, 1>;
using iVec2 = Mat<int, 2, 1>;

template<typename ValType>
class Mat<ValType, 3, 1> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 3;
	constexpr static int num_cols = 1;

	ValType x;
	ValType y;
	ValType z;

	const ValType& operator[](const int i) const { return (&x)[i]; }
	ValType& operator[](const int i) { return (&x)[i]; }
};

using Vec3 = Mat<float, 3, 1>;
using iVec3 = Mat<int, 3, 1>;

template<typename ValType>
class Mat<ValType, 4, 1> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 4;
	constexpr static int num_cols = 1;

	ValType x;
	ValType y;
	ValType z;
	ValType w;

	const ValType& operator[](const int i) const { return (&x)[i]; }
	ValType& operator[](const int i) { return (&x)[i]; }
};

using Vec4 = Mat<float, 4, 1>;
using iVec4 = Mat<int, 4, 1>;

template<typename ValType>
class Mat<ValType, 2, 2> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 2;
	constexpr static int num_cols = 2;

	Mat<ValType, 2, 1> col_vec_0;
	Mat<ValType, 2, 1> col_vec_1;

	const auto& operator[](const int i) const { return (&col_vec_0)[i]; }
	auto& operator[](const int i) { return (&col_vec_0)[i]; }

	auto& T()
	{
		std::swap((*this)[0][1], (*this)[1][0]);
		return *this;
	}
};

using Mat2 = Mat<float, 2, 2>;
using iMat2 = Mat<int, 2, 2>;

template<typename ValType>
class Mat<ValType, 3, 3> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 3;
	constexpr static int num_cols = 3;

	Mat<ValType, 3, 1> col_vec_0;
	Mat<ValType, 3, 1> col_vec_1;
	Mat<ValType, 3, 1> col_vec_2;

	const auto& operator[](const int i) const { return (&col_vec_0)[i]; }
	auto& operator[](const int i) { return (&col_vec_0)[i]; }

	auto& T()
	{
		std::swap((*this)[0][1], (*this)[1][0]);
		std::swap((*this)[0][2], (*this)[2][0]);
		std::swap((*this)[1][2], (*this)[2][1]);
		return *this;
	}
};

using Mat3 = Mat<float, 3, 3>;
using iMat3 = Mat<int, 3, 3>;

template<typename ValType>
class Mat<ValType, 4, 4> {
public:

	using value_type = ValType;
	constexpr static int num_rows = 4;
	constexpr static int num_cols = 4;

	Mat<ValType, 4, 1> col_vec_0;
	Mat<ValType, 4, 1> col_vec_1;
	Mat<ValType, 4, 1> col_vec_2;
	Mat<ValType, 4, 1> col_vec_3;

	const auto& operator[](const int i) const { return (&col_vec_0)[i]; }
	auto& operator[](const int i) { return (&col_vec_0)[i]; }

	auto& T()
	{
		std::swap((*this)[0][1], (*this)[1][0]);
		std::swap((*this)[0][2], (*this)[2][0]);
		std::swap((*this)[0][3], (*this)[3][0]);
		std::swap((*this)[1][2], (*this)[2][1]);
		std::swap((*this)[1][3], (*this)[3][1]);
		std::swap((*this)[2][3], (*this)[3][2]);
		return *this;
	}
};

using Mat4 = Mat<float, 4, 4>;
using iMat4 = Mat<int, 4, 4>;

////
// begin & end functions:
// Iterating each values in Mat in column major order.
////

template<typename ValType, int NumRows, int NumCols>
typename std::enable_if<NumCols == 1, ValType*>::type
	begin(Mat<ValType, NumRows, NumCols>& a)
{
	return (ValType*)(&a[0]);
}

template<typename ValType, int NumRows, int NumCols>
typename std::enable_if<NumCols != 1, ValType*>::type
	begin(Mat<ValType, NumRows, NumCols>& a)
{
	return (ValType*)(&a[0][0]);
}

template<typename ValType, int NumRows, int NumCols>
typename std::enable_if<NumCols == 1, const ValType*>::type
	begin(const Mat<ValType, NumRows, NumCols>& a)
{
	return (const ValType*)(&a[0]);
}

template<typename ValType, int NumRows, int NumCols>
typename std::enable_if<NumCols != 1, const ValType*>::type
	begin(const Mat<ValType, NumRows, NumCols>& a)
{
	return (const ValType*)(&a[0][0]);
}

template<typename ValType, int NumRows, int NumCols>
ValType* end(Mat<ValType, NumRows, NumCols>& a)
{
	return begin(a) + NumRows * NumCols;
}

template<typename ValType, int NumRows, int NumCols>
const ValType* end(const Mat<ValType, NumRows, NumCols>& a)
{
	return begin(a) + NumRows * NumCols;
}

////
// Element-wise operations.
////

#define ElementwiseOpsForMat(Op) \
template<typename ValType1, typename ValType2, int NumRows, int NumCols> \
auto operator##Op##=( \
	Mat<ValType1, NumRows, NumCols>& lhs, \
	const Mat<ValType2, NumRows, NumCols>& rhs) \
{ \
for (int i = 0; i < NumRows * NumCols; ++i) \
	* (begin(lhs) + i) Op##= *(begin(rhs) + i); \
return lhs; \
} \
\
template<typename ValType, typename ScalarType, int NumRows, int NumCols> \
typename std::enable_if< \
	std::is_same<float, ScalarType>::value \
	|| std::is_same<int, ScalarType>::value, \
	Mat<ValType, NumRows, NumCols>>::type \
operator##Op##=(Mat<ValType, NumRows, NumCols> & lhs, const ScalarType & rhs) \
{ \
for (int i = 0; i < NumRows * NumCols; ++i) \
	* (begin(lhs) + i) Op##= rhs; \
return lhs; \
} \
\
template<typename ValType1, typename ValType2, int NumRows, int NumCols> \
auto operator##Op( \
	const Mat<ValType1, NumRows, NumCols> & lhs, \
	const Mat<ValType2, NumRows, NumCols> & rhs) \
{ \
Mat<decltype(ValType1() + ValType2()), NumRows, NumCols> tmp{}; \
for (int i = 0; i < NumRows * NumCols; ++i) \
	* (begin(tmp) + i) = *(begin(lhs) + i) Op *(begin(rhs) + i); \
return tmp; \
} \
\
template<typename ValType, typename ScalarType, int NumRows, int NumCols> \
typename std::enable_if< \
	std::is_same<float, ScalarType>::value \
	|| std::is_same<int, ScalarType>::value, \
	Mat<decltype(ValType() + ScalarType()), NumRows, NumCols>>::type \
operator##Op( \
const Mat<ValType, NumRows, NumCols> & lhs, \
const ScalarType & rhs) \
{ \
Mat<decltype(ValType() + ScalarType()), NumRows, NumCols> tmp{}; \
for (int i = 0; i < NumRows * NumCols; ++i) \
	* (begin(tmp) + i) = *(begin(lhs) + i) Op rhs; \
return tmp; \
} \
\
template<typename ValType, typename ScalarType, int NumRows, int NumCols> \
typename std::enable_if< \
	std::is_same<float, ScalarType>::value \
	|| std::is_same<int, ScalarType>::value, \
	Mat<decltype(ValType() * ScalarType()), NumRows, NumCols>>::type \
operator##Op( \
const ScalarType & lhs, \
const Mat<ValType, NumRows, NumCols> & rhs) \
{ \
Mat<decltype(ValType() + ScalarType()), NumRows, NumCols> tmp{}; \
for (int i = 0; i < NumRows * NumCols; ++i) \
	* (begin(tmp) + i) = lhs Op * (begin(rhs) + i); \
return tmp; \
}

template<typename ValType1, typename ValType2, int NumRows, int NumCols>
bool operator==(
	const Mat<ValType1, NumRows, NumCols>& lhs,
	const Mat<ValType2, NumRows, NumCols>& rhs)
{
	auto lhs_iter = begin(lhs);
	auto rhs_iter = begin(rhs);
	while (lhs_iter != end(lhs)) {
		if (*lhs_iter != *rhs_iter)
			return false;
		lhs_iter++;
		rhs_iter++;
	}
	return true;
}

template<typename ValType1, typename ValType2, int NumRows, int NumCols>
bool operator!=(
	const Mat<ValType1, NumRows, NumCols>& lhs,
	const Mat<ValType2, NumRows, NumCols>& rhs)
{
	return !(lhs == rhs);
}

ElementwiseOpsForMat(+);
ElementwiseOpsForMat(-);
ElementwiseOpsForMat(*);
ElementwiseOpsForMat(/);

////
// Mod for int ValType.
////

template<int NumRows, int NumCols>
auto operator%=(
    Mat<int, NumRows, NumCols>& lhs, const Mat<int, NumRows, NumCols>& rhs)
{
    for (int i = 0; i < NumRows * NumCols; ++i)
        * (begin(lhs) + i) %= *(begin(rhs) + i);
    return lhs;
}

template<int NumRows, int NumCols>
auto operator%=(Mat<int, NumRows, NumCols> & lhs, int rhs)
{
    for (int i = 0; i < NumRows * NumCols; ++i)
        * (begin(lhs) + i) %= rhs;
    return lhs;
}

template<int NumRows, int NumCols>
auto operator%(
    Mat<int, NumRows, NumCols> lhs, const Mat<int, NumRows, NumCols>& rhs)
{
    return lhs %= rhs;
}

template<int NumRows, int NumCols>
auto operator%(Mat<int, NumRows, NumCols> lhs, int rhs)
{
    return lhs %= rhs;
}

template<int NumRows, int NumCols>
auto operator%(int lhs, const Mat<int, NumRows, NumCols> & rhs)
{
    Mat<int, NumRows, NumCols> tmp{};
    for (int i = 0; i < NumRows * NumCols; ++i)
        * (begin(tmp) + i) = lhs % *(begin(rhs) + i);
    return tmp;
}

////

template<typename ValType, int NumCols, int NumRows>
auto abs(Mat<ValType, NumCols, NumRows> a)
{
	for (auto* p = begin(a); p != end(a); ++p)
		*p = std::abs(*p);
	return a;
}

template<typename ValType, int NumCols, int NumRows>
auto operator-(Mat<ValType, NumCols, NumRows> a)
{
	for (auto* p = begin(a); p != end(a); ++p)
		*p = -*p;
	return a;
}

////
// Matrix operations.
////

template<typename ValType, int NumRows>
typename std::enable_if<
	(NumRows>1), Mat<ValType, NumRows, NumRows>>::type
transpose(Mat<ValType, NumRows, NumRows> m)
{
	return m.T();
}

inline auto det(const Mat<float, 2, 2> &m)
{
	const auto* p = begin(m);
	return p[0] * p[3] - p[1] * p[2];
}

inline auto det(const Mat<float, 3, 3> &m)
{
	const auto* p = begin(m);
	return p[0] * (p[4] * p[8] - p[5] * p[7])
		- p[3] * (p[1] * p[8] - p[2] * p[7])
		+ p[6] * (p[1] * p[5] - p[2] * p[4]);
}

inline auto det(const Mat<float, 4, 4> &m)
{
	const auto* p = begin(m);
	return p[10] * p[13] * p[3] * p[4] + p[0] * p[10] * p[15] * p[5]
		- p[10] * p[12] * p[3] * p[5] + p[11] * (-p[13] * p[2] * p[4]
		- p[0] * p[14] * p[5] + p[12] * p[2] * p[5] + p[0] * p[13] * p[6])
		- p[0] * p[10] * p[13] * p[7] - p[15] * p[2] * p[5] * p[8]
		+ p[14] * p[3] * p[5] * p[8] - p[13] * p[3] * p[6] * p[8]
		+ p[13] * p[2] * p[7] * p[8] + p[1] * (p[11] * p[14] * p[4]
		- p[10] * p[15] * p[4] - p[11] * p[12] * p[6] + p[10] * p[12] * p[7]
		+ p[15] * p[6] * p[8] - p[14] * p[7] * p[8])
		+ p[15] * p[2] * p[4] * p[9] - p[14] * p[3] * p[4] * p[9]
		- p[0] * p[15] * p[6] * p[9] + p[12] * p[3] * p[6] * p[9]
		+ p[0] * p[14] * p[7] * p[9] - p[12] * p[2] * p[7] * p[9];
}

inline auto inv(const Mat<float, 2, 2>& m)
{
	const auto* p = begin(m);
	return Mat<float, 2, 2>{
		{p[3], -p[1]},
		{ -p[2], p[0] }
	} /= det(m);
}

inline auto inv(const Mat<float, 3, 3>& m)
{
	const auto *p = begin(m);
	return Mat<float, 3, 3>{
	{
		-p[5] * p[7] + p[4] * p[8],
			p[2] * p[7] - p[1] * p[8],
			-p[2] * p[4] + p[1] * p[5]
	},
	{
		p[5] * p[6] - p[3] * p[8],
		-p[2] * p[6] + p[0] * p[8],
		p[2] * p[3] - p[0] * p[5]
	},
	{
		-p[4] * p[6] + p[3] * p[7],
		p[1] * p[6] - p[0] * p[7],
		-p[1] * p[3] + p[0] * p[4]
	}
	} /= det(m);
}

inline auto inv(const Mat<float, 4, 4>& m)
{
	const auto* p = begin(m);
	return Mat<float, 4, 4>{
	{
		-p[11] * p[14] * p[5] + p[10] * p[15] * p[5] + p[11] * p[13] * p[6]
			- p[10] * p[13] * p[7] - p[15] * p[6] * p[9] + p[14] * p[7] * p[9],
			p[1] * p[11] * p[14] - p[1] * p[10] * p[15] - p[11] * p[13] * p[2]
			+ p[10] * p[13] * p[3] + p[15] * p[2] * p[9] - p[14] * p[3] * p[9],
			-p[15] * p[2] * p[5] + p[14] * p[3] * p[5] + p[1] * p[15] * p[6]
			- p[13] * p[3] * p[6] - p[1] * p[14] * p[7] + p[13] * p[2] * p[7],
			p[11] * p[2] * p[5] - p[10] * p[3] * p[5] - p[1] * p[11] * p[6]
			+ p[1] * p[10] * p[7] + p[3] * p[6] * p[9] - p[2] * p[7] * p[9]
	},
	{
		p[11] * p[14] * p[4] - p[10] * p[15] * p[4] - p[11] * p[12] * p[6]
		+ p[10] * p[12] * p[7] + p[15] * p[6] * p[8] - p[14] * p[7] * p[8],
		-p[0] * p[11] * p[14] + p[0] * p[10] * p[15] + p[11] * p[12] * p[2]
		- p[10] * p[12] * p[3] - p[15] * p[2] * p[8] + p[14] * p[3] * p[8],
		p[15] * p[2] * p[4] - p[14] * p[3] * p[4] - p[0] * p[15] * p[6]
		+ p[12] * p[3] * p[6] + p[0] * p[14] * p[7] - p[12] * p[2] * p[7],
		-p[11] * p[2] * p[4] + p[10] * p[3] * p[4] + p[0] * p[11] * p[6]
		- p[0] * p[10] * p[7] - p[3] * p[6] * p[8] + p[2] * p[7] * p[8]
	},
	{
		-p[11] * p[13] * p[4] + p[11] * p[12] * p[5] - p[15] * p[5] * p[8]
		+ p[13] * p[7] * p[8] + p[15] * p[4] * p[9] - p[12] * p[7] * p[9],
		-p[1] * p[11] * p[12] + p[0] * p[11] * p[13] + p[1] * p[15] * p[8]
		- p[13] * p[3] * p[8] - p[0] * p[15] * p[9] + p[12] * p[3] * p[9],
		-p[1] * p[15] * p[4] + p[13] * p[3] * p[4] + p[0] * p[15] * p[5]
		- p[12] * p[3] * p[5] + p[1] * p[12] * p[7] - p[0] * p[13] * p[7],
		p[1] * p[11] * p[4] - p[0] * p[11] * p[5] + p[3] * p[5] * p[8]
		- p[1] * p[7] * p[8] - p[3] * p[4] * p[9] + p[0] * p[7] * p[9]
	},
	{
		p[10] * p[13] * p[4] - p[10] * p[12] * p[5] + p[14] * p[5] * p[8]
		- p[13] * p[6] * p[8] - p[14] * p[4] * p[9] + p[12] * p[6] * p[9],
		p[1] * p[10] * p[12] - p[0] * p[10] * p[13] - p[1] * p[14] * p[8]
		+ p[13] * p[2] * p[8] + p[0] * p[14] * p[9] - p[12] * p[2] * p[9],
		p[1] * p[14] * p[4] - p[13] * p[2] * p[4] - p[0] * p[14] * p[5]
		+ p[12] * p[2] * p[5] - p[1] * p[12] * p[6] + p[0] * p[13] * p[6],
		-p[1] * p[10] * p[4] + p[0] * p[10] * p[5] - p[2] * p[5] * p[8]
		+ p[1] * p[6] * p[8] + p[2] * p[4] * p[9] - p[0] * p[6] * p[9]
	}
	} /= det(m);
}

template<typename ValType, int NumCols, int NumRows>
ValType value_sum(const Mat<ValType, NumCols, NumRows>& a)
{
	ValType sum{ 0 };
	for (const auto* p = begin(a); p != end(a); ++p)
		sum += *p;
	return sum;
}

// Inner-product of two vecters.
template<int NumRows, int NumCols>
typename std::enable_if<NumCols == 1, float>::type
dot(const Mat<float, NumRows, NumCols>& p,
	const Mat<float, NumRows, NumCols>& q)
{
	auto result = p * q;
	return value_sum(result);
}

// Inner-product of matrix A, with vector v (Av).
template<int MatSize, int NumCols>
typename std::enable_if<
	MatSize != 1 && NumCols == 1, Mat<float, MatSize, 1>>::type
dot(const Mat<float, MatSize, MatSize>& A,
	const Mat<float, MatSize, NumCols>& v)
{
	Mat<float, MatSize, 1> result{};
	for (int i = 0; i < MatSize; ++i)
		result += A[i]*v[i];
	return result;
}

// Inner-product of two matrices, A and B.
template<int MatSize>
typename std::enable_if<
	MatSize != 1, Mat<float, MatSize, MatSize>>::type
dot(const Mat<float, MatSize, MatSize>& A,
	const Mat<float, MatSize, MatSize>& B)
{
	Mat<float, MatSize, MatSize> result{};
	for (int i = 0; i < MatSize; ++i)
		result[i] = dot(A, B[i]);
	return result;
}

template<int NumRows>
auto length(const Mat<float,NumRows, 1>& v)
{
	return std::sqrtf(dot(v,v));
}

template<int NumRows>
auto normalize(const Mat<float, NumRows, 1>& v)
{
	return v / length(v);
}

inline Vec3 cross(const Vec3& p, const Vec3& q)
{
	return Vec3{
		p.y * q.z - q.y * p.z,
		p.z * q.x - q.z * p.x,
		p.x * q.y - q.x * p.y
	};
}

////
// Quaternion w+xi+yj+zk.
////

class Quat {
public:
	float w;
	float x;
	float y;
	float z;

	Quat()
		: w{1}, x{ 0 }, y{ 0 }, z{ 0 }
	{}

	Quat(float w, float x, float y, float z)
		: w{ w }, x{ x }, y{ y }, z{ z }
	{}

	Quat(float w, const Vec3& v)
		: w{ w }, x{ v.x }, y{ v.y }, z{ v.z }
	{}

	float real() const { return w; }
	void real(float w) { this->w = w; }
	Vec3 imag() const { return Vec3{ x, y, z }; }
	void imag(const Vec3& v) { x = v.x; y = v.y; z = v.z; }

	// Input axis should be a unit vector.
	static Quat angle_axis(float angle, Vec3 axis)
	{
		return Quat{ cos(angle * .5f),
			axis * std::sinf(angle * .5f)};
	}
};

inline float dot(const Quat& p, const Quat& q)
{
	return p.x*q.x + p.y*q.y + p.z*q.z + p.w*q.w;
}

inline Quat& operator+=(Quat lhs, const Quat& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
	return lhs;
}

inline Quat operator+(Quat lhs, const Quat& rhs)
{
	return lhs += rhs;
}

inline Quat& operator-=(Quat lhs, const Quat& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.w -= rhs.w;
	return lhs;
}

inline Quat operator-(Quat lhs, const Quat& rhs)
{
	return lhs -= rhs;
}

inline Quat operator*(const Quat& p, float s)
{
	return Quat{p.w*s,p.x*s,p.y*s,p.z*s};
}

inline Quat operator*(float s, const Quat& p)
{
	return p*s;
}

inline Quat& operator*=(Quat & lhs, const Quat & rhs)
{
	const Quat lhs_{ lhs };
	lhs.w = lhs_.w * rhs.w - lhs_.x * rhs.x - lhs_.y * rhs.y - lhs_.z * rhs.z;
	lhs.x = lhs_.w * rhs.x + lhs_.x * rhs.w + lhs_.y * rhs.z - lhs_.z * rhs.y;
	lhs.y = lhs_.w * rhs.y + lhs_.y * rhs.w + lhs_.z * rhs.x - lhs_.x * rhs.z;
	lhs.z = lhs_.w * rhs.z + lhs_.z * rhs.w + lhs_.x * rhs.y - lhs_.y * rhs.x;
	return lhs;
}

inline Quat operator*(Quat lhs, const Quat & rhs)
{
	return lhs *= rhs;
}

inline Quat& operator/=(Quat& p, float s)
{
	p.x /= s;
	p.y /= s;
	p.z /= s;
	p.w /= s;
	return p;
}

inline Quat operator/(const Quat& p, float s)
{
	Quat result{ p };
	return result /= s;;
}

inline Quat operator/(float w, const Quat& p)
{
	return Quat{w/p.x,w/p.y,w/p.z,w/p.w};
}

inline Quat & operator/=(Quat & lhs, const Quat & rhs)
{
	const Quat lhs_{ lhs };

	lhs.w = lhs_.w * rhs.w + lhs_.x * rhs.x + lhs_.y * rhs.y + lhs_.z * rhs.z;
	lhs.x = lhs_.x * rhs.w - lhs_.w * rhs.x - lhs_.z * rhs.y + lhs_.y * rhs.z;
	lhs.y = lhs_.y * rhs.w + lhs_.z * rhs.x - lhs_.w * rhs.y - lhs_.x * rhs.z;
	lhs.z = lhs_.z * rhs.w - lhs_.y * rhs.x + lhs_.x * rhs.y - lhs_.w * rhs.z;

	lhs /= dot(rhs, rhs);

	return lhs;
}

inline Quat operator/(Quat lhs, const Quat & rhs)
{
	return lhs /= rhs;
}

inline Quat conj(const Quat& q)
{
	return Quat{q.w, -q.x,-q.y,-q.z};
}

inline Quat inv(const Quat& q)
{
	return conj(q) / dot(q,q);
}

inline float length(const Quat& q)
{
	return std::sqrtf(dot(q,q));
}

inline Quat normalize(Quat q)
{
	return q / length(q);
}

// Input Quat should be a unit Quaternion.
inline Vec3 rotate(const Quat& q, const Vec3& v)
{
	return 2.f*dot(q.imag(), v) * q.imag()
		+ (q.w*q.w - dot(q.imag(), q.imag())) * v
		+ 2.f*q.w * cross(q.imag(), v);
}

// Input q should be normalized first.
inline Mat3 Quat2Mat3(const Quat& q)
{
	float xx = q.x*q.x, yy = q.y*q.y, zz = q.z*q.z, xz = q.x*q.z, \
		xy = q.x*q.y, yz = q.y*q.z, wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;

	return Mat3{
		{ 1.f - 2.f * (yy + zz), 2.f * (xy + wz), 2.f * (xz - wy)},
		{ 2.f * (xy - wz), 1.f - 2.f * (xx + zz), 2.f * (yz + wx)},
		{ 2.f * (xz + wy), 2.f * (yz - wx), 1.f - 2.f * (xx + yy)}};
}

inline float* begin(Quat& q) { return &(q.w); }
inline const float* begin(const Quat& q) { return &(q.w); }
inline float* end(Quat& q) {return begin(q) + 4; }
inline const float* end(const Quat& q) {return begin(q) + 4; }

////
// Hash functionalities
////

// https://stackoverflow.com/q/35985960
template <typename T>
std::size_t hash_combine(const std::size_t& seed, const T& v)
{
	auto hasher = std::hash<T>();
    return seed ^ (hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

template<typename ValType, int NumRows, int NumCols>
std::size_t hash(const Mat<ValType, NumRows, NumCols>& a)
{
	std::size_t seed = 0xc01ddeadbeefbead;
    for (auto val: a)
		seed = hash_combine(seed, val);
	return seed;
}

// PCG, A Family of Better Random Number Generators.
class PCG {
public:

	using result_type = uint32_t;

	PCG(uint64_t seed)
		: state_{ seed }
	{}

	static uint32_t max() { return std::numeric_limits<uint32_t>::max(); }
	static uint32_t min() { return std::numeric_limits<uint32_t>::min(); }

	uint32_t operator()()
	{
		// Advance state.
		state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;

		// PCG randomize.
		auto xorshift = static_cast<uint32_t>((state_ ^ (state_ >> 18u)) >> 27u);

		auto shift = state_ >> 59u;
		return (xorshift >> shift) | (xorshift << ((-shift) & 31));
	}

private:
	uint64_t state_;
};

////
// Utility functions.
////

template<typename MatType, typename ScalarType>
typename std::enable_if<
	is_Mat<MatType>::value, MatType>::type
diag(const ScalarType& v)
{
	MatType m{};
	static_assert(
		MatType::num_rows == MatType::num_cols, "mat size not equal.");
	for (int d = 0; d < MatType::num_cols; ++d)
		m[d][d] = static_cast<MatType::value_type>(v);
	return m;
}

template<typename MatType1, typename MatType2>
typename std::enable_if<
	is_Mat<MatType1>::value && 
	is_Mat<MatType2>::value, MatType1>::type
cast(const MatType2& src)
{
	MatType1 dest{};
	int min_rows = std::min(MatType1::num_rows, MatType2::num_rows);
	int min_cols = std::min(MatType1::num_cols, MatType2::num_cols);
	
	for (int col = 0; col < min_cols; ++col)
		for (int row = 0; row < min_rows; ++row) {
			begin(dest)[row+col*MatType1::num_rows] = \
				begin(src)[row+col*MatType2::num_rows];
		}

	return dest;
}

inline float to_radian(const float degree)
{
	return degree * pi / 180.f;
}

template<typename T>
inline T lerp(T v0, T v1, float t)
{
	return v0+(v1-v0)*t;
}

inline float clamp(float s, float min, float max)
{
	return s>max?max:(s<min?min:s);
}

template<typename ValType, int NumRows, int NumCols>
auto minimum(
	Mat<ValType, NumRows, NumCols> lhs,
	const Mat<ValType, NumRows, NumCols>& rhs)
{
	auto lhs_iter = begin(lhs);
	auto rhs_iter = begin(rhs);
	while (lhs_iter!=end(lhs)) {
		*lhs_iter = std::min(*lhs_iter,*rhs_iter);
		lhs_iter++;
		rhs_iter++;
	}
	return lhs;
}

template<typename ValType, int NumRows, int NumCols>
auto maximum(
	Mat<ValType, NumRows, NumCols> lhs,
	const Mat<ValType, NumRows, NumCols>& rhs)
{
	auto lhs_iter = begin(lhs);
	auto rhs_iter = begin(rhs);
	while (lhs_iter!=end(lhs)) {
		*lhs_iter = std::max(*lhs_iter,*rhs_iter);
		lhs_iter++;
		rhs_iter++;
	}
	return lhs;
}

template<typename ValType1, typename ValType2, int NumRows, int NumCols,
	typename BinaryPredicate>
bool compare(
	const Mat<ValType1, NumRows, NumCols>& lhs,
	const Mat<ValType2, NumRows, NumCols>& rhs,
	BinaryPredicate op)
{
	auto lhs_iter = begin(lhs);
	auto rhs_iter = begin(rhs);
	while (lhs_iter != end(lhs)) {
		if (!op(*lhs_iter, *rhs_iter))
			return false;
		++lhs_iter;
		++rhs_iter;
	}
	return true;
}

inline Vec3 unproject(
	const Vec3& winpos, 
	const Mat4& view, 
	const Mat4& proj, 
	const Vec4& viewport)
{
	Vec4 tmp{
		(winpos.x - viewport.x) / viewport.z * 2.f - 1.f,
		(winpos.y - viewport.y) / viewport.w * 2.f - 1.f,
		winpos.z * 2.f - 1.f,
		1.f };

	Vec4 obj = dot(inv(dot(proj, view)), tmp);
	obj /= obj.w;
	return cast<Vec3>(obj);
}

inline Mat2 rotation_transform(float angle)
{
	return Mat2{
		{std::cosf(angle), std::sinf(angle)}, 
		{-std::sinf(angle), std::cosf(angle)}};
}

// Input axis should be a unit vector.
inline Mat3 rotation_transform(float angle, Vec3 axis)
{
	float cosw = std::cosf(angle), sin_w = std::sinf(angle);
	float _1cosw = 1-cosw;
	float x = axis.x, y = axis.y, z = axis.z;
	float zz = z*z, xx = x*x, yy = y*y, yz = y*z, xz = x*z, xy = x*y;
	float x_sin_w = x*sin_w, y_sin_w = y*sin_w, z_sin_w = z*sin_w;
	float xz_1cosw = xz*_1cosw;
	float xy_1cosw = xy*_1cosw;
	float yz_1cosw = yz*_1cosw;
	return Mat3{
		{ xx*_1cosw+cosw, xy_1cosw+z*sin_w, xz_1cosw-y_sin_w},
		{ xy_1cosw-z_sin_w, yy*_1cosw+cosw, yz_1cosw+x_sin_w},
		{ xz_1cosw+y_sin_w, yz_1cosw-x_sin_w, zz*_1cosw+cosw}};
}

template<int NumRows>
typename std::enable_if<
	NumRows==2||NumRows==3, Mat<float, NumRows+1,NumRows+1>>::type
affine_transform(
	const Mat<float, NumRows, NumRows>& basis,
	const Mat<float, NumRows, 1>& translation)
{
	Mat<float, NumRows+1,NumRows+1> result{};
	for (int col = 0; col < NumRows; ++col)
		for (int row = 0; row < NumRows; ++row)
			result[col][row] = basis[col][row];
	for (int row = 0; row < NumRows; ++row)
		result[NumRows][row] = translation[row];
	result[NumRows][NumRows] = 1;
	return result;
}

// Solve for coordinates under new basis and origin.
inline Mat4 view_transform(
	Mat3 view_base, const Vec3& view_point)
{
	view_base.T();
	return affine_transform(view_base, dot(view_base, -view_point));
}

inline Mat4 lookat(const Vec3 eye, const Vec3 spot, const Vec3 up)
{
	const Vec3 forward_ = normalize(spot - eye);
	const Vec3 s = normalize(cross(forward_, up));
	const Vec3 up_ = normalize(cross(s, forward_));

	 return view_transform(Mat3{s,up_,-forward_}, eye);
}

inline Mat4 projective_transform(
	float fovy, float aspect, float znear, float zfar)
{
	float tan_half_fovy = std::tanf(fovy / 2.f);
	float tan_half_fovx = aspect * tan_half_fovy;

	Mat4 tmp{};
	tmp[0][0] = 1.f / (tan_half_fovx);
	tmp[1][1] = 1.f / (tan_half_fovy);
	tmp[2][2] = -(zfar + znear) / (zfar - znear);
	tmp[2][3] = -1.f;
	tmp[3][2] = -(2.f * zfar * znear) / (zfar - znear);
	return tmp;
}

inline Mat4 orthographic_transform(
	float left, float right, float bottom, float top, 
	float near_plane, float far_plane)
{
	Mat4 tmp{};
	tmp[0][0] = 2.f / (right - left);
	tmp[1][1] = 2.f / (top - bottom);
	tmp[2][2] = -2.f / (far_plane - near_plane);
	tmp[3][0] = -(right + left) / (right - left);
	tmp[3][1] = -(top + bottom) / (top - bottom);
	tmp[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
	tmp[3][3] = 1;
	return tmp;
}

inline Vec3 point_transform(
    const calc::Mat4& transform, const Vec3 point)
{
    auto point_ = cast<Vec4>(point);
    point_.w = 1.f;
    point_ = dot(transform, point_);
    point_ /= point_.w;
    return cast<Vec3>(point_);
}

////
// Ray tracing.
////

class Ray {
public:
	Vec3 o;
	Vec3 d;

	// hit point: o+s*d.
	mutable float s;
};

template<typename VecType>
class Box;

template <typename VecType>
Box<VecType> box_from_points(const std::vector<VecType>& points);

template<typename VecType>
class Box {
public:

	Box() 
		: sup_{-1}, inf_{1}
	{}

	Box(VecType center, VecType size)
		: sup_{center+.5f*size}, inf_{center-.5f*size}
	{}

	VecType sup() { return sup_; }
	VecType inf() { return inf_; }

	VecType center() const {return (sup_+inf_)*.5f;}
	VecType size() const {return sup_-inf_;}

	void update(VecType point)
	{
		if (sup_.x < inf_.x) {
			*this = box_from_points(std::vector<VecType>{point});
			return;
		}

		inf_ = calc::minimum(inf_,point);
		sup_ = calc::maximum(sup_,point);
	}

private:
	VecType sup_;
	VecType inf_;
};

template <typename VecType>
Box<VecType> box_from_points(const std::vector<VecType>& points)
{
	if (points.size() == 0)
		return Box<VecType>{};

	VecType inf = points[0], sup = points[0];
	for (int i = 1; i < points.size(); ++i) {
		inf = calc::minimum(inf,points[i]);
		sup = calc::maximum(sup,points[i]);
	}
	auto center = (inf+sup)*.5f;
	auto size = sup - inf;
	return Box<VecType>{center, size};
}

using Box2D = Box<Vec2>;
using Box3D = Box<Vec3>;

// template<typename VecType>
// bool intersect(
// 	const Box<VecType>& box1, 
// 	const Box<VecType>& box2, 
// 	Box<VecType>& result)
// {
// 	auto inf = maximum(box1.minimum(),box2.minimum());
// 	auto sup = minimum(box1.maximum(),box2.maximum());
// 	if (compare(inf,sup,[](
// 			const VecType::value_type& lhs,
// 			const VecType::value_type& rhs)
// 			{return lhs <= rhs;})) {
// 		result = Box<VecType>::from_points({inf, sup});
// 		return true;
// 	}
// 	else
// 		return false;
// }

template<typename VecType>
auto box_union(
	const Box<VecType>& box1, 
	const Box<VecType>& box2)
{
	if (box1.size() == VecType{})
		return box2;
	if (box2.size() == VecType{})
		return box1;
	auto inf = minimum(box1.minimum(),box2.minimum());
	auto sup = maximum(box1.maximum(),box2.maximum());
	return Box<VecType>::from_points({inf, sup});
}

class Sphere {
public:
	Vec3 o;
	float r;
};

inline bool hit(const Sphere &sphere, const Ray &ray)
{
	auto b = 2.f*dot(ray.o - sphere.o, ray.d);
	auto c = dot(ray.o - sphere.o, ray.o - sphere.o) - \
		sphere.r * sphere.r;
	auto delta = b * b - 4 * c;
	if (delta <= eps)
		return false;

	delta = std::sqrtf(delta);
	auto t1 = std::min(.5f*(-b - delta), .5f*(-b + delta));
	auto t2 = std::max(.5f*(-b - delta), .5f*(-b + delta));

	if (t2 < eps)
		return false;

	if (t1 >= eps)
		ray.s = t1;
	else
		ray.s = t2;

	return true;
}


}


#endif
