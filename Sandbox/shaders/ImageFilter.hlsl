// ImageFilter.hlsl : Image filtering done on the GPU

Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Output : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float width;
	float height;
	Input.GetDimensions(width, height);

	if (DTid.x >= width || DTid.y >= height)
		return;

	uint2 pixelPos = uint2(DTid.x, DTid.y);

	float4 sepiaColor;
	sepiaColor.x = 0.393f * Input[pixelPos].x + 0.769f * Input[pixelPos].y + 0.189f * Input[pixelPos].z;
	sepiaColor.y = 0.349f * Input[pixelPos].x + 0.686f * Input[pixelPos].y + 0.168f * Input[pixelPos].z;
	sepiaColor.z = 0.372f * Input[pixelPos].x + 0.534f * Input[pixelPos].y + 0.131f * Input[pixelPos].z;
	sepiaColor.w = 1.0f;

	Output[pixelPos] = sepiaColor;
}