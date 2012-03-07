#include "CameraDevice.h"

CameraDevice::CameraDevice()
{
	positionCamera	= D3DXVECTOR3( 0, 0, -80 );
	CameraUp		= D3DXVECTOR3( 0, 1, 0 );
	TargetDir		= D3DXVECTOR3( 0, 0, 1 );
	DirX		    = D3DXVECTOR3( 1, 0, 0 );
	DirY		    = D3DXVECTOR3( 0, 1, 0 );
	Point		    = D3DXVECTOR3( 0, 0, 0 );
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
	D3DXMatrixPerspectiveFovLH(&m_Proj, D3DX_PI / 4, (FLOAT)Width / Height, 1.0f, 300.0f);
	StepCamera  = 0.5f;
	AngleCamera = 1.0f * D3DX_PI / 180;
	Sensivity   = 0.8f;
}

CameraDevice::~CameraDevice()
{
}

void CameraDevice::Refresh()
{
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void CameraDevice::MoveForv()
{
	positionCamera += TargetDir * StepCamera;
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void CameraDevice::MoveBack()
{
	positionCamera += TargetDir * -StepCamera;
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void CameraDevice::MoveRight()
{
	D3DXVECTOR3 T;
	D3DXVec3Cross(&T, &TargetDir, &CameraUp);
	positionCamera -= T * StepCamera;
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void CameraDevice::MoveLeft()
{
	D3DXVECTOR3 T;
	D3DXVec3Cross(&T, &TargetDir, &CameraUp);
	positionCamera += T * StepCamera;
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void  CameraDevice::MouseRotateLeft()
{
	D3DXMATRIX Rotate;
	D3DXMatrixRotationY( &Rotate, -AngleCamera / Sensivity);
	D3DXVECTOR4 T;
	D3DXVec3Transform( &T, &TargetDir, &Rotate );
	TargetDir = D3DXVECTOR3( T.x, T.y, T.z );
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void  CameraDevice::MouseRotateRight()
{
	D3DXMATRIX Rotate;
	D3DXMatrixRotationY( &Rotate, AngleCamera / Sensivity);
	D3DXVECTOR4 T;
	D3DXVec3Transform( &T, &TargetDir, &Rotate );
	TargetDir = D3DXVECTOR3( T.x, T.y, T.z );
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}
void  CameraDevice::MouseRotateUp()
{
	D3DXVec3Normalize( &TargetDir, &TargetDir );
	D3DXVec3Normalize( &CameraUp, &CameraUp );
	D3DXVec3Cross(&DirX, &TargetDir, &CameraUp);
	D3DXVec3Normalize( &DirX, &DirX );
	D3DXMATRIX Rotate;
	D3DXMatrixRotationAxis( &Rotate, &DirX, -AngleCamera / Sensivity );
	D3DXVECTOR4 T;
	D3DXVec3Transform( &T, &TargetDir, &Rotate );
	TargetDir = D3DXVECTOR3( T.x, T.y, T.z );
	DirX = D3DXVECTOR3( DirX.x, 0, DirX.z );
	D3DXVec3Cross(&CameraUp, &DirX, &TargetDir);
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void  CameraDevice::MouseRotateDown()
{

	D3DXVec3Normalize( &TargetDir, &TargetDir );
	D3DXVec3Normalize( &CameraUp, &CameraUp );
	D3DXVec3Cross(&DirX, &TargetDir, &CameraUp);
	D3DXVec3Normalize( &DirX, &DirX );
	D3DXMATRIX Rotate;
	D3DXMatrixRotationAxis( &Rotate, &DirX, AngleCamera / Sensivity );
	D3DXVECTOR4 T;
	D3DXVec3Transform( &T, &TargetDir, &Rotate );
	TargetDir = D3DXVECTOR3( T.x, T.y, T.z );
	DirX = D3DXVECTOR3( DirX.x, 0, DirX.z );
	D3DXVec3Cross(&CameraUp, &DirX, &TargetDir);
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}

void CameraDevice::RefreshMirror()
{
	D3DXMatrixLookAtLH( &m_View, &positionCamera, &Point, &CameraUp );
}