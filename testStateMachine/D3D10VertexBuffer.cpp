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

VertexBuffer* D3D10VertexBuffer::CreateVertexBuffer(VBData *vbData, BOOL bStatic)
{
    if (!vbData)
    {
        AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: vbData NULL"));
        return NULL;
    }

    HRESULT err;

    D3D10VertexBuffer *buf = new D3D10VertexBuffer;
    buf->m_numVerts = vbData->VertList.Num();

    D3D10_BUFFER_DESC bd;
    D3D10_SUBRESOURCE_DATA srd;
    zero(&bd, sizeof(bd));
    zero(&srd, sizeof(srd));

    bd.Usage = bStatic ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
    bd.CPUAccessFlags = bStatic ? 0 : D3D10_CPU_ACCESS_WRITE;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;

    //----------------------------------------

    buf->m_VertexSize = sizeof(Vect);
    bd.ByteWidth = sizeof(Vect)*buf->m_numVerts;
    srd.pSysMem = vbData->VertList.Array();

    err = GetD3D()->CreateBuffer(&bd, &srd, &buf->m_pVertexBuffer);
    if (FAILED(err))
    {
        AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: Failed to create the vertex portion of the vertex buffer, result = %08lX"), err);
        delete buf;
        return NULL;
    }

    //----------------------------------------

    if (vbData->NormalList.Num())
    {
        buf->m_NormalSize = sizeof(Vect);
        bd.ByteWidth = sizeof(Vect)*buf->m_numVerts;
        srd.pSysMem = vbData->NormalList.Array();
        err = GetD3D()->CreateBuffer(&bd, &srd, &buf->m_pNormalBuffer);
        if (FAILED(err))
        {
            AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: Failed to create the normal portion of the vertex buffer, result = %08lX"), err);
            delete buf;
            return NULL;
        }
    }

    //----------------------------------------

    if (vbData->ColorList.Num())
    {
        buf->m_ColorSize = sizeof(DWORD);
        bd.ByteWidth = sizeof(DWORD)*buf->m_numVerts;
        srd.pSysMem = vbData->ColorList.Array();
        err = GetD3D()->CreateBuffer(&bd, &srd, &buf->m_pColorBuffer);
        if (FAILED(err))
        {
            AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: Failed to create the color portion of the vertex buffer, result = %08lX"), err);
            delete buf;
            return NULL;
        }
    }

    //----------------------------------------

    if (vbData->TangentList.Num())
    {
        buf->m_TangentSize = sizeof(Vect);
        bd.ByteWidth = sizeof(Vect)*buf->m_numVerts;
        srd.pSysMem = vbData->TangentList.Array();
        err = GetD3D()->CreateBuffer(&bd, &srd, &buf->m_pTangentBuffer);
        if (FAILED(err))
        {
            AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: Failed to create the tangent portion of the vertex buffer, result = %08lX"), err);
            delete buf;
            return NULL;
        }
    }

    //----------------------------------------

    if (vbData->UVList.Num())
    {
        buf->m_UVBuffers.SetSize(vbData->UVList.Num());
        buf->m_UVSizes.SetSize(vbData->UVList.Num());

        for (UINT i=0; i<vbData->UVList.Num(); i++)
        {
            List<UVCoord> &textureVerts = vbData->UVList[i];

            buf->m_UVSizes[i] = sizeof(UVCoord);
            bd.ByteWidth = buf->m_UVSizes[i]*buf->m_numVerts;
            srd.pSysMem = textureVerts.Array();

            ID3D10Buffer *tvBuffer;
            err = GetD3D()->CreateBuffer(&bd, &srd, &tvBuffer);
            if (FAILED(err))
            {
                AppWarning(TEXT("D3D10VertexBuffer::CreateVertexBuffer: Failed to create the texture vertex %d portion of the vertex buffer, result = %08lX"), i, err);
                delete buf;
                return NULL;
            }

            buf->m_UVBuffers[i] = tvBuffer;
        }
    }

    //----------------------------------------

    buf->m_bDynamic = !bStatic;

    if (bStatic)
    {
        delete vbData;
        buf->m_pData = NULL;
    }
    else
        buf->m_pData = vbData;

    return buf;
}


