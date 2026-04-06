// Basic.hlsl
// 버텍스 셰이더: 월드/뷰/프로젝션 변환
// 픽셀 셰이더 : 단색 출력

cbuffer CBMatrix : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

cbuffer CBColor : register(b1)
{
    float4 Color;
};

// ── 버텍스 셰이더 ─────────────────────────────────────────────────────
struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};

VSOutput VS(VSInput input)
{
    VSOutput output;
    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.pos = pos;
    return output;
}

// ── 픽셀 셰이더 ──────────────────────────────────────────────────────
float4 PS(VSOutput input) : SV_TARGET
{
    return Color;
}
