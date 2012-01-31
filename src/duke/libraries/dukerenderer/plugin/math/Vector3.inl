#include <cmath>

template<class T>
inline Vector3<T>::Vector3(T X, T Y, T Z) :
	x(X), y(Y), z(Z) {
}

template<class T>
inline void Vector3<T>::Set(T X, T Y, T Z) {
	x = X;
	y = Y;
	z = Z;
}

template<class T>
inline T Vector3<T>::Length() const {
	return sqrt(LengthSq());
}

template<class T>
inline T Vector3<T>::LengthSq() const {
	return x * x + y * y + z * z;
}

template<class T>
inline void Vector3<T>::Normalize() {
	T Norm = Length();

	if (std::abs(Norm) > std::numeric_limits<T>::epsilon()) {
		x /= Norm;
		y /= Norm;
		z /= Norm;
	}
}

template<class T>
inline Vector3<T> Vector3<T>::operator +() const {
	return this;
}

template<class T>
inline Vector3<T> Vector3<T>::operator -() const {
	return Vector3<T> (-x, -y, -z);
}

template<class T>
inline Vector3<T> Vector3<T>::operator +(const Vector3<T>& v) const {
	return Vector3<T> (x + v.x, y + v.y, z + v.z);
}

template<class T>
inline Vector3<T> Vector3<T>::operator -(const Vector3<T>& v) const {
	return Vector3<T> (x - v.x, y - v.y, z - v.z);
}

template<class T>
inline const Vector3<T>& Vector3<T>::operator +=(const Vector3<T>& v) {
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

template<class T>
inline const Vector3<T>& Vector3<T>::operator -=(const Vector3<T>& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

template<class T>
inline const Vector3<T>& Vector3<T>::operator *=(T t) {
	x *= t;
	y *= t;
	z *= t;

	return *this;
}

template<class T>
inline const Vector3<T>& Vector3<T>::operator /=(T t) {
	x /= t;
	y /= t;
	z /= t;

	return *this;
}

template<class T>
inline bool Vector3<T>::operator ==(const Vector3<T>& v) const {
	return ((std::abs(x - v.x) <= std::numeric_limits<T>::epsilon()) && (std::abs(y - v.y) <= std::numeric_limits<T>::epsilon()) && (std::abs(z - v.z)
			<= std::numeric_limits<T>::epsilon()));
}

template<class T>
inline bool Vector3<T>::operator !=(const Vector3<T>& v) const {
	return !(*this == v);
}

template<class T>
inline Vector3<T>::operator T*() {
	return &x;
}

template<class T>
inline Vector3<T> operator *(const Vector3<T>& v, T t) {
	return Vector3<T> (v.x * t, v.y * t, v.z * t);
}

template<class T>
inline Vector3<T> operator /(const Vector3<T>& v, T t) {
	return Vector3<T> (v.x / t, v.y / t, v.z / t);
}

template<class T>
inline Vector3<T> operator *(T t, const Vector3<T>& v) {
	return v * t;
}

template<class T>
inline T VectorDot(const Vector3<T>& v1, const Vector3<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<class T>
inline Vector3<T> VectorCross(const Vector3<T>& v1, const Vector3<T>& v2) {
	return Vector3<T> (v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template<class T>
inline std::istream& operator >>(std::istream& stream, Vector3<T>& vector) {
	return stream >> vector.x >> vector.y >> vector.z;
}

template<class T>
inline std::ostream& operator <<(std::ostream& stream, const Vector3<T>& vector) {
	return stream << vector.x << " " << vector.y << " " << vector.z;
}
