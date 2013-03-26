#include "Benchmark.hpp"

#include <duke/gl/GlFwApp.hpp>
#include <duke/gl/GL.hpp>
#include <duke/gl/GLUtils.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/GlObjects.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/engine/rendering/ShaderFactory.hpp>
#include <duke/memory/Allocator.hpp>

#include <glm/glm.hpp>

#include <chrono>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

using namespace std;

namespace duke {

using namespace gl;

struct TextureConfiguration {
	GLuint internalFormat;
	GLenum pixel_format;
	GLenum pixel_type;
};

class IUploadTest {
public:
	virtual ~IUploadTest() {
	}
	virtual Binder<GlTextureObject> initializeAndBind(const TextureConfiguration configuration, const glm::uvec2 textureSize) = 0;
	virtual void updateTexture(const TextureConfiguration configuration, const glm::uvec2 textureSize, const void *pData, const GLsizeiptr dataSize) = 0;
	virtual void destroy() = 0;
	virtual const char* name() const = 0;
};

class SynchronousUnpack: public IUploadTest {
public:
	virtual Binder<GlTextureObject> initializeAndBind(const TextureConfiguration conf, const glm::uvec2 size) {
		auto textureBound = texture.scope_bind_texture();
		glTexImage2D(texture.target, 0, conf.internalFormat, size.x, size.y, 0, conf.pixel_format, conf.pixel_type, nullptr);
		return std::move(textureBound);
	}
	virtual void updateTexture(const TextureConfiguration conf, const glm::uvec2 textureSize, const void *pData, const GLsizeiptr dataSize) {
		glTexSubImage2D(texture.target, 0, 0, 0, textureSize.x, textureSize.y, conf.pixel_format, conf.pixel_type, pData);
	}
	virtual void destroy() {
	}
	virtual const char* name() const {
		return "Synchronous";
	}
protected:
	GlTextureRectangle texture;
};

class PBOUnpack: public SynchronousUnpack {
public:
	virtual void updateTexture(const TextureConfiguration conf, const glm::uvec2 textureSize, const void *pData, const GLsizeiptr dataSize) {
		auto pboBound = pbo.scope_bind_buffer();
		if (firstUpdate)
			glBufferData(pbo.target, dataSize, 0, pbo.usage);
		GLubyte* ptr = (GLubyte*) glMapBufferRange(pbo.target, 0, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(ptr, pData, dataSize);
		glUnmapBuffer(pbo.target);
		glTexSubImage2D(texture.target, 0, 0, 0, textureSize.x, textureSize.y, conf.pixel_format, conf.pixel_type, nullptr);
		firstUpdate = false;
	}
	virtual void destroy() {
		firstUpdate = true;
	}
	virtual const char* name() const {
		return "Asynchronous";
	}
private:
	GlStreamUploadPbo pbo;
	bool firstUpdate = true;
};

void benchmark() {
	const size_t seconds = 2;
	const size_t viewportWidth = 512;
	const size_t viewportHeight = 128;
	const size_t allocatedDataSize = 120 * 1024 * 1024; //allocating 120 MB

	AlignedMalloc allocator;
	const auto pSharedData = make_shared_memory<char>(allocatedDataSize, allocator);
	char * const pData = pSharedData.get();
	generate(pData, pData + allocatedDataSize, rand);

	DukeGLFWApplication application;
//	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
//	unique_ptr<DukeGLFWWindow> pUpload(application.createWindow<DukeGLFWWindow>(viewportWidth, viewportHeight, "Upload context...", nullptr, nullptr));
//	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
//	unique_ptr<DukeGLFWWindow> pWindow(application.createWindow<DukeGLFWWindow>(viewportWidth, viewportHeight, "please wait while benchmarking...", nullptr, pUpload->getHandle()));
	unique_ptr<DukeGLFWWindow> pWindow(application.createWindow<DukeGLFWWindow>(viewportWidth, viewportHeight, "please wait while benchmarking...", nullptr, nullptr));

	const vector<glm::uvec2> textureSizes = { glm::uvec2(1920, 1080), glm::uvec2(2048, 1536), glm::uvec2(4096, 3072) };
	const vector<TextureConfiguration> configurations = { //
			{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE }, //
					{ GL_RGB, GL_BGR, GL_UNSIGNED_BYTE }, //
					{ GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE }, //
					{ GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE }, //
					{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, //
					{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE }, //
					{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8 }, //
					{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 }, //
					{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV }, //
					{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV }, //
					{ GL_RGB, GL_RGB, GL_HALF_FLOAT }, //
					{ GL_RGB, GL_BGR, GL_HALF_FLOAT }, //
					{ GL_RGB16F, GL_RGB, GL_HALF_FLOAT }, //
					{ GL_RGB16F, GL_BGR, GL_HALF_FLOAT }, //
					{ GL_RGBA, GL_RGBA, GL_HALF_FLOAT }, //
					{ GL_RGBA, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16, GL_RGBA, GL_HALF_FLOAT }, //
					{ GL_RGBA16, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16F, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, //
			};

	const vector<shared_ptr<IUploadTest> > vpTesters = { make_shared<SynchronousUnpack>(), make_shared<PBOUnpack>() };

	glfwSwapInterval(0); // ensuring no vsync

	SharedMesh pMesh = createSquare();
	const ShaderDescription description = ShaderDescription::createTextureDesc(false, false, false, false, ColorSpace::Linear);
	Program program(makeVertexShader(buildVertexShaderSource(description).c_str()), //
			makeFragmentShader(
					R"(
#version 330
smooth in vec2 vVaryingTexCoord;
uniform sampler2DRect gTextureSampler;
out vec4 vFragColor;
void main() {
	vFragColor = texture(gTextureSampler, vVaryingTexCoord);
}
)"));
	for (glm::uvec2 textureSize : textureSizes) {
		for (TextureConfiguration conf : configurations) {
			for (const shared_ptr<IUploadTest> &pTester : vpTesters) {
				const GLsizeiptr dataSize = textureSize.x * textureSize.y * getBytePerPixels(conf.pixel_format, conf.pixel_type);
				printf("%15s %ux%u %15s %10s %30s %10ld ", //
						pTester->name(), //
						textureSize.x, textureSize.y, //
						getInternalFormatString(conf.internalFormat), //
						getPixelFormatString(conf.pixel_format), //
						getPixelTypeString(conf.pixel_type), //
						dataSize);
				fflush(stdout);
				{
					auto textureBound = pTester->initializeAndBind(conf, textureSize);
					program.use();
					setTextureDimensions(program.getUniformLocation("gImage"), textureSize.x, textureSize.y, 0);
					glUniform2i(program.getUniformLocation("gViewport"), viewportWidth, viewportHeight);
					glUniform1i(program.getUniformLocation("gTextureSampler"), 0);
					glUniform2i(program.getUniformLocation("gPan"), 0, 0);
					glUniform1i(program.getUniformLocation("gZoom"), 0);
					size_t count = 0;
					for (const auto start = chrono::steady_clock::now(); chrono::steady_clock::now() - start < chrono::seconds(seconds); ++count) {
						pTester->updateTexture(conf, textureSize, pData + (count % 2), dataSize);
						pMesh->draw();
						glfwSwapBuffers(pWindow->getHandle());
					}
					pTester->destroy();
					const double gbPerSeconds = count * dataSize / 1000. / 1000 / 1000 / seconds;
					printf("%4.1f %4.2f %4.2f\n", //
							count / double(seconds), //
							gbPerSeconds, //
							1000 * seconds / double(count));
				}
			}
		}
	}
}

} /* namespace duke */
