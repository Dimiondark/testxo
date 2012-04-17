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

bool CSound::InitializeOpenAL()
{
	// ������� ���������.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

	// �������� ���������.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

	// ���������� ���������. (������ 3 �������� � ����������� ���, ��������� 3 � �������)
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
	pDevice = 0;
	// ��������� �������� �� ��������� ����������
	pDevice = alcOpenDevice( 0 );
	// �������� �� ������
	if ( !pDevice )
	{
		Log("Default sound device not present");
		return false;
	}
	pContext = 0;
	// ������� �������� ����������
	pContext = alcCreateContext( pDevice, 0 );
	if ( !pContext ) 
	{
		Log("Default sound contex not present");
		return false;
	}
	// ������ �������� �������
	alcMakeContextCurrent( pContext );

	// ������������� ��������� ���������
	// �������
	alListenerfv(AL_POSITION,    ListenerPos);
	// ��������
	alListenerfv(AL_VELOCITY,    ListenerVel);
	// ����������
	alListenerfv(AL_ORIENTATION, ListenerOri);
	return true;
}

void CSound::DestroyOpenAL()
{
	// ������� ��� �������
// 	for (TBuf::iterator i = Buffers.begin(); i != Buffers.end(); i++)
// 		alDeleteBuffers(1, &i->second.ID);
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
// 	if (!CheckALError()) 
// 		return false;

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