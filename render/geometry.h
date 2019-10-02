#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

//定义基本的 int float  二维/三维/四维 的向量/坐标 的运算
template<size_t DimCols, size_t DimRows, typename T> class mat;

//定义一个泛型数组
template <size_t DIM, typename T> 
struct vec {
	
	vec() 
	{ 
		for (size_t i = DIM; i--; data_[i] = T()); 
	}

	//已放弃使用assert()的原因是，频繁的调用会极大的影响程序的性能，增加额外的开销。
	T& operator[](const size_t i) 
	{ 
		assert(i < DIM); 
		return data_[i]; 
	}
	const T& operator[](const size_t i) const 
	{ 
		assert(i < DIM); 

		return data_[i];
	}
//private:
public:
	T data_[DIM]; 
};

/////////////////////////////////////////////////////////////////////////////////

//模板偏特化
template <typename T> 
struct vec<2, T> {
	vec() : x(T()), y(T()) {}
	vec(T X, T Y) : x(X), y(Y) {}

	//嵌套 方便传入类型跟返回类型T 不一致
	template <class U> 
	vec<2, T>(const vec<2, U>& v);

	T& operator[](const size_t i) { 
		assert(i < 2); 
		return i <= 0 ? x : y;
	}

	const T& operator[](const size_t i) const { 
		assert(i < 2); 
		return i <= 0 ? x : y; 
	}
	T x, y;
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T> 
struct vec<3, T> {
	vec() : x(T()), y(T()), z(T()) {}
	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

	template <class U> 
	vec<3, T>(const vec<3, U>& v);

	T& operator[](const size_t i) { 
		assert(i < 3); 
		return i <= 0 ? x : (1 == i ? y : z); 
	}

	template <class U>
	vec<3, T>& operator=(vec<3, U> v) {
		
		x = T(v.x + .5f);
		y = T(v.y + .5f);
		z = T(v.z + .5f);
		return *this;
	}

	const T& operator[](const size_t i) const 
	{ 
		assert(i < 3); 
		return i <= 0 ? x : (1 == i ? y : z);
	}
	// 矢量归一化  注意归一化，不涉及W。 只有四维向三维投影的时候，W归一化才有用
	float norm() { return std::sqrt(x * x + y * y + z * z); }
	vec<3, T>& normalize(T l = 1) {
		//if (length != 0.0f)
		*this = (*this) * (l / norm()); 
		return *this;
	}

	T x, y, z;
};


template <typename T>
struct vec<4, T> {
	vec() : x(T()), y(T()), z(T()),w(1) {}
	vec(T X, T Y, T Z) : x(X), y(Y), z(Z), w(1) {}
	vec(T X, T Y, T Z,T W) : x(X), y(Y), z(Z), w(W) {}
	template <class U>
	vec<4, T>(const vec<4, U>& v);

	T& operator[](const size_t i) {
		assert(i < 4);
		return i <= 0 ? x : (1 == i ? y : (2 == i ? z:w));
	}

	vec<4, T>& operator=(vec<4, T> v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	template <class U>
	vec<4, T>& operator=(vec<4, U> v) {

		x = T(v.x + .5f);
		y = T(v.y + .5f);
		z = T(v.z + .5f);
		w = T(v.w + .5f);
		return *this;
	}
	
//齐次坐标的第二个功效就是区分向量跟点.  w为1则为点.  w为0则为向量。  因为w作用是位移，向量位移没什么意义。
// z = x + y  w一直为1
	template <class U>
	vec<4, T>& operator+(vec<4, U> v) {

		x = T(v.x + this->x);
		y = T(v.y + this->y);
		z = T(v.z + this->z);
		return *this;
	}

	vec<4, T>& operator+(vec<4, T> v) {

		x = T(v.x + this->x);
		y = T(v.y + this->y);
		z = T(v.z + this->z);
		return *this;
	}

	template <class U>
	vec<4, T>& operator-(vec<4, U> u) {

		x = T(this->x - u.x);
		y = T(this->y - u.y);
		z = T(this->z - u.z);
		return *this;
	}

	vec<4, T>& operator-(vec<4, T> u) {

		x = T(this->x - u.x);
		y = T(this->y - u.y);
		z = T(this->z - u.z);
		return *this;
	}

	vec<4, T>& operator*(float u) {

		x = T(this->x * u);
		y = T(this->y * u);
		z = T(this->z * u);
		return *this;
	}

	//矢量点乘
	template <class U>
	float operator*(vec<4, U> v) {
		return x * v.x + y * v.y + z * v.z;
	}


	const T& operator[](const size_t i) const
	{
		assert(i < 4);
		return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
	}

	//vector_length   |v|
	float norm() { 
		return std::sqrt(x * x + y * y + z * z); 
	}

	//
	vec<4, T>& normalize(T l = 1) { 
		x= x * (l / norm());
		y = y * (l / norm());
		z = z * (l / norm());
		return *this; 
	}

	T x, y, z,w;
};


/////////////////////////////////////////////////////////////////////////////////

template<typename T>
T interp(T x1, T x2, T t) {
	return x1 + (x2 - x1) * t;
}

template<typename T>
//不允许超过上下限 int
T CMID(T x, T min, T max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}


/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM, typename T> 
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
	T ret = T();
	for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
	return ret;
}


template<size_t DIM, typename T>vec<DIM, T> 
operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs)
{
	for (size_t i = DIM; i--; lhs[i] += rhs[i]);
	return lhs;
}

