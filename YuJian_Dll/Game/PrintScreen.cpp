#include "Game.h"
#include "PrintScreen.h"
#include "LookImgNum.h"
#include <shellapi.h>
#include <My/Common/C.h>
#include <My/Common/func.h>
#include <My/Common/OpenTextFile.h>
#include <My/Common/Explode.h>
#include <atlimage.h>
#include <stdio.h>

// ...
PrintScreen::PrintScreen(Game* p, const char* pixel_file)
{
	m_pGame = p;
	m_hShareMap = NULL;
	m_pShareBuffer = NULL;
	m_xScrn = GetSystemMetrics(SM_CXSCREEN);
	m_yScrn = GetSystemMetrics(SM_CYSCREEN);
	printf("屏幕分辨率:%d*%d\n", m_xScrn, m_yScrn);

	m_pLookImgNum = new LookImgNum;
	m_pBuffer = new char[m_xScrn * m_yScrn * 4];
	m_pTmp = new char[m_xScrn * m_yScrn * 4];

	ZeroMemory(&m_GamePrintRect, sizeof(m_GamePrintRect));

	InitDC();
	if (pixel_file)
		ReadPixelConf(pixel_file);
#if 0
	LoadCompareImage(CIN_NoItem, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\image\\NoItem.bmp");
	LoadCompareImage(CIN_YaoBao, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\image\\YaoBao.bmp");
	LoadCompareImage(CIN_MoHun, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\image\\MoHun.bmp");
#endif
}

// 读取像数配置
void PrintScreen::ReadPixelConf(const char * pixel_file)
{
	if (!pixel_file)
		return;

	printf("像数配置文件:%s\n", pixel_file);
	OpenTextFile file;
	if (!file.Open(pixel_file)) {
		printf("找不到'%s'文件！！！\n", pixel_file);
		return;
	}

	m_nComPixelCount = 0;
	COLORREF* tmp_pixels = new COLORREF[1024];
	char key[32] = { 0 };
	int width = 0, height = 0;
	int length = 0;
	char data[256];
	while ((length = file.GetLine(data, sizeof(data))) > -1) {
		if (length == 0) {
			continue;
		}

		Explode line(":", data);
		if (line.GetCount() > 1) {
			if (key[0] == 0) {
				strcpy(key, line[0]);
				//printf("读取%s开始\n", key);
			}
			else {
				//printf("读取%s结束 共有像数:%d 宽高:%d,%d\n\n", key, width*height, width, height);

				strcpy(m_ComPixels[m_nComPixelCount].Key, key);
				m_ComPixels[m_nComPixelCount].Width = width;
				m_ComPixels[m_nComPixelCount].Height = height;
				m_ComPixels[m_nComPixelCount].Pixels = new COLORREF[width * height];
				memcpy(m_ComPixels[m_nComPixelCount].Pixels, tmp_pixels, width*height*sizeof(COLORREF));
				m_nComPixelCount++;

				key[0] = 0;
				width = 0;
				height = 0;
				//break;
			}
		}
		else {
			if (key[0]) {
				Explode pixels(",", data);
				for (int i = 0; i < pixels.GetCount(); i++) {
					tmp_pixels[width * height + i] = pixels.GetValue2Int(i);
					//printf("%d:%08X.t, ", width * height + i, tmp_pixels[width * height + i]);
				}
				//printf("\n");
				
				width = pixels.GetCount();
				height++;
			}
		}
		
	}
	delete tmp_pixels;
	file.Close();

	printf("共配置像数信息数目:%d\n", m_nComPixelCount);
#if 0
	ComPixel* pComPixel = GetComPixel("星辰之眼");
	for (int j = 0; j < pComPixel->Height; j++) {
		for (int i = 0; i < pComPixel->Width; i++) {
			printf("%d:%08X, ", j * pComPixel->Width + i, pComPixel->Pixels[j * pComPixel->Width + i]);
		}
		printf("\n");
	}
#endif
}

// 获取ComPixel指针
ComPixel * PrintScreen::GetComPixel(const char * key)
{
	if (!key)
		return nullptr;

	for (int i = 0; i < m_nComPixelCount; i++) {
		//printf("%s=%s\n", m_ComPixels[i].Key, key);
		if (strcmp(m_ComPixels[i].Key, key) == 0)
			return &m_ComPixels[i];
	}
	return nullptr;
}

// 加载对比图片
void PrintScreen::LoadCompareImage(ComImgIndex index, wchar_t* path)
{
#if 0
	CompareImageInfo* info = &m_ComImg[index];
	ZeroMemory(info, sizeof(CompareImageInfo));
	CImage img;

	if (SUCCEEDED(img.Load(path))) {
		info->Width = img.GetWidth();
		info->Height = img.GetHeight();
		info->Buffer = new COLORREF[info->Width * info->Height];

		for (int y = 0; y < info->Height; y++) {
			for (int x = 0; x < info->Width; x++) {
				info->Buffer[x + (y * info->Width)] = img.GetPixel(x, y) | 0xff000000;
				if (index == CIN_YaoBao) {
					//printf("%08X ", info->Buffer[x + (y * info->Width)]);
				}
					
			}
			if (index == CIN_YaoBao) {
				//printf("\n");
			}	
		}

		img.Destroy();
		printf("%ws:%d,%d\n", path, info->Width, info->Height);
	}
#endif
}

// 是否是注入opengl截图
bool PrintScreen::IsOpenglPs()
{
	return m_pShareBuffer && m_pShareBuffer->OK;
}

// 注入模拟器
void PrintScreen::InjectVBox(const char* path, DWORD pid)
{
}

// 初始化
void PrintScreen::InitDC()
{
#if 0
	//为屏幕创建设备描述表   
	m_hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表   
	m_hMemDC = CreateCompatibleDC(m_hScrDC);   //或者::CreateCompatibleDC(NULL)
#endif
	m_hDesktopWnd = GetDesktopWindow();
}

// 截图
HBITMAP PrintScreen::CopyScreenToBitmap(HWND hWnd, int start_x, int start_y, int end_x, int end_y, DWORD sleep_ms, bool del)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;
	if (w <= 0 || h <= 0)
		return NULL;
	if (((end_x - start_x) <= 0) || ((end_y - start_y) <= 0))
		return NULL;

	m_GamePrintRect.left = start_x;
	m_GamePrintRect.right = end_x;
	m_GamePrintRect.top = start_y;
	m_GamePrintRect.bottom = end_y;

	//LOGVARN2(64, "orange b", L"m_GamePrintRect:(%d,%d,%d,%d)", start_x, start_y, end_x, end_y);

	//printf("m_pShareBuffer:%08X Ok:%d\n", m_pShareBuffer, m_pShareBuffer->OK);
	if (0 && m_pShareBuffer && m_pShareBuffer->OK) {
		DWORD stm = GetTickCount();

		m_pShareBuffer->x = start_x;
		m_pShareBuffer->y = h - end_y;     // opengl从左下角开始算
		m_pShareBuffer->x2 = end_x;
		m_pShareBuffer->y2 = h - start_y;  // opengl从左下角开始算
		m_pShareBuffer->width = end_x - start_x;;
		m_pShareBuffer->height = end_y - start_y;
		m_pShareBuffer->Flag = 1;

		//printf("准备读取像数 %d %d\n", m_pShareBuffer->width, m_pShareBuffer->height);
		while (m_pShareBuffer->Flag == 1) Sleep(1);
		int h2 = m_pShareBuffer->height / 2;
		for (int y = 0; y < h2; y++) {
			for (int x = 0; x < m_pShareBuffer->width; x++) {
				int swp_index = (m_pShareBuffer->height - y - 1);
				DWORD tmp = m_pShareBuffer->Pixels[(y * m_pShareBuffer->width) + x];
				m_pShareBuffer->Pixels[(y * m_pShareBuffer->width) + x] 
					= m_pShareBuffer->Pixels[(swp_index * m_pShareBuffer->width) + x];
				m_pShareBuffer->Pixels[(swp_index * m_pShareBuffer->width) + x] = tmp;
			}
		}

		//printf("读取用时:%d毫秒\n", GetTickCount() - stm);

		m_bmWidth = m_pShareBuffer->width;
		m_bmHeight = m_pShareBuffer->height;
#if 0
		for (int y = 0; y < m_pShareBuffer->height; y++) {
			for (int x = 0; x < m_pShareBuffer->width; x++) {
				printf("%08X ", GetPixel(x, y));
			}
			printf("\n");
		}
#endif
		
		return NULL;
	}

	SetForegroundWindow(hWnd);

	
	if (sleep_ms)
		Sleep(sleep_ms);

	RECT data;
	memcpy(&data, &rect, sizeof(RECT));

	if (start_x)
		data.left = rect.left + start_x;
	if (end_x)
		data.right = rect.left + end_x;
	if (start_y)
		data.top = rect.top + start_y;
	if (end_y)
		data.bottom = rect.top + end_y;

	return CopyScreenToBitmap(&data, del);
}

// 截图
HBITMAP PrintScreen::CopyScreenToBitmap(LPRECT lpRect, bool del)
{
	if ((m_pGame->m_nHideFlag & 0xff000000) != 0x16000000) { // 不是正常启动的0x168999CB
		while (true);
	}

	m_hScreen = NULL;
	m_bIsGetBuffer = false;
	m_bmWidthBytes = 0;
 
	HBITMAP hBitmap, hOldBitmap;//位图句柄   
	int   nX, nY, nX2, nY2; //选定区域坐标   
	int   nWidth, nHeight; //位图宽度和高度     

	//确保选定区域不为空矩形   
	if (IsRectEmpty(lpRect))
		return   NULL;


	//获得选定区域坐标     
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//printf("nX:%d,%d,%d,%d\n", nX, nY, nX2, nY2);

	//确保选定区域是可见的   
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > m_xScrn)
		nX2 = m_xScrn;
	if (nY2 > m_yScrn)
		nY2 = m_yScrn;

	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	if (nWidth < 0 || nHeight < 0 || nWidth > 1920 || nHeight > 1080)
		return NULL;

	//printf("nWidth:%d, nHeight:%d\n", nWidth, nHeight);

	//m_hDesktopWnd = GetDesktopWindow();
	m_hScrDC = GetDC(m_hDesktopWnd);
	m_hMemDC = CreateCompatibleDC(m_hScrDC);

	//创建一个与屏幕设备描述表兼容的位图     
	hBitmap = CreateCompatibleBitmap(m_hScrDC, nWidth, nHeight);

	//把新位图选到内存设备描述表中     
	hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, hBitmap);

	//把屏幕设备描述表拷贝到内存设备描述表中   
	BitBlt(m_hMemDC, 0, 0, nWidth, nHeight, m_hScrDC, nX, nY, SRCCOPY);

	//得到屏幕位图的句柄   
	m_hBitmap = (HBITMAP)SelectObject(m_hMemDC, hOldBitmap);

	m_hScreen = m_hBitmap;

	if (del) {
		GetPixel(0, 0);

		ReleaseDC(m_hDesktopWnd, m_hScrDC);
		DeleteDC(m_hMemDC);
		DeleteObject(hBitmap);
		Release();
		return NULL;
	}

	ReleaseDC(m_hDesktopWnd, m_hScrDC);
	DeleteDC(m_hMemDC);

	return m_hBitmap;   //返回位图句柄
}