D3D10VertexBuffer::~D3D10VertexBuffer()
{
    for (UINT i=0; i<m_UVBuffers.Num(); i++)
        SafeRelease(m_UVBuffers[i]);

    SafeRelease(m_pTangentBuffer);
    SafeRelease(m_pColorBuffer);
    SafeRelease(m_pNormalBuffer);
    SafeRelease(m_pVertexBuffer);

    delete m_pData;
}

void D3D10VertexBuffer::FlushBuffers()
{
    if (!m_bDynamic)
    {
        AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: Cannot flush buffers on a non-dynamic vertex buffer"));
        return;
    }

    HRESULT err;

    //---------------------------------------------------

    BYTE *outData;
    if (FAILED(err = m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
    {
        AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: failed to map vertex buffer, result = %08lX"), err);
        return;
    }

    mcpy(outData, m_pData->VertList.Array(), sizeof(Vect)*m_numVerts);

    m_pVertexBuffer->Unmap();

    //---------------------------------------------------

    if (m_pNormalBuffer)
    {
        if (FAILED(err = m_pNormalBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
        {
            AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: failed to map normal buffer, result = %08lX"), err);
            return;
        }

        mcpy(outData, m_pData->NormalList.Array(), sizeof(Vect)*m_numVerts);
        m_pNormalBuffer->Unmap();
    }

    //---------------------------------------------------

    if (m_pColorBuffer)
    {
        if (FAILED(err = m_pColorBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
        {
            AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: failed to map color buffer, result = %08lX"), err);
            return;
        }

        mcpy(outData, m_pData->ColorList.Array(), sizeof(Vect)*m_numVerts);
        m_pColorBuffer->Unmap();
    }

    //---------------------------------------------------

    if (m_pTangentBuffer)
    {
        if (FAILED(err = m_pTangentBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
        {
            AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: failed to map tangent buffer, result = %08lX"), err);
            return;
        }

        mcpy(outData, m_pData->TangentList.Array(), sizeof(Vect)*m_numVerts);
        m_pTangentBuffer->Unmap();
    }

    //---------------------------------------------------

    if (m_UVBuffers.Num())
    {
        for (UINT i=0; i<m_UVBuffers.Num(); i++)
        {
            List<UVCoord> &textureVerts = m_pData->UVList[i];

            ID3D10Buffer *buffer = m_UVBuffers[i];

            if (FAILED(err = buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&outData)))
            {
                AppWarning(TEXT("D3D10VertexBuffer::FlushBuffers: failed to map texture vertex buffer %d, result = %08lX"), i, err);
                return;
            }

            mcpy(outData, textureVerts.Array(), sizeof(UVCoord)*m_numVerts);
            buffer->Unmap();
        }
    }
}

VBData* D3D10VertexBuffer::GetData()
{
    if (!m_bDynamic)
    {
        AppWarning(TEXT("D3D10VertexBuffer::GetData: Cannot get vertex data of a non-dynamic vertex buffer"));
        return NULL;
    }

    return m_pData;
}

void D3D10VertexBuffer::MakeBufferList(D3D10VertexShader *vShader, List<ID3D10Buffer*> &bufferList, List<UINT> &strides) const
{
    assert(vShader);

    bufferList << m_pVertexBuffer;
    strides << m_VertexSize;

    if (vShader->m_bHasNormals)
    {
        if (m_pNormalBuffer)
        {
            bufferList << m_pNormalBuffer;
            strides << m_NormalSize;
        }
        else
            AppWarning(TEXT("Trying to use a vertex buffer without normals with a vertex shader that requires normals"));
    }

    if (vShader->m_bHasColors)
    {
        if (m_pColorBuffer)
        {
            bufferList << m_pColorBuffer;
            strides << m_ColorSize;
        }
        else
            AppWarning(TEXT("Trying to use a vertex buffer without colors with a vertex shader that requires colors"));
    }

    if (vShader->m_bHasTangents)
    {
        if (m_pTangentBuffer)
        {
            bufferList << m_pTangentBuffer;
            strides << m_TangentSize;
        }
        else
            AppWarning(TEXT("Trying to use a vertex buffer without tangents with a vertex shader that requires tangents"));
    }

    if (vShader->m_nTextureCoords <= m_UVBuffers.Num())
    {
        for (UINT i=0; i<vShader->m_nTextureCoords; i++)
        {
            bufferList << m_UVBuffers[i];
            strides << m_UVSizes[i];
        }
    }
    else
        AppWarning(TEXT("Trying to use a vertex buffer with insufficient texture coordinates compared to the vertex shader requirements"));
}