template<size_t DIM, typename T>vec<DIM, T> 
operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
	for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
	return lhs;
}

template<size_t DIM, typename T, typename U> vec<DIM, T> 
operator*(vec<DIM, T> lhs, const U& rhs) {
	for (size_t i = DIM; i--; lhs[i] *= rhs);
	return lhs;
}

template<size_t DIM, typename T, typename U> vec<DIM, T> 
operator/(vec<DIM, T> lhs, const U& rhs) {
	for (size_t i = DIM; i--; lhs[i] /= rhs);
	return lhs;
}

template<size_t LEN, size_t DIM, typename T> vec<LEN, T> 
embed(const vec<DIM, T>& v, T fill = 1) {
	vec<LEN, T> ret;
	for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));
	return ret;
}

template<size_t LEN, size_t DIM, typename T> 
vec<LEN, T> proj(const vec<DIM, T>& v) {
	vec<LEN, T> ret;
	for (size_t i = LEN; i--; ret[i] = v[i]);
	return ret;
}

template <typename T> 
vec<3, T> cross(vec<3, T> v1, vec<3, T> v2) {
	return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
// 矢量叉乘  如果 x y 标准化，那算出来的叉乘Z也是标准化的xy平面的法线。 否则的话，要标准化一次Z
template <typename T>
vec<4, T> cross(vec<4, T> v1, vec<4, T> v2) { //参数 const指针?
	return vec<4, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x,1);
}

// 矢量lerp插值，t取值 [0, 1]
template <typename T>
void vector_interp(vec<4, T>* z, const vec<4, T>* x1, const vec<4, T>* x2, float t)
{
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = interp(x1->w, x2->w, t);//这是深度插值。  非线性插值，用的 1/Z
}


template <size_t DIM, typename T> 
std::ostream& operator<<(std::ostream& out, vec<DIM, T>& v) {
	for (unsigned int i = 0; i < DIM; i++) {
		out << v[i] << " ";
	}
	return out;
}

/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM, typename T> 
struct dt {
	static T det(const mat<DIM, DIM, T>& src) {
		T ret = 0;
		for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
		return ret;
	}
};

template<typename T> 
struct dt<1, T> {
	static T det(const mat<1, 1, T>& src) {
		return src[0][0];
	}
};

/////////////////////////////////////////////////////////////////////////////////
//行 -列
template<size_t DimRows, size_t DimCols, typename T> 
class mat {
	vec<DimCols, T> rows[DimRows];
public:
	mat() {}

	vec<DimCols, T>& operator[] (const size_t idx) {
		assert(idx < DimRows);
		return rows[idx];
	}

	const vec<DimCols, T>& operator[] (const size_t idx) const {
		assert(idx < DimRows);
		return rows[idx];
	}

	vec<DimRows, T> col(const size_t idx) const {
		assert(idx < DimCols);
		vec<DimRows, T> ret;
		for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
		return ret;
	}

	void set_col(size_t idx, vec<DimRows, T> v) {
		assert(idx < DimCols);
		for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
	}

	static mat<DimRows, DimCols, T> identity() {
		mat<DimRows, DimCols, T> ret;
		for (size_t i = DimRows; i--; )
			for (size_t j = DimCols; j--; ret[i][j] = (i == j));
		return ret;
	}

	T det() const {
		return dt<DimCols, T>::det(*this);
	}

	mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
		mat<DimRows - 1, DimCols - 1, T> ret;
		for (size_t i = DimRows - 1; i--; )
			for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
		return ret;
	}

	T cofactor(size_t row, size_t col) const {
		return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
	}

	mat<DimRows, DimCols, T> adjugate() const {
		mat<DimRows, DimCols, T> ret;
		for (size_t i = DimRows; i--; )
			for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
		return ret;
	}

	mat<DimRows, DimCols, T> invert_transpose() {
		mat<DimRows, DimCols, T> ret = adjugate();
		T tmp = ret[0] * rows[0];
		return ret / tmp;
	}

	mat<DimRows, DimCols, T> invert() {
		return invert_transpose().transpose();
	}

	mat<DimCols, DimRows, T> transpose() {
		mat<DimCols, DimRows, T> ret;
		for (size_t i = DimCols; i--; ret[i] = this->col(i));
		return ret;
	}
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DimRows, size_t DimCols, typename T> 
vec<DimRows, T> operator*(const mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs) {
	vec<DimRows, T> ret;
	for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
	return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs) {
	mat<R1, C2, T> result;
	for (size_t i = R1; i--; )
		for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
	return result;
}

template<size_t DimRows, size_t DimCols, typename T>
mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T& rhs) {
	for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
	return lhs;
}

template <size_t DimRows, size_t DimCols, class T> 
std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m) {
	for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
	return out;
}

/////////////////////////////////////////////////////////////////////////////////

typedef vec<2, float> Vec2f;
typedef vec<2, int>   Vec2i;
typedef vec<3, float> Vec3f;
typedef vec<3, int>   Vec3i;
typedef vec<4, float> Vec4f;
typedef mat<4, 4, float> Matrix;
typedef Vec4f point_t;
///typedef vec<5, float> Vec5f;
#endif //__GEOMETRY_H__