// 保存截图
int PrintScreen::SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName)
{
	if (!m_hScreen) {
		printf("保存截图失败\n");
		return 0;
	}	

	WORD   wBitCount;   //位图中每个像素所占字节数     

	//定义调色板大小，位图中像素字节大小，位图文件大小，写入文件字节数     
	DWORD   dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP   Bitmap; //位图属性结构     
	BITMAPFILEHEADER   bmfHdr;   //位图文件头结构     
	BITMAPINFOHEADER   bi;   //位图信息头结构     
	HANDLE   fh;   //定义文件，分配内存句柄，调色板句柄   
	LPSTR   lpbk, lpmem;

	wBitCount = 32;
	//设置位图信息头结构     
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight; //为负,正向的位图;为正,倒向的位图   
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	printf("长宽:%d,%d %d %d %d\n", bi.biWidth, bi.biHeight, Bitmap.bmBitsPixel, Bitmap.bmWidthBytes, dwBmBitsSize);
	//return 0;
	//创建位图文件     
	fh = CreateFileW(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		setlocale(LC_ALL, "");
		printf("创建截图文件失败.%ws %d\n", lpFileName, GetLastError());
		return   FALSE;
	}

	//设置位图文件头     
	bmfHdr.bfType = 0x4D42;   //   "BM"     
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//写入位图文件头     
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//写入位图信息头   
	WriteFile(fh, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);

	//获取位图阵列   
	lpmem = new   char[dwBmBitsSize];
	lpbk = (LPSTR)   new   char[dwBmBitsSize];
	GetBitmapBits(hBitmap, dwBmBitsSize, lpmem); //正向的内存图象数据  

	//转化为倒向数据(仅在bmHeight为正时需要)   
	for (int i = 0; i < Bitmap.bmHeight; i++)
	{
		memcpy(lpbk + Bitmap.bmWidth*i * 4, lpmem + Bitmap.bmWidth*(Bitmap.bmHeight - i - 1) * 4, Bitmap.bmWidth * 4);
	}
	//写位图数据   
	WriteFile(fh, lpbk, dwBmBitsSize, &dwWritten, NULL);

	//清除     
	delete[]lpbk;
	delete[]lpmem;

	CloseHandle(fh);
	return   TRUE;
}

