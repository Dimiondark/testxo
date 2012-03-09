#include "CameraDevice.h"

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}


using namespace std;

IDirect3DDevice9	   *g_pD3DDevice  = NULL; //���� ����������
IDirect3DCubeTexture9  *CubeTexture   = NULL;
IDirect3DTexture9      *pTextura001   = NULL;

enum         NameShader { Sky , Diffuse, Mirror };
bool         g_Exit = false;
FILE        *g_FileLog;
D3DXVECTOR4  Light( 0.0f, 1.0f, -1.0f, 1.0f );
bool         g_Wireframe = false;
FLOAT        Diffuse_intensity = 1.0f;
CameraDevice Camera;

struct CCell
{
	float       Radius;
	D3DXVECTOR3 Centr;	
	int         Value;
	CCell()
	{		
		Value  = 10;
		Radius = 5.0f;			
		Centr  = D3DXVECTOR3( 0, 0, 0 );			
	}
	void SetCenter( float x, float y, float z)
	{
		D3DXMATRIX MatrixWorld;
		D3DXMatrixTranslation( &MatrixWorld, 0, 0, 0 );
		Centr = D3DXVECTOR3( x, y, z );
		D3DXVec3TransformNormal( &Centr, &Centr, &MatrixWorld );
	}
};

struct CVertexFVF
{
	FLOAT X,   Y,  Z;
	FLOAT nx, ny, nz;
	FLOAT tu, tv;
};

struct CMouseState
{
	LONG     lX;
	LONG     lY;
	LONG     lZ;
	BYTE     rgbButtons[8];
	CMouseState()
	{
		lX = 0;
		lY = 0;
		lZ = 0;
	}
};

struct CLuaScript
{
	lua_State    *m_luaVM;
	void         *m_FileBuffer;
	unsigned int  m_FileSize;
	bool          lua_dobuffer( lua_State* Lua, void const* Buffer, int Size );
	CLuaScript();
   ~CLuaScript();
};

class CD3DDevice
{
public:
	IDirect3D9			   *m_pDirect3D; // ��������� �� ������� ��������� ���������� �� Direct3D
	IDirect3DTexture9      *pTextura002;
	IDirect3DTexture9      *m_pTexturaSky;
	IDirect3DTexture9	   *m_Texture;
	HRESULT                 IntialDirect3D(HWND hwnd);	
	HRESULT				    LoadTexture();
	void				    Release();
};

struct CShader
{
	IDirect3DPixelShader9  *pPixelShader [3];
	IDirect3DVertexShader9 *pVertexShader[3];
	ID3DXConstantTable     *pConstTableVS[3];
	ID3DXConstantTable     *pConstTablePS[3];
	int						m_CountShader;
	HRESULT                 InitialShader();
	~CShader();
};

class CInputDevice
{
public:
	LPDIRECTINPUT8			pInput;
	LPDIRECTINPUTDEVICE8    pKeyboard;
	LPDIRECTINPUTDEVICE8    pMouse;
	CMouseState             mouse;
	~CInputDevice();
	HRESULT                 InitialInput(HWND hwnd);
	bool                    ScanInput();
};

struct CSky
{
	IDirect3DVertexBuffer9 *m_pVerBufSky;
	IDirect3DIndexBuffer9  *m_pBufIndexSky;
	HRESULT                 InitialSky();
	~CSky();
};

class CMesh3D
{
public:
	ID3DXMesh              *m_pMesh;
	D3DMATERIAL9           *m_pMeshMaterial;
	IDirect3DTexture9     **m_pMeshTextura;
	DWORD                   m_TexturCount; 
	IDirect3DVertexBuffer9 *m_VertexBuffer;
	IDirect3DIndexBuffer9  *m_IndexBuffer;
	DWORD 					m_SizeFVF;
	float                   m_Alpha;
	HRESULT                 InitialMesh( LPCSTR Name );
	void					Release();
	void                    DrawMyMesh();
	void					SetMatrixWorld( D3DXMATRIX  Matrix );
	void					SetMatrixView( D3DXMATRIX  Matrix );
	void					SetMatrixProjection( D3DXMATRIX  Matrix );
private:
	D3DXMATRIX              m_MatrixWorld;
	D3DXMATRIX              m_MatrixView;
	D3DXMATRIX              m_MatrixProjection;
};


CD3DDevice   g_DeviceD3D;
CLuaScript   g_Lua;
CCell        g_Cell[3][3];
CShader      g_Shader;
CInputDevice g_DeviceInput;
CSky         g_Sky;
CMesh3D      g_MeshS;
CMesh3D      g_MeshX;
CMesh3D      g_MeshO;

