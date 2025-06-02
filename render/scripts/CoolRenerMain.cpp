#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <time.h>  
#include <time.h>
#include <Windows.h>


#include <filesystem> // C++17 推荐使用
// #ifdef _WIN32
// 	#include <direct.h> // For _getcwd on Windows
// #else
// 	#include <unistd.h> // For getcwd on Linux/macOS
// #endif

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h> // For PATH_MAX
#elif defined(__APPLE__)
#include <mach-o/dyld.h> // For _NSGetExecutablePath
#endif



//#include "../raytracer1/common.h"
//#include "../raytracer1/raytracer.h"
//#include "../raytracer1/scene.h"
//#include "../raytracer1/surface.h"
#include "include\Pipline.h"
#include "include\CMaterail.h"
#include "include\Camera.h"
#include "include\tgaimage.h"
#include "include\Model.h"
#include "include\Shader.h"
#include "include\geometry.h"
#include "include\TriangleDrawTest.h"
#include "include\mathTest.h"
#include "include\skeleton.h"
using namespace std;

//#include "../raytracer1/testapp.h"
#pragma comment( lib,"winmm.lib" )
#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib") //gdi绘图
#pragma comment(lib, "user32.lib")
#endif

//extern skeleton_t* skeleton_load(const char* filename);

//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// 当前键盘按下状态
static HWND screen_handle = NULL;		// 主窗口 HWND
static HDC screen_dc = NULL;			// 配套的 HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// 老的 BITMAP  位图

//自定义的deviese 和windows 之间的关联 就在于这个 
unsigned char *screen_fb = NULL;		// frame buffer  帧缓冲
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// 屏幕初始化
int screen_close(void);								// 关闭屏幕
void screen_dispatch(void);							// 处理消息
void screen_update(LPCSTR);							// 显示FrameBuffer

													// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

#define SCRWIDTH	800
#define SCRHEIGHT	600

//extern Pixel* buffer;
LPVOID ptr;

//Raytracer::Surface* surface = 0;
//Pixel* buffer = 0;
//Raytracer::Engine* tracer = 0;

//void Set2RayTracer()
//{
//	ptr = buffer;
//}

//int RaytracerIT()
//{
//	int cc;
//	// prepare output canvas
//	surface = new Raytracer::Surface(SCRWIDTH, SCRHEIGHT);
//	buffer = surface->GetBuffer();
//	surface->Clear(0);
//	surface->InitCharset();
//	surface->Print("timings:", 2, 2, 0xffffffff);
//	// prepare renderer
//	tracer = new Raytracer::Engine();
//	tracer->GetScene()->InitScene();
//	tracer->SetTarget(surface->GetBuffer(), SCRWIDTH, SCRHEIGHT);
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
//		//DrawWindow();
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
//		surface->Print(t, tpos, 2, 0xffffffff);
//		tpos += 100;
//	}
//	return 1;
//}



// 初始化窗口并设置标题
int screen_init(int w, int h, const TCHAR *title)
{
	//__stdcall* WNDPROC 
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0, NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, w * h * 4, 0, 0, 0, 0 } };

	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;

	HDC hDC;

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc))
		return -1;

	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL)
		return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0); //创建帧缓冲Bitmap ptr
	if (screen_hb == NULL)
		return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);

	screen_fb = (unsigned char*)ptr; //把帧缓冲指针赋值给外面
	screen_w = w;
	screen_h = h;
	screen_pitch = w * 4;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left; //屏幕宽  X轴终点
	wy = rect.bottom - rect.top;//屏幕高  Y轴终点
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2; //屏幕X起点    为了让渲染界面居中。  所以左边右边都留一半作为起点/终点
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;//屏幕Y起点
	if (sy < 0)
		sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);
	screen_dispatch();

	memset(screen_keys, 0, sizeof(int) * 512); //使用 0 重写 sizeof(int) * 512位的 byte字节。
	memset(screen_fb, 0, w * h * 4);

	return 0;
}

