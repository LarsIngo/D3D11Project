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
    float3 color : COLOR;
};

[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> TriStream)
{
    GSOutput output;

    float3 inPosition = input[0].position.xyz;
    float4 inColor = input[0].color;

    for (uint i = 0; i < 4; ++i)
    {
        float x = i == 1 || i == 3;
        float y = i == 0 || i == 1;

        output.position = float4(inPosition.x + x, inPosition.y + y, 0.f, 1.f);
        output.color = inColor;

        TriStream.Append(output);
    }
}
