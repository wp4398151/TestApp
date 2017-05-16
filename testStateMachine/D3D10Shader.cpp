/********************************************************************************
 Copyright (C) 2012 Hugh Bailey <obs.jim@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/

#include "stdafx.h"
#include "D3D10System.h"

void D3D10Shader::LoadDefaults()
{
    for (UINT i=0; i<m_Params.Num(); i++)
    {
        ShaderParam &param = m_Params[i];

        if (param.defaultValue.Num())
        {
            param.bChanged = TRUE;
            param.curValue.CopyList(param.defaultValue);
        }
    }
}

bool D3D10Shader::ProcessData(ShaderProcessor &processor, CTSTR lpFileName)
{
    m_Params.TransferFrom(processor.Params);
    m_Samplers.TransferFrom(processor.Samplers);

    m_constantSize = 0;
    for (UINT i=0; i<m_Params.Num(); i++)
    {
        ShaderParam &param = m_Params[i];

        switch (param.type)
        {
        case Parameter_Bool:
        case Parameter_Float:
        case Parameter_Int:
            m_constantSize += sizeof(float);
            break;
        case Parameter_Vector2:
            m_constantSize += sizeof(float)*2;
            break;
        case Parameter_Vector:
            m_constantSize += sizeof(float)*3;
            break;
        case Parameter_Vector4:
            m_constantSize += sizeof(float)*4;
            break;
        case Parameter_Matrix3x3:
            m_constantSize += sizeof(float)*3*3;
            break;
        case Parameter_Matrix:
            m_constantSize += sizeof(float)*4*4;
            break;
        }
    }

    if (m_constantSize)
    {
        D3D10_BUFFER_DESC bd;
        zero(&bd, sizeof(bd));

        bd.ByteWidth        = (m_constantSize+15)&0xFFFFFFF0; //align to 128bit boundry
        bd.Usage            = D3D10_USAGE_DYNAMIC;
        bd.BindFlags        = D3D10_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags   = D3D10_CPU_ACCESS_WRITE;

        HRESULT err = GetD3D()->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
        if (FAILED(err))
        {
            AppWarning(TEXT("Unable to create constant buffer for shader '%s', result = %08lX"), lpFileName, err);
            return false;
        }
    }

    LoadDefaults();

    return true;
}

void D3D10VertexShader::CreateVertexShaderBlob(ShaderBlob &blob, CTSTR lpShader, CTSTR lpFileName)
{
	D3D10System *d3d10Sys = static_cast<D3D10System*>(g_GraphSys);
    LPCSTR lpVSType = d3d10Sys->m_bDisableCompatibilityMode ? "vs_4_0" : "vs_4_0_level_9_3";

    ID3D10Blob *shaderBlob = NULL;

	HRESULT hr = ReadBlobFromObj(lpFileName, &shaderBlob);
	if (!SUCCEEDED(hr)){
		CrashError(TEXT("GetVertexShaderBlocbFromObj Failed!"));
		return;
	}
    //if (FAILED(err))
    //{
    //    if (errorMessages)
    //    {
    //        if (errorMessages->GetBufferSize())
    //        {
    //            LPSTR lpErrors = (LPSTR)errorMessages->GetBufferPointer();
    //            Log(TEXT("Error compiling vertex shader '%s':\r\n\r\n%S\r\n"), lpFileName, lpErrors);
    //        }
    //    }
    //    CrashError(TEXT("Compilation of vertex shader '%s' failed, result = %08lX"), lpFileName, err);
    //    return;
    //}

    blob.assign((char*)shaderBlob->GetBufferPointer(), (char*)shaderBlob->GetBufferPointer() + shaderBlob->GetBufferSize());
	SafeRelease(shaderBlob);
}

Shader* D3D10VertexShader::CreateVertexShaderFromBlob(ShaderBlob const &blob, CTSTR lpShader, CTSTR lpFileName)
{
	MessageBoxA(NULL, "Test ProcessShader ", "Debug", MB_OK);
    ShaderProcessor shaderProcessor;
    if (!shaderProcessor.ProcessShader(lpShader, lpFileName))
        AppWarning(TEXT("Unable to process vertex shader '%s'"), lpFileName); //don't exit, leave it to the actual shader compiler to tell the errors

    //-----------------------------------------------

    if (!blob.size())
        return nullptr;

    ComPtr<ID3D10VertexShader> vShader;
    ID3D10InputLayout *vShaderLayout;

    HRESULT err = GetD3D()->CreateVertexShader(&blob.front(), blob.size(), vShader.Assign());
    if (FAILED(err))
    {
        CrashError(TEXT("Unable to create vertex shader '%s', result = %08lX"), lpFileName, err);
        return NULL;
    }

    err = GetD3D()->CreateInputLayout(shaderProcessor.generatedLayout.Array(), shaderProcessor.generatedLayout.Num(), &blob.front(), blob.size(), &vShaderLayout);
    if (FAILED(err))
    {
        CrashError(TEXT("Unable to create vertex layout for vertex shader '%s', result = %08lX"), lpFileName, err);
        return NULL;
    }

    //-----------------------------------------------

    D3D10VertexShader *shader = new D3D10VertexShader;
    shader->m_pVertexShader = vShader.Detach();
    shader->m_pInputLayout = vShaderLayout;
    if (!shader->ProcessData(shaderProcessor, lpFileName))
    {
        delete shader;
        return NULL;
    }

    shader->m_bHasNormals = shaderProcessor.bHasNormals;
    shader->m_bHasColors = shaderProcessor.bHasColors;
    shader->m_bHasTangents = shaderProcessor.bHasTangents;
    shader->m_nTextureCoords = shaderProcessor.numTextureCoords;
    shader->hViewProj = shader->GetParameterByName(TEXT("ViewProj"));

    return shader;
}

Shader* D3D10VertexShader::CreateVertexShader(CTSTR lpShader, CTSTR lpFileName)
{
    ShaderBlob blob;
    CreateVertexShaderBlob(blob, lpShader, lpFileName);
    return CreateVertexShaderFromBlob(blob, lpShader, lpFileName);
}

void D3D10PixelShader::CreatePixelShaderBlob(ShaderBlob &blob, CTSTR lpShader, CTSTR lpFileName)
{
	D3D10System *d3d10Sys = static_cast<D3D10System*>(g_GraphSys);
    LPCSTR lpPSType = d3d10Sys->m_bDisableCompatibilityMode ? "ps_4_0" : "ps_4_0_level_9_3";

    ID3D10Blob* shaderBlob = NULL;

	if (!ReadBlobFromObj(lpFileName, &shaderBlob))
	{
		CrashError(TEXT("GetPixelShaderBlobFromObj Failed"));
		return;
	}

    //if (FAILED(err))
    //{
    //    if (errorMessages)
    //    {
    //        if (errorMessages->GetBufferSize())
    //        {
    //            LPSTR lpErrors = (LPSTR)errorMessages->GetBufferPointer();
    //            Log(TEXT("Error compiling pixel shader '%s':\r\n\r\n%S\r\n"), lpFileName, lpErrors);
    //        }
    //    }

    //    CrashError(TEXT("Compilation of pixel shader '%s' failed, result = %08lX"), lpFileName, err);
    //    return;
    //}

    blob.assign((char*)shaderBlob->GetBufferPointer(), (char*)shaderBlob->GetBufferPointer() + shaderBlob->GetBufferSize());
	SafeRelease(shaderBlob);
}

Shader *D3D10PixelShader::CreatePixelShaderFromBlob(ShaderBlob const &blob, CTSTR lpShader, CTSTR lpFileName)
{
    ShaderProcessor shaderProcessor;
    if (!shaderProcessor.ProcessShader(lpShader, lpFileName))
        AppWarning(TEXT("Unable to process pixel shader '%s'"), lpFileName); //don't exit, leave it to the actual shader compiler to tell the errors

    //-----------------------------------------------

    if (!blob.size())
        return nullptr;

    ID3D10PixelShader *pShader;

    HRESULT err = GetD3D()->CreatePixelShader(&blob.front(), blob.size(), &pShader);
    if (FAILED(err))
    {
        CrashError(TEXT("Unable to create pixel shader '%s', result = %08lX"), lpFileName, err);
        return NULL;
    }

    //-----------------------------------------------

    D3D10PixelShader *shader = new D3D10PixelShader;
    shader->m_pPixelShader = pShader;
    if (!shader->ProcessData(shaderProcessor, lpFileName))
    {
        delete shader;
        return NULL;
    }

    return shader;

}

Shader* D3D10PixelShader::CreatePixelShader(CTSTR lpShader, CTSTR lpFileName)
{
    ShaderBlob blob;
    CreatePixelShaderBlob(blob, lpShader, lpFileName);
    return CreatePixelShaderFromBlob(blob, lpShader, lpFileName);
}

D3D10Shader::~D3D10Shader()
{
    for (UINT i=0; i<m_Samplers.Num(); i++)
        m_Samplers[i].FreeData();
    for (UINT i=0; i<m_Params.Num(); i++)
        m_Params[i].FreeData();

    SafeRelease(m_pConstantBuffer);
}

D3D10VertexShader::~D3D10VertexShader()
{
    SafeRelease(m_pVertexShader);
    SafeRelease(m_pInputLayout);
}

D3D10PixelShader::~D3D10PixelShader()
{
    SafeRelease(m_pPixelShader);
}


int    D3D10Shader::NumParams() const
{
    return m_Params.Num();
}

HANDLE D3D10Shader::GetParameter(UINT parameter) const
{
    if (parameter >= m_Params.Num())
        return NULL;
    return (HANDLE)(m_Params+parameter);
}

HANDLE D3D10Shader::GetParameterByName(CTSTR lpName) const
{
    for (UINT i=0; i<m_Params.Num(); i++)
    {
        ShaderParam &param = m_Params[i];
        if (param.name == lpName)
            return (HANDLE)&param;
    }

    return NULL;
}

/*#define GetValidHandle() \
    ShaderParam *param = (ShaderParam*)hObject; \
    if(!hObject) \
    { \
        AppWarning(TEXT("Invalid handle input as shader parameter")); \
        return; \
    }*/