int screen_close(void) {
	if (screen_dc) {
		if (screen_ob) {
			SelectObject(screen_dc, screen_ob);
			screen_ob = NULL;
		}
		DeleteDC(screen_dc);
		screen_dc = NULL;
	}
	if (screen_hb) {
		DeleteObject(screen_hb);
		screen_hb = NULL;
	}
	if (screen_handle) {
		CloseWindow(screen_handle);
		screen_handle = NULL;
	}
	return 0;
}

void Control(WPARAM wParam, LPARAM lParam);


static LRESULT screen_events(HWND hWnd, UINT msg,WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE: screen_exit = 1; break;
	case WM_KEYDOWN: screen_keys[wParam & 511] = 1; 
		break;
	case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
	case WM_MOUSEMOVE:
			Control(wParam,lParam); break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void screen_dispatch(void) {
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			break;
		if (!GetMessage(&msg, NULL, 0, 0))
			break;
		DispatchMessage(&msg);
	}
}


//真正的渲染函数
void screen_update(LPCSTR type) {
	//Device Contexts 句柄
	HDC hDC = GetDC(screen_handle); 
	//HDC设备上下文是一种包含有关某个设备（如显示器或打印机）的绘制属性信息的 Windows 数据结构。所有绘制调用都通过设备上下文对象进行，这些对象封装了用于绘制线条、形状和文本的 Windows API。

	//设备上下文绘图有很多种方法  SetPixel非常非常慢 自己维护一个Color数组然后用BitBlt比较快
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	TextOut(hDC, 0, 20, type, strlen(type));
	ReleaseDC(screen_handle, hDC);
}


//=====================================================================
// 主程序
//=====================================================================

//渲染一个四角面  这个是 U3D里 模型选择 自己计算法线，而不导入法线的情况
void draw_plane(device_t *device, Shader* mShader, int a, int b, int c, int d) {

	//初始化四个顶点
	vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];

	//初始化UV   
	p1.tc.x = 0, p1.tc.y = 0,
		p2.tc.x = 0, p2.tc.y = 1;
	p3.tc.x = 1, p3.tc.y = 1,
		p4.tc.x = 1, p4.tc.y = 0;

	//将四角面拆为两个三角形
	//device_draw_primitive(device, mShader, &p1, &p2, &p3);
	//device_draw_primitive(device, mShader, &p3, &p4, &p1);
}


//void draw_Object_Shadow(Object_t Cube, Shader* mShader, device_t *device)
//{
//	//阴影
//	device->transform.model = Cube.model;
//	transform_update(&device->transform);
//
//	//shadowCamera.transform.mvp = (&device->transform)->mvp;
//
//	//直接渲染三角形
//	for (int i = 0; i < Cube.mesh_num; i += 3)
//	{
//		vertex_t v1 = (Cube.mesh)[i];
//		vertex_t v2 = (Cube.mesh)[i + 1];
//		vertex_t v3 = (Cube.mesh)[i + 2];
//
//		device_draw_primitive_shadow(device,mShader, &v1, &v2, &v3);
//	}
//}

//clock_t  start;
//clock_t  finish;
//float duration;

void Init_Obj(Object_t& Cube)
{	//matrix_set_rotate(&m, Cube.axis.x, Cube.axis.y, Cube.axis.z, 0, Cube.pos.x, Cube.pos.y, Cube.pos.z); //theta  是物体本身的x,y,z轴相对的旋转

	Matrix44f  r;
	//TODO: 计算 旋转 和位移 需要的矩阵

//输入 当前物体原点在世界坐标系中的位置和旋转，  来反推世界矩阵
//axis.x 绕X轴的旋转角度

//物体转世界坐标系  	//平移-> 旋转-》缩放 
	//Matrix44f s = matrix_set_scale(Cube.scale);
	matrix_Obj2World(r, Cube.axis, Cube.pos);

	Matrix44f t = Matrix44f().identity();
	t[3][0] = Cube.pos.x;
	t[3][1] = Cube.pos.y;
	t[3][2] = Cube.pos.z;
	 matrix_mul(Cube.model, r , t);

	// matrix_mul(Cube.model, Cube.model, s);
	//Cube.model = s * r * t;
}


