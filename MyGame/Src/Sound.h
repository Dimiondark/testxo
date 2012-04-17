#pragma once

#include "Init.h"
#include <al.h>
#include <alc.h>

class CSound
{
private:
	ALCdevice*		pDevice;
	ALCcontext*		pContext;
public:
	bool InitializeOpenAL();
	void DestroyOpenAL();
	ALboolean CheckALCError();
	ALboolean CheckALError();
};

class remSnd  
{
public:
	ALfloat mVel[3];
	ALfloat mPos[3];
	bool  mLooped;

	// Functions
	bool Open(const std::string &Filename, bool Looped, bool Streamed);
	bool IsStreamed();
	void Play();
	void Close();
	void Update();
	void Move(float X, float Y, float Z);
	void Stop();

	// Construction/destruction
	remSnd();
	virtual ~remSnd();

private:
	// ������������� ���������
	ALuint      mSourceID;
	// ��������� �� ��� ����?
	bool      mStreamed;

	bool LoadWavFile (const std::string &Filename);
};

typedef struct 
{
	unsigned int  ID;
	std::string   Filename;
	unsigned int  Rate;
	unsigned int  Format;
} SndInfo;

//map<ALuint, SndInfo> Buffers;