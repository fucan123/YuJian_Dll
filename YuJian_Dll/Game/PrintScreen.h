#pragma once
#include <Windows.h>
#include <gdiplus.h>

// ��ģ���
#define NUM_MODE_W 9
// ��ģ�߶�
#define NUM_MODE_H 14

enum ComImgIndex
{
	CIN_NoItem,     // û����ƷͼƬ
	CIN_LockItem,   // û�п�������
	CIN_YaoBao,     // ҩ��
	CIN_ShouJi,     // �������ּ�
	CIN_FaXing,     // �������緢�Ͱ�
	CIN_XingChen,   // �ǳ�֮��
	CIN_HuanMo,     // ��ħ��ʯ
	CIN_MoHun,      // ħ�꾧ʯ
	CIN_LingHun,    // ��꾧ʯ
	CIN_QingChi,    // ������
	CIN_ZiMing,     // ��ڤ���
	CIN_KaDi,       // �������
	CIN_YaEr,       // �Ƕ����
	CIN_MuBa,       // �°����
	CIN_LuDi,       // ³�����
	CIN_XiangLian,  // ���ȵ�����
	CIN_InFB,       // �Ƿ��ڸ���
	CIN_MapOpen,    // ��ͼ�Ƿ��
	CIN_SureBtn,    // ����ȷ����ť
	CIN_CanLogin,   // �Ƿ��ѵ���¼����
	CIN_InTeamFlag, // ���ͼ����Ϣ[�����Ϣ������ͼ��]
	CIN_InFBFlag,   // ������ͼ����Ϣ[Ը���������Ϣ������ͼ��]
	CIN_MAX,
};

typedef struct com_pixel 
{
	char Key[32];      // ����
	int  Width;        // ���
	int  Height;       // �߶�
	COLORREF* Pixels;  // ��������
} ComPixel;

typedef struct compare_point 
{
	int x;
	int y;
} ComPoint;

// �Ա�ͼƬ��Ϣ
typedef struct compare_image
{
	INT       Width;  // ���
	INT       Height; // �߶�
	COLORREF* Buffer; // ��ɫ
} CompareImageInfo;

// �����ȡ������Ϣ
typedef struct share_read_pixel_data
{
	DWORD x;                  // Ҫ��ȡ��x��ʼλ��
	DWORD x2;                 // Ҫ��ȡ��x����λ��
	DWORD y;                  // Ҫ��ȡ��y��ʼλ��
	DWORD y2;                 // Ҫ��ȡ��y����λ��
	DWORD width;              // ��
	DWORD height;             // ��
	DWORD Pixels[1920 * 1080];// ������Ϣ
	DWORD Flag;               // 0-�����κβ��� 1-������Ҫ��ȡ������Ϣ
	DWORD OK;                 // �Ƿ����ʹ�ô�dll��ȡ����
} ShareReadPixelData;

class Game;
class LookImgNum;
class PrintScreen
{
public:
	// ...
	PrintScreen(Game* p, const char* pixel_file);
public:
	// ��ȡ��������
	void ReadPixelConf(const char* pixel_file);
	// ��ȡComPixelָ��
	ComPixel* GetComPixel(const char* key);
	// ���ضԱ�ͼƬ
	void LoadCompareImage(ComImgIndex index, wchar_t* path);

	// �Ƿ���ע��opengl��ͼ
	bool IsOpenglPs();
	// ע��PID
	void InjectVBox(const char* path, DWORD pid);
	// ��ʼ��
	void InitDC();
	// ��ͼ
	HBITMAP CopyScreenToBitmap(HWND hWnd, int start_x=0, int start_y=0, int end_x=0, int end_y=0, DWORD sleep_ms=0, bool del=false);
	// ��ͼ
	HBITMAP CopyScreenToBitmap(LPRECT lpRect, bool del=false);
	// �����ͼ
	int SaveBitmapToFile(HBITMAP   hBitmap, LPCWSTR   lpFileName);

	// ��ȡλͼ����
	COLORREF GetPixel(int x, int y);
	// ���������ɫ
	bool CheckPixel(DWORD color, DWORD diff=0);
	// ��ȡ����λ��
	int GetPixelPos(DWORD color, int& pos_x, int& pos_y, DWORD diff = 0);
	// ��ȡ������ɫ�������ж���
	int  GetPixelCount(DWORD color, DWORD diff=0, bool print=false);
	// ��ȡ��ɫ����ֵ������
	int  GetGrayPiexlCount(bool print = false);
	// ��ȡ��ɫ����ֵ������(start_x=x��ʼλ��, end_x=x����λ�ã�
	int  GetGrayPiexlCount(int start_x, int end_x, bool print = false);
	// �Ƚ�ͼƬ ���ط���Ҫ������
	int  CompareImage(ComImgIndex index, ComPoint* save=nullptr, int length=0);
	// �Ƚ����� ���ط���Ҫ������
	int  ComparePixel(const char* key, ComPoint* save = nullptr, int length = 0);