void DrawMyText(IDirect3DDevice9 *g_pD3DDevice, char* StrokaTexta, int x, int y, int x1, int y1, D3DCOLOR MyColor)
{
	RECT  Rec;
	HFONT hFont;
	ID3DXFont *pFont = NULL; 
	hFont = CreateFont(30, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 0, 1, 0, 0, 0, DEFAULT_PITCH | FF_MODERN, "Arial");
	Rec.left   = x;
	Rec.top    = y;
	Rec.right  = x1;
	Rec.bottom = y1;
	D3DXCreateFont( g_pD3DDevice, 30, 10, FW_NORMAL, 0, FALSE, 0, 0, 0, DEFAULT_PITCH | FF_MODERN, "Arial", &pFont );
	pFont->DrawText(NULL, StrokaTexta, -1, &Rec, DT_WORDBREAK, MyColor);
	if (pFont != NULL)
		pFont -> Release();
}

POINT PickObject()
{
	POINT Point;
	float px = 0.0f;
	float py = 0.0f;
	D3DVIEWPORT9 ViewPort;
	RECT ClientRec;

	GetClientRect ( GetForegroundWindow(), &ClientRec);
	ClientToScreen( GetForegroundWindow(), (LPPOINT)&ClientRec);
	GetCursorPos( &Point );
	int x = Point.x - ClientRec.left;
	int y = Point.y - ClientRec.top;
	g_pD3DDevice->GetViewport( &ViewPort );

	px = (  2.0f * x / ViewPort.Width  - 1.0f) / Camera.m_Proj._11;
	py = ( -2.0f * y / ViewPort.Height + 1.0f) / Camera.m_Proj._22;	

	D3DXVECTOR3 Direction = D3DXVECTOR3( px, py, 1.0f );

	D3DXMATRIX MatV;
	D3DXMatrixInverse( &MatV, NULL, &Camera.m_View ); 
	D3DXVECTOR3 PosView = D3DXVECTOR3( MatV._41, MatV._42, MatV._43 ); //   ������� ���������� ������ �� ������� ����	
	D3DXVec3TransformNormal( &Direction, &Direction, &MatV );
	D3DXVec3Normalize( &Direction, &Direction );
	POINT NumObject[9];
	int Count = -1;
	for ( int ArrY = 0; ArrY < 3; ++ArrY )
		for ( int ArrX = 0; ArrX < 3; ++ArrX )
		{	
			D3DXVECTOR3 v =  PosView - g_Cell[ArrX][ArrY].Centr;
			float b = 2.0f * D3DXVec3Dot( &Direction, &v );
			float c = D3DXVec3Dot( &v, &v ) - g_Cell[ArrX][ArrY].Radius * g_Cell[ArrX][ArrY].Radius ;
			// ������� ������������
			float Discr = ( b * b ) - ( 4.0f * c );
			// ��������� �� ������ �����
			if ( Discr >= 0.0f )
			{
				Discr = sqrtf(Discr);
				float s0 = ( -b + Discr ) / 2.0f;
				float s1 = ( -b - Discr ) / 2.0f;
				// ���� ���� ������� >= 0, ��� ���������� �����
				if ( ( s0 >= 0.0f ) && ( s1 >= 0.0f ) )
				{
					++Count;
					NumObject[Count].x = ArrX;
					NumObject[Count].y = ArrY;					
				}
			}
		}
	float Dist = 100000.f;
	if ( Count < 0 )
	{
		NumObject[0].x = -1;
		NumObject[0].y = -1;
		return NumObject[0];
	}
	if ( Count == 0 )	
		return NumObject[0];
	if ( Count > 0 )
	{
		for ( int i = 0; i < Count; ++i)
		{
			D3DXVECTOR3 T = PosView - g_Cell[NumObject[i].x][NumObject[i].y].Centr;
			float DistVec = D3DXVec3LengthSq( &T );
			if ( DistVec < Dist )
			{
				NumObject[0].x = NumObject[i].x;
				NumObject[0].y = NumObject[i].y;
			}
		}
	}
	
	return NumObject[0];
}