void draw_Model(device_t *device, Shader *mShader)
{
	//这里把屏幕制空了
	mShader->transform.projection = mShader->curCamera.projection_trans;
	mShader->transform.view = mShader->curCamera.view;
	Init_Model_matrix(mShader->transform.model,mShader->transform.worldPos, mShader->transform.worldRot, mShader->transform.worldScale);

	transform_update(&mShader->transform);

	start = clock();
	//TODO： shader初始化时 传入顶点索引列表
	//直接渲染三角形

	Model Cube = *(mShader->mModel);
	for (int i = 0; i < Cube.nfaces(); i++) //三角面数量
	{
		/*for (int j = 0; j < 3; j++)
		{
			mShader->vertex_shader(&v1, 1, 1);
		}*/

		//Vec3f v11 = mShader->mModel->vert(i, 0); //真实顶点

		//Vec3f vNormal = mModel->normal(nfaces_index, vertex_index);	 //真实法线

		mShader->vertex_shader(mShader->v1, i, 0);
		mShader->vertex_shader(mShader->v2, i, 1);
		mShader->vertex_shader(mShader->v3, i, 2);

		device_draw_primitive(device, mShader, mShader->v1, mShader->v2, mShader->v3);
	}
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("pos tramsform----- %f seconds\n", duration);
}

void draw_Object(const Object_t& Cube, device_t *device, Shader* mShader)
{

	mShader->transform.projection = mShader->curCamera.projection_trans;
	mShader->transform.view = mShader->curCamera.view;
	Init_Model_matrix(mShader->transform.model, mShader->transform.worldPos, mShader->transform.worldRot, mShader->transform.worldScale);

	mShader->transform.model = Cube.model;
	transform_update(&mShader->transform);

	//直接渲染三角形
	for (int i = 0; i < Cube.mesh_num; i += 3)
	{
		//以后只有改变的时候，才去动态缩放一次
		vertex_t v1 = (Cube.mesh)[i];
		vertex_t v2 = (Cube.mesh)[i+1];
		vertex_t v3 = (Cube.mesh)[i+2];

		cross(v1.worldPos, v1.pos, mShader->transform.model);
		//投影坐标 cvv空间
		cross(v1.project_pos, v1.pos, mShader->transform.mvp);
		//法线转换到世界空间
		//cross(v1->world_normal, v1->normal, transform.model);

		cross(v2.worldPos, v2.pos, mShader->transform.model);
		cross(v2.project_pos, v2.pos, mShader->transform.mvp);
		cross(v3.worldPos, v3.pos, mShader->transform.model);
		cross(v3.project_pos, v3.pos, mShader->transform.mvp);

		device_draw_primitive(device, mShader, &v1, &v2, &v3);
	}
}


////传递进来的theta 初始值为1, 代表物体的旋转角度（物体的局部坐标系相对于世界坐标系的旋转）
//void draw_box(device_t *device, float theta)
//{
//	matrix_t m;
//	matrix_Obj2World(&m,  Vec4f(0, theta, beta,1), Vec4f(2, 4, 5,1));
//	//matrix_set_rotate(&m, -3, -0.5, 1, theta,0,0,0); 
//
//	device->transform.model = m;
//	transform_update(&device->transform);
//
//
//	//四个面
//	draw_plane(device, 0, 1, 2, 3);
//	draw_plane(device, 4, 5, 6, 7);
//	draw_plane(device, 0, 4, 5, 1);
//	draw_plane(device, 1, 5, 6, 2);
//	draw_plane(device, 2, 6, 7, 3);
//	draw_plane(device, 3, 7, 4, 0);
//}

//初始化一个纹理
void init_texture(Shader *mShader) {

	//初始化shader所渲染的材质的初始tramsform

	mShader->transform.worldPos = point_t(0, 3, 1, 1);
	mShader->transform.worldRot = Vec4f(0, 0, 0, 1);
	mShader->transform.worldScale = Vec4f(2.0f, 2.0f, 2.0f, 1);

	static IUINT32 texture[256][256];
	int i, j;

	//初始化一个256X256的纹理
	for (j = 0; j < 256; j++)
	{
		for (i = 0; i < 256; i++)
		{
			int x = i / 32,
				y = j / 32;
			texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef; // &1用来快速判断是否是偶数， 交叉使用两种颜色
		}
	}
	device_set_texture(mShader, texture, 1024, 256, 256);
}

