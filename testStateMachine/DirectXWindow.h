#pragma once

#include <D3D10_1.h>
#include <D3DX10.h>
#include <DXGI.h>

class D3D10Texture
{
public:
	DXGI_FORMAT m_format;
	ID3D10Texture2D          *m_pTexture;
	ID3D10ShaderResourceView *m_pResource;
	ID3D10RenderTargetView   *m_pRenderTarget;
	UINT m_width, m_height;
};

class CDirectXWindow
{
public:
	CDirectXWindow();
	~CDirectXWindow();
	bool Init(HWND hwnd,		// 渲染窗口
				POINT &rcSize,	// 窗口大小
				RECT& rc, 		// 渲染区域
				IDXGIAdapter* pAdapter);
	bool InitModel();
	void Render();
	void Clear();
	void Transform();
	void SaveToDisk();
	bool MapTextrue();
	bool MapTextrueFromFrame(ID3D10Texture2D* pTex2D);
	bool CreateTargetViewWithTex(DXGI_FORMAT dxgiFormat);
	void SetTexRenderTarget(bool bStencil);
	void DrawTexture(bool bTexFrameResource);
	void SetWindowRenderTarget(bool bStencil);
	ID3D10Texture2D* CopyFromSwapBackbuffer();
	IDXGISwapChain*			m_pSwapChain;
	ID3D10Device1*			m_pd3dDevice;
private:
	static char				s_strEffect[];
	static char				s_DrawTextureV[];
	static char				s_DrawTextureP[];

	HWND					m_hwnd;
	POINT					m_ptSize;			
	RECT					m_rcClipRect;
	ID3D10RenderTargetView* m_pRenderTargetView;
	ID3D10Effect*           m_pEffect;
	ID3D10EffectTechnique*  m_pTechnique;
	ID3D10InputLayout*      m_pVertexLayout;
	ID3D10Buffer*           m_pVertexBuffer;
	ID3D10Texture2D*		m_pDepthStencil;
	ID3D10DepthStencilView* m_pDepthStencilView;

	D3DXMATRIX              m_World;
	D3DXMATRIX              m_View;
	D3DXMATRIX              m_Projection;
	ID3D10EffectMatrixVariable* m_pWorldVariable = NULL;
	ID3D10EffectMatrixVariable* m_pViewVariable = NULL;
	ID3D10EffectMatrixVariable* m_pProjectionVariable = NULL;

	ID3D10ShaderResourceView*			m_pTextureRV;
	ID3D10ShaderResourceView*			m_pFrameResource;
	ID3D10EffectShaderResourceVariable*	m_pDiffuseVariable;
	D3D10Texture*			m_TargetTexture;
};