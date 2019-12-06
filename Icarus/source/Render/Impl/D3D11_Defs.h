/* D3D11_Defs.h
Author: Jeff Kiah
Orig.Date: 6/01/12
Description: use as precompiled header for Renderer_D3D11 classes
*/
#pragma once

#include <comdef.h>
#include <d3d11.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "FW1FontWrapper_1_1/FW1FontWrapper.h"

#define COM_PTR(x)		_COM_SMARTPTR_TYPEDEF(x, __uuidof(x))

using std::wstring;
using std::string;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

COM_PTR(ID3D11Device);
COM_PTR(ID3D11DeviceContext);
COM_PTR(IDXGISwapChain);
COM_PTR(ID3D11InputLayout);

COM_PTR(ID3D11Buffer);

COM_PTR(ID3D11VertexShader);
COM_PTR(ID3D11PixelShader);
COM_PTR(ID3D11GeometryShader);
COM_PTR(ID3D11HullShader);
COM_PTR(ID3D11DomainShader);
COM_PTR(ID3D11ComputeShader);

COM_PTR(ID3D11Texture1D);
COM_PTR(ID3D11Texture2D);
COM_PTR(ID3D11Texture3D);

COM_PTR(ID3D11RenderTargetView);
COM_PTR(ID3D11ShaderResourceView);
COM_PTR(ID3DBlob);

COM_PTR(IFW1Factory);
COM_PTR(IFW1FontWrapper);
