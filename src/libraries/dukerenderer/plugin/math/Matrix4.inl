inline Matrix4::Matrix4(float m11, float m12, float m13, float m14,
                          float m21, float m22, float m23, float m24,
                          float m31, float m32, float m33, float m34,
                          float m41, float m42, float m43, float m44) :
a11(m11), a12(m12), a13(m13), a14(m14),
a21(m21), a22(m22), a23(m23), a24(m24),
a31(m31), a32(m32), a33(m33), a34(m34),
a41(m41), a42(m42), a43(m43), a44(m44)
{

}

inline void Matrix4::Identity()
{
    a11 = 1.0f; a12 = 0.0f; a13 = 0.0f; a14 = 0.0f;
    a21 = 0.0f; a22 = 1.0f; a23 = 0.0f; a24 = 0.0f;
    a31 = 0.0f; a32 = 0.0f; a33 = 1.0f; a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline float Matrix4::Determinant() const
{
    float A = a22 * (a33 * a44 - a43 * a34) - a32 * (a23 * a44 - a43 * a24) + a42 * (a23 * a34 - a33 * a24);
    float B = a12 * (a33 * a44 - a43 * a34) - a32 * (a13 * a44 - a43 * a14) + a42 * (a13 * a34 - a33 * a14);
    float C = a12 * (a23 * a44 - a43 * a24) - a22 * (a13 * a44 - a43 * a14) + a42 * (a13 * a24 - a23 * a14);
    float D = a12 * (a23 * a34 - a33 * a24) - a22 * (a13 * a34 - a33 * a14) + a32 * (a13 * a24 - a23 * a14);

    return a11 * A - a21 * B + a31 * C - a41 * D;
}

inline Matrix4 Matrix4::Transpose() const
{
    return Matrix4(a11, a21, a31, a41,
                    a12, a22, a32, a42,
                    a13, a23, a33, a43,
                    a14, a24, a34, a44);
}

inline Matrix4 Matrix4::Inverse() const
{
    Matrix4 Ret;
    float Det = Determinant();

    if (std::fabs(Det) > std::numeric_limits<float>::epsilon())
    {
        Ret.a11 =  (a22 * (a33 * a44 - a34 * a43) - a32 * (a23 * a44 - a43 * a24) + a42 * (a23 * a34 - a33 *  a24)) / Det;
        Ret.a12 = -(a12 * (a33 * a44 - a43 * a34) - a32 * (a13 * a44 - a43 * a14) + a42 * (a13 * a34 - a33 *  a14)) / Det;
        Ret.a13 =  (a12 * (a23 * a44 - a43 * a24) - a22 * (a13 * a44 - a43 * a14) + a42 * (a13 * a24 - a23 *  a14)) / Det;
        Ret.a14 = -(a12 * (a23 * a34 - a33 * a24) - a22 * (a13 * a34 - a33 * a14) + a32 * (a13 * a24 - a23 *  a14)) / Det;

        Ret.a21 = -(a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) + a24 * (a31 * a43 - a33 *  a41)) / Det;
        Ret.a22 =  (a11 * (a33 * a44 - a34 * a43) - a13 * (a31 * a44 - a34 * a41) + a14 * (a31 * a43 - a33 *  a41)) / Det;
        Ret.a23 = -(a11 * (a23 * a44 - a24 * a43) - a13 * (a21 * a44 - a24 * a41) + a14 * (a21 * a43 - a23 *  a41)) / Det;
        Ret.a24 =  (a11 * (a23 * a34 - a24 * a33) - a13 * (a21 * a34 - a24 * a31) + a14 * (a21 * a33 - a23 *  a31)) / Det;

        Ret.a31 =  (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) + a24 * (a31 * a42 - a32 *  a41)) / Det;
        Ret.a32 = -(a11 * (a32 * a44 - a34 * a42) - a12 * (a31 * a44 - a34 * a41) + a14 * (a31 * a42 - a32 *  a41)) / Det;
        Ret.a33 =  (a11 * (a22 * a44 - a24 * a42) - a12 * (a21 * a44 - a24 * a41) + a14 * (a21 * a42 - a22 *  a41)) / Det;
        Ret.a34 = -(a11 * (a22 * a34 - a24 * a32) - a12 * (a21 * a34 - a24 * a31) + a14 * (a21 * a32 - a22 *  a31)) / Det;

        Ret.a41 = -(a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) + a23 * (a31 * a42 - a32 *  a41)) / Det;
        Ret.a42 =  (a11 * (a32 * a43 - a33 * a42) - a12 * (a31 * a43 - a33 * a41) + a13 * (a31 * a42 - a32 *  a41)) / Det;
        Ret.a43 = -(a11 * (a22 * a43 - a23 * a42) - a12 * (a21 * a43 - a23 * a41) + a13 * (a21 * a42 - a22 *  a41)) / Det;
        Ret.a44 =  (a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 *  a31)) / Det;
    }

    return Ret;
}