void RenderingDirect3D(HWND hwnd)
{
	D3DXMATRIX  MatrixWorld, MatrixWorldX, MatrixWorldY, MatrixWorldZ;
	D3DXMATRIX  tmp;
	D3DXMATRIX  MatrixView;
	D3DXMATRIX  MatrixProjection;
	char        str[50];
	D3DXVECTOR4 Scale( tan( D3DX_PI / 8 * (FLOAT)Height / Width), tan( D3DX_PI / 8 * (FLOAT)Height / Width  ), 1.0f, 1.0f );
	//----------------------------------------------����� �������-------------------------------
	if ( g_Wireframe )
		g_pD3DDevice -> SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		g_pD3DDevice -> SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	//------------------------------------------------------------------------------------------

	//UINT  Time  = timeGetTime()  9000;
	FLOAT Angle = timeGetTime() / 2000.0f;
	
	MatrixView       = Camera.m_View;
	MatrixProjection = Camera.m_Proj;

	if ( g_pD3DDevice == NULL )
		return;
	
	g_pD3DDevice -> Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);// ������� ������� ������
	g_pD3DDevice -> BeginScene(); // ������ ����������

	//------------------------------------------Render Sky----------------------------------------
	g_pD3DDevice -> SetRenderState(  D3DRS_ZENABLE, false );
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ); // ���������� �������� ��� ��������� ��������
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ); // ���������� �������� ��� ��������� ��������
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP ); // ���������� �������� ��� ��������� ��������

	D3DXMatrixTranslation( &MatrixWorld, 1.0f, 1.0f, 1.0f );
	tmp = MatrixWorld * MatrixView * MatrixProjection;
	g_Shader.pConstTableVS[Sky] -> SetMatrix( g_pD3DDevice, "mat_mvp",   &tmp );
	g_Shader.pConstTableVS[Sky] -> SetVector( g_pD3DDevice, "vec_light", &Light );
	g_Shader.pConstTableVS[Sky] -> SetVector( g_pD3DDevice, "scale",     &Scale );
	g_Shader.pConstTablePS[Sky] -> SetMatrix( g_pD3DDevice, "mat_view",  &MatrixView );
	// ����� ����������� �����	
	g_pD3DDevice -> SetStreamSource(0, g_Sky.m_pVerBufSky, 0, sizeof( CVertexFVF ) ); // ����� ������ ������ � ������� ������
	g_pD3DDevice -> SetFVF( D3DFVF_CUSTOMVERTEX ); // ��������������� ������ ������
	g_pD3DDevice -> SetIndices( g_Sky.m_pBufIndexSky );
	g_pD3DDevice -> SetTexture( 0, CubeTexture );
	// ������������� �������
	g_pD3DDevice -> SetVertexShader( g_Shader.pVertexShader[Sky] );
	g_pD3DDevice -> SetPixelShader(  g_Shader.pPixelShader [Sky] );
	// ����� ����������
	//g_pD3DDevice -> DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2 );

	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	g_pD3DDevice -> SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
	g_pD3DDevice -> SetRenderState(  D3DRS_ZENABLE, true );
	//------------------------------------------Render Mesh----------------------------------------
	
	//------------------Setka--------------
	D3DXMatrixRotationY( &MatrixWorld, 0 );
	g_MeshS.SetMatrixWorld( MatrixWorld );
	g_MeshS.SetMatrixView( MatrixView );
	g_MeshS.SetMatrixProjection( MatrixProjection );
	g_MeshS.DrawMyMesh();
	for ( int y = 0; y < 3; ++y )
		for ( int x = 0; x < 3; ++x )
		{
			if ( g_Cell[x][y].Value == 1 )
			{
				//--------------------X-------------------
				D3DXMatrixRotationY(   &MatrixWorldY, Angle );
				D3DXMatrixTranslation( &MatrixWorldX, ( x * 16 - 16 ), ( 16 - y * 16 ), 0 );
				MatrixWorld = MatrixWorldY * MatrixWorldX;
				g_MeshX.SetMatrixWorld( MatrixWorld );
				g_MeshX.SetMatrixView( MatrixView );
				g_MeshX.SetMatrixProjection( MatrixProjection );
				g_MeshX.m_Alpha = 1.0f;
				g_MeshX.DrawMyMesh();
			}
			if ( g_Cell[x][y].Value == 0 )
			{		
				//--------------------O-------------------
				D3DXMatrixRotationX(   &MatrixWorldY, -Angle );
				D3DXMatrixTranslation( &MatrixWorldX, ( x * 16 - 16 ), ( 16 - y * 16  ), 0 );
				D3DXMatrixMultiply(&MatrixWorld, &MatrixWorldY, &MatrixWorldX);
				g_MeshO.SetMatrixWorld( MatrixWorld );
				g_MeshO.SetMatrixView( MatrixView );
				g_MeshO.SetMatrixProjection( MatrixProjection );
				g_MeshO.m_Alpha = 1.0f;
				g_MeshO.DrawMyMesh();
			}
		}
		POINT P = PickObject();
		if ( P.x >= 0)
		{
			if ( g_Cell[P.x][P.y].Value == 10 )
			{
				//--------------------X-------------------
				D3DXMatrixRotationY(   &MatrixWorldY, Angle );
				D3DXMatrixTranslation( &MatrixWorldX, ( P.x * 16 - 16 ), ( 16 - P.y * 16 ), 0 );
				MatrixWorld = MatrixWorldY * MatrixWorldX;
				g_MeshX.SetMatrixWorld( MatrixWorld );
				g_MeshX.SetMatrixView( MatrixView );
				g_MeshX.SetMatrixProjection( MatrixProjection );
				g_MeshX.m_Alpha = 0.4f;
				g_MeshX.DrawMyMesh();
			}
		}
		sprintf(str, "%f                %d", Angle, P.y);		
		DrawMyText(g_pD3DDevice, str, 10, 10, 500, 700, D3DCOLOR_ARGB(250, 250, 250,50));	

	g_pD3DDevice -> EndScene();
	g_pD3DDevice -> Present(NULL, NULL, NULL, NULL); // ����� ����������� ������� ������ � ����
}

