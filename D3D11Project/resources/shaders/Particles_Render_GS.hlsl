// Input.
struct GSInput
{
    float4 position : SV_POSITION;
    float4 velocity : VELOCITY;
    float4 color : COLOR;
    float4 pad : PAD;
};

// Output.
struct GSOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLDPOSITION;
    float3 color : COLOR;
    float2 uv : UV;
};

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> TriStream)
{

    GSOutput output;

    //// ----- Generate billboarded particle ----- //
    float3 worldPosition = input[0].position.xyz;
    float2 scale = input[0].velocity.xy; //TMP
    float3 color = input[0].color.xyz;

    float3 particleFrontDirection = float3(0, 0, -1);
    float3 paticleSideDirection = float3(-1, 0, 0);
    float3 paticleUpDirection = float3(0, 1, 0);

    for (uint i = 0; i < 4; ++i)
    {
        float x = i == 1 || i == 3;
        float y = i == 0 || i == 1;
        output.position.xyz = worldPosition + paticleSideDirection * (x * 2.f - 1.f) * scale.x + paticleUpDirection * (y * 2.f - 1.f) * scale.y;
        output.position.w = input[0].position.w;
        output.uv.x = x;
        output.uv.y = 1.f - y;
        output.worldPosition = output.position.xyz;
        output.position = output.position;
        output.color = color;

        TriStream.Append(output);
    }
}
