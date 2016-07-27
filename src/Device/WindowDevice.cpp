#include <./Device/WindowDevice.h>
extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//後から修正(各Windowデバイスごとにイベント処理できるように変更)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplDX11_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
bool func(HWND, UINT, WPARAM, LPARAM);
WindowDevice::WindowDevice() {
	this->runflg = true;
}
WindowDevice::~WindowDevice() {
}

unsigned long WindowDevice::Init(HINSTANCE ins, int ncms, std::string title, unsigned long width, unsigned long height, bool windowmode) {
	unsigned long ret = 0;

	static std::string classname = (this->title + TEXT("_CLASS"));
	// ウィンドウクラス構造体を設定します。
	this->wcex.cbSize = sizeof(WNDCLASSEX);
	this->wcex.style = CS_CLASSDC;
	this->wcex.lpfnWndProc = (WNDPROC)WndProc;
	this->wcex.cbClsExtra = 0;
	this->wcex.cbWndExtra = 0;
	this->wcex.hInstance = ins;
	this->wcex.hIcon = nullptr;
	this->wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	this->wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	this->wcex.lpszMenuName = nullptr;
	this->wcex.lpszClassName = classname.c_str();
	this->wcex.hIconSm = nullptr;
	this->title = title;

	if (!RegisterClassEx(&this->wcex)) {
		ret = -1;
		return ret;
	}

	//スタイル等の設定を後から自由に設定できるように変更
	this->handle = CreateWindowEx(
		0,
		classname.c_str(),
		this->title.c_str(),
		WS_OVERLAPPEDWINDOW,
		0, 0,
		width + GetSystemMetrics(SM_CXSIZEFRAME) * 2,
		height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME),
		nullptr,
		nullptr,
		ins,
		nullptr);

	if (!this->handle) {
		ret = -2;
		return ret;
	}

	this->windowmode = windowmode;
	this->width = width;
	this->height = height;
	this->ReSize(this->width, this->height);
	ShowWindow(this->handle, ncms);
	UpdateWindow(this->handle);

	return ret;
}

ULONG WindowDevice::MessageLoop() {
	unsigned long ret = 0;
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			this->runflg = false;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message;
}

void WindowDevice::ReSize(ULONG sx, ULONG sy) {
	this->width = sx;
	this->height = sy;

	RECT rc1;
	RECT rc2;

	GetWindowRect(this->handle, &rc1);
	GetClientRect(this->handle, &rc2);
	sx += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	sy += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(this->handle, NULL, 0, 0, sx, sy, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
}
void WindowDevice::setTitle(std::string title) {
	SetWindowText(this->handle, this->title.c_str());
	this->title = title;
}
std::string WindowDevice::getTitle() {
	return this->title;
}

