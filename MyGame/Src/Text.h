#pragma once
#include "Init.h"

struct CText
{
	struct CVertexPT
	{
		FLOAT x,   y,  z;
		FLOAT u, v;
		CVertexPT()
		{	}
		CVertexPT( float X, float Y, float Z, float U, float V ) : x(X), y(Y), z(Z), u(U), v(V)
		{	}
	};
	IDirect3DVertexBuffer9* m_pVerBuf;
	IDirect3DIndexBuffer9*  m_pIndexBuf;
	IDirect3DDevice9*       m_pD3DDevice;
	IDirect3DTexture9*      m_Texture;	
	IDirect3DTexture9*      m_TextureTarget;	
	HRESULT                 Init( IDirect3DDevice9* D3DDevice );
	void                    Render( CShader const& Shader, IDirect3DTexture9* Texture, const D3DXMATRIX&  MatrixWorldTrans, int Num );
	void                    RenderInt( int Number, CShader const& Shader );
	void                    RenderImage( CShader const& Shader, float Scale, const D3DXMATRIX&  MatrixWorldTrans );
	void                    Release();
	~CText();
};