float Get_FPS()
{
	static float  fps = 0; //我们需要计算的FPS值
	static int    frameCount = 0;//帧数
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间


	if (currentTime - lastTime > 1.0f) 
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime; 
		frameCount = 0;
	}

	return fps;
}


void Create_Obj()
{
	//初始化一个地板
	Object_t ground;

	ground.pos = { -2, -3, 0,1 };
	ground.axis = { 0, 0, 0, 0 };
	ground.mesh = ground_mesh;
	ground.mesh_num = 4;
	ground.scale = Vec4f(9, 1, 4, 1);
	//ground.theta = 0;

	//初始化一个物体
	Object_t Cube; 
	Cube.pos = { 1, 2, 1 ,1 };
	Cube.axis = { 0, 0, 0, 0 };
	Cube.mesh = box_mesh;
	Cube.mesh_num = 34;
	Cube.scale =Vec4f(3,3,3,1);
	//Cube.theta = 1;  //当前物体的旋转弧度。  1 就是 180度

	Scene_render_Objs.push_back(Cube);
	Scene_render_Objs.push_back(ground);
}

void CreateCamera(device_t* device,int width, int height)
{
	//初始化主摄像机
	camera_main.eye = { 0, 3, -10, 1 };

	camera_main.eyeTarget = { 0, 0, 0, 1};
	camera_main.worldup = { 0, 1, 0, 0 };

	camera_main.fov = PI * 0.5f;
	camera_main.zn = 1;
	camera_main.zf = 500;
	camera_main.aspect = (float)width / ((float)height);
	//camera_main.rotation = { -0.15, 0, 0, 1 };

}


void Init_ShadowCamera(device_t* device)
{
	if (dirLight.shadow)
	{
		//初始化阴影摄像机 采用深度测试，然后将设定的阴影颜色跟当前像素颜色混合。在像素阶段执行
		camera* shadowCamera = &cameras[0];
		shadowCamera->eye = Vec4f{0,10,10,1};
		shadowCamera->eyeTarget = Vec4f{ 0,0,0,1 };
		shadowCamera->worldup = { 0, 1, 0, 1 };
	}
}


Shader* InitShader(Shader* tShader, device_t& device, Model* lModel)
{
	init_texture(tShader); //纹理初始化
	tShader->mModel = lModel;
	device.render_state = RENDER_STATE_WIREFRAME;
	return tShader;
}



std::filesystem::path getExecutablePath() {
    std::vector<char> buffer;
    size_t path_len = 0;

#ifdef _WIN32
    buffer.resize(MAX_PATH);
    path_len = GetModuleFileNameA(NULL, buffer.data(), buffer.size());
    while (path_len == buffer.size()) { // Buffer too small, resize and try again
        buffer.resize(buffer.size() * 2);
        path_len = GetModuleFileNameA(NULL, buffer.data(), buffer.size());
    }
#elif defined(__linux__)
    buffer.resize(PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size());
    if (len != -1) {
        path_len = static_cast<size_t>(len);
    }
#elif defined(__APPLE__)
    buffer.resize(PATH_MAX); // PATH_MAX is usually sufficient
    uint32_t buf_size = buffer.size();
    if (_NSGetExecutablePath(buffer.data(), &buf_size) == 0) {
        path_len = static_cast<size_t>(buf_size);
    } else {
        // Buffer too small, _NSGetExecutablePath will return -1 and set buf_size to required size
        buffer.resize(buf_size);
        if (_NSGetExecutablePath(buffer.data(), &buf_size) == 0) {
            path_len = static_cast<size_t>(buf_size);
        }
    }
#else
    // Fallback or error for other platforms
    return {}; // Or throw an exception
#endif

    if (path_len > 0) {
        return std::filesystem::path(std::string(buffer.data(), path_len));
    }
    return {};
}