int SaveField( lua_State *luaVM )
{	
	lua_newtable( luaVM );//������� �������, ��������� �� �� ������� �����
	for (int i = 1; i <= 9; ++i) 
	{
		lua_pushnumber(luaVM, i);               //������ � ���� ����� (key)
		lua_pushnumber(luaVM, i + 1 );//��������� �������� ����� (value)
		lua_settable  (luaVM, -3);              //�������� � ������� ���� ����-��������: table[key] = value		
	}
	return 1;
}

void CheckPC()
{

}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{	
	switch ( Message )
	{
	case WM_CLOSE:
		g_Exit = true;
		break;
	case WM_KEYDOWN:
		if ( wparam == VK_ESCAPE )
			g_Exit = true;		
		if ( wparam == VK_F4 )
			g_Wireframe = !g_Wireframe;
		break;
	}
	return DefWindowProc( hwnd, Message, wparam, lparam );
}  

struct CFps
{
	int			 m_count;
	int          m_fps;
	int          m_last_tick;
	int			 m_this_tick;
	int			 Fps();

	CFps(): m_count(0)
	{	}
};

int CFps::Fps()
{	
	m_this_tick = GetTickCount();
	if ( m_this_tick - m_last_tick >= 1000 )
	{
		m_last_tick = m_this_tick;
		m_fps       = m_count;
		m_count     = 0;
	}
	else m_count++;
return m_fps;
}





int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine, int       nCmdShow)
{
	HWND		hwnd;
	MSG			msg;
	WNDCLASS	w;	
	CFps        g_fps;
	D3DVIEWPORT9 vp;

	memset(&w,0,sizeof(WNDCLASS));
	w.style         = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc   = WndProc;
	w.hInstance     = hInstance;
	w.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	w.lpszClassName = "My Class";
	w.hIcon         = LoadIcon(NULL,IDI_QUESTION);//����������� ������ ���������� Win API 	
	RegisterClass(&w);
	hwnd = CreateWindow( "My Class", "����", WS_SYSMENU | WS_MINIMIZEBOX,
		                 250, 150, Width+6, Height+28, NULL, NULL, hInstance, NULL );	
	char str[8];
	//memset(Field,0,sizeof(int)*9);
	//ZeroMemory(Field, sizeof(Field));
	srand(1000);
	for (int y = 0; y < 3; ++y)
		for (int x = 0; x < 3; ++x)
			g_Cell[x][y].SetCenter( x * 16 - 16, 16 - y * 16, 0 );		
	g_Cell[2][0].Value = 0;
	if ( SUCCEEDED(g_DeviceD3D.IntialDirect3D(hwnd) ) )
	{	
		if ( SUCCEEDED( g_DeviceD3D.LoadTexture() ) )
		{			
			ShowWindow(hwnd,nCmdShow);
			ZeroMemory(&msg, sizeof(msg));
			g_MeshS.InitialMesh("Setka.x");
			g_MeshO.InitialMesh("O.x");
			g_MeshX.InitialMesh("X.x");	
			g_Sky.InitialSky();
			g_DeviceInput.InitialInput(hwnd);					
			g_Shader.InitialShader();
			g_fps.m_last_tick = GetTickCount();
			while( !g_Exit )
			{						
				g_pD3DDevice -> GetViewport(&vp);
				//sprintf(str, "FPS=%d", g_fps.Fps());
				//SetWindowText(hwnd,str);
				g_DeviceInput.ScanInput();
				RenderingDirect3D( hwnd );
				if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage(  &msg );
				}
			}						
		}
	}	
	g_MeshS.Release();
	g_MeshX.Release();
	g_MeshO.Release();
	g_DeviceD3D.Release();
	return 0;
}








