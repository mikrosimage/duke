#include "Parameters.hpp"

Parameters::Parameters() {
	add( { "gamma", float_value, "1" });
	add( { "exposure", float_value, "1" });
	add( { "channel", string_value });
	add( { "colorspace", string_value });
	add( { "lut", string_value });
	add( { "framerate", string_value });
	add( { "zoom", float_value, "1" });
	add( { "currentframe", int_value });
}
