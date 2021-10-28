#include "csh.h"

#include <d3dcompiler.h>
#include <cstdint>
#include <cassert>
#include <iostream>

struct RendererData
{
    ID3D11Device* device;
    ID3D11DeviceContext* ctx;
    D3D_FEATURE_LEVEL feature_level;
};

static RendererData rhi;

void CSH_Init()
{
    // Create D3D11 Device
    D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	uint32_t driverSize = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	uint32_t levelSize = ARRAYSIZE(levels);
	HRESULT success = 0;

    for (int i = 0; i < driverSize;)
	{
		success = D3D11CreateDevice(NULL, driverTypes[i], NULL, D3D11_CREATE_DEVICE_DEBUG, levels, levelSize, D3D11_SDK_VERSION, &rhi.device, &rhi.feature_level, &rhi.ctx);

		if (SUCCEEDED(success))
			break;
		++i;
	}

    std::cout << "[CSH] D3D11 device successfully created" << std::endl;
}

void CSH_Shutdown()
{
    rhi.ctx->Release();
    rhi.device->Release();
    std::cout << "[CSH] Shutdown" << std::endl;
}

// CSH_Buffer //

CSH_Buffer::CSH_Buffer(uint32_t elementSize, uint32_t count, void* init_data)
{
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = elementSize * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	if (init_data)
	{
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = init_data;
		HRESULT res = rhi.device->CreateBuffer(&desc, &subresource_data, &mBuffer);
		assert(SUCCEEDED(res));
	}
	else
	{
		HRESULT res = rhi.device->CreateBuffer(&desc, nullptr, &mBuffer);
		assert(SUCCEEDED(res));
	}
}

CSH_Buffer::~CSH_Buffer()
{
	if (mShaderResourceView) mShaderResourceView->Release();
	if (mShaderUnorderedAccessView) mShaderUnorderedAccessView->Release();
	mBuffer->Release();
}

void CSH_Buffer::BuildSRV()
{
	D3D11_BUFFER_DESC descBuf;
	mBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
    } 
	else if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
    } 
	else
    {
		std::cout << "[CSH] Invalid argument when creating SRV" << std::endl;
    }

	HRESULT res = rhi.device->CreateShaderResourceView(mBuffer, &desc, &mShaderResourceView);
	assert(SUCCEEDED(res));
}

void CSH_Buffer::BuildUAV()
{
	D3D11_BUFFER_DESC descBuf = {};
    mBuffer->GetDesc( &descBuf );
        
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;

    if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
    } 
	else if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
    } 
	else
    {
        std::cout << "[CSH] Invalid argument when creating UAV" << std::endl;
    }

	HRESULT res = rhi.device->CreateUnorderedAccessView(mBuffer, &desc, &mShaderUnorderedAccessView);
	assert(SUCCEEDED(res));
}

void CSH_Buffer::BindSRV(int slot)
{
	rhi.ctx->CSSetShaderResources(slot, 1, &mShaderResourceView);
}

void CSH_Buffer::BindUAV(int slot)
{
	rhi.ctx->CSSetUnorderedAccessViews(slot, 1, &mShaderUnorderedAccessView, nullptr);
}

void CSH_Buffer::UnbindSRV(int slot)
{
	ID3D11ShaderResourceView* nullSrv = nullptr;
	rhi.ctx->CSSetShaderResources(slot, 1, &nullSrv);
}

void CSH_Buffer::UnbindUAV(int slot)
{
	ID3D11UnorderedAccessView* nullUav = nullptr;
	rhi.ctx->CSSetUnorderedAccessViews(slot, 1, &nullUav, nullptr);
}

void* CSH_Buffer::GetData()
{
	ID3D11Buffer* outputBuffer;

	D3D11_BUFFER_DESC desc = {};
	mBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

	if (SUCCEEDED(rhi.device->CreateBuffer(&desc, nullptr, &outputBuffer)))
	{
		rhi.ctx->CopyResource(outputBuffer, mBuffer);
	}

	void* data;
	rhi.ctx->Map(outputBuffer, 0, D3D11_MAP_READ, 0, &mMappedSubresource);
	data = mMappedSubresource.pData;

	outputBuffer->Release();

	return data;
}

// CSH_ComputeShader //

CSH_ComputeShader::CSH_ComputeShader(const std::wstring& path)
{
	ID3DBlob* errorBlob;
	HRESULT res = D3DCompileFromFile(path.c_str(), 
									nullptr, 
									((ID3DInclude*)(UINT_PTR)1), 
									"main", 
									"cs_5_0", 
									D3DCOMPILE_ENABLE_STRICTNESS, 
									0, 
									&mShaderBlob, 
									&errorBlob);
	assert(SUCCEEDED(res));
	if (errorBlob)
		std::cout << (char*)errorBlob->GetBufferPointer() << std::endl;

	rhi.device->CreateComputeShader(mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), nullptr, &mShader);
}

CSH_ComputeShader::~CSH_ComputeShader()
{
	mShader->Release();
	mShaderBlob->Release();
}

void CSH_ComputeShader::Bind()
{
	rhi.ctx->CSSetShader(mShader, nullptr, 0);
}

void CSH_ComputeShader::Unbind()
{
	rhi.ctx->CSSetShader(nullptr, nullptr, 0);
}

void CSH_ComputeShader::Dispatch(int x, int y, int z)
{
	rhi.ctx->Dispatch(x, y, z);
}