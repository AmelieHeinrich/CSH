// Add.hlsl, just a basic compute program that add numbers

struct Addition
{
    int value;
};

StructuredBuffer<Addition> Buffer0 : register(t0);
StructuredBuffer<Addition> Buffer1 : register(t1);
RWStructuredBuffer<Addition> BufferOut : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    BufferOut[DTid.x].value = Buffer0[DTid.x].value + Buffer1[DTid.x].value;
}