int main(void)
{


	//skeleton_load("D:/github/CoolEngine/obj/crab/crab.ani");
	//FBX
	//skeleton_load("D:\\GitHub\\CoolEngine\\render\\obj\\footman\\Footman_Run.fbx");

	//TestMathmain();
	//TestTrianglemain();
	//return 0;

	//5种模式： 线框、填充、透视顶点插值颜色、透视面法线、透视顶点法线  
	int states[] = { RENDER_STATE_WIREFRAME,RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_surfaceNormal_color ,RENDER_STATE_verterNormal_color };

	char * states_text[] = { RENDER_STATE_WIREFRAME_TEXT,RENDER_STATE_TEXTURE_TEXT, RENDER_STATE_COLOR_TEXT, RENDER_STATE_surfaceNormal_color_TEXT ,RENDER_STATE_verterNormal_color_TEXT };

	int indicator = 0; //模式的数组索引，避免越界
	int kbhit = 0;//一个bool值  保证每一帧只执行一次键盘输入
	float alpha = 1; //当前物体的旋转弧度。  1 就是 180度
	float beta = 1;
	TCHAR *title = _T("coolRender -- ")_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");

	int width = 1280;
	int height = 720;

	if (screen_init(width, height, title)) //初始化WINDOWS窗口并设置标题
		return -1;

	device_t device;
	device_init(&device, width, height, screen_fb); //设备初始

	CreateCamera(&device, width, height);//初始化主摄像机

   	Init_ShadowCamera(&device);//阴影摄像机

	Create_Obj();//初始化场景里的物体


	std::filesystem::path target_path;
	try {
		std::filesystem::path exe_path = getExecutablePath();
        if (exe_path.empty()) {
            std::cerr << "Could not determine executable path." << std::endl;
            return 1;
        }

        std::cout << "Executable path: " << exe_path << std::endl;

		target_path = exe_path.parent_path().parent_path().parent_path().parent_path()/ "obj";
        std::cout << "Calculated target path (using exe path): " << target_path << std::endl;

	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
	std::string path_str = target_path.u8string();

	// 创建一个足够大的 char 缓冲区，包括 null 终止符
	std::vector<char> buffer(path_str.length() + 1);

	// 复制字符串内容到缓冲区
	// path_str.c_str() 返回一个 const char* 指向内部字符串
	std::strcpy(buffer.data(), path_str.c_str());

	// 现在你可以将 buffer.data() 作为 char* 传递给 C 风格的 API
	char* rootPath = buffer.data();



	//char* rootPath;
	char buffer2[200];

	//getcwd()会将当前工作目录的绝对路径复制到参数buffer所指的内存空间中,参数maxlen为buffer的空间大小。 也可以将buffer作为输出参数
	//if ((rootPath = getcwd(NULL, 0)) == NULL)
	//{
	//	perror("getcwd error");
	//}
	//else
	{
		char src[200] = "/african_head/african_head.obj";
			
		//	"/diablo3_pose/diablo3_pose.obj";

		//需要拷贝到数组再操作，不能直接操作字符串常量char*
		strcpy(buffer2, rootPath);

		strcat(buffer2, src);
		//getcwd是C的标准库API，内部用的malloc而不是new
		//free(rootPath);
	}


	printf("load model... %s\n", buffer2);
	Model* lModel = new Model(buffer2);

	//每个物理实例化一个shader
	/*for (int i = 0; i < Scene_render_Objs.size(); i++)
	{
		draw_Object(Scene_render_Objs[i], &device, tShader);
	}*/


	Shader* tShader = new Shader();
	InitShader(tShader,device,lModel);


	LPCSTR type = "默认渲染模式";

	char *renderType = RENDER_STATE_WIREFRAME_TEXT;
	float fps = 0;
	char out[200];

	int ifCull = 0;

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0)
	{
		//start = clock();

		fps = Get_FPS();

		screen_dispatch(); //分发msg
		device_clear(&device, 1); //清空缓冲
		//device_clear(&device); // Zbuffer frameBuffer

		if (screen_keys[VK_UP]) camera_main.eye.z += 0.1f; //摄像机前进  pos -= 0.01f;
		if (screen_keys[VK_DOWN]) camera_main.eye.z -= 0.1f; //摄像机后退
		if (screen_keys[VK_LEFT]) alpha += 0.1f; //物体向左旋转
		if (screen_keys[VK_RIGHT]) alpha -= 0.1f; //物体向右旋转


		if (screen_keys[0x41]) beta -= 0.1f; //欧拉角
		if (screen_keys[0x42]) beta += 0.1f; //欧拉角

		camera_update(&camera_main); //摄像机不断更新矩阵
		//动态灯光 阴影
		camera_updateShadow(&cameras[0], &camera_main);
		cameras[0].width = width;
		cameras[0].height = height;

		if (screen_keys[VK_F1])
		{
			ifCull = 0;
		}
		else if (screen_keys[VK_F2])
		{
			ifCull = 1;
		}
		else if (screen_keys[VK_F3])
		{
			ifCull = 2;
		}
		
		if (screen_keys[VK_SPACE])
		{
			if (kbhit == 0)//保证每次按下只执行一次键盘输入
			{
				kbhit = 1;
				if (++indicator >= 5)
					indicator = 0;
				device.render_state = states[indicator];
				renderType = states_text[indicator];
			}		
		}
		else
		{
			kbhit = 0;
		}

		//渲染一个立方体 
		//draw_box(&device, alpha);

		// 渲染物体,更新设备
		tShader->transform.screen_width = (float)width;
		tShader->transform.screen_height = (float)height;
		tShader->render_state = device.render_state;
		//1.初始化物体的矩阵跟属性
		for (int i = 0; i < Scene_render_Objs.size(); i++)
		{
			Scene_render_Objs[i].axis.y = alpha;
			Init_Obj(Scene_render_Objs[i]);
		}

		//初始化每个shader

		tShader->curCamera = camera_main;
		tShader->cull = ifCull;
		//if (device.shadowbuffer != NULL) {
		//	for (int y = 0; y < device.height; y++)
		//		for (int x = 0; x < device.width; x++)
		//			device.shadowbuffer[y * device.width + x] = 0;
		//}

	//	finish = clock();
	//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
		//printf("ready for render ----- %f seconds\n", duration);


		//渲染阴影的深度缓冲
		//for (int i = 0; i < Scene_render_Objs.size(); i++)
		//{
		//	draw_Object_Shadow(Scene_render_Objs[i], &tShader, &device);
		//}

		tShader->transform.worldRot.y = alpha;

		//3.渲染物体
		draw_Model(&device, tShader);

		for (int i = 0; i < Scene_render_Objs.size(); i++)
		{
			draw_Object(Scene_render_Objs[i], &device, tShader);
		}

		sprintf(out, "%3.1f", fps);
		//strcpy(out, renderType);//将第一个字符串p拷贝到q中  
		strcat(out, renderType);//将第二个字符串h拼接到q之后

		//string outText = out;

		char c[8];
		itoa(ifCull, c, 16);
		strcat(out, c);
		type = (LPCSTR)out;
		//Set2RayTracer();
		//RaytracerIT();
		//真正的渲染函数

		//start = clock();

		screen_update(type);

		//finish = clock();
		//duration = (double)(finish - start) / CLOCKS_PER_SEC;
		//printf("screen_update----- %f seconds\n", duration);

		Sleep(1);
	}

	device_destroy(&device);
	delete(tShader);
	//static_device_t = nullptr;
	return 0;
}



void Control(WPARAM wParam, LPARAM lParam)
{
	int mouse_x = (int)LOWORD(lParam);
	int mouse_y = (int)HIWORD(lParam);
	int button = (int)wParam;
	HDC hDC = GetDC(screen_handle);
	if (button&MK_LBUTTON)
	{
		TextOut(hDC, 0, 16, "3D渲染demo", strlen("3D渲染demo"));
	}
}