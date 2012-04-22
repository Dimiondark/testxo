#include "Sound.h"

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

	pDevice = 0;
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

void CSound::Play()
{	
	char		*filename;
	ALuint		source;
	ALfloat		mPos[3] = {-10.0, 0.0, 0.0};
	ALfloat		mVel[3] = {0.0, 0.0, 0.0};

	unsigned int buffer;
	ALvoid		*p;
	ALsizei		size;
	ALsizei		rate; 
	ALenum		format;
	ALboolean	loop;

	filename = "galil.wav";	

	// ������������� ��������� ���������
	// �������	
	alListenerfv( AL_POSITION,	  ListenerPos);
	// ��������
	alListenerfv( AL_VELOCITY,    ListenerVel);
	// ����������
	alListenerfv( AL_ORIENTATION, ListenerOri);

	alGenSources (1, &source);
	/* ��� �����; �������� � ����������� */
	alSourcef  (source, AL_PITCH, 1.0f);
	alSourcef  (source, AL_GAIN, 1.0f);
	alSourcefv (source, AL_POSITION, mPos);
	alSourcefv (source, AL_VELOCITY, mVel);
	/* �������� ��? */
	alSourcef (source, AL_LOOPING, AL_FALSE);

	/* ��������� ������ */
	alGenBuffers (1, &buffer);
	alutLoadWAVFile ((ALbyte *) filename, &format, &p, &size, &rate, &loop);
	alBufferData (buffer, format, p, size, rate);
	alutUnloadWAV ( format, p, size, rate );
	alSourcei( source, AL_BUFFER, buffer );
	
	alSourcePlay (source);
	ALint status;
	do
	{
		alutSleep (0.1f);
		alGetSourcei (source, AL_SOURCE_STATE, &status);
	} 
	while (status == AL_PLAYING);

	alSourceStop (source);

	alDeleteSources (1, &source);
	alDeleteBuffers (1, &buffer);
}

CSound::~CSound()
{
	// ��������� ������� ��������
	alcMakeContextCurrent(0);
	// ���������� ��������
	alcDestroyContext(pContext);
	// ��������� �������� ����������
	alcCloseDevice(pDevice);
}








bool remSnd::Open(const std::string &Filename, bool Looped, bool Streamed)
{
	// ��������� ���� �� �������
	//ifstream a(Filename.c_str());
// 	if (!a.is_open()) 
// 		return false;
// 	a.close();

	mLooped    = Looped;

	// ������� �������� ��������������� ������ �����
	alGenSources(1, &mSourceID);

	alSourcef (mSourceID, AL_PITCH,    1.0f);
	alSourcef (mSourceID, AL_GAIN,    1.0f);
	alSourcefv(mSourceID, AL_POSITION,  mPos);
	alSourcefv(mSourceID, AL_VELOCITY,  mVel);
	alSourcei (mSourceID, AL_LOOPING,  mLooped);

	// Extract ext
// 	std::string Ext = ExtractFileExt(Filename);
// 	if (Ext == "WAV") 
		return LoadWavFile(Filename);
	/*if (Ext == "OGG") 
	{
	mStreamed = Streamed;
	return LoadOggFile(Filename, Streamed);
	} */
	return false;
}

void remSnd::Play()
{
	alSourcePlay(mSourceID);
}

void remSnd::Close()
{
	alSourceStop(mSourceID);
	if (alIsSource(mSourceID)) alDeleteSources(1, &mSourceID);
}

void remSnd::Stop()
{
	alSourceStop(mSourceID);
}

void remSnd::Move(float X, float Y, float Z)
{
	ALfloat Pos[3] = { X, Y, Z };
	alSourcefv(mSourceID, AL_POSITION, Pos);
}

bool remSnd::LoadWavFile(const std::string &Filename)
{/*
	// ��������� ���������� ��������������
	SndInfo    buffer;
	// ������ ������ � ������
	ALenum    format;
	// ��������� �� ������ ������ �����
	ALvoid    *data;
	// ������ ����� �������
	ALsizei    size;
	// ������� ����� � ������
	ALsizei    freq;
	// ������������� ������������ ���������������
	ALboolean  loop;
	// ������������� ������
	ALuint    BufID = 0;

	// ��������� SndInfo �������
	buffer.Filename = Filename;
	// ����, � ��� �� ��� ������������� ������ � ������ ������?
	for (TBuf::iterator i = Buffers.begin(); i != Buffers.end(); i++)
	{
		if (i->second.Filename == Filename) BufID = i->first;
	}

	// ���� ���� ��������� �������
	if (!BufID)
	{
		// ������ �����
		alGenBuffers(1, &buffer.ID);
		if (!CheckALError()) return false;
		// ��������� ������ �� wav �����
		alutLoadWAVFile((ALbyte *)Filename.data(), &format, &data,
			&size, &freq, &loop);
		if (!CheckALError()) return false;

		buffer.Format      = format;
		buffer.Rate      = freq;
		// ��������� ����� �������
		alBufferData(buffer.ID, format, data, size, freq);
		if (!CheckALError()) return false;
		// ��������� ���� �� �������������
		alutUnloadWAV(format, data, size, freq);
		if (!CheckALError()) return false;

		// ��������� ���� ����� � ������
		Buffers[buffer.ID] = buffer;
	}
	else 
		buffer = Buffers[BufID];

	// ����������� ����� � ����������
	alSourcei (mSourceID, AL_BUFFER, buffer.ID);
*/
	return true;
}


