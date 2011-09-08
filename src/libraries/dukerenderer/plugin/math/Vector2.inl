#include <limits>
#include <cmath>
#include <cstdlib>

template <class T>
inline Vector2<T>::Vector2(T X, T Y) :
x(X),
y(Y)
{
}

template <class T>
inline void Vector2<T>::Set(T X, T Y)
{
    x = X;
    y = Y;
}

template <class T>
inline T Vector2<T>::Length() const
{
    return std::sqrt(LengthSq());
}

template <class T>
inline T Vector2<T>::LengthSq() const
{
    return x * x + y * y;
}

template <class T>
inline void Vector2<T>::Normalize()
{
    T Norm = Length();

    if (std::abs(Norm) > std::numeric_limits<T>::epsilon())
    {
        x /= Norm;
        y /= Norm;
    }
}

template <class T>
inline Vector2<T> Vector2<T>::operator +() const
{
    return this;
}

template <class T>
inline Vector2<T> Vector2<T>::operator -() const
{
    return Vector2<T>(-x, -y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator +(const Vector2<T>& v) const
{
    return Vector2<T>(x + v.x, y + v.y);
}

template <class T>
inline Vector2<T> Vector2<T>::operator -(const Vector2<T>& v) const
{
    return Vector2<T>(x - v.x, y - v.y);
}

template <class T>
inline const Vector2<T>& Vector2<T>::operator +=(const Vector2<T>& v)
{
    x += v.x;
    y += v.y;

    return *this;
}

template <class T>
inline const Vector2<T>& Vector2<T>::operator -=(const Vector2<T>& v)
{
    x -= v.x;
    y -= v.y;

    return *this;
}

template <class T>
inline Vector2<T> Vector2<T>::operator *(T t) const
{
    return Vector2<T>(x * t, y * t);
}

template <class T>
inline Vector2<T> Vector2<T>::operator /(T t) const
{
    return Vector2<T>(x / t, y / t);
}

template <class T>
inline const Vector2<T>& Vector2<T>::operator *=(T t)
{
    x *= t;
    y *= t;

    return *this;
}

template <class T>
inline const Vector2<T>& Vector2<T>::operator /=(T t)
{
    x /= t;
    y /= t;

    return *this;
}

template <class T>
inline bool Vector2<T>::operator ==(const Vector2<T>& v) const
{
    return ((std::abs(x - v.x) <= std::numeric_limits<T>::epsilon()) &&
            (std::abs(y - v.y) <= std::numeric_limits<T>::epsilon()));
}

template <class T>
inline bool Vector2<T>::operator !=(const Vector2<T>& v) const
{
    return !(*this == v);
}

template <class T>
inline Vector2<T>::operator T*()
{
    return &x;
}

template <class T>
inline Vector2<T> operator *(const Vector2<T>& v, T t)
{
    return Vector2<T>(v.x * t, v.y * t);
}

template <class T>
inline Vector2<T> operator /(const Vector2<T>& v, T t)
{
    return Vector2<T>(v.x / t, v.y / t);
}

template <class T>
inline Vector2<T> operator *(T t, const Vector2<T>& v)
{
    return v * t;
}

template <class T>
inline T VectorDot(const Vector2<T>& v1, const Vector2<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

template <class T>
inline Vector2<T> VectorCross(const Vector2<T>& v1, const Vector2<T>& v2)
{
    return Vector2<T>(/* ??? */);
}

template <class T>
inline std::istream& operator >>(std::istream& stream, Vector2<T>& vector)
{
    return stream >> vector.x >> vector.y;
}

template <class T>
inline std::ostream& operator <<(std::ostream& stream, const Vector2<T>& vector)
{
    return stream << vector.x << " " << vector.y;
}
