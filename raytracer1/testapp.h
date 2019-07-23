//// Don't look here. The interesting stuff is elsewhere.
//#pragma once
////#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <stdlib.h>
//#include "common.h"
//#include "raytracer.h"
//#include "scene.h"
//#include "surface.h"
//
//#define SCRWIDTH	800
//#define SCRHEIGHT	600
//
////static WNDCLASS wc;
////static HWND wnd;
////static char bitmapbuffer[sizeof( BITMAPINFO ) + 16];
////static BITMAPINFO* bh;
////HDC window_hdc;
//Raytracer::Surface* surface = 0;
//Pixel* buffer = 0;
//Raytracer::Engine* tracer = 0;
//
////void DrawWindow();
//
////static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
////{
////	int result = 0, keycode = 0;
////	switch (message)
////	{
////	case WM_PAINT:
////		if (!buffer) break;
////		StretchDIBits( window_hdc, 0, 0, SCRWIDTH, SCRHEIGHT, 0, 0, SCRWIDTH, SCRHEIGHT, buffer, bh, DIB_RGB_COLORS, SRCCOPY );
////		ValidateRect( wnd, NULL );
////		break;
////	case WM_KEYDOWN:
////		if ((wParam & 0xFF) != 27) break;
////	case WM_CLOSE:
////		ReleaseDC( wnd, window_hdc );
////		DestroyWindow( wnd );
////		SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, 1, 0, 0 );
////		ExitProcess( 0 );
////		break;
////	default:
////		result = DefWindowProc(hWnd,message,wParam,lParam);
////	}
////	return result;
////}
//
//int RaytracerIT()
//{
//	//RECT rect;
//	int cc;
//	/*wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
//	wc.lpfnWndProc = WndProc;
//	wc.cbClsExtra = wc.cbWndExtra = 0;
//	wc.hInstance = 0;
//	wc.hIcon = NULL;
//	wc.hCursor = LoadCursor(0,IDC_ARROW);
//	wc.hbrBackground = NULL;
//	wc.lpszMenuName = NULL;
//	wc.lpszClassName = "raytracer";
//	if (!RegisterClass(&wc)) return FALSE;*/
//	/*rect.left = rect.top = 0;
//	rect.right = SCRWIDTH, rect.bottom = SCRHEIGHT;
//	AdjustWindowRect( &rect, WS_POPUP|WS_SYSMENU|WS_CAPTION, 0 );
//	rect.right -= rect.left, rect.bottom -= rect.top;
//	wnd = CreateWindowEx( 0, "raytracer", "raytracer", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
//		CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom, 0, 0, 0, 0 );
//	ShowWindow(wnd,SW_NORMAL);*/
//	//for ( cc = 0; cc < sizeof( BITMAPINFOHEADER ) + 16; cc++ )
//		//bitmapbuffer[cc] = 0;
//	/*bh = (BITMAPINFO *)&bitmapbuffer;
//	bh->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
//	bh->bmiHeader.biPlanes = 1;
//	bh->bmiHeader.biBitCount = 32;
//	bh->bmiHeader.biCompression = BI_BITFIELDS;
//	bh->bmiHeader.biWidth = SCRWIDTH, bh->bmiHeader.biHeight = -SCRHEIGHT;
//	((unsigned long*)bh->bmiColors)[0] = 255 << 16;
//	((unsigned long*)bh->bmiColors)[1] = 255 << 8;
//	((unsigned long*)bh->bmiColors)[2] = 255;*/
//	//window_hdc = GetDC(wnd);
//	//SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
//	// prepare output canvas
//	surface = new Raytracer::Surface( SCRWIDTH, SCRHEIGHT );
//	buffer = surface->GetBuffer();
//	surface->Clear( 0 );
//	surface->InitCharset();
//	surface->Print( "timings:", 2, 2, 0xffffffff );
//	// prepare renderer
//	tracer = new Raytracer::Engine();
//	tracer->GetScene()->InitScene();
//	tracer->SetTarget( surface->GetBuffer(), SCRWIDTH, SCRHEIGHT );
//	int tpos = 60;
//	// go
//	while (1)
//	{
//		int fstart = GetTickCount();
//		tracer->InitRender();
//		// while (!tracer->RenderTiles()) DrawWindow();
//		while (!tracer->Render())
//		{
//
//		}
//			//DrawWindow();
//		int ftime = GetTickCount() - fstart;
//		char t[] = "00:00.000";
//		t[6] = (ftime / 100) % 10 + '0';
//		t[7] = (ftime / 10) % 10 + '0';
//		t[8] = (ftime % 10) + '0';
//		int secs = (ftime / 1000) % 60;
//		int mins = (ftime / 60000) % 100;
//		t[3] = ((secs / 10) % 10) + '0';
//		t[4] = (secs % 10) + '0';
//		t[1] = (mins % 10) + '0';
//		t[0] = ((mins / 10) % 10) + '0';
//		surface->Print( t, tpos, 2, 0xffffffff );
//		tpos += 100;
//	}
//	return 1;
//}
//
////void DrawWindow()
////{
////	MSG message;
////	HACCEL haccel = 0;
////	InvalidateRect( wnd,NULL,TRUE );
////	SendMessage( wnd, WM_PAINT, 0, 0 );
////	while (PeekMessage( &message, wnd, 0, 0, PM_REMOVE ))
////	{
////		if (TranslateAccelerator( wnd, haccel, &message ) == 0)
////		{
////			TranslateMessage( &message );
////			DispatchMessage( &message );
////		}
////	}
////	Sleep( 0 );
////}