#define GetValidHandle() \
    ShaderParam *param = (ShaderParam*)hObject; \
    if(!hObject) \
        return;


void   D3D10Shader::GetParameterInfo(HANDLE hObject, ShaderParameterInfo &paramInfo) const
{
    GetValidHandle();

    paramInfo.type = param->type;
    paramInfo.name = param->name;
}


void   D3D10Shader::SetBool(HANDLE hObject, BOOL bValue)
{
    GetValidHandle();

    BOOL bSizeChanged = param->curValue.SetSize(sizeof(BOOL));
    BOOL &curVal = *(BOOL*)param->curValue.Array();

    if (bSizeChanged || curVal != bValue)
    {
        curVal = bValue;
        param->bChanged = TRUE;
    }
}

void   D3D10Shader::SetFloat(HANDLE hObject, float fValue)
{
    GetValidHandle();

    BOOL bSizeChanged = param->curValue.SetSize(sizeof(float));
    float &curVal = *(float*)param->curValue.Array();

    if (bSizeChanged || curVal != fValue)
    {
        curVal = fValue;
        param->bChanged = TRUE;
    }
}

void   D3D10Shader::SetInt(HANDLE hObject, int iValue)
{
    GetValidHandle();

    BOOL bSizeChanged = param->curValue.SetSize(sizeof(int));
    int &curVal = *(int*)param->curValue.Array();

    if (bSizeChanged || curVal != iValue)
    {
        curVal = iValue;
        param->bChanged = TRUE;
    }
}

