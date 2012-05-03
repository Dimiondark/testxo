#pragma once
#include "Mesh.h"
#include "Init.h"
#include "Sound.h"

enum                 Weapon          {  M16,   AK47,   MaxWeapon };
std::string  const   WeaponNames[] = { "M16", "AK47" };


class CWeapon
{
private:
	unsigned int  		 m_AmountBullet;       // ����� ���������� ����
	unsigned int  		 m_ChargerBullet;      // ������� � ������
	unsigned int         m_MaxChargerBullet;   // ����������� ������� � ������
	unsigned int 		 m_Damage;		       // ���� �� ����
	DWORD                m_RateOfFire;		   // ����� ����� ������ � ������������	
	DWORD                m_LastTimeFire;
  	Weapon         		 m_NameWeapon;
	CMesh3D        		 m_Mesh;
	bool                 m_Fire;
	D3DXMATRIX     		 m_MatrixWorld;
	IDirect3DDevice9*    m_pD3DDevice;
	CSound               m_Sound;
	//CameraDevice         m_Camera;
public:
	CWeapon( std::string NameWeapon, IDirect3DDevice9* pD3DDevice );
	char*  ReadIniFile( const char *filename, const char *section, const char *key );
 	Weapon GetWeaponType()	    {	return m_NameWeapon; 	};
	void   SetEndFire();
	void   Fire();
	bool   GetFire()			{	return m_Fire;			};
	void   Recharge();	// �����������
	void   RenderWeapon( CameraDevice const& Camera, CShader const& Shader );
	int    GetChargerBullet()	{	return m_ChargerBullet;	};
	int    GetAmountBullet()	{	return m_AmountBullet;	};
	bool   Hit( HWND hwnd, ID3DXMesh* pMesh, CameraDevice const& Camera, D3DXVECTOR3& Point, const CSphere& sphera );
   ~CWeapon();
	RECT   Size;
};