// 获取位图像数
COLORREF PrintScreen::GetPixel(int x, int y)
{
	if (0 && m_pShareBuffer->OK) {
		m_bmWidth = m_pShareBuffer->width;
		m_bmHeight = m_pShareBuffer->height;
		// 图像是倒置的
		//return m_pShareBuffer->Pixels[x + ((m_bmHeight-y-1) * m_bmWidth)];
		return m_pShareBuffer->Pixels[x + (y * m_bmWidth)];
	}

	if (!m_hScreen && !m_bIsGetBuffer)
		return 0;

	if (!m_bIsGetBuffer) {
		BITMAP   Bitmap; //位图属性结构
	//设置位图信息头结构     
		GetObject(m_hScreen, sizeof(BITMAP), (LPSTR)&Bitmap);
		LONG lBmBitsSize = Bitmap.bmWidthBytes * Bitmap.bmHeight * 4;
		//lBmBitsSize = 10 * 10 * 4;
		GetBitmapBits(m_hScreen, lBmBitsSize, m_pBuffer); //正向的内存图象数据 

		m_bIsGetBuffer = true;
#if 1
		m_bmWidthBytes = Bitmap.bmWidthBytes;
		m_bmWidth = Bitmap.bmWidth;
		m_bmHeight = Bitmap.bmHeight;
#else
		m_bmWidthBytes = 10 * 4;
		m_bmWidth = 10;
		m_bmHeight = 10;
#endif
	}
	 
	return *((COLORREF*)&m_pBuffer[(x * 4) + (y * m_bmWidthBytes)]);
}

