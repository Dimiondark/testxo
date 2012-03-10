float4x4 mat_mvp;  //������������ �������, ������� � ������������ ������.
float4x4 mat_view; //������� �������.
float4   scale;
// �������� ������.
struct VS_INPUT_STRUCT
{
  float4 position: POSITION;
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
  Out_struct.position = In_struct.position;

  Out_struct.uv = mul( (float3x3)mat_view,float3( In_struct.position.xy * scale, 1 ) );
  return Out_struct;
}