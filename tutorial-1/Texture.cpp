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

#define Vertex_Format (D3DFVF_XYZ | D3DFVF_TEX1)
struct Vertex {
	float x, y, z;
	float u, v;
};

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
	Application app("Okienko");

	// Inicjalizacja Direct3D
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

	// Parametry urzadzenia
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferHeight = app.get_height();
	d3dpp.BackBufferWidth = app.get_width();
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Tworzenie urzadzenia
	IDirect3DDevice9* dev;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, app.window_handle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dev);

	app.init_font(dev, "Courier New");

	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// Brak obcinania scian
    dev->SetRenderState(D3DRS_LIGHTING, false);			// Brak swiatla

	// Inicjalizacja kamery
//	D3DXVECTOR3 eye(0, 5, -10);
    D3DXVECTOR3 target(0, 0, 0);
    D3DXVECTOR3 up(0, 1, 0);
    D3DXMATRIXA16 view;

    D3DXMATRIX mProjection;
	D3DXMatrixPerspectiveFovLH(&mProjection, D3DX_PI * 0.5f, app.get_width()/(float)app.get_height(), 0.001, 50);
    dev->SetTransform(D3DTS_PROJECTION, &mProjection);

	LPDIRECT3DTEXTURE9 texture, wood1, wood2;
	if(FAILED(D3DXCreateTextureFromFile(dev, "images\\texture.jpg", &texture))) return 0;
	if(FAILED(D3DXCreateTextureFromFile(dev, "images\\texture2.jpg", &wood1))) return 0;
	if(FAILED(D3DXCreateTextureFromFile(dev, "images\\texture3.jpg", &wood2))) return 0;

	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// Model pudelka
	Vertex box[] = {
	//	  X   Y   Z  U V
		{ 1,  1,  1, 1, 1},
		{-1,  1,  1, 0, 1},
		{-1,  1, -1, 0, 0},
		{ 1,  1, -1, 1, 0},

		{ 1, -1,  1, 1, 1},
		{-1, -1,  1, 0, 1},
		{-1, -1, -1, 0, 0},
		{ 1, -1, -1, 1, 0},

		// 0 4 5 1
		{ 1,  1,  1, 1, 1},
		{ 1, -1,  1, 1, 0},
		{-1, -1,  1, 0, 0},
		{-1,  1,  1, 0, 1},

		// 5 1 2 6
		{-1, -1,  1, 0, 0},
		{-1,  1,  1, 0, 1},
		{-1,  1, -1, 1, 1},
		{-1, -1, -1, 1, 0},

		// 2 6 7 3
		{-1,  1, -1, 1, 1},
		{-1, -1, -1, 1, 0},
		{ 1, -1, -1, 0, 0},
		{ 1,  1, -1, 0, 1},

		// 7 3 0 4
		{ 1, -1, -1, 0, 0},
		{ 1,  1, -1, 0, 1},
		{ 1,  1,  1, 1, 1},
		{ 1, -1,  1, 1, 0}
	};

	int box_size = (24) * sizeof(Vertex);

	// Tworzenie bufora wierzcholkow
	IDirect3DVertexBuffer9* box_buffer;
	dev->CreateVertexBuffer(box_size, 0, Vertex_Format, D3DPOOL_MANAGED, &box_buffer, NULL);

	VOID* pVoid;
	box_buffer->Lock(0, box_size, (void**)&pVoid, 0);
	memcpy(pVoid, box, box_size);
	box_buffer->Unlock();

	dev->SetFVF(Vertex_Format);
	dev->SetStreamSource(0, box_buffer, 0, sizeof(Vertex));

	float radius = 2.5;
	int cached_fps = 0, fps = 0, last_time = GetTickCount();
	while(app.running()){
		float alfa = app.get_alfa();
		float beta = app.get_beta();
		// Aktualizujemy kamere
		D3DXVECTOR3 eye(
			radius * cos(alfa) * sin(beta),
			radius * cos(beta),
			radius * sin(alfa) * sin(beta)
		);

		D3DXMatrixLookAtLH(&view, &eye, &target, &up);
		dev->SetTransform(D3DTS_VIEW, &view);

		// Rysujemy pudeleczko
		dev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		dev->BeginScene();

		for(int i = 0; i < 6; i++){
			// "Losujemy" tekstury
			if(i % 2) dev->SetTexture(0, wood1);
			else dev->SetTexture(0, wood2);
			dev->DrawPrimitive(D3DPT_TRIANGLEFAN, i*4, 2);
		}

		app.print(10, 10, "FPS = %d", cached_fps);
		app.print(10, 24, "Mysz X = %d", app.get_mouse_x());
		app.print(10, 38, "Mysz Y = %d", app.get_mouse_y());

		dev->EndScene();
		dev->Present(NULL, NULL, NULL, NULL);

		fps++;
		int current_time = GetTickCount();
		if(current_time - last_time > 1000){
			last_time = current_time;
			cached_fps = fps;
			fps = 0;
		}
	}

	// Zwalniamy zasoby
	texture->Release();
	wood1->Release();
	wood2->Release();
	box_buffer->Release();
	dev->Release();
	d3d->Release();
	return 0;
}

