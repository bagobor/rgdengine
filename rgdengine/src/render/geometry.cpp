#pragma once

#include "precompiled.h"

#include <rgde/render/geometry.h>
#include <rgde/render/render_device.h>

#include <rgde/core/coreComPtr.h>

#include <rgde/base/xml_helpers.h>
#include <rgde/base/lexical_cast.h>

#include <d3dx9.h>

extern LPDIRECT3DDEVICE9       g_d3d;


namespace render
{
    class geometry_impl : public base_geometry, public device_object
	{
		typedef core::com_ptr<IDirect3DVertexDeclaration9>	SPDirect3DVertexDeclaration9;
		typedef core::com_ptr<IDirect3DVertexBuffer9>		SPDirect3DVertexBuffer9;

	public:
		geometry_impl(const vertex::vertex_decl decl, bool is_dynamic)
			:m_vb(0), m_vertex_decl(0), m_dynamic(is_dynamic), m_size(0)
		{
			g_d3d->CreateVertexDeclaration((const D3DVERTEXELEMENT9*)decl, &m_vertex_decl);
		}

		virtual ~geometry_impl()
		{
		}

		virtual void onLostDevice()
		{
			//m_vertex_decl->;
			//m_vb->OnLost();// Buffer to hold vertices
		}

		virtual void onResetDevice()
		{
		}

		void recreateVB(size_t bytes)
		{
			if (m_size < bytes)
			{
				if( m_vb.get() != NULL )
					m_vb.get()->Release();

				if (!m_dynamic)
					g_d3d->CreateVertexBuffer((UINT)bytes, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_vb, NULL);
				else
					g_d3d->CreateVertexBuffer((UINT)bytes, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, NULL);

				m_size = bytes;
				m_used_size = m_size;
			}
			else
			{
				m_used_size = bytes;
			}
		}

		virtual void update(const void *pdata, size_t bytes, size_t size_of_vertex)
		{
			m_nSizeOfVertex = size_of_vertex;

			recreateVB(bytes);

			if (!m_vb) return;

			// Fill the vertex buffer.
			void* pVertices = 0;

			m_vb->Lock( 0, (UINT)bytes, (void**)&pVertices, m_dynamic ? D3DLOCK_DISCARD : 0);

				memcpy( pVertices, pdata, bytes);
			m_vb->Unlock();
		}

		virtual void render(primitive_type ePrimType, unsigned nPrimNum)
		{
			if (0 == nPrimNum) return;

			g_d3d->SetStreamSource( 0, m_vb.get(), 0, (UINT)m_nSizeOfVertex );
			g_d3d->SetVertexDeclaration(m_vertex_decl.get());
			D3DPRIMITIVETYPE dxPrimTypeEnum = (D3DPRIMITIVETYPE)ePrimType;
			g_d3d->DrawPrimitive(dxPrimTypeEnum, 0, nPrimNum);

			render_device::get().add_statistics(nPrimNum * 3, nPrimNum);
		}
	private:
		size_t							m_used_size;
		size_t							m_size;
		bool							m_dynamic;
		size_t							m_nSizeOfVertex;
		SPDirect3DVertexDeclaration9	m_vertex_decl;
		SPDirect3DVertexBuffer9			m_vb;// Buffer to hold vertices
	};

	base_geometry* base_geometry::create(const vertex::vertex_decl decl, bool is_dynamic)
	{
		return new geometry_impl(decl, is_dynamic);
	}


	class IndexedGeometryImpl : public IIndexedGeometry, public device_object
	{
	public:
		IndexedGeometryImpl(const vertex::vertex_decl decl, bool bUse32bitIndixes, bool is_dynamic)
			: m_ib_size(0)
			, m_ib_used_size(0)
			, m_vb_size(0)
			, m_vb_used_size(0)
			, m_is_dynamic(is_dynamic)
		{
			m_bUse32bitIndixes = bUse32bitIndixes;
			m_pVB	= 0;
			m_pIB	= 0;
			g_d3d->CreateVertexDeclaration((const D3DVERTEXELEMENT9*)decl, &m_pVertexDeclaration);
		}
		virtual ~IndexedGeometryImpl()
		{
			if (0 != m_pVertexDeclaration)
				m_pVertexDeclaration->Release();

			if (0 != m_pVB)
				m_pVB->Release();

			if (0 != m_pIB)
				m_pIB->Release();
		}
		
		virtual void onLostDevice()
		{
			//m_vertex_decl->;
			//m_vb->OnLost();// Buffer to hold vertices
		}

		virtual void onResetDevice()
		{
		}

		void recreateVB(size_t bytes)
		{
			if (m_vb_size < bytes)
			{
			if( m_pVB != 0 )
				m_pVB->Release();

			if (!m_is_dynamic)
				g_d3d->CreateVertexBuffer((UINT)bytes, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB, NULL);
			else
				g_d3d->CreateVertexBuffer((UINT)bytes, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);

				m_vb_size = bytes;
				m_vb_used_size = m_vb_size;
			}
			else
			{
				m_vb_used_size = bytes;
			}
		}