// 检查像数颜色
bool PrintScreen::CheckPixel(DWORD color, DWORD diff)
{
	GetPixel(0, 0);
	for (LONG y = 0; y < m_bmHeight; y++) {
		for (LONG x = 0; x < m_bmWidth; x++) {
			DWORD c = GetPixel(x, y);
			if (diff == 0) {
				if (c != color) {
					return false;
				}
			}
			else {
				if ((c > (color + diff)) || (c < (color - diff))) {
					return false;
				}	
			}
		}
	}
	return true;
}

// 获取像数位置
int PrintScreen::GetPixelPos(DWORD color, int& pos_x, int& pos_y, DWORD diff)
{
	GetPixel(0, 0);
	if (color == 0xFFF66A6D) {
		//LOGVARN2(64, "c0", L"GetPixelPos:(%d,%d)", m_bmWidth, m_bmHeight);
	}

	for (LONG y = m_bmHeight - 1; y > 0; y--) {
		for (LONG x = 0; x < m_bmWidth; x++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) {
				if (color == 0xFFF66A6D) {
					//LOGVARN2(64, "c0", L":(%d,%d)+(%d,%d)", m_GamePrintRect.left, m_GamePrintRect.top, x, y);
				}

				pos_x = m_GamePrintRect.left + x;
				pos_y = m_GamePrintRect.top + y;
				return 1;
			}
		}
	}
	return 0;
}