	// ʶ������(color=��ɫ, diff=��ֵ, start_x=x��ʼλ��, end_x=x����λ��, start_y=y��ʼλ��, end_y=y����λ�ã�
	int  LookNum(int start_x, int end_x, int start_y, int end_y, DWORD color, DWORD diff, int d_v=0, bool print = false);

	// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ color=�Ƚϵ���ɫֵ diff=����Ĳ�ֵ
	bool IsThePixel(int pixel, int color, int diff);

	// ��ȡ����Ʒ������Ϣ
	void GetNoItemPixelData(int save[255]);
	// ��ȡδ������Ʒ��������
	void GetLockItemPixelData(int save[255]);
	// ��ȡҩ��������Ϣ
	void GetYaoBaoPixelData(int save[255]);
	// ��ȡ�������ּ�������Ϣ
	void GetShouJiPixelData(int save[255]);
	// ��ȡ�������緢�Ͱ�������Ϣ
	void GetFaXingPixelData(int save[255]);
	// ��ȡ�ǳ�֮��������Ϣ
	void GetXingChenPixelData(int save[255]);
	// ��ȡ��ħ��ʯ������Ϣ
	void GetHuanMoPixelData(int save[255]);
	// ��ȡħ�꾧ʯ������Ϣ
	void GetMoHunPixelData(int save[255]);
	// ��ȡ��꾧ʯ������Ϣ
	void GetLingHunPixelData(int save[255]);
	// ��ȡ������������Ϣ
	void GetQingChiPixelData(int save[255]);
	// ��ȡ��ڤ���������Ϣ
	void GetZiMingPixelData(int save[255]);
	// ��ȡ�������������Ϣ
	void GetKaDiPixelData(int save[255]);
	// ��ȡ�Ƕ����������Ϣ
	void GetYaErPixelData(int save[255]);
	// ��ȡ�°����������Ϣ
	void GetMuBaPixelData(int save[255]);
	// ��ȡ³�����������Ϣ
	void GetLuDiPixelData(int save[255]);
	// ��ȡ���ȵ�����������Ϣ
	void GetXiangLianPixelData(int save[255]);
	// ��ȡ�Ƿ��ڸ���������Ϣ[��ȡ��ͼ���Ƶ�һ����]
	void GetInFBPiexlData(int save[255]);
	// ��ȡ��ͼ��������Ϣ
	void GetMapOpenPiexlData(int save[255]);
	// ��ȡ����ȷ����ť������Ϣ
	void GetSureBtnPiexlData(int save[255]);
	// ��ȡ��¼��ť������Ϣ
	void GetCanLoginPiexlData(int save[255]);
	// ��ȡ�罻ͼ��������Ϣ
	void GetInTeamFlagPiexlData(int save[255]);
	// ��������ͬ��ͼ��
	void GetInFBFlagPiexlData(int save[255]);
	// �ͷ�
	void Release();

	// �Ƿ�����
	bool IsLocked() { return m_bLocked;  }
	// ����
	void Lock() { m_bLocked = true; }
	// ����
	void UnLock() { m_bLocked = false; }
private:
	// �Ƿ�������
	bool m_bLocked = false;
	// �Ƚ�ͼƬ
	bool  CompareImage(int screen_x, int screen_y, ComImgIndex index);
	// �Ƚ�����
	bool  ComparePixel(int screen_x, int screen_y, ComPixel* p);
public:
	// ...
	Game* m_pGame;
	// ʶ��ͼ������
	LookImgNum* m_pLookImgNum;
	// �������Ϸ�Ľ�ͼλ��
	RECT m_GamePrintRect;

	HWND m_hDesktopWnd;
	HDC m_hScrDC;
	HDC m_hMemDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hScreen = NULL;
	HANDLE  m_hShareMap;               // �����ڴ�
	ShareReadPixelData* m_pShareBuffer;// �����ڴ�

	int m_xScrn = 0;
	int m_yScrn = 0;

	char* m_pBuffer;
	bool m_bIsGetBuffer = false;
	LONG m_bmWidthBytes = 0;
	LONG m_bmWidth = 0;
	LONG m_bmHeight = 0;
public:
	// �Ƚ���Ʒ����
	CompareImageInfo m_ComImg[1];

	// ���õ������б�
	ComPixel m_ComPixels[100];
	// ���õ���������
	int m_nComPixelCount;
};