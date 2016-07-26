#ifndef _HEADER_GFW_WINDOW_
#define _HEADER_GFW_WINDOW_
#pragma warning(push)
#include <Windows.h>
#include <string>


class WindowDevice {
public:
	virtual ~WindowDevice();
	static WindowDevice &getInstance() {
		static WindowDevice ins;
		return ins;
	}
	unsigned long Init(HINSTANCE,int,std::string, unsigned long, unsigned long,bool = true);

	void setTitle(std::string);
	std::string getTitle();
	void ReSize(ULONG, ULONG);
	bool getRunFlg() { return this->runflg; }
	ULONG MessageLoop();
	HWND getHandle() { return this->handle; }
	unsigned long getWidth() { return this->width; }
	unsigned long getHeight() { return this->height; }
	bool getWindowMode() { return this->windowmode; }
protected:
private:
	WindowDevice();

	HWND handle;
	WNDCLASSEX wcex;
	std::string title;
	bool windowmode;
	UINT width, height;

	volatile bool runflg;
};

#pragma warning(pop)
#endif