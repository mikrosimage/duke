#pragma once

namespace duke {

class Mesh;
struct Context;
struct ShaderPool;

void renderWithBoundTexture(const ShaderPool &shaderPool, const Mesh *pMesh, const Context &context);

} /* namespace duke */
