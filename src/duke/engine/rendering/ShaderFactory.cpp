#include "ShaderFactory.hpp"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <tuple>

using namespace std;

namespace duke {
ShaderDescription::ShaderDescription() {
}

static inline std::tuple<bool, bool, bool, bool, bool, bool, ColorSpace> asTuple(const ShaderDescription &sd) {
	return std::make_tuple(sd.grayscale, sd.sampleTexture, sd.displayUv, sd.swapEndianness, sd.swapRedAndBlue, sd.tenBitUnpack, sd.colorspace);
}
bool ShaderDescription::operator<(const ShaderDescription &other) const {
	return asTuple(*this) < asTuple(other);
}

ShaderDescription ShaderDescription::createUvDesc() {
	ShaderDescription description;
	description.sampleTexture = false;
	description.displayUv = true;
	return description;
}

ShaderDescription ShaderDescription::createSolidDesc() {
	ShaderDescription description;
	description.sampleTexture = false;
	return description;
}

ShaderDescription ShaderDescription::createTextureDesc(bool grayscale, bool swapEndianness, bool swapRedAndBlue, bool tenBitUnpack, ColorSpace colorspace) {
	ShaderDescription description;
	description.grayscale = grayscale;
	description.sampleTexture = true;
	description.swapEndianness = swapEndianness;
	description.swapRedAndBlue = swapRedAndBlue;
	description.tenBitUnpack = tenBitUnpack;
	description.colorspace = colorspace;
	return description;
}

static const char * const pColorSpaceConversions =
		R"(
vec3 lintolin(vec3 sample) {
	return sample;
}
vec3 cineontolin(vec3 sample) {
	return 1.010915615730753*(pow(vec3(10), (1023*sample-685)/300)-0.010797751623277);
}
vec3 srgbtolin(vec3 sample) {
	return mix(sample/12.92, pow((sample+0.055)/1.055,vec3(2.4)), lessThan(sample, vec3(0.04045)));
}
vec3 lintosrgb(vec3 sample) {
	sample = mix((1.055*pow(sample,vec3(1/2.4)))-vec3(0.055), 12.92*sample, lessThan(sample, vec3(0.0031308)));
	return clamp(sample, vec3(0), vec3(1));
}
)";

static const char * const pSampleTenbitsUnpack =
		R"(
vec4 unpack(uvec4 sample) {
	uint red   = (sample.a << 2u) | (sample.b >> 6u);
	uint green = ((sample.b & 0x3Fu) << 4u) | (sample.g >> 4u);
	uint blue  = ((sample.g & 0x0Fu) << 6u) | (sample.r >> 2u);
	uint alpha = 1023u;//;((sample.r & 0x03u) << 8u);
	return vec4(red, green, blue, alpha)/1023.;
}
smooth in vec2 vVaryingTexCoord;
uniform usampler2DRect gTextureSampler;
vec4 sample(vec2 offset) {
	return unpack(swizzle(texture(gTextureSampler, vVaryingTexCoord+offset)));
}
)";

static const char * const pSampleRegular =
		R"(
smooth in vec2 vVaryingTexCoord;
uniform sampler2DRect gTextureSampler;
vec4 sample(vec2 offset) {
	return swizzle(texture(gTextureSampler, vVaryingTexCoord+offset));
}
)";

static const char* const pTexturedMain =
		R"(
out vec4 vFragColor;
uniform bvec4 gShowChannel;
uniform float gExposure;
uniform float gGamma;
uniform float gZoom;
uniform int gIsPlaying;

vec2 random(vec2 seed) {
    /* use the fragment position for a different seed per-pixel */
    const vec2 scale = vec2(12.9898, 12.9899);
    return (fract(sin(dot(gl_FragCoord.xy + seed, scale)) * 43758.5453 + seed) *2) -1;
}

vec4 sampleToLinear(vec2 offset) {
    vec4 sampled = sample(offset);
    sampled.rgb = toLinear(sampled.rgb);
    // sampled.a=1;
    return sampled;
}

float gaussian(vec2 offset){
	const float sigma = 0.5;
	const float sigmaSquared = sigma*sigma;
	const float pi = 3.14159265359;
	const float A = 1 / sqrt(2*pi*sigmaSquared);
    vec2 squared = offset * offset;
    return A*exp(-(squared.x+squared.y)/(2*sigmaSquared));
}

float triangle(vec2 offset){
    float absolute = abs(offset.x*offset.y);
    return mix(1.f-absolute,0.f, absolute>1.f);
}

void main(void)
{
    vec4 color = vec4(0,0,0,0);

    if(gZoom<1 && gIsPlaying!=1) {
        float span = 1/gZoom;
        float total = 0.0;

        const int samples = 4;

        for (float x = -samples; x <= samples; x++) {
            for (float y = -samples; y <= samples; y++) {
                vec2 percent = (vec2(x,y) + random(vec2(0,.1))) / (samples+1); // discarding extreme values where weight = 0
				vec4 sample = sampleToLinear(percent*span);
				
				/* switch to pre-multiplied alpha to correctly blur transparent images */
				sample.rgb *= sample.a;

				float weight = triangle(percent);
				color += sample * weight;
				total += weight;
            }
		}

        color /= total;
        
        if(color.a>0) // switch back from pre-multiplied alpha
            color.rgb /= color.a;
    } else {
        color = sampleToLinear(vec2(0));
    }

    if(any(gShowChannel.xyz))
        color *= vec4(gShowChannel.xyz,1);

    if(gShowChannel.w)
        color = vec4(color.aaa,1);

    color.rgb = color.rgb * gExposure;
    color.rgb = pow(color.rgb,vec3(gGamma));

    color.rgb = toScreen(color.rgb);

    vFragColor = color;
}
)";

