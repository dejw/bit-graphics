/*
  Author: Dawid Fatyga
*/

#ifndef _application_h_
#define _application_h_

#include <stdio.h>
#include <stdarg.h>

/* FIXME: Oj bardzo brzydko. */
int mouse_x = 0, mouse_y = 0, old_mouse_x = 0, old_mouse_y = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg) {
		case WM_CLOSE :
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY :
			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE :
			old_mouse_x = mouse_x;
			old_mouse_y = mouse_y;
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			if(mouse_x & 1 << 15) mouse_x -= (1 << 16);
			if(mouse_y & 1 << 15) mouse_y -= (1 << 16);
			break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

class Application {
	HWND hWnd;
	int width, height;

	LPD3DXFONT font;
	ID3DXSprite* sprite;
public:
	Application(const char* name, int width = 800, int height = 600): width(width), height(height) {
		HINSTANCE hProg = GetModuleHandle(0);
		WNDCLASS wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hProg;
		wc.lpszClassName = "WndClass";
		wc.lpszMenuName = 0;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.hIcon = LoadIcon(hProg, IDI_WINLOGO);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hCursor = LoadCursor(hProg, IDC_ARROW);

		RegisterClass(&wc);
		hWnd = CreateWindowEx(0, "WndClass", name, WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, hProg, 0);
		ShowWindow(hWnd, 5);
	}

	~Application(void){
		font->Release();
		sprite->Release();
	}
	inline void init_font(IDirect3DDevice9* device, const char* face, int size = 12, bool bold = false, bool italic = false){
		D3DXCreateFont( device,
			-size, 0, bold ? 800 : 0, 1, italic,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			face, &font );
		D3DXCreateSprite(device, &sprite);
	}

	inline bool running(void){
		MSG msg;
		if(GetKeyState(VK_ESCAPE) & 0x0800) return false;
		if(PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_DESTROY
			|| msg.message == WM_CLOSE
			|| msg.message == WM_QUIT)
				return false;
		}
		return true;
	}

	inline HWND window_handle(void){ return hWnd; }
	inline int get_width(void) { return width; }
	inline int get_height(void) { return height; }

	inline int get_mouse_x(void){ return mouse_x; }
	inline int get_mouse_y(void){ return mouse_y; }

	inline int get_relative_mouse_x(void){ return mouse_x - old_mouse_x; }
	inline int get_relative_mouse_y(void){ return mouse_y - old_mouse_y; }

	inline float get_alfa(void){
		return (float) - (get_mouse_x() - get_width() /2)/ ((float)get_width() /2) * D3DX_PI;
	}
	inline float get_beta(void){
		return (float) - (get_mouse_y() - get_height()/2)/ ((float)get_height()/2) * D3DX_PI * 0.5 + D3DX_PI * 0.5;
	}

	void print(int x, int y, const char* format, ... ){
		if(font && sprite){
			char buffer[1024];
			va_list args;
			va_start(args, format);
			vsprintf(buffer, format, args);
			va_end(args);

			sprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
			int width = 640, height = 480;
			RECT rect = { x, y, x + width, y + height };
			font->DrawText( sprite, buffer, -1, &rect, DT_EXPANDTABS|DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255) );
			sprite->End();
		}
	}
};

#endif /* _application_h_ */

