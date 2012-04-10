#pragma once

class CMesh3D;

enum Weapon { M16, AK47 };

class CWeapon
{
protected:
	unsigned short m_AmountBullet;    // ���������� ����
	unsigned short m_ChargerBullet;   // ������� � ������
	unsigned short m_Damage;		  // ���� �� ����
  	Weapon         m_NameWeapon;  
public:
 	virtual Weapon GetWeaponType() = 0; 
	virtual void   Recharge() = 0;
	CWeapon(): m_AmountBullet(0), m_ChargerBullet(0), m_Damage(0)
	{	}
	virtual ~CWeapon();

};

class CAutomatic_M16 : public CWeapon
{
private:
	CMesh3D       m_Mesh;
public:
	CAutomatic_M16();

};

class CAutomatic_AK47 : public CWeapon
{
private:
	CMesh3D       m_Mesh;
public:
	CAutomatic_AK47();

};