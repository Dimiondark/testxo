#include "Sound.h"
#include <string>

ALboolean CSound::CheckALCError()
{
	ALenum ErrCode;
	std::string Err = "ALC error: ";
	if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
	{
		Err += (char *)alcGetString(pDevice, ErrCode);
		//ERRMSG(Err.data());
		return AL_FALSE;
	}
	return AL_TRUE;
}

ALboolean CSound::CheckALError()
{
	ALenum ErrCode;
	std::string Err = "OpenAL error: ";
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += (char *)alGetString(ErrCode);
		//ERRMSG(Err.data());
		return AL_FALSE;
	}
	return AL_TRUE;
}

CSound::CSound()
{
	// ������� ���������.
	ListenerPos[0] = 0.0;
	ListenerPos[1] = 0.0;
	ListenerPos[2] = 0.0;

	// �������� ���������.
	ListenerVel[0] = 0.0;
	ListenerVel[1] = 0.0;
	ListenerVel[2] = 0.0;

	// ���������� ���������. (������ 3 �������� � ����������� ���, ��������� 3 � �������)
	ListenerOri[0] =  0.0; 
	ListenerOri[1] =  0.0;
	ListenerOri[2] = -1.0;
	ListenerOri[3] =  0.0;
	ListenerOri[4] =  1.0;
	ListenerOri[5] =  0.0;

	pContext = alcGetCurrentContext();
	//pDevice = alcGetContextsDevice( pContext );
	if ( !pContext )
	{
		// ��������� �������� �� ��������� ����������
		pDevice = alcOpenDevice( 0 );

		if ( pDevice )
		{
			Log("Open sound device");
			pContext = 0;
			pContext = alcCreateContext( pDevice, 0 );
			if ( pContext )
				alcMakeContextCurrent( pContext );	// ������ �������� �������
			else
			{
				Log("Default sound contex not present");
				alcCloseDevice( pDevice );
				return;
			}
		}	
		else
		{
			Log("Default sound device not present");
			return;
		}
	}
	else 
		pDevice = alcGetContextsDevice( pContext );

	// ������������� ��������� ���������
	// �������	
	alListenerfv( AL_POSITION,	  ListenerPos);
	// ��������
	alListenerfv( AL_VELOCITY,    ListenerVel);
	// ����������
	alListenerfv( AL_ORIENTATION, ListenerOri);
	char		*filename_shot;
	char		*filename_Recharge;
	char		*filename_Fon;
	ALvoid		*p;
	ALsizei		size;
	ALsizei		rate; 
	ALenum		format;
	ALboolean	loop;

	filename_shot = "galil.wav";
	filename_Recharge = "ak47_clipout.wav";
	filename_Fon = "Gladiator.wav";


	/* ��������� ������ */
	alGenBuffers( 3, buffer );
	alGenSources( 3, source ); // ������ ������ ���������� ��������

	alutLoadWAVFile( (ALbyte *) filename_shot, &format, &p, &size, &rate, &loop );
	alBufferData( buffer[0], format, p, size, rate );
	alutUnloadWAV( format, p, size, rate );

	alutLoadWAVFile( (ALbyte *) filename_Recharge, &format, &p, &size, &rate, &loop );
	alBufferData( buffer[1], format, p, size, rate );
	alutUnloadWAV( format, p, size, rate );

	alutLoadWAVFile( (ALbyte *) filename_Fon, &format, &p, &size, &rate, &loop );
	alBufferData( buffer[2], format, p, size, rate );
	alutUnloadWAV( format, p, size, rate );


	Play(2);
}

void CSound::Play(int i)
{		
	ALfloat		mPos[3] = {0.0, 0.0, 0.0};
	ALfloat		mVel[3] = {0.0, 0.0, 0.0};

	
	/* ��� �����; �������� � ����������� */
	alSourcef  (source[i], AL_PITCH, 1.0f);
	alSourcef  (source[i], AL_GAIN, 1.0f);
	alSourcefv (source[i], AL_POSITION, mPos);
	alSourcefv (source[i], AL_VELOCITY, mVel);
	/* �������� ��? */
	alSourcef (source[i], AL_LOOPING, AL_FALSE);
	alSourcei( source[i], AL_BUFFER, buffer[i] );
	alSourcePlay( source[i] );

// 	ALint status;
// 	do
// 	{
// 		alutSleep (0.1f);
// 		alGetSourcei (source, AL_SOURCE_STATE, &status);
// 	} 
// 	while (status == AL_PLAYING);
// 
// 	alSourceStop (source);
// 

}

CSound::~CSound()
{
	alDeleteSources( 3, source );
	alDeleteBuffers( 3, buffer );
	// ��������� ������� ��������
	alcMakeContextCurrent(0);
	// ���������� ��������
	alcDestroyContext( pContext );
	// ��������� �������� ����������
	alcCloseDevice( pDevice );
}






