#pragma once
#include "Mesh.h"
#include "CameraDevice.h"

enum Weapon { M16, AK47, MaxWeapon };

class CWeapon
{
protected:
	unsigned short m_AmountBullet;    // ���������� ����
	unsigned short m_ChargerBullet;   // ������� � ������
	unsigned short m_Damage;		  // ���� �� ����
  	Weapon         m_NameWeapon;
	CMesh3D        m_Mesh;
public:
 	virtual Weapon GetWeaponType() = 0;
	virtual void   Recharge()      = 0;
	virtual void   RenderWeapon( CameraDevice const& Camera, const D3DXMATRIX&  MatrixWorld, CShader const& Shader ) = 0;
	
	//virtual ~CWeapon();

};

class CAutomatic_M16 : public CWeapon
{
public:
	CAutomatic_M16( LPCSTR Name, IDirect3DDevice9* pD3DDevice );
   ~CAutomatic_M16();
    void   RenderWeapon( CameraDevice const& Camera, const D3DXMATRIX&  MatrixWorld, CShader const& Shader );
	void   Recharge();
	Weapon GetWeaponType()
	{
		return 	m_NameWeapon; 
	}

};

class CAutomatic_AK47 : public CWeapon
{
public:
	CAutomatic_AK47( LPCSTR Name, IDirect3DDevice9* pD3DDevice );
   ~CAutomatic_AK47();
    void   RenderWeapon( CameraDevice const& Camera, const D3DXMATRIX&  MatrixWorld, CShader const& Shader );
	void   Recharge();
	Weapon GetWeaponType()
	{
		return 	m_NameWeapon; 
	}

};