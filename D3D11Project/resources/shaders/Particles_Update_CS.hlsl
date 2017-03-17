// Particle.
struct Particle
{
    float4 position;
    float4 velocity;
    float4 color;
    float4 pad;
};
// Input particles.
StructuredBuffer<Particle> g_InputParticles : register(t0);

// Output particles.
RWStructuredBuffer<Particle> g_OutputParticles : register(u0);

[numthreads(1, 1, 1)]
void main(uint tID : SV_DispatchThreadID)
{
    g_OutputParticles[tID] = g_InputParticles[tID];
}