inline void Matrix4::SetTranslation(float x, float y, float z)
{
    a11 = 1.0f; a12 = 0.0f; a13 = 0.0f; a14 = x;
    a21 = 0.0f; a22 = 1.0f; a23 = 0.0f; a24 = y;
    a31 = 0.0f; a32 = 0.0f; a33 = 1.0f; a34 = z;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::SetScaling(float x, float y, float z)
{
    a11 = x;    a12 = 0.0f; a13 = 0.0f; a14 = 0.0f;
    a21 = 0.0f; a22 = y;    a23 = 0.0f; a24 = 0.0f;
    a31 = 0.0f; a32 = 0.0f; a33 = z;    a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::SetRotationX(float Angle)
{
    float Cos = std::cos(Angle);
    float Sin = std::sin(Angle);

    a11 = 1.0f; a12 = 0.0f; a13 = 0.0f; a14 = 0.0f;
    a21 = 0.0f; a22 = Cos;  a23 = Sin;  a24 = 0.0f;
    a31 = 0.0f; a32 = -Sin; a33 = Cos;  a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::SetRotationY(float Angle)
{
    float Cos = std::cos(Angle);
    float Sin = std::sin(Angle);

    a11 = Cos;  a12 = 0.0f; a13 = -Sin; a14 = 0.0f;
    a21 = 0.0f; a22 = 1.0f; a23 = 0.0f; a24 = 0.0f;
    a31 = Sin;  a32 = 0.0f; a33 = Cos;  a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::SetRotationZ(float Angle)
{
    float Cos = std::cos(Angle);
    float Sin = std::sin(Angle);

    a11 = Cos;  a12 = Sin;  a13 = 0.0f; a14 = 0.0f;
    a21 = -Sin; a22 = Cos;  a23 = 0.0f; a24 = 0.0f;
    a31 = 0.0f; a32 = 0.0f; a33 = 1.0f; a34 = 0.0f;
    a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::SetRotationX(float Angle, const TVector3F& Center)
{
    Matrix4 Tr1, Tr2, Rot;

    Tr1.SetTranslation(Center.x, Center.y, Center.z);
    Tr2.SetTranslation(-Center.x, -Center.y, -Center.z);
    Rot.SetRotationX(Angle);

    *this = Tr1 * Rot * Tr2;
}

inline void Matrix4::SetRotationY(float Angle, const TVector3F& Center)
{
    Matrix4 Tr1, Tr2, Rot;

    Tr1.SetTranslation(Center.x, Center.y, Center.z);
    Tr2.SetTranslation(-Center.x, -Center.y, -Center.z);
    Rot.SetRotationY(Angle);

    *this = Tr1 * Rot * Tr2;
}

inline void Matrix4::SetRotationZ(float Angle, const TVector3F& Center)
{
    Matrix4 Tr1, Tr2, Rot;

    Tr1.SetTranslation(Center.x, Center.y, Center.z);
    Tr2.SetTranslation(-Center.x, -Center.y, -Center.z);
    Rot.SetRotationZ(Angle);

    *this = Tr1 * Rot * Tr2;
}

inline TVector3F Matrix4::GetTranslation() const
{
    return TVector3F(a14, a24, a34);
}

inline TVector3F Matrix4::Transform(const TVector3F& v, float w) const
{
    return TVector3F(v.x * a11 + v.y * a21 + v.z * a31 + w * a41,
                     v.x * a12 + v.y * a22 + v.z * a32 + w * a42,
                     v.x * a13 + v.y * a23 + v.z * a33 + w * a43);
}

inline TVector4F Matrix4::Transform(const TVector4F& v) const
{
    return TVector4F(v.x * a11 + v.y * a21 + v.z * a31 + v.w * a41,
                     v.x * a12 + v.y * a22 + v.z * a32 + v.w * a42,
                     v.x * a13 + v.y * a23 + v.z * a33 + v.w * a43,
                     v.x * a14 + v.y * a24 + v.z * a34 + v.w * a44);
}

inline void Matrix4::OrthoOffCenter(float Left, float Top, float Right, float Bottom)
{
    a11 = 2 / (Right - Left); a12 = 0.0f;               a13 = 0.0f; a14 = (Left + Right) / (Left - Right);
    a21 = 0.0f;               a22 = 2 / (Top - Bottom); a23 = 0.0f; a24 = (Bottom + Top) / (Bottom - Top);
    a31 = 0.0f;               a32 = 0.0f;               a33 = 1.0f; a34 = 0.0f;
    a41 = 0.0f;               a42 = 0.0f;               a43 = 0.0f; a44 = 1.0f;
}

inline void Matrix4::PerspectiveFOV(float Fov, float Ratio, float Near, float Far)
{
    float YScale = 1.0f / std::tan(Fov / 2);
    float XScale = YScale / Ratio;
    float Coeff  = Far / (Far - Near);

    a11 = XScale; a12 = 0.0f;   a13 = 0.0f;  a14 = 0.0f;
    a21 = 0.0f;   a22 = YScale; a23 = 0.0f;  a24 = 0.0f;
    a31 = 0.0f;   a32 = 0.0f;   a33 = Coeff; a34 = Near * -Coeff;
    a41 = 0.0f;   a42 = 0.0f;   a43 = 1.0f;  a44 = 0.0f;
}

inline void Matrix4::LookAt(const TVector3F& From, const TVector3F& To, const TVector3F& Up)
{
    // TODO : g�rer le cas o� (To - From) et Up sont colin�aires

    TVector3F ZAxis = To - From;
    ZAxis.Normalize();
    TVector3F XAxis = VectorCross(Up, ZAxis);
    XAxis.Normalize();
    TVector3F YAxis = VectorCross(ZAxis, XAxis);

    a11 = XAxis.x; a12 = XAxis.y; a13 = XAxis.z; a14 = -VectorDot(XAxis, From);
    a21 = YAxis.x; a22 = YAxis.y; a23 = YAxis.z; a24 = -VectorDot(YAxis, From);
    a31 = ZAxis.x; a32 = ZAxis.y; a33 = ZAxis.z; a34 = -VectorDot(ZAxis, From);
    a41 = 0.0f;    a42 = 0.0f;    a43 = 0.0f;    a44 = 1.0f;
}

inline Matrix4 Matrix4::operator +() const
{
    return *this;
}

inline Matrix4 Matrix4::operator -() const
{
    return Matrix4(-a11, -a12, -a13, -a14,
                    -a21, -a22, -a23, -a24,
                    -a31, -a32, -a33, -a34,
                    -a41, -a42, -a43, -a44);
}

inline Matrix4 Matrix4::operator +(const Matrix4& m) const
{
    return Matrix4(a11 + m.a11, a12 + m.a12, a13 + m.a13, a14 + m.a14,
                    a21 + m.a21, a22 + m.a22, a23 + m.a23, a24 + m.a24,
                    a31 + m.a31, a32 + m.a32, a33 + m.a33, a34 + m.a34,
                    a41 + m.a41, a42 + m.a42, a43 + m.a43, a44 + m.a44);
}

inline Matrix4 Matrix4::operator -(const Matrix4& m) const
{
    return Matrix4(a11 - m.a11, a12 - m.a12, a13 - m.a13, a14 - m.a14,
                    a21 - m.a21, a22 - m.a22, a23 - m.a23, a24 - m.a24,
                    a31 - m.a31, a32 - m.a32, a33 - m.a33, a34 - m.a34,
                    a41 - m.a41, a42 - m.a42, a43 - m.a43, a44 - m.a44);
}

inline const Matrix4& Matrix4::operator +=(const Matrix4& m)
{
    a11 += m.a11; a12 += m.a12; a13 += m.a13; a14 += m.a14;
    a21 += m.a21; a22 += m.a22; a23 += m.a23; a24 += m.a24;
    a31 += m.a31; a32 += m.a32; a33 += m.a33; a34 += m.a34;
    a41 += m.a41; a42 += m.a42; a43 += m.a43; a44 += m.a44;

    return *this;
}

inline const Matrix4& Matrix4::operator -=(const Matrix4& m)
{
    a11 -= m.a11; a12 -= m.a12; a13 -= m.a13; a14 -= m.a14;
    a21 -= m.a21; a22 -= m.a22; a23 -= m.a23; a24 -= m.a24;
    a31 -= m.a31; a32 -= m.a32; a33 -= m.a33; a34 -= m.a34;
    a41 -= m.a41; a42 -= m.a42; a43 -= m.a43; a44 -= m.a44;

    return *this;
}

inline Matrix4 Matrix4::operator *(const Matrix4& m) const
{
    return Matrix4(a11 * m.a11 + a21 * m.a12 + a31 * m.a13 + a41 * m.a14,
                    a12 * m.a11 + a22 * m.a12 + a32 * m.a13 + a42 * m.a14,
                    a13 * m.a11 + a23 * m.a12 + a33 * m.a13 + a43 * m.a14,
                    a14 * m.a11 + a24 * m.a12 + a34 * m.a13 + a44 * m.a14,

                    a11 * m.a21 + a21 * m.a22 + a31 * m.a23 + a41 * m.a24,
                    a12 * m.a21 + a22 * m.a22 + a32 * m.a23 + a42 * m.a24,
                    a13 * m.a21 + a23 * m.a22 + a33 * m.a23 + a43 * m.a24,
                    a14 * m.a21 + a24 * m.a22 + a34 * m.a23 + a44 * m.a24,

                    a11 * m.a31 + a21 * m.a32 + a31 * m.a33 + a41 * m.a34,
                    a12 * m.a31 + a22 * m.a32 + a32 * m.a33 + a42 * m.a34,
                    a13 * m.a31 + a23 * m.a32 + a33 * m.a33 + a43 * m.a34,
                    a14 * m.a31 + a24 * m.a32 + a34 * m.a33 + a44 * m.a34,

                    a11 * m.a41 + a21 * m.a42 + a31 * m.a43 + a41 * m.a44,
                    a12 * m.a41 + a22 * m.a42 + a32 * m.a43 + a42 * m.a44,
                    a13 * m.a41 + a23 * m.a42 + a33 * m.a43 + a43 * m.a44,
                    a14 * m.a41 + a24 * m.a42 + a34 * m.a43 + a44 * m.a44);
}

inline const Matrix4& Matrix4::operator *=(const Matrix4& m)
{
    *this = *this * m;

    return *this;
}

inline const Matrix4& Matrix4::operator *=(float t)
{
    a11 *= t; a12 *= t; a13 *= t; a14 *= t;
    a21 *= t; a22 *= t; a23 *= t; a24 *= t;
    a31 *= t; a32 *= t; a33 *= t; a34 *= t;
    a41 *= t; a42 *= t; a43 *= t; a44 *= t;

    return *this;
}

inline const Matrix4& Matrix4::operator /=(float t)
{
    a11 /= t; a12 /= t; a13 /= t; a14 /= t;
    a21 /= t; a22 /= t; a23 /= t; a24 /= t;
    a31 /= t; a32 /= t; a33 /= t; a34 /= t;
    a41 /= t; a42 /= t; a43 /= t; a44 /= t;

    return *this;
}

inline bool Matrix4::operator ==(const Matrix4& m) const
{
    return ((std::fabs(a11 - m.a11) < std::numeric_limits<float>::epsilon()) && (std::fabs(a12 - m.a12) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a13 - m.a13) < std::numeric_limits<float>::epsilon()) && (std::fabs(a14 - m.a14) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a21 - m.a21) < std::numeric_limits<float>::epsilon()) && (std::fabs(a22 - m.a22) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a23 - m.a23) < std::numeric_limits<float>::epsilon()) && (std::fabs(a24 - m.a24) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a31 - m.a31) < std::numeric_limits<float>::epsilon()) && (std::fabs(a32 - m.a32) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a33 - m.a33) < std::numeric_limits<float>::epsilon()) && (std::fabs(a34 - m.a34) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a41 - m.a41) < std::numeric_limits<float>::epsilon()) && (std::fabs(a42 - m.a42) < std::numeric_limits<float>::epsilon()) &&
            (std::fabs(a43 - m.a43) < std::numeric_limits<float>::epsilon()) && (std::fabs(a44 - m.a44) < std::numeric_limits<float>::epsilon()));
}

inline bool Matrix4::operator !=(const Matrix4& m) const
{
    return !(*this == m);
}

inline float& Matrix4::operator ()(std::size_t i, std::size_t j)
{
    return operator float*()[i + 4 * j];
}

inline const float& Matrix4::operator ()(std::size_t i, std::size_t j) const
{
    return operator ()(i, j);
}

inline Matrix4::operator const float*() const
{
    return &a11;
}

inline Matrix4::operator float*()
{
    return &a11;
}

inline Matrix4 operator *(const Matrix4& m, float t)
{
    return Matrix4(m.a11 * t, m.a12 * t, m.a13 * t, m.a14 * t,
                    m.a21 * t, m.a22 * t, m.a23 * t, m.a24 * t,
                    m.a31 * t, m.a32 * t, m.a33 * t, m.a34 * t,
                    m.a41 * t, m.a42 * t, m.a43 * t, m.a44 * t);
}

inline Matrix4 operator *(float t, const Matrix4& m)
{
    return m * t;
}

inline Matrix4 operator /(const Matrix4& m, float t)
{
    return Matrix4(m.a11 / t, m.a12 / t, m.a13 / t, m.a14 / t,
                    m.a21 / t, m.a22 / t, m.a23 / t, m.a24 / t,
                    m.a31 / t, m.a32 / t, m.a33 / t, m.a34 / t,
                    m.a41 / t, m.a42 / t, m.a43 / t, m.a44 / t);
}

inline std::istream& operator >>(std::istream& Stream, Matrix4& mat)
{
    Stream >> mat.a11 >> mat.a12 >> mat.a13 >> mat.a14;
    Stream >> mat.a21 >> mat.a22 >> mat.a23 >> mat.a24;
    Stream >> mat.a31 >> mat.a32 >> mat.a33 >> mat.a34;
    Stream >> mat.a41 >> mat.a42 >> mat.a43 >> mat.a44;

    return Stream;
}

inline std::ostream& operator <<(std::ostream& Stream, const Matrix4& mat)
{
    Stream << mat.a11 << " " << mat.a12 << " " << mat.a13 << " " << mat.a14 << std::endl;
    Stream << mat.a21 << " " << mat.a22 << " " << mat.a23 << " " << mat.a24 << std::endl;
    Stream << mat.a31 << " " << mat.a32 << " " << mat.a33 << " " << mat.a34 << std::endl;
    Stream << mat.a41 << " " << mat.a42 << " " << mat.a43 << " " << mat.a44 << std::endl;

    return Stream;
}
