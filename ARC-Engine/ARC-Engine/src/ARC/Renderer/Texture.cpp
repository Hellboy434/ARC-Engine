#include "arc_pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform\OpenGl\OpenGLTexture.h"
#include "ARC\Core\Core.h"

namespace ARC {

	TRef<CTexture2D> CTexture2D::Create(const std::string& _Path)
	{
		switch (CRenderer::GetCurrentAPI())
		{
		case CRendererAPI::ERendererAPI::None:
			ARC_CORE_ASSERT(false, "Selected renderer API is not supported (ERendererAPI::None)");
			return nullptr;
		case CRendererAPI::ERendererAPI::OpenGL:
			return std::make_shared<COpenGLTexture2D>(_Path);
		}
		ARC_CORE_ASSERT(false, "Unknown renderer API");

		return nullptr;
	}
}