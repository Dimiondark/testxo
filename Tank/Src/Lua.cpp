#include "Lua.h"

#pragma warning( disable: 4996 )

#include <vector>

CLua::CLua() :
	m_FileBuffer( 0 )
{
	unsigned int FileSize = 0;
	FILE* const FO = fopen( "InitShader.lua", "rb" );
	if( FO )
	{
		fseek( FO, 0,SEEK_END );					// ������������� ��������� �� ����� �����
		FileSize   = ftell(FO);					// ���������� ���������� ���� �� ������ �� ���������         
		m_FileBuffer = malloc( FileSize );		// ���������� ��������� �� ����������� ������ �������� m_FileSize
		fseek( FO, 0, SEEK_SET );					// ������������� ��������� �� ������ �����
		fread( m_FileBuffer, 1, FileSize, FO );	// ������ � ���������� � ������ �� 1 �����
		fclose( FO );
	}

	m_LuaVM = lua_open();

	if( !m_LuaVM ) 
		Log( "Error Initializing Lua" );

	// ������������� ����������� ������������ ������� lua
	luaopen_base( m_LuaVM );
	luaopen_table( m_LuaVM );
	luaopen_string( m_LuaVM );
	luaopen_math( m_LuaVM );
	luaopen_os( m_LuaVM );

	lua_dobuffer( m_LuaVM, m_FileBuffer, FileSize );
}

bool CLua::lua_dobuffer( lua_State* Lua, void const* Buffer, int Size )
{
	return true;
// 	if ( !Size )
// 		return true;
// 
// 	if ( luaL_loadbuffer( Lua, (char const*)Buffer, Size, 0 ) )
// 	{
// 		char const* ErrorMsg = lua_tostring( Lua, -1 );
// 		lua_pop( Lua, 1 );
// 		Log( ErrorMsg );
// 		return false;
// 	}

	luaL_loadfile(Lua, "InitShader.lua");
	if ( lua_pcall( Lua, 0, LUA_MULTRET, 0 ) )
	{
		char const* ErrorMsg = lua_tostring( Lua, -1 );
		lua_pop( Lua, 1 );
		Log( ErrorMsg );
		return false;
	}

	std::vector<std::string > vec;		
	lua_getglobal( Lua, "tank" );
	
	int n = lua_gettop( Lua );//�������� ���������� ���������� ���������� �������
   
    for (int i = 1; i <= n; ++i)
    {
        if( lua_isnumber( Lua, i ) )//��������� ��� i-�� ���������            
            printf( "result[%d] = %f\n", i, lua_tonumber( Lua, i ) );//���� ��� ������������ ����� ������� ��� �� �����
        else if( lua_isstring( Lua, i ) )//�������� �� ������
            printf( "result[%d] = %s\n", i, lua_tostring( Lua, i ) );
        else if( lua_istable( Lua, i ))//���� �������
        {
			lua_pushnil( Lua );//������ �� ������� ����� NULL

            while( lua_next( Lua, i ) )
            {
                if( lua_type( Lua, -2 ) == LUA_TNUMBER )
                    printf ( "key = %f", lua_tonumber( Lua, -2 ) );
				else if( lua_type( Lua, -2 ) == LUA_TSTRING )
				{
                    //vec.push_back( lua_tostring( Lua, -2 ) );
				}

                //print value
                if( lua_type( Lua, -1 ) == LUA_TNUMBER )
                    printf( "\tvalue = %f\n", lua_tonumber( Lua, -1 ) );
				else if( lua_type( Lua, -1 ) == LUA_TSTRING )
				{
					const char* str = lua_tostring( Lua, -1 );
					if( str )
						vec.push_back( str );
				}

                //������� �������� value �� �����, ���-�� �� ��������� ��������
                //���� ������������� ��� ���������� ��������� ���� �������� �� �������
                lua_pop( Lua, 1 );
            }
        }
        else if( lua_isuserdata( Lua, i ) ) //���������������� ��� ������
        {
            void* udata = lua_touserdata( Lua, i );
            printf( "lua userdata %06", udata );
        }
    }

	lua_getglobal( Lua, "mass" );
	int mass = (int)lua_tonumber( Lua, -1 );
	lua_pop( Lua, 1 );

	lua_getglobal( Lua, "wheele" );
	if( !lua_istable( Lua,-1) )
	{
		lua_close( Lua );
		exit(0);
	}

	int tIndex = lua_gettop( Lua );	
	lua_pushnil( Lua ); // first key

	while( lua_next( Lua, tIndex ) != 0 )
	{
		//cout<<"key has type="<<lua_typename(Lua, lua_type(Lua, -2));
		vec.push_back( lua_tostring( Lua, -1 ) );
		lua_pop( Lua, 1 );
	} 

	Log( "Initial Script" );	
	return true;
}

CLua::~CLua()
{
	free( m_FileBuffer );
	lua_close( m_LuaVM );
}

bool CLua::LoadParamTank( const std::string& srPathScript, CParamTank** pParamTank )
{
	bool bResult = false;

	if( pParamTank || srPathScript.empty() )
	{
		if( lua_State* pLua = lua_open() )
		{
			// ������������� ����������� ������������ ������� lua
			luaopen_base( pLua );
			luaopen_table( pLua );
			luaopen_string( pLua );
			luaopen_math( pLua );
			luaopen_os( pLua );

			luaL_loadfile( pLua, srPathScript.c_str() );

			if( !lua_pcall( pLua, 0, LUA_MULTRET, 0 ) )
			{

			}
			else
			{
				char const* ErrorMsg = lua_tostring( pLua, -1 );
				lua_pop( pLua, 1 );
				Log( ErrorMsg );				
			}

			lua_close( pLua );
		}
		else
			Log( "Error Initializing Lua" );
	}

	return bResult;
}