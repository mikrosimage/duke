//#define TEST
#ifndef TEST

#include <duke/cmdline/CmdLineParameters.h>
#include <duke/engine/Duke.h>

#include <duke/gl/Buffers.h>

int main(int argc, char** argv) {
	StreamUploadPbo vbo;
	auto bound = scope_bind(vbo);
	TextureRectangle tex;
	auto bound2 = scope_bind(tex);
	try {
		const duke::CmdLineParameters parameters(argc, argv);
		duke::Duke duke(parameters);
		duke.run();
	} catch (duke::commandline_error &e) {
		fprintf(stderr, "Command line says : %s\n", e.what());
		return EXIT_FAILURE;
	} catch (std::exception &e) {
		fprintf(stderr, "Unexpected error\n%s\n", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#endif // TEST