static const char* const pSolidMain = R"(
out vec4 vFragColor;
uniform vec4 gSolidColor;

void main(void)
{
    vFragColor = gSolidColor;
}
)";

static const char* const pUvMain = R"(
out vec4 vFragColor;
smooth in vec2 vVaryingTexCoord;

void main(void)
{
    vFragColor = vec4(vVaryingTexCoord,0,1);
}
)";

static const char* getToLinearFunction(const ColorSpace fromColorspace) {
	switch (fromColorspace) {
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

static const char* getToScreenFunction(const ColorSpace fromColorspace) {
	switch (fromColorspace) {
	case ColorSpace::KodakLog:
	case ColorSpace::Linear:
	case ColorSpace::sRGB:
	case ColorSpace::GammaCorrected:
		return "lintosrgb";
	case ColorSpace::Auto:
	default:
		throw std::runtime_error("ColorSpace must be resolved at this point");
	}
}

static void appendToLinearFunction(ostream&stream, const ColorSpace colorspace) {
	stream << endl << "vec3 toLinear(vec3 sample){return " << getToLinearFunction(colorspace) << "(sample);}" << endl;
}

static void appendToScreenFunction(ostream&stream, const ColorSpace colorspace) {
	stream << endl << "vec3 toScreen(vec3 sample){return " << getToScreenFunction(colorspace) << "(sample);}" << endl;
}

static void appendSampler(ostream&stream, const ShaderDescription &description) {
	stream << (description.tenBitUnpack ? pSampleTenbitsUnpack : pSampleRegular);
}

static void appendSwizzle(ostream&stream, const ShaderDescription &description) {
	const char* type = description.tenBitUnpack ? "uvec4" : "vec4";
	string swizzling = description.grayscale ? "rrra" : "rgba";
	if (description.swapRedAndBlue)
		std::swap(swizzling[0], swizzling[2]);
	if (description.swapEndianness)
		std::reverse(swizzling.begin(), swizzling.end());
	stream << type << " swizzle(" << type << " sample){return sample." << swizzling << ";}";
}

std::string buildFragmentShaderSource(const ShaderDescription &description) {
	ostringstream oss;
	oss << "#version 330" << endl;
	if (description.sampleTexture) {
		oss << pColorSpaceConversions << endl;
		appendToLinearFunction(oss, description.colorspace);
		appendToScreenFunction(oss, description.colorspace);
		appendSwizzle(oss, description);
		appendSampler(oss, description);
		oss << pTexturedMain;
	} else {
		if (description.displayUv)
			oss << pUvMain;
		else
			oss << pSolidMain;
	}
	return oss.str();
}

std::string buildVertexShaderSource(const ShaderDescription &description) {
	return R"(
	#version 330

	layout (location = 0) in vec3 Position;
	layout (location = 1) in vec2 UV;

	uniform ivec2 gViewport;
	uniform ivec2 gImage;
	uniform ivec2 gPan;
	uniform float gZoom;

	out vec2 vVaryingTexCoord; 

	mat4 ortho(int left, int right, int bottom, int top) {
		mat4 Result = mat4(1);
		Result[0][0] = float(2) / (right - left);
		Result[1][1] = float(2) / (top - bottom);
		Result[2][2] = - float(1);
		Result[3][3] = 1;
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);
		return Result;
	}

	mat4 translate(mat4 m, vec3 v) {
		mat4 Result = mat4(m);
		Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return Result;
	}

	mat4 scale(mat4 m, vec3 v) {
		mat4 Result = mat4(m);
		Result[0] = m[0] * v[0];
		Result[1] = m[1] * v[1];
		Result[2] = m[2] * v[2];
		Result[3] = m[3];
		return Result;
	}

	void main() {
		ivec2 translating = ivec2(0); // translation must be integer to prevent aliasing
		translating += gViewport; // moving to center
		translating /= 2; // moving to center
		translating += gPan; // moving to center
		vec2 scaling = vec2(1);
		scaling /= 2; // bringing square from [-1,1] to [-.5,.5]
		scaling *= gImage; // to pixel dimension
		scaling *= gZoom; // zoom
		mat4 world = mat4(1);
		world = translate(world, vec3(translating, 0)); // move to center
		world = scale(world, vec3(scaling, 1));
		mat4 proj = ortho(0, gViewport.x, 0, gViewport.y);
		mat4 worldViewProj = proj * world;
		gl_Position = worldViewProj * vec4(Position, 1.0);
		vVaryingTexCoord = UV * abs(gImage);
	})";
}

SharedProgram buildProgram(const ShaderDescription &description) {
	const string vsSource = buildVertexShaderSource(description);
	const string fsSource = buildFragmentShaderSource(description);
	return make_shared<Program>(makeVertexShader(vsSource.c_str()), makeFragmentShader(fsSource.c_str()));
}

} /* namespace duke */