		virtual void updateVB(const void *data, size_t nBytes, size_t size_of_vertex)
		{
			m_nSizeOfVertex = size_of_vertex;

			if (0 == nBytes) return;

			//g_d3d->CreateVertexBuffer( (UINT)bytes, D3DUSAGE_WRITEONLY, 0,
			//	D3DPOOL_MANAGED, &m_vb, NULL );

			recreateVB(nBytes);

			if (!m_pVB) return;

			// Fill the vertex buffer.
			void* pVertices = 0;
			m_pVB->Lock( 0, (UINT)nBytes, (void**)&pVertices, m_is_dynamic ? D3DLOCK_DISCARD : 0 );
				memcpy( pVertices, data, nBytes);
			m_pVB->Unlock();
		}

		void recreateIB(size_t bytes)
		{
			if (m_ib_size < bytes)
			{
				if (0 != m_pIB)
					m_pIB->Release();

				//if (0 == nBytes)
				//	return;

				D3DFORMAT format;
				if (m_bUse32bitIndixes)
					format = D3DFMT_INDEX32;
				else 
					format = D3DFMT_INDEX16;


				if (!m_is_dynamic)
					g_d3d->CreateIndexBuffer((UINT)bytes, D3DUSAGE_WRITEONLY, format, D3DPOOL_MANAGED, &m_pIB, NULL);
				else
					g_d3d->CreateIndexBuffer((UINT)bytes, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, format, D3DPOOL_DEFAULT, &m_pIB, NULL);

				m_ib_size = bytes;
				m_ib_used_size = m_ib_size;
			}
			else
			{
				m_ib_used_size = bytes;
			}
		}

		virtual void updateIB(const void *data, size_t nBytes)
		{
			if (0 == nBytes)
				return;

			recreateIB(nBytes);

			if (!m_pIB) return;

			void* pIndexes = 0;
			m_pIB->Lock(0, (UINT)nBytes, &pIndexes, m_is_dynamic ? D3DLOCK_DISCARD : 0);
				memcpy( pIndexes, data, nBytes);
			m_pIB->Unlock();
		}

		virtual void render(primitive_type ePrimType, unsigned nBaseVertexIndex, unsigned min_index, unsigned nNumVertices, unsigned nStartIndex, unsigned nPrimitiveCount)
		{
			if (0 == nPrimitiveCount)
				return;

			g_d3d->SetStreamSource( 0, m_pVB, 0, (UINT)m_nSizeOfVertex );
			g_d3d->SetIndices(m_pIB);

			g_d3d->SetVertexDeclaration(m_pVertexDeclaration);
			D3DPRIMITIVETYPE dxPrimTypeEnum = (D3DPRIMITIVETYPE)ePrimType;
			g_d3d->DrawIndexedPrimitive(dxPrimTypeEnum, nBaseVertexIndex, min_index, nNumVertices, nStartIndex, nPrimitiveCount);

			render_device::get().add_statistics(nNumVertices, nPrimitiveCount);
		}

	private:
		size_t							m_vb_used_size;
		size_t							m_vb_size;

		size_t							m_ib_used_size;
		size_t							m_ib_size;

		bool							m_is_dynamic;

		bool							m_bUse32bitIndixes;
		size_t							m_nSizeOfVertex;
		LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;
		LPDIRECT3DVERTEXBUFFER9			m_pVB;// Buffer to hold vertices
		IDirect3DIndexBuffer9*			m_pIB;
	};

	IIndexedGeometry* IIndexedGeometry::create(const vertex::vertex_decl decl, bool bUse32bitIndixes, bool is_dynamic)
	{
		return new IndexedGeometryImpl(decl, bUse32bitIndixes, is_dynamic);
	}

	void readColor( TiXmlElement* node, math::Color& color)
	{		
		double r,g,b,a; 
		r = g = b = a = 1; 		
		TiXmlElement *pElement = node->FirstChildElement("color");
		if ( 0  != pElement )
		{
			pElement->Attribute("r", &r);
			pElement->Attribute("g", &g);
			pElement->Attribute("b", &b);
			pElement->Attribute("a", &a);
		}
		color = math::Color( (unsigned char)(r*255),(unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255) );
	}

	void readPosition( TiXmlElement* node, math::vec3f& position)
	{
		if ( TiXmlElement *elem = node->FirstChildElement("position") )
			base::read(position, elem);
	}

	void readTangent( TiXmlElement* node, math::vec3f& tangent)
	{
		if (TiXmlElement *elem = node->FirstChildElement("tangent") )
			base::read(tangent, elem);
	}

	void readBinormal( TiXmlElement* node, math::vec3f& binormal)
	{
		if (TiXmlElement *elem = node->FirstChildElement("binormal") )
			base::read(binormal, elem);
	}

	void readNormal( TiXmlElement* node, math::vec3f& normal)
	{		
		if (TiXmlElement *elem = node->FirstChildElement("normal") )
			base::read(normal, elem);
	}

	void readTexCoords( TiXmlElement* node, math::vec2f& tex)
	{
		if (TiXmlElement *elem = node->FirstChildElement("uvset") )
			base::read(tex, elem);
	}

	void readTexCoords2( TiXmlElement* node, math::vec2f& tex0, math::vec2f& tex1)
	{
		if (TiXmlElement *elem = node->FirstChildElement("uvset") )
		{
			base::read(tex0, elem);

			if (elem = elem->NextSiblingElement("uvset"))
				base::read(tex1, elem);
		}
	}

	void readWeights( TiXmlElement* node, math::vec4f& weights)
	{		
		if (TiXmlElement *elem = node->FirstChildElement("weights") )
			base::read(weights, elem);
	}

}