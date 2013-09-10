#include "duke/StringUtils.hpp"
#include "duke/engine/ColorSpace.hpp"

namespace duke {

ColorSpace resolveFromName(const char* pColorspace) {
	if (pColorspace) {
		if (streq(pColorspace, "Linear"))
			return ColorSpace::Linear;
		if (streq(pColorspace, "sRGB") || streq(pColorspace, "GammaCorrected"))
			return ColorSpace::sRGB;
		if (streq(pColorspace, "KodakLog"))
			return ColorSpace::KodakLog;
		if (streq(pColorspace, "Rec709"))
			return ColorSpace::Rec709;
		if (streq(pColorspace, "AdobeRGB"))
			return ColorSpace::AdobeRGB;
		if (streq(pColorspace, "AlexaV3LogC"))
			return ColorSpace::AlexaLogC;
	}
	return ColorSpace::Auto;
}

ColorSpace resolveFromExtension(const char* pFileExtension) {
	if (pFileExtension) {
		if (streq(pFileExtension, "dpx"))
			return ColorSpace::KodakLog;
		if (streq(pFileExtension, "exr"))
			return ColorSpace::Linear;
		if (streq(pFileExtension, "jpg"))
			return ColorSpace::sRGB;
		if (streq(pFileExtension, "png"))
			return ColorSpace::sRGB;
	}
	printf("Unable to find default ColorSpace for extension '%s' assuming sRGB\n", pFileExtension);
	return ColorSpace::sRGB;
}

// GLSL ColorSpace conversion functions
const char *pColorSpaceConversions =
		R"(
vec3 lintolin(vec3 sample) {
	return sample;
}
vec3 alexatolin(vec3 sample) {
    return mix(pow(vec3(10.0),((sample-0.385537)/0.247190)-0.052272)/5.555556 , (sample-0.092809)/5.367655, lessThan(sample, vec3(0.149658)));
}
vec3 cineontolin(vec3 sample) {
	return 1.010915615730753*(pow(vec3(10), (1023*sample-685)/300)-0.010797751623277);
}
vec3 srgbtolin(vec3 sample) {
    return mix(pow((sample+0.055)/1.055,vec3(2.4)), sample/12.92, lessThan(sample, vec3(0.04045)));
}
vec3 lintosrgb(vec3 sample) {
    sample = mix((1.055*pow(sample,vec3(1/2.4)))-vec3(0.055), 12.92*sample, lessThan(sample, vec3(0.0031308)));
    return clamp(sample, vec3(0), vec3(1));
})";

const char* getToLinearFunction(const ColorSpace fromColorspace) {
    switch (fromColorspace) {
        case ColorSpace::AlexaLogC:
            return "alexatolin";
        case ColorSpace::KodakLog:
            return "cineontolin";
        case ColorSpace::Linear:
            return "lintolin";
        case ColorSpace::sRGB:
        case ColorSpace::GammaCorrected:
            return "srgbtolin";
        case ColorSpace::Auto:
        default:
            throw std::runtime_error("ColorSpace must be resolved at this point");
    }
}

const char* getToScreenFunction(const ColorSpace fromColorspace) {
    switch (fromColorspace) {
        case ColorSpace::AlexaLogC:
        case ColorSpace::KodakLog:
        case ColorSpace::Linear:
            return "lintolin";
        case ColorSpace::sRGB:
        case ColorSpace::GammaCorrected:
        case ColorSpace::Auto:
            return "lintosrgb"; // this is the default screen colorspace though
        default:
            throw std::runtime_error("ColorSpace must be resolved at this point");
    }
}



}//namespace duke




