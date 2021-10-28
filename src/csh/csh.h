#ifndef CSH_H
#define CSH_H

#include <string>
#include <d3d11.h>

void CSH_Init();
void CSH_Shutdown();

class CSH_Buffer
{
public:
    CSH_Buffer(uint32_t elementSize, uint32_t count, void* init_data);
    ~CSH_Buffer();

    void BuildSRV();
    void BuildUAV();

    void BindSRV(int slot);
    void BindUAV(int slot);

    void UnbindSRV(int slot);
    void UnbindUAV(int slot);

    void* GetData();
private:
    ID3D11Buffer* mBuffer = nullptr;
    ID3D11ShaderResourceView* mShaderResourceView = nullptr;
    ID3D11UnorderedAccessView* mShaderUnorderedAccessView = nullptr;
    D3D11_MAPPED_SUBRESOURCE mMappedSubresource;
};

class CSH_ImageData
{
public:
    CSH_ImageData(int width, int height);
    CSH_ImageData(const std::string& path);
    ~CSH_ImageData();

    void BuildSRV();
    void BuildUAV();

    void BindSRV(int slot);
    void BindUAV(int slot);

    void UnbindSRV(int slot);
    void UnbindUAV(int slot);

    void OutputToImage(const std::string& path);

    int Width;
    int Height;
    int NumChannels;
    uint8_t* Data = nullptr;
private:
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mShaderResourceView = nullptr;
    ID3D11UnorderedAccessView* mUnorderedAccessView = nullptr;
    D3D11_MAPPED_SUBRESOURCE mMappedSubresource;
};

class CSH_ComputeShader
{
public:
    CSH_ComputeShader(const std::wstring& shaderPath);
    ~CSH_ComputeShader();

    void Bind();
    void Unbind();

    void Dispatch(int threadGroupCountX, int threadGroupCountY, int threadGroupCountZ);
private:
    ID3D10Blob* mShaderBlob = nullptr;
    ID3D11ComputeShader* mShader = nullptr;
};

#endif