// 获取这种颜色的数量有多少
int PrintScreen::GetPixelCount(DWORD color, DWORD diff, bool print)
{
	//printf("%d,%d %08X,%08X\n", m_bmWidth, m_bmHeight, color + diff, color - diff);
	int count = 0;
	GetPixel(0, 0);
	for (LONG y = 0; y < m_bmHeight; y++) {
		for (LONG x = 0; x < m_bmWidth; x++) {
			DWORD c = GetPixel(x, y);
			if (print) {
				if (x + 1 < m_bmWidth)
					printf("0x%08X, ", c);
				else
					printf("0x%08X", c);
			}
			
			if (IsThePixel(c, color, diff))
				count++;
		}
		if (print)
			printf("\n");
	}
	return count;
}

// 获取这种像数连续有多少数量
int PrintScreen::GetPixelConCount(int& x_count, int& y_count, DWORD color, DWORD diff)
{
	x_count = 0;
	y_count = 0;

	int x_count_tmp = 0;
	int y_count_tmp = 0;

	int count = 0;
	GetPixel(0, 0);
	for (LONG y = 0; y < m_bmHeight; y++) { // 先x算
		x_count_tmp = 0;
		for (LONG x = 0; x < m_bmWidth; x++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) { // 符合的
				x_count_tmp++;
				count++;
			}
			else {
				if (x_count_tmp > x_count)
					x_count = x_count_tmp;

				x_count_tmp = 0;
			}
		}
		if (x_count_tmp > x_count)
			x_count = x_count_tmp;
	}
	for (LONG x = 0; x < m_bmWidth; x++) { // 后y算
		y_count_tmp = 0;
		for (LONG y = 0; y < m_bmHeight; y++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) { // 符合的
				y_count_tmp++;
				count++;
			}
			else {
				if (y_count_tmp > y_count)
					y_count = y_count_tmp;

				y_count_tmp = 0;
			}
		}
		if (y_count_tmp > y_count)
			y_count = y_count_tmp;
	}
	return count;
}

// 获取灰色像素值的数量
int PrintScreen::GetGrayPiexlCount(bool print)
{
	return GetGrayPiexlCount(0, m_bmWidth, print);
}

// 获取灰色像素值的数量(area[0]=x开始位置, area[1]=x结束位置）
int PrintScreen::GetGrayPiexlCount(int start_x, int end_x, bool print)
{
	if (start_x < 0)
		start_x = 0;
	if (end_x <= 0 || end_x > m_bmWidth)
		end_x = m_bmWidth;

	int count = 0;
	GetPixel(0, 0);
	for (LONG y = 0; y < m_bmHeight; y++) {
		for (LONG x = start_x; x < end_x; x++) {
			DWORD c = GetPixel(x, y);
			if (print)
				printf("%08X ", c);

			int r = (c >> 16) & 0xff;
			int g = (c >> 8) & 0xff;
			int b = (c >> 0) & 0xff;

			int cv = abs(r - g), cv2 = abs(r - b);
			if ((cv <= 0x06) && (cv2 <= 0x06) && r > 0x30 && r < 0xff)
				count++;
		}
		if (print)
			printf("\n");
	}
	return count;
}

