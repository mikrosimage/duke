#include <cmath>

template<class T>
inline Vector4<T>::Vector4(T X, T Y, T Z, T W) :
	x(X), y(Y), z(Z), w(W) {

}

template<class T>
inline void Vector4<T>::Set(T X, T Y, T Z, T W) {
	x = X;
	y = Y;
	z = Z;
	w = W;
}

template<class T>
inline T Vector4<T>::Length() const {
	return std::sqrt(LengthSq());
}

template<class T>
inline T Vector4<T>::LengthSq() const {
	return x * x + y * y + z * z + w * w;
}

template<class T>
inline void Vector4<T>::Normalize() {
	T Norm = Length();

	if (std::abs(Norm) > std::numeric_limits<T>::epsilon()) {
		x /= Norm;
		y /= Norm;
		z /= Norm;
		w /= Norm;
	}
}

template<class T>
inline Vector4<T> Vector4<T>::operator +() const {
	return this;
}

template<class T>
inline Vector4<T> Vector4<T>::operator -() const {
	return Vector4<T> (-x, -y, -z, -w);
}

template<class T>
inline Vector4<T> Vector4<T>::operator +(const Vector4<T>& v) const {
	return Vector4<T> (x + v.x, y + v.y, z + v.z, w + v.w);
}

template<class T>
inline Vector4<T> Vector4<T>::operator -(const Vector4<T>& v) const {
	return Vector4<T> (x - v.x, y - v.y, z - v.z, w - v.w);
}

template<class T>
inline const Vector4<T>& Vector4<T>::operator +=(const Vector4<T>& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

template<class T>
inline const Vector4<T>& Vector4<T>::operator -=(const Vector4<T>& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

template<class T>
inline Vector4<T> Vector4<T>::operator *(T t) const {
	return Vector4<T> (x * t, y * t, z * t, w * t);
}

template<class T>
inline Vector4<T> Vector4<T>::operator /(T t) const {
	return Vector4<T> (x / t, y / t, z / t, w / t);
}

template<class T>
inline const Vector4<T>& Vector4<T>::operator *=(T t) {
	x *= t;
	y *= t;
	z *= t;
	w *= t;

	return *this;
}

template<class T>
inline const Vector4<T>& Vector4<T>::operator /=(T t) {
	x /= t;
	y /= t;
	z /= t;
	w /= t;

	return *this;
}

template<class T>
inline bool Vector4<T>::operator ==(const Vector4<T>& v) const {
	return ((std::abs(x - v.x) <= std::numeric_limits<T>::epsilon()) && (std::abs(y - v.y) <= std::numeric_limits<T>::epsilon()) && (std::abs(z - v.z)
			<= std::numeric_limits<T>::epsilon()) && (std::abs(w - v.w) <= std::numeric_limits<T>::epsilon()));
}

template<class T>
inline bool Vector4<T>::operator !=(const Vector4<T>& v) const {
	return !(*this == v);
}

template<class T>
inline Vector4<T>::operator T*() {
	return &x;
}

template<class T>
inline Vector4<T> operator *(const Vector4<T>& v, T t) {
	return Vector4<T> (v.x * t, v.y * t, v.z * t, v.w * t);
}

template<class T>
inline Vector4<T> operator /(const Vector4<T>& v, T t) {
	return Vector4<T> (v.x / t, v.y / t, v.z / t, v.w / t);
}

template<class T>
inline Vector4<T> operator *(T t, const Vector4<T>& v) {
	return v * t;
}

template<class T>
inline T VectorDot(const Vector4<T>& v1, const Vector4<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

template<class T>
inline Vector4<T> VectorCross(const Vector4<T>& v1, const Vector4<T>& v2) {
	// ?? Correct ??
	return Vector4<T> (v1.y * v2.z - v1.z * v2.y, v1.z * v2.w - v1.w * v2.z, v1.w * v2.x - v1.x * v2.w, v1.x * v2.y - v1.y * v2.x);
}

template<class T>
inline std::istream& operator >>(std::istream& stream, Vector4<T>& vector) {
	return stream >> vector.x >> vector.y >> vector.z >> vector.w;
}

template<class T>
inline std::ostream& operator <<(std::ostream& stream, const Vector4<T>& vector) {
	return stream << vector.x << " " << vector.y << " " << vector.z << " " << vector.w;
}