void   D3D10Shader::SetMatrix(HANDLE hObject, float *matrix)
{
    SetValue(hObject, matrix, sizeof(float)*4*4);
}

void   D3D10Shader::SetVector(HANDLE hObject, const Vect &value)
{
    SetValue(hObject, value.ptr, sizeof(float)*3);
}

void   D3D10Shader::SetVector2(HANDLE hObject, const Vect2 &value)
{
    SetValue(hObject, value.ptr, sizeof(Vect2));
}

void   D3D10Shader::SetVector4(HANDLE hObject, const Vect4 &value)
{
    SetValue(hObject, value.ptr, sizeof(Vect4));
}

void   D3D10Shader::SetTexture(HANDLE hObject, BaseTexture *texture)
{
    GetValidHandle();

    BOOL bSizeChanged = param->curValue.SetSize(sizeof(const BaseTexture*));
    const BaseTexture *&curVal = *(const BaseTexture**)param->curValue.Array();

    if (bSizeChanged || curVal != texture)
    {
        curVal = texture;
        param->bChanged = TRUE;
    }
}

void   D3D10Shader::SetValue(HANDLE hObject, const void *val, DWORD dwSize)
{
    GetValidHandle();

    BOOL bSizeChanged = param->curValue.SetSize(dwSize);

    if (bSizeChanged || !mcmp(param->curValue.Array(), val, dwSize))
    {
        mcpy(param->curValue.Array(), val, dwSize);
        param->bChanged = TRUE;
    }
}

void  D3D10Shader::UpdateParams()
{
    List<BYTE> shaderConstantData;
    bool bUpload = false;

    for (UINT i=0; i<m_Params.Num(); i++)
    {
        ShaderParam &param = m_Params[i];

        if (param.type != Parameter_Texture)
        {
            if (!param.curValue.Num())
            {
                AppWarning(TEXT("D3D10Shader::UpdateParams: shader parameter '%s' not set"), param.name.Array());
                bUpload = false;
                break;
            }

            shaderConstantData.AppendList(param.curValue);

            if (param.bChanged)
            {
                bUpload = true;
                param.bChanged = false;
            }
        }
        else
        {
            if (param.curValue.Num())
            {
                Texture *texture = *(Texture**)param.curValue.Array();
                LoadTexture(texture, param.textureID);
            }
        }
    }

    if (shaderConstantData.Num() != m_constantSize)
    {
        AppWarning(TEXT("D3D10Shader::UpdateParams: invalid parameter specifications, constant size given: %d, constant size expected: %d"), shaderConstantData.Num(), m_constantSize);
        bUpload = false;
    }

    if (bUpload)
    {
        BYTE *outData;

        HRESULT err;
        if (FAILED(err = m_pConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
        {
            AppWarning(TEXT("D3D10Shader::UpdateParams: could not map constant buffer, result = %08lX"), err);
            return;
        }

        mcpy(outData, shaderConstantData.Array(), shaderConstantData.Num());
        m_pConstantBuffer->Unmap();
    }
}