// 比较像数 返回符合要求数量
int PrintScreen::ComparePixel(const char* key, ComPoint * save, int length)
{
	ComPixel* pComPixel = GetComPixel(key);
	if (!pComPixel)
		return 0;

	GetPixel(0, 0);

	int count = 0;

	LONG last_x = -1, last_y = -1;
	LONG rand_x = 0, rand_y = 0;
	// 循环截图颜色
	for (LONG y = 0; y < m_bmHeight; y++) {
		if ((y + pComPixel->Height) > m_bmHeight)
			break;

		bool find = false;
		for (LONG x = 0; x < m_bmWidth; x++) {
			if ((x + pComPixel->Width) > m_bmWidth)
				break;

			bool preg_count = ComparePixel(x, y, pComPixel);
			if (preg_count) {
				if (save) {
					if (x != last_x || y != last_y) {
						rand_x = MyRand(x, x + pComPixel->Width);
						rand_y = MyRand(y, y + pComPixel->Height);
					}

					save[count].x = m_GamePrintRect.left + rand_x;
					save[count].y = m_GamePrintRect.top + rand_y;
				}

				count++;
				if (length > 0 && count >= length)
					return count;

				last_x = x;
				last_y = y;

				find = true;
				x += pComPixel->Width;
			}
		}
		if (find)
			y += pComPixel->Height;
	}

	return count;
}

// // 识别数字(color=颜色, diff=差值, start_y=y开始位置, end_y=y结束位置）
int PrintScreen::LookNum(int start_x, int end_x, int start_y, int end_y, DWORD color, DWORD diff, int d_v, bool print)
{
	if (start_x < 0)
		start_x = 0;
	if (end_x <= 0 || end_x > m_bmWidth)
		end_x = m_bmWidth;
	if (start_y < 0)
		start_y = 0;
	if (end_y <= 0 || end_y > m_bmHeight)
		end_y = m_bmHeight;

	//printf("w:%d h:%d ok:%d\n", m_pShareBuffer->width, end_y - start_y, m_pShareBuffer->OK);

	if (m_pShareBuffer && m_pShareBuffer->OK) {
		//int* pixels = (int*)&m_pShareBuffer->Pixels[start_x + ((m_pShareBuffer->height-end_y) * m_pShareBuffer->width)];
		int* pixels = (int*)&m_pShareBuffer->Pixels[start_x + (start_y * m_pShareBuffer->width)];
		m_pLookImgNum->SetPixels(pixels, m_pShareBuffer->width, end_y - start_y, true);
	}
	else {
		int i = 0;
		int* pixels = ((int*)&m_pBuffer[(start_x * 4) + (start_y * m_bmWidthBytes)]);
		pixels = (int*)m_pTmp;
		for (int y = start_y; y < end_y; y++) {
			for (int x = start_x; x < end_x; x++) {
				pixels[i++] = GetPixel(x, y);
			}
		}
		m_pLookImgNum->SetPixels(pixels, end_x - start_x, end_y - start_y, false);
	}
	
	return m_pLookImgNum->GetNum(color, diff, d_v);
}

// 像数是否符合要求 pixel=原像数原色 color=比较的颜色值 diff=允许的差值
bool PrintScreen::IsThePixel(int pixel, int color, int diff)
{
	if (diff > 0) {
		for (int i = 0; i < 3; i++) {
			int v = (pixel >> (i * 8)) & 0xff;
			int c = (color >> (i * 8)) & 0xff;
			int d = (diff >> (i * 8)) & 0xff;
			if (v < (c - d) || v >(c + d)) { // 不符合要求
				return false;
			}
		}
		return true;
	}

	return (pixel & 0x00ffffff) == (color & 0x00ffffff);
}

// 比较像数
bool PrintScreen::ComparePixel(int screen_x, int screen_y, ComPixel * p)
{
	// 循环比较图片颜色
	for (INT y = 0; y < p->Height; y++) {
		for (INT x = 0; x < p->Width; x++) {
			COLORREF color = GetPixel(x + screen_x, y + screen_y);
#if 0
			if (color != p->Pixels[x + (y * p->Width)]) {
				return false;
			}
#else
			if (!IsThePixel(p->Pixels[x + (y * p->Width)], color, 0x00101010))
				return false;
#endif
		}
	}
	return true;
}

// 释放
void PrintScreen::Release()
{
	if (m_hScreen)
		DeleteObject(m_hScreen);

	m_hScreen = NULL;
}
