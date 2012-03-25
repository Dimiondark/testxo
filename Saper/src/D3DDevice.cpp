#include "D3DDevice.h"


IDirect3DDevice9* g_pD3DDevice  = 0; //���� ����������


HRESULT CShader::LoadShader( std::string FileName, IDirect3DDevice9* pD3DDevice, FILE* FileLog )
{
	LPD3DXBUFFER pErrors        = 0;
	LPD3DXBUFFER pShaderBuff    = 0;
		
	m_pVertexShader = 0;
	m_pPixelShader  = 0;
	m_pConstTableVS = 0;
	m_pConstTablePS = 0;
	
	//-------------------------------SkyShader----------------------------
	/*/ ���������� ������
	D3DXCompileShaderFromFile( "shader//Sky.vsh", 0, 0, "main", "vs_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
								&pShaderBuff, &pErrors, &m_pConstTableVS[Sky] );
	if ( pShaderBuff )
	{
		g_pD3DDevice->CreateVertexShader(( DWORD* )pShaderBuff->GetBufferPointer(), &m_pVertexShader[Sky]);
		pShaderBuff -> Release();
	}
	// ���������� ������
	D3DXCompileShaderFromFile( "shader//Sky.psh", 0, 0, "main", "ps_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
								&pShaderBuff, &pErrors, &m_pConstTablePS[Sky] );
	if ( pShaderBuff )
	{
		g_pD3DDevice->CreatePixelShader(( DWORD* )pShaderBuff->GetBufferPointer(), &m_pPixelShader[Sky]);
		pShaderBuff -> Release();
	}*/
	//-------------------------------Diffuse----------------------------
	HRESULT hr;
	// ���������� ������
	std::string FileNameVS = FileName + std::string( ".vsh" );
	hr = D3DXCompileShaderFromFile( FileNameVS.c_str(), 0, 0, "main", "vs_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3, &pShaderBuff, &pErrors, &m_pConstTableVS );
	if ( ( hr != S_OK ) && ( FileLog ) )
		fprintf( FileLog, "error load shader '%s'\n", FileNameVS.c_str() );
	if ( pShaderBuff )
	{
		pD3DDevice->CreateVertexShader(( DWORD* )pShaderBuff->GetBufferPointer(), &m_pVertexShader );
		pShaderBuff -> Release();
	}
	// ���������� ������
	std::string FileNamePS = FileName + std::string( ".psh" );
	hr = D3DXCompileShaderFromFile( FileNamePS.c_str(), 0, 0, "main", "ps_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3, &pShaderBuff, &pErrors, &m_pConstTablePS );
	if ( ( hr != S_OK ) && ( FileLog ) )
		fprintf( FileLog, "error load shader '%s'\n", FileNamePS.c_str() );
	if ( pShaderBuff )
	{
		pD3DDevice->CreatePixelShader(( DWORD* )pShaderBuff->GetBufferPointer(), &m_pPixelShader );
		pShaderBuff -> Release();
	}
	if  ( FileLog ) 
		fprintf( FileLog, "Load shader '%s'\n", FileName.c_str() );
	return S_OK;
}

void CShader::Release()
{		
	if ( m_pVertexShader )
		m_pVertexShader->Release();
	if ( m_pPixelShader )
		m_pPixelShader->Release();
	if ( m_pConstTableVS )
		m_pConstTableVS->Release();
	if ( m_pConstTablePS )
		m_pConstTablePS->Release();
	}

HRESULT CD3DDevice::IntialDirect3D( HWND hwnd , FILE* FileLog)
{
	m_pDirect3D  = 0;
	g_pD3DDevice = 0;
	D3DPRESENT_PARAMETERS Direct3DParametr; // ��������� �������� �������� ���������� 
	D3DDISPLAYMODE        Display; // ���������� ��������� �������

	if ( ( m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == 0 ) // �������� ������� ���������
		return E_FAIL;	
	if ( FileLog ) 
		fprintf( FileLog, "Initial Direct3D\n" );
	if ( FAILED( m_pDirect3D -> GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &Display ) ) ) // �������� ������� ������ �������
		return E_FAIL;

	ZeroMemory( &Direct3DParametr, sizeof( Direct3DParametr ) );
	Direct3DParametr.Windowed               = TRUE;					 // ����� ����� ���� (��� ������������� �����)
	Direct3DParametr.SwapEffect             = D3DSWAPEFFECT_DISCARD; // ���������� ����� �������
	Direct3DParametr.BackBufferFormat       = Display.Format;		 // ������ ����������� ������� ������
	Direct3DParametr.EnableAutoDepthStencil = TRUE;					 // �������� Z-�����
	Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D16;
	Direct3DParametr.PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT; 

	if ( FAILED( m_pDirect3D -> CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
											  &Direct3DParametr, &g_pD3DDevice ) ) ) // �������� ��������� ����������
		return E_FAIL;
	if ( FileLog ) 
		fprintf( FileLog, "Initial CreateDevice Direct3D\n" );
	g_pD3DDevice -> SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );				//  ����� ��������� �������� � ���������� �� ������� �������
	g_pD3DDevice -> SetRenderState( D3DRS_LIGHTING, FALSE );					// ����������� ������ �� ������
	g_pD3DDevice -> SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );				// ��������� ������������ Z-�����
	g_pD3DDevice -> SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );            // �������� �����-�����
	g_pD3DDevice -> SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_pD3DDevice -> SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_pD3DDevice -> SetRenderState( D3DRS_AMBIENT, 0xffffffff );
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ); // ���������� �������� ��� ��������� ��������

	return S_OK;
}

void CD3DDevice::Release()
{	
	if ( g_pD3DDevice )
		g_pD3DDevice -> Release();
	if ( m_pDirect3D )
		m_pDirect3D -> Release();	
};