HRESULT CInputDevice::InitialInput(HWND hwnd)
{	
	pInput    = NULL;
	pKeyboard = NULL;
	pMouse    = NULL;
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize		= sizeof( DIPROPDWORD );
	dipdw.diph.dwHeaderSize	= sizeof( DIPROPHEADER );
	dipdw.diph.dwObj		= 0;
	dipdw.diph.dwHow		= DIPH_DEVICE;
	dipdw.dwData			= 32;
	if (FAILED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL)))
		return E_FAIL;
	fprintf( g_FileLog, "Initial DirectInput8\n" );

	if FAILED(pInput -> CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL)) //�������� ���������� ����������
		return E_FAIL;
	if FAILED(pKeyboard -> SetDataFormat(&c_dfDIKeyboard))
		return E_FAIL;
	if FAILED(pKeyboard -> SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
		return E_FAIL;
	if( FAILED(pKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
		return E_FAIL;
	if FAILED(pKeyboard -> Acquire())
		return E_FAIL;

	if FAILED(pInput -> CreateDevice(GUID_SysMouse, &pMouse, NULL)) // �������� ���������� ����
		return E_FAIL;	
	if FAILED(pMouse -> SetDataFormat(&c_dfDIMouse2))
		return E_FAIL;	
	if FAILED(pMouse -> SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))
		return E_FAIL;	
	if FAILED(pMouse -> Acquire())
		return E_FAIL;

return S_OK;
}

bool CInputDevice::ScanInput()
{	
	char     keyboard[256];     
	LONG     dx, dy, dz;

	if FAILED( pKeyboard -> GetDeviceState(sizeof(keyboard), (LPVOID)&keyboard) )
	{
		pKeyboard -> Acquire();
		return FALSE;
	}
	
	if ( KEYDOWN(keyboard, DIK_RIGHT) || KEYDOWN(keyboard, DIK_D))
		Camera.MoveRight();
	if ( KEYDOWN(keyboard, DIK_LEFT) || KEYDOWN(keyboard, DIK_A))
		Camera.MoveLeft();
	if ( KEYDOWN(keyboard, DIK_UP) || KEYDOWN(keyboard, DIK_W))     
		Camera.MoveForv();
	if ( KEYDOWN(keyboard, DIK_DOWN) || KEYDOWN(keyboard, DIK_S))
		Camera.MoveBack();

	if FAILED( pMouse -> GetDeviceState( sizeof( CMouseState ), (LPVOID)&mouse ) )
	{
		pMouse -> Acquire();
		return FALSE;
	}
	dx = mouse.lX;
	dy = mouse.lY;
	dz = mouse.lZ;

	if ( mouse.rgbButtons[LEFT_BUTTON]&0x80 )
	{
		POINT Point = PickObject();
		if ( ( Point.x >= 0 ) && ( g_Cell[Point.x][Point.y].Value > 1 ) )
		{
			g_Cell[Point.x][Point.y].Value = 1;
			CheckPC();
		}
	}
return TRUE;
}

CInputDevice::~CInputDevice()
{
	if (pInput)
	{
		if (pKeyboard)
		{
			if (pKeyboard != NULL)
				pKeyboard -> Unacquire();
			if (pKeyboard != NULL)
				pKeyboard -> Release();
			pKeyboard = NULL;
		}
		if (pMouse)
		{
			if (pMouse != NULL)
				pMouse -> Unacquire();
			if (pMouse != NULL)
				pMouse -> Release();
			pMouse = NULL;
		}
		if (pInput != NULL)
			pInput -> Release();
		pInput = NULL;
	}
}

bool CLuaScript::lua_dobuffer( lua_State* Lua, void const* Buffer, int Size )
{
	if ( !Size )
		return true;
	// ������ ���������� � ���� input.txt
	g_FileLog = fopen ("log.txt", "w");

	if ( luaL_loadbuffer( Lua, (char const*)Buffer, Size, 0 ) )
	{
		char const* ErrorMsg = lua_tostring( Lua, -1 );
		lua_pop( Lua, 1 );
		fprintf( g_FileLog, "%s",ErrorMsg );
		return false;
	}

	if ( lua_pcall( Lua, 0, LUA_MULTRET, 0 ) )
	{
		char const* ErrorMsg = lua_tostring( Lua, -1 );
		lua_pop( Lua, 1 );
		fprintf( g_FileLog, "%s", ErrorMsg );
		return false;
	}
	fprintf( g_FileLog, "Initial Script \n" );	
	return true;
}

CLuaScript::CLuaScript()
{
	m_FileBuffer = 0;
	m_FileSize   = 0;
	FILE* const FO = fopen( "CheckComputer.lua", "rb" );
	if ( FO )
	{
		fseek(FO,0,SEEK_END);			// ������������� ��������� �� ����� �����
		m_FileSize   = ftell(FO);			// ���������� ���������� ���� �� ������ �� ���������         
		m_FileBuffer = malloc( m_FileSize );// ���������� ��������� �� ����������� ������ �������� m_FileSize
		fseek(FO,0,SEEK_SET);			// ������������� ��������� �� ������ �����
		fread( m_FileBuffer, 1, m_FileSize, FO );// ������ � ���������� � ������ �� 1 �����
		fclose(FO);
	}
	m_luaVM = lua_open();
	if ( m_luaVM == NULL ) 
		fprintf( g_FileLog, "Error Initializing lua\n" );

	// ������������� ����������� ������������ ������� lua
	luaopen_base  ( m_luaVM );
	luaopen_table ( m_luaVM );
	luaopen_string( m_luaVM );
	luaopen_math  ( m_luaVM );
	luaopen_os    ( m_luaVM );

	lua_register( m_luaVM, "SaveField", SaveField);
	lua_dobuffer( m_luaVM, m_FileBuffer, m_FileSize );
}

CLuaScript::~CLuaScript()
{

	free( m_FileBuffer );
	lua_close( m_luaVM );
}

HRESULT CD3DDevice::IntialDirect3D( HWND hwnd )
{
	m_pDirect3D  = NULL;
	g_pD3DDevice = NULL;
	D3DPRESENT_PARAMETERS Direct3DParametr; // ��������� �������� �������� ���������� 
	D3DDISPLAYMODE        Display; // ���������� ��������� �������

	if ( ( m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL ) // �������� ������� ���������
		return E_FAIL;	
	fprintf( g_FileLog, "Initial Direct3D\n" );
	if ( FAILED( m_pDirect3D -> GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &Display ) ) ) // �������� ������� ������ �������
		return E_FAIL;

	ZeroMemory( &Direct3DParametr, sizeof( Direct3DParametr ) );
	Direct3DParametr.Windowed               = TRUE;					 // ����� ����� ���� (��� ������������� �����)
	Direct3DParametr.SwapEffect             = D3DSWAPEFFECT_DISCARD; // ���������� ����� �������
	Direct3DParametr.BackBufferFormat       = Display.Format;		 // ������ ����������� ������� ������
	Direct3DParametr.EnableAutoDepthStencil = TRUE;					 // �������� Z-�����
	Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D16;
	Direct3DParametr.PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT; // 60fps
	//Direct3DParametr.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE; //������������ fps
	/*/---------------------------������������� �����--------------------
	Direct3DParametr.BackBufferWidth  = GetSystemMetrics(SM_CXSCREEN);
	Direct3DParametr.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	Direct3DParametr.BackBufferCount  = 3;
	Direct3DParametr.FullScreen_RefreshRateInHz = Display.RefreshRate;
	//------------------------------------------------------------------*/
	if ( FAILED( m_pDirect3D -> CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&Direct3DParametr, &g_pD3DDevice ) ) ) // �������� ��������� ����������
		return E_FAIL;
	fprintf( g_FileLog, "Initial CreateDevice Direct3D\n" );
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

HRESULT	CD3DDevice::LoadTexture()
{
	pTextura001   = NULL;
	pTextura002   = NULL;
	m_pTexturaSky = NULL;
	m_Texture     = NULL;
	CubeTexture   = NULL;
	if ( FAILED( D3DXCreateTextureFromFile( g_pD3DDevice, "Textures/TexturaUV.jpg", &pTextura001 )))
		MessageBox( NULL, "�� ������� ��������� �������� ", "", MB_OK );
	if ( FAILED( D3DXCreateTextureFromFile( g_pD3DDevice, "Textures/bricks003.jpg", &pTextura002 )))
		MessageBox( NULL, "�� ������� ��������� �������� ", "", MB_OK );		
	if ( FAILED( D3DXCreateTextureFromFile( g_pD3DDevice, "Textures/Sky.jpg", &m_pTexturaSky )))
		MessageBox( NULL, "�� ������� ��������� �������� ", "", MB_OK );
	if ( FAILED( D3DXCreateCubeTextureFromFileEx( g_pD3DDevice, "Textures/sky_cube_mipmap.dds", D3DX_DEFAULT, D3DX_FROM_FILE, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, 
		D3DX_FILTER_NONE, 0, 0, 0, &CubeTexture )))
		MessageBox( NULL, "�� ������� ��������� �������� SkyCube", "", MB_OK );
	return S_OK;
}

void CD3DDevice::Release()
{
	if ( CubeTexture != NULL )
		CubeTexture -> Release();
	if ( m_pTexturaSky != NULL )
		m_pTexturaSky -> Release();	
	if ( m_Texture   != NULL )
		m_Texture   -> Release();	
	if ( pTextura002 != NULL )
		pTextura002 -> Release();
	if ( pTextura001 != NULL )
		pTextura001 -> Release();
	if ( g_pD3DDevice != NULL )
		g_pD3DDevice -> Release();
	if ( m_pDirect3D != NULL )
		m_pDirect3D -> Release();
	fclose( g_FileLog );
};

HRESULT CShader::InitialShader()
{
	LPD3DXBUFFER pErrors        = NULL;
	LPD3DXBUFFER pShaderBuff    = NULL;
	m_CountShader = 2;
	for (int i = 0; i < m_CountShader; ++i)
	{	
		pVertexShader[i] = NULL;
		pPixelShader[i]  = NULL;
		pConstTableVS[i] = NULL;
		pConstTablePS[i] = NULL;
	}
	//-------------------------------SkyShader----------------------------
	// ���������� ������
	D3DXCompileShaderFromFile( "Sky.vsh", NULL, NULL, "main", "vs_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
							    &pShaderBuff, &pErrors, &pConstTableVS[Sky] );
	g_pD3DDevice->CreateVertexShader(( DWORD* )pShaderBuff->GetBufferPointer(), &pVertexShader[Sky]);
	pShaderBuff -> Release();
	// ���������� ������
	D3DXCompileShaderFromFile( "Sky.psh", NULL, NULL, "main", "ps_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
								&pShaderBuff, &pErrors, &pConstTablePS[Sky] );
	g_pD3DDevice->CreatePixelShader(( DWORD* )pShaderBuff->GetBufferPointer(), &pPixelShader[Sky]);
	pShaderBuff -> Release();
	//-------------------------------Diffuse----------------------------
	// ���������� ������
	D3DXCompileShaderFromFile( "Diffuse.vsh", NULL, NULL, "main", "vs_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
								&pShaderBuff, &pErrors, &pConstTableVS[Diffuse] );
	g_pD3DDevice->CreateVertexShader(( DWORD* )pShaderBuff->GetBufferPointer(), &pVertexShader[Diffuse]);
	pShaderBuff -> Release();
	// ���������� ������
	D3DXCompileShaderFromFile( "Diffuse.psh", NULL, NULL, "main", "ps_2_0", D3DXSHADER_OPTIMIZATION_LEVEL3,
								&pShaderBuff, &pErrors, &pConstTablePS[Diffuse] );
	g_pD3DDevice->CreatePixelShader(( DWORD* )pShaderBuff->GetBufferPointer(), &pPixelShader[Diffuse]);
	pShaderBuff -> Release();

	return S_OK;
}

CShader::~CShader()
{
	for (int i = 0; i < m_CountShader; ++i)
	{	
		if (pVertexShader[i] != NULL)
			pVertexShader[i] -> Release();
		if (pPixelShader[i] != NULL)
			pPixelShader[i] -> Release();
		if (pConstTableVS[i] != NULL)
			pConstTableVS[i] -> Release();
		if (pConstTablePS[i] != NULL)
			pConstTablePS[i] -> Release();
	}
}

HRESULT CSky::InitialSky()
{
	void *pBV;
	void *pBI;

	m_pVerBufSky   = NULL; // ��������� �� ����� ������
	m_pBufIndexSky = NULL; // ��������� �� ����� ������

	CVertexFVF SkyVershin[] =
	{			
		{ 1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f}, // 0
		{-1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f}, // 1	
		{-1.0f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f}, // 2		
		{ 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f}, // 3
		// X        Y     Z    nx    ny    nz     tu    tv
	};
	const unsigned short SkyIndex[] =
	{
		0,1,2,    2,3,0,		
	};
	if ( FAILED( g_pD3DDevice -> CreateVertexBuffer( 4 * sizeof( CVertexFVF ), 0, // ������ ����� ������
		D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pVerBufSky, NULL ) ) )
		return E_FAIL;
	if ( FAILED( m_pVerBufSky -> Lock( 0, sizeof( SkyVershin ), ( void** )&pBV, 0 ) ) ) // ������������
		return E_FAIL; 
	memcpy( pBV, SkyVershin, sizeof( SkyVershin ) ); // ����������� ������ � �������� � ����� ������
	m_pVerBufSky -> Unlock(); // ���������������

	if ( FAILED( g_pD3DDevice -> CreateIndexBuffer( 6 * sizeof( short ), 0, D3DFMT_INDEX16,         // ������ ����� ������
		D3DPOOL_DEFAULT, &m_pBufIndexSky, NULL ) ) )
		return E_FAIL;
	if ( FAILED( m_pBufIndexSky -> Lock( 0, sizeof( SkyIndex ), ( void** )&pBI, 0 ) ) ) // ������������
		return E_FAIL; 
	memcpy( pBI, SkyIndex, sizeof( SkyIndex ) ); // ����������� ������ � �������� � ����� ������
	m_pBufIndexSky -> Unlock(); // ���������������	

	return S_OK;
}

CSky::~CSky()
{
	if ( m_pBufIndexSky != NULL )
		m_pBufIndexSky -> Release();
	if ( m_pVerBufSky != NULL )
		m_pVerBufSky -> Release();
}

HRESULT CMesh3D::InitialMesh(LPCSTR Name)
{
	m_pMesh         = NULL;
	m_pMeshMaterial = NULL;
	m_pMeshTextura  = NULL;
	m_SizeFVF       = 0;
	m_Alpha         = 1.0f;	
	ID3DXBuffer *pMaterialBuffer  = NULL;
	if (FAILED(D3DXLoadMeshFromX(Name, D3DXMESH_SYSTEMMEM, g_pD3DDevice, NULL, &pMaterialBuffer, NULL, &m_TexturCount, &m_pMesh)))
	{
		MessageBox(NULL, "�� ������� ��������� X-file", "", MB_OK);
		fprintf( g_FileLog, "No Initial LoadMeshFromX\n" );
		return E_FAIL;
	}

	if ( m_pMesh->GetFVF() & D3DFVF_XYZ ) 
		m_SizeFVF += sizeof(float)*3;
	if ( m_pMesh->GetFVF() & D3DFVF_NORMAL ) 
		m_SizeFVF += sizeof(float)*3;
	if ( m_pMesh->GetFVF() & D3DFVF_TEX1 )
		m_SizeFVF += sizeof(float)*2;

	m_pMesh->GetVertexBuffer( &m_VertexBuffer );
	m_pMesh->GetIndexBuffer(  &m_IndexBuffer  );
	// ��������� �������� ��������� � ��������{�����} ���������
	D3DXMATERIAL *D3DXMeshMaterial = (D3DXMATERIAL *)pMaterialBuffer->GetBufferPointer();
	m_pMeshMaterial  = new D3DMATERIAL9[m_TexturCount];
	m_pMeshTextura   = new IDirect3DTexture9*[m_TexturCount];
	for ( DWORD i = 0; i < m_TexturCount; i++ )
	{
		// �������� ��������
		m_pMeshMaterial[i] = D3DXMeshMaterial[i].MatD3D;
		// ���������� ����������� ����
		m_pMeshMaterial[i].Ambient = m_pMeshMaterial[i].Diffuse;
		// ��������� ��������
		if ( FAILED( D3DXCreateTextureFromFile( g_pD3DDevice, D3DXMeshMaterial[i].pTextureFilename, &m_pMeshTextura[i] )))
		{
			//MessageBox(NULL, "�� ������� ��������� �������� �� ������", "", MB_OK);
			m_pMeshTextura[i] = NULL;
		}
	}
	// ���������� ����� ���������
	pMaterialBuffer->Release();

	return S_OK;
}

void CMesh3D::SetMatrixWorld(D3DXMATRIX Matrix)
{
	m_MatrixWorld = Matrix;
}

void CMesh3D::SetMatrixView(D3DXMATRIX Matrix)
{
	m_MatrixView = Matrix;
}

void CMesh3D::SetMatrixProjection(D3DXMATRIX Matrix)
{
	m_MatrixProjection = Matrix;
}
void CMesh3D::DrawMyMesh()
{
	D3DXMATRIX  wvp;

	wvp = m_MatrixWorld * m_MatrixView * m_MatrixProjection;

	g_Shader.pConstTableVS[Diffuse] -> SetMatrix( g_pD3DDevice, "mat_mvp",   &wvp );
	g_Shader.pConstTableVS[Diffuse] -> SetMatrix( g_pD3DDevice, "mat_world", &m_MatrixWorld );
	g_Shader.pConstTableVS[Diffuse] -> SetVector( g_pD3DDevice, "vec_light", &Light );
	g_Shader.pConstTablePS[Diffuse] -> SetFloat(  g_pD3DDevice, "diffuse_intensity", Diffuse_intensity );	
	g_Shader.pConstTablePS[Diffuse] -> SetFloat(  g_pD3DDevice, "Alpha", m_Alpha );	

	// ������������� �������
	g_pD3DDevice->SetVertexShader( g_Shader.pVertexShader[Diffuse] );
	g_pD3DDevice->SetPixelShader(  g_Shader.pPixelShader [Diffuse] );

	g_pD3DDevice->SetStreamSource( 0, m_VertexBuffer, 0, m_SizeFVF );
	g_pD3DDevice->SetIndices( m_IndexBuffer );
	for ( int i = 0; i < m_TexturCount; ++i )
	{
		g_pD3DDevice -> SetMaterial( &m_pMeshMaterial[i] );
		g_pD3DDevice -> SetTexture( 0, m_pMeshTextura[i] );
		//m_pMesh -> DrawSubset(i);
	}
	g_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pMesh->GetNumVertices(), 0, m_pMesh->GetNumFaces() ); 	
}

void CMesh3D::Release()
{
	if ( m_IndexBuffer != NULL )
		m_IndexBuffer->Release();
	if ( m_VertexBuffer != NULL )
		m_VertexBuffer->Release();
	if ( m_pMeshMaterial != NULL )
		delete[] m_pMeshMaterial;
	if ( m_pMeshTextura )
	{
		for ( int i = 1; i < m_TexturCount; ++i )
		{
			if ( m_pMeshTextura[i] )
				m_pMeshTextura[i] -> Release();
		}
		delete []m_pMeshTextura;
	}
	if ( m_pMesh != NULL )
		m_pMesh -> Release();
}