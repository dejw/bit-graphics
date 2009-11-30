/*
  Author: Dawid Fatyga
*/

#ifdef DEBUG
#define D3D_DEBUG_INFO
  #pragma comment (lib, "d3dx9d.lib")
#else
  #pragma comment (lib, "d3dx9.lib")
#endif // DEBUG
#pragma comment (lib, "d3d9.lib")

#include <d3dx9.h>

#include "Application.h"

#define Vertex_Format (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)	// Flexible Vertex Formats
struct Vertex {
	float x, y, z, rhw;
	DWORD color;
};

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	Application app("Okienko");

	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferHeight = app.get_height();
	d3dpp.BackBufferWidth = app.get_width();

	IDirect3DDevice9* dev;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, app.window_handle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dev);

	Vertex triangle[] = {
	//	X     Y    Z  W  Color
		{400, 100, 1, 1, D3DCOLOR_XRGB(0, 0, 255)},
		{600, 500, 1, 1, D3DCOLOR_XRGB(0, 255, 0)},
		{200, 500, 1, 1, D3DCOLOR_XRGB(255, 0, 0)}
	};
/*
	IDirect3DVertexBuffer9* v_buffer;
	dev->CreateVertexBuffer(3 * sizeof(Vertex), 0, Vertex_Format, D3DPOOL_MANAGED, &v_buffer, NULL);

	VOID* pVoid;
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, triangle, sizeof(triangle));
    v_buffer->Unlock();
*/
	while(app.running()){
		dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		dev->SetFVF(Vertex_Format);

		dev->BeginScene();
		dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, triangle, sizeof(Vertex));
/*
        dev->SetStreamSource(0, v_buffer, 0, sizeof(Vertex));
        dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
*/
		dev->EndScene();
		dev->Present(NULL, NULL, NULL, NULL);
	}

/*	v_buffer->Release(); */

	dev->Release();
	d3d->Release();
	return 0;
}

