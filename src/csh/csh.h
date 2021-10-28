#ifndef CSH_H
#define CSH_H

#include <string>
#include <d3d11.h>

// Initialised CSH
void CSH_Init();
// Shuts down CSH
void CSH_Shutdown();

// Class representing an input or output buffer
class CSH_Buffer
{
public:
    // Element size is the size of the struct, count is the number of elements in the buffer, and init_data is the element bufffer
    CSH_Buffer(uint32_t elementSize, uint32_t count, void* init_data);
    ~CSH_Buffer();

    // SRV (Shader Resource View) is for input buffers
    void BuildSRV();
    // UAV (Unordered Access View) is for output buffers
    void BuildUAV();

    void BindSRV(int slot);
    void BindUAV(int slot);

    void UnbindSRV(int slot);
    void UnbindUAV(int slot);

    // Get the data (only for output buffers)
    void* GetData();
private:
    ID3D11Buffer* mBuffer = nullptr;
    ID3D11ShaderResourceView* mShaderResourceView = nullptr;
    ID3D11UnorderedAccessView* mShaderUnorderedAccessView = nullptr;
    D3D11_MAPPED_SUBRESOURCE mMappedSubresource;
};

// Class representing a texture/image
class CSH_ImageData
{
public:
    // Width and height constructor for output images
    CSH_ImageData(int width, int height);
    // Path constructor for input images
    CSH_ImageData(const std::string& path);
    ~CSH_ImageData();

    void BuildSRV();
    void BuildUAV();

    void BindSRV(int slot);
    void BindUAV(int slot);

    void UnbindSRV(int slot);
    void UnbindUAV(int slot);

    // Write the output of a compute shader to an image on disk (output images only)
    void OutputToImage(const std::string& path);

    int Width;
    int Height;
    // NumChannels is always 4
    int NumChannels;
    uint8_t* Data = nullptr;
private:
    ID3D11Texture2D* mTexture = nullptr;
    ID3D11ShaderResourceView* mShaderResourceView = nullptr;
    ID3D11UnorderedAccessView* mUnorderedAccessView = nullptr;
    D3D11_MAPPED_SUBRESOURCE mMappedSubresource;
};

// Class representing a compute shader
class CSH_ComputeShader
{
public:
    // wstring requires an L before writing the string (example: L"Hello")
    CSH_ComputeShader(const std::wstring& shaderPath);
    ~CSH_ComputeShader();

    void Bind();
    void Unbind();

    // the 3 parameters should be divided by the arguments of "numthreads" in the compute shader
    void Dispatch(int threadGroupCountX, int threadGroupCountY, int threadGroupCountZ);
private:
    ID3D10Blob* mShaderBlob = nullptr;
    ID3D11ComputeShader* mShader = nullptr;
};

#endif