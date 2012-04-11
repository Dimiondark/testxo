float4x4 mat_mvp; 

// �������� ������.
struct VS_INPUT_STRUCT
{
  float4 position: POSITION;  
  float3 uv      : TEXCOORD0;
};
// ��������� ������.
struct VS_OUTPUT_STRUCT
{
  float4 position: POSITION;
  float3 uv      : TEXCOORD0;  
};

VS_OUTPUT_STRUCT main (VS_INPUT_STRUCT In_struct)
{
	VS_OUTPUT_STRUCT Out_struct;

	//��������� ������� �������.
	Out_struct.position = mul( In_struct.position, mat_mvp);
	Out_struct.uv = In_struct.uv;
return Out_struct;
}