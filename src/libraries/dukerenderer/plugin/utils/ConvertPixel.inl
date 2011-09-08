// TODO : prendre en compte l'endianess... (faire deux fichiers : ConvertPixelBigEndian.inl et ConvertPixelLittleEndian.inl)


/*
template <>
inline void ConvertPixel<PXF_L8, PXF_L8>(const unsigned char* src, unsigned char* dest)
{
    *dest = *src;
}
//
//template <>
//inline void ConvertPixel<PXF_L8, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = *src;
//    dest[1] = 0xFF;
//}
//
//template <>
//inline void ConvertPixel<PXF_L8, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    *reinterpret_cast<unsigned short*>(dest) = 0x8000 |
//                                               ((*src >> 3) << 10) |
//                                               ((*src >> 3) <<  5) |
//                                               ((*src >> 3) <<  0);
//}
//
//template <>
//inline void ConvertPixel<PXF_L8, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (*src & 0xF0) | (*src >> 4);
//    dest[1] = 0xF0 | (*src >> 4);
//}
//
//template <>
//inline void ConvertPixel<PXF_L8, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = *src;
//    dest[1] = *src;
//    dest[2] = *src;
//}
//
//template <>
//inline void ConvertPixel<PXF_L8, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = *src;
//    dest[1] = *src;
//    dest[2] = *src;
//    dest[3] = 0xFF;
//}
//
//
//////////////////////////////////////////////////////////////////
//// Sp�cialisations pour les conversions PXF_A8L8 -> ???
//////////////////////////////////////////////////////////////////
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_L8>(const unsigned char* src, unsigned char* dest)
//{
//    *dest = src[0];
//}
//
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = src[0];
//    dest[1] = src[1];
//}
//
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    *reinterpret_cast<unsigned short*>(dest) = ((src[1] >> 7) << 15) |
//                                               ((src[0] >> 3) << 10) |
//                                               ((src[0] >> 3) <<  5) |
//                                               ((src[0] >> 3) <<  0);
//}
//
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (src[0] & 0xF0) | (src[0] >> 4);
//    dest[1] = (src[1] & 0xF0) | (src[0] >> 4);
//}
//
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = src[0];
//    dest[1] = src[0];
//    dest[2] = src[0];
//}
//
//template <>
//inline void ConvertPixel<PXF_A8L8, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = src[0];
//    dest[1] = src[0];
//    dest[2] = src[0];
//    dest[3] = src[1];
//}
//
//
//////////////////////////////////////////////////////////////////
//// Sp�cialisations pour les conversions PXF_A1R5G5B5 -> ???
//////////////////////////////////////////////////////////////////
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_L8>(const unsigned char* src, unsigned char* dest)
//{
//    unsigned short Pix = *reinterpret_cast<const unsigned short*>(src);
//    *dest = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
//                                       ((Pix & 0x03E0) >> 2) * 0.59 +
//                                       ((Pix & 0x001F) << 3) * 0.11);
//}
//
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    unsigned short Pix = *reinterpret_cast<const unsigned short*>(src);
//    dest[0] = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
//                                         ((Pix & 0x03E0) >> 2) * 0.59 +
//                                         ((Pix & 0x001F) << 3) * 0.11);
//    dest[1] = src[1] & 0x8000 ? 0xFF : 0x00;
//}
//
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = src[0];
//    dest[1] = src[1];
//}
//
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    unsigned short Pix = *reinterpret_cast<const unsigned short*>(src);
//    dest[1] = (Pix & 0x8000 ? 0xF0 : 0x00) | ((Pix & 0x7C00) >> 11);
//    dest[0] = ((Pix & 0x03C0) >> 2) | ((Pix & 0x001F) >> 1);
//}
//
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    unsigned short Pix = *reinterpret_cast<const unsigned short*>(src);
//    dest[2] = (Pix & 0x7C00) >> 7;
//    dest[1] = (Pix & 0x03E0) >> 2;
//    dest[0] = (Pix & 0x001F) << 3;
//}
//
//template <>
//inline void ConvertPixel<PXF_A1R5G5B5, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    unsigned short Pix = *reinterpret_cast<const unsigned short*>(src);
//    dest[3] = (Pix & 0x8000) >> 8;
//    dest[2] = (Pix & 0x7C00) >> 7;
//    dest[1] = (Pix & 0x03E0) >> 2;
//    dest[0] = (Pix & 0x001F) << 3;
//}
//
//
//////////////////////////////////////////////////////////////////
//// Sp�cialisations pour les conversions PXF_A4R4G4B4 -> ???
//////////////////////////////////////////////////////////////////
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_L8>(const unsigned char* src, unsigned char* dest)
//{
//    *dest = static_cast<unsigned char>(((src[1] & 0x0F) << 4) * 0.30 +
//                                       ((src[0] & 0xF0) >> 0) * 0.59 +
//                                       ((src[0] & 0x0F) << 4) * 0.11);
//}
//
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = static_cast<unsigned char>(((src[1] & 0x0F) << 4) * 0.30 +
//                                         ((src[0] & 0xF0) >> 0) * 0.59 +
//                                         ((src[0] & 0x0F) << 4) * 0.11);
//    dest[1] = src[1] & 0xF0;
//}
//
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    *reinterpret_cast<unsigned short*>(dest) = ((src[1] & 0x80) <<  8) |
//                                               ((src[1] & 0x0F) << 11) |
//                                               ((src[0] & 0xF0) <<  2) |
//                                               ((src[0] & 0x0F) <<  1);
//}
//
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = src[0];
//    dest[1] = src[1];
//}
//
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (src[0] & 0x0F) << 4;
//    dest[1] = (src[0] & 0xF0);
//    dest[2] = (src[1] & 0x0F) << 4;
//}
//
//template <>
//inline void ConvertPixel<PXF_A4R4G4B4, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (src[0] & 0x0F) << 4;
//    dest[1] = (src[0] & 0xF0);
//    dest[2] = (src[1] & 0x0F) << 4;
//    dest[3] = (src[1] & 0xF0);
//}
//
//
//////////////////////////////////////////////////////////////////
//// Sp�cialisations pour les conversions PXF_R8G8B8 -> ???
//////////////////////////////////////////////////////////////////
//template <>
//inline void ConvertPixel<PXF_R8G8B8, PXF_L8>(const unsigned char* src, unsigned char* dest)
//{
//    *dest = static_cast<unsigned char>(src[2] * 0.30 + src[1] * 0.59 + src[0] * 0.11);
//}
//
//template <>
//inline void ConvertPixel<PXF_R8G8B8, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = static_cast<unsigned char>(src[2] * 0.30 + src[1] * 0.59 + src[0] * 0.11);
//    dest[1] = 0xFF;
//}
//
//template <>
//inline void ConvertPixel<PXF_R8G8B8, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    *reinterpret_cast<unsigned short*>(dest) = 0x8000 |
//                                               ((src[2] >> 3) << 10) |
//                                               ((src[1] >> 3) <<  5) |
//                                               ((src[0] >> 3) <<  0);
//}
//
//template <>
//inline void ConvertPixel<PXF_R8G8B8, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (src[1] & 0xF0) | (src[0] >> 4);
//    dest[1] = 0xF0 | (src[2] >> 4);
//}
//
template <>
inline void ConvertPixel<PXF_R8G8B8, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

template <>
inline void ConvertPixel<PXF_R8G8B8, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = 0xFF;
}
//
//
//////////////////////////////////////////////////////////////////
//// Sp�cialisations pour les conversions PXF_A8R8G8B8 -> ???
//////////////////////////////////////////////////////////////////
//template <>
//inline void ConvertPixel<PXF_A8R8G8B8, PXF_L8>(const unsigned char* src, unsigned char* dest)
//{
//    *dest = static_cast<unsigned char>(src[2] * 0.30 + src[1] * 0.59 + src[0] * 0.11);
//}
//
//template <>
//inline void ConvertPixel<PXF_A8R8G8B8, PXF_A8L8>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = static_cast<unsigned char>(src[2] * 0.30 + src[1] * 0.59 + src[0] * 0.11);
//    dest[1] = src[3];
//}
//
//template <>
//inline void ConvertPixel<PXF_A8R8G8B8, PXF_A1R5G5B5>(const unsigned char* src, unsigned char* dest)
//{
//    *reinterpret_cast<unsigned short*>(dest) = ((src[3] >> 7) << 15) |
//                                               ((src[2] >> 3) << 10) |
//                                               ((src[1] >> 3) <<  5) |
//                                               ((src[0] >> 3) <<  0);
//}
//
//template <>
//inline void ConvertPixel<PXF_A8R8G8B8, PXF_A4R4G4B4>(const unsigned char* src, unsigned char* dest)
//{
//    dest[0] = (src[1] & 0xF0) | (src[0] >> 4);
//    dest[1] = (src[3] & 0xF0) | (src[2] >> 4);
//}
//
template <>
inline void ConvertPixel<PXF_A8R8G8B8, PXF_R8G8B8>(const unsigned char* src, unsigned char* dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

template <>
inline void ConvertPixel<PXF_A8R8G8B8, PXF_A8R8G8B8>(const unsigned char* src, unsigned char* dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}
*/