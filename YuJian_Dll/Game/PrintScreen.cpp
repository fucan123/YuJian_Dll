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
	printf("��Ļ�ֱ���:%d*%d\n", m_xScrn, m_yScrn);

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

// ��ȡ��������
void PrintScreen::ReadPixelConf(const char * pixel_file)
{
	if (!pixel_file)
		return;

	printf("���������ļ�:%s\n", pixel_file);
	OpenTextFile file;
	if (!file.Open(pixel_file)) {
		printf("�Ҳ���'%s'�ļ�������\n", pixel_file);
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
				//printf("��ȡ%s��ʼ\n", key);
			}
			else {
				//printf("��ȡ%s���� ��������:%d ���:%d,%d\n\n", key, width*height, width, height);

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

	printf("������������Ϣ��Ŀ:%d\n", m_nComPixelCount);
#if 0
	ComPixel* pComPixel = GetComPixel("�ǳ�֮��");
	for (int j = 0; j < pComPixel->Height; j++) {
		for (int i = 0; i < pComPixel->Width; i++) {
			printf("%d:%08X, ", j * pComPixel->Width + i, pComPixel->Pixels[j * pComPixel->Width + i]);
		}
		printf("\n");
	}
#endif
}

// ��ȡComPixelָ��
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

// ���ضԱ�ͼƬ
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

// �Ƿ���ע��opengl��ͼ
bool PrintScreen::IsOpenglPs()
{
	return m_pShareBuffer && m_pShareBuffer->OK;
}

// ע��ģ����
void PrintScreen::InjectVBox(const char* path, DWORD pid)
{
}

// ��ʼ��
void PrintScreen::InitDC()
{
#if 0
	//Ϊ��Ļ�����豸������   
	m_hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������   
	m_hMemDC = CreateCompatibleDC(m_hScrDC);   //����::CreateCompatibleDC(NULL)
#endif
	m_hDesktopWnd = GetDesktopWindow();
}

// ��ͼ
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
		m_pShareBuffer->y = h - end_y;     // opengl�����½ǿ�ʼ��
		m_pShareBuffer->x2 = end_x;
		m_pShareBuffer->y2 = h - start_y;  // opengl�����½ǿ�ʼ��
		m_pShareBuffer->width = end_x - start_x;;
		m_pShareBuffer->height = end_y - start_y;
		m_pShareBuffer->Flag = 1;

		//printf("׼����ȡ���� %d %d\n", m_pShareBuffer->width, m_pShareBuffer->height);
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

		//printf("��ȡ��ʱ:%d����\n", GetTickCount() - stm);

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

// ��ͼ
HBITMAP PrintScreen::CopyScreenToBitmap(LPRECT lpRect, bool del)
{
	if ((m_pGame->m_nHideFlag & 0xff000000) != 0x16000000) { // ��������������0x168999CB
		while (true);
	}

	m_hScreen = NULL;
	m_bIsGetBuffer = false;
	m_bmWidthBytes = 0;
 
	HBITMAP hBitmap, hOldBitmap;//λͼ���   
	int   nX, nY, nX2, nY2; //ѡ����������   
	int   nWidth, nHeight; //λͼ��Ⱥ͸߶�     

	//ȷ��ѡ������Ϊ�վ���   
	if (IsRectEmpty(lpRect))
		return   NULL;


	//���ѡ����������     
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;

	//printf("nX:%d,%d,%d,%d\n", nX, nY, nX2, nY2);

	//ȷ��ѡ�������ǿɼ���   
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

	//����һ������Ļ�豸��������ݵ�λͼ     
	hBitmap = CreateCompatibleBitmap(m_hScrDC, nWidth, nHeight);

	//����λͼѡ���ڴ��豸��������     
	hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, hBitmap);

	//����Ļ�豸�����������ڴ��豸��������   
	BitBlt(m_hMemDC, 0, 0, nWidth, nHeight, m_hScrDC, nX, nY, SRCCOPY);

	//�õ���Ļλͼ�ľ��   
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

	return m_hBitmap;   //����λͼ���
}

// �����ͼ
int PrintScreen::SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName)
{
	if (!m_hScreen) {
		printf("�����ͼʧ��\n");
		return 0;
	}	

	WORD   wBitCount;   //λͼ��ÿ��������ռ�ֽ���     

	//�����ɫ���С��λͼ�������ֽڴ�С��λͼ�ļ���С��д���ļ��ֽ���     
	DWORD   dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP   Bitmap; //λͼ���Խṹ     
	BITMAPFILEHEADER   bmfHdr;   //λͼ�ļ�ͷ�ṹ     
	BITMAPINFOHEADER   bi;   //λͼ��Ϣͷ�ṹ     
	HANDLE   fh;   //�����ļ��������ڴ�������ɫ����   
	LPSTR   lpbk, lpmem;

	wBitCount = 32;
	//����λͼ��Ϣͷ�ṹ     
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight; //Ϊ��,�����λͼ;Ϊ��,�����λͼ   
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	printf("����:%d,%d %d %d %d\n", bi.biWidth, bi.biHeight, Bitmap.bmBitsPixel, Bitmap.bmWidthBytes, dwBmBitsSize);
	//return 0;
	//����λͼ�ļ�     
	fh = CreateFileW(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		setlocale(LC_ALL, "");
		printf("������ͼ�ļ�ʧ��.%ws %d\n", lpFileName, GetLastError());
		return   FALSE;
	}

	//����λͼ�ļ�ͷ     
	bmfHdr.bfType = 0x4D42;   //   "BM"     
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//д��λͼ�ļ�ͷ     
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//д��λͼ��Ϣͷ   
	WriteFile(fh, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);

	//��ȡλͼ����   
	lpmem = new   char[dwBmBitsSize];
	lpbk = (LPSTR)   new   char[dwBmBitsSize];
	GetBitmapBits(hBitmap, dwBmBitsSize, lpmem); //������ڴ�ͼ������  

	//ת��Ϊ��������(����bmHeightΪ��ʱ��Ҫ)   
	for (int i = 0; i < Bitmap.bmHeight; i++)
	{
		memcpy(lpbk + Bitmap.bmWidth*i * 4, lpmem + Bitmap.bmWidth*(Bitmap.bmHeight - i - 1) * 4, Bitmap.bmWidth * 4);
	}
	//дλͼ����   
	WriteFile(fh, lpbk, dwBmBitsSize, &dwWritten, NULL);

	//���     
	delete[]lpbk;
	delete[]lpmem;

	CloseHandle(fh);
	return   TRUE;
}

// ��ȡλͼ����
COLORREF PrintScreen::GetPixel(int x, int y)
{
	if (0 && m_pShareBuffer->OK) {
		m_bmWidth = m_pShareBuffer->width;
		m_bmHeight = m_pShareBuffer->height;
		// ͼ���ǵ��õ�
		//return m_pShareBuffer->Pixels[x + ((m_bmHeight-y-1) * m_bmWidth)];
		return m_pShareBuffer->Pixels[x + (y * m_bmWidth)];
	}

	if (!m_hScreen && !m_bIsGetBuffer)
		return 0;

	if (!m_bIsGetBuffer) {
		BITMAP   Bitmap; //λͼ���Խṹ
	//����λͼ��Ϣͷ�ṹ     
		GetObject(m_hScreen, sizeof(BITMAP), (LPSTR)&Bitmap);
		LONG lBmBitsSize = Bitmap.bmWidthBytes * Bitmap.bmHeight * 4;
		//lBmBitsSize = 10 * 10 * 4;
		GetBitmapBits(m_hScreen, lBmBitsSize, m_pBuffer); //������ڴ�ͼ������ 

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

// ���������ɫ
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

// ��ȡ����λ��
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

// ��ȡ������ɫ�������ж���
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

// ��ȡ�������������ж�������
int PrintScreen::GetPixelConCount(int& x_count, int& y_count, DWORD color, DWORD diff)
{
	x_count = 0;
	y_count = 0;

	int x_count_tmp = 0;
	int y_count_tmp = 0;

	int count = 0;
	GetPixel(0, 0);
	for (LONG y = 0; y < m_bmHeight; y++) { // ��x��
		x_count_tmp = 0;
		for (LONG x = 0; x < m_bmWidth; x++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) { // ���ϵ�
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
	for (LONG x = 0; x < m_bmWidth; x++) { // ��y��
		y_count_tmp = 0;
		for (LONG y = 0; y < m_bmHeight; y++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) { // ���ϵ�
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

// ��ȡ��ɫ����ֵ������
int PrintScreen::GetGrayPiexlCount(bool print)
{
	return GetGrayPiexlCount(0, m_bmWidth, print);
}

// ��ȡ��ɫ����ֵ������(area[0]=x��ʼλ��, area[1]=x����λ�ã�
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

// �Ƚ����� ���ط���Ҫ������
int PrintScreen::ComparePixel(const char* key, ComPoint * save, int length)
{
	ComPixel* pComPixel = GetComPixel(key);
	if (!pComPixel)
		return 0;

	GetPixel(0, 0);

	int count = 0;

	LONG last_x = -1, last_y = -1;
	LONG rand_x = 0, rand_y = 0;
	// ѭ����ͼ��ɫ
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

// // ʶ������(color=��ɫ, diff=��ֵ, start_y=y��ʼλ��, end_y=y����λ�ã�
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

// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ color=�Ƚϵ���ɫֵ diff=����Ĳ�ֵ
bool PrintScreen::IsThePixel(int pixel, int color, int diff)
{
	if (diff > 0) {
		for (int i = 0; i < 3; i++) {
			int v = (pixel >> (i * 8)) & 0xff;
			int c = (color >> (i * 8)) & 0xff;
			int d = (diff >> (i * 8)) & 0xff;
			if (v < (c - d) || v >(c + d)) { // ������Ҫ��
				return false;
			}
		}
		return true;
	}

	return (pixel & 0x00ffffff) == (color & 0x00ffffff);
}

// �Ƚ�����
bool PrintScreen::ComparePixel(int screen_x, int screen_y, ComPixel * p)
{
	// ѭ���Ƚ�ͼƬ��ɫ
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

// �ͷ�
void PrintScreen::Release()
{
	if (m_hScreen)
		DeleteObject(m_hScreen);

	m_hScreen = NULL;
}
