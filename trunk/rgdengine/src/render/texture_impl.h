#pragma once

#include <d3dx9.h>
#include <rgde/render/texture.h>

namespace render
{
	class texture_d3d9 : public texture
	{
	public:
							texture_d3d9(const std::string& file_name);
							~texture_d3d9();

		static texture_ptr		CreateFromFile(const std::string& file_name);

		void				createTextureFromFile(const std::string& file_name);
		void				createRenderTexture(const math::Vec2i &size, TextureFormat format);

		IDirect3DTexture9*	getDxTexture();

		TextureFormat		getFormat()		const { return m_eFormat; }
		TextureUsage		getUsage()		const { return m_eUsage;  }
		int					getHeight()		const { return m_nHeight; }
		int					getWidth()		const { return m_nWidth;  }
		ETextureType		getType()		const { return m_type;   }

		const std::string& getFileName() const {return m_strFileName;}

		bool				hasAlpha()		const
		{
			return m_eFormat == A32B32G32R32F || m_eFormat == A16B16G16R16F || m_eFormat == ARGB8 || m_eFormat == A16B16G16R16 || m_eFormat == A8B8G8R8 || m_eFormat == A8R8G8B8 || m_eFormat == DXT3;
		}

	protected:
		texture_d3d9() {}
		ETextureType	convertType(D3DRESOURCETYPE d3dType);
		
	protected:
		IDirect3DTexture9	*m_texture;	
		D3DSURFACE_DESC		m_desc;
		std::string			m_strFileName;

		TextureUsage		m_eUsage;
		unsigned			m_nHeight;
		unsigned			m_nWidth;
		TextureFormat		m_eFormat;
		ETextureType		m_type;
	};
}