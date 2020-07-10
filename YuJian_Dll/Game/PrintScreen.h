#pragma once
#include <Windows.h>
#include <gdiplus.h>

// 字模宽度
#define NUM_MODE_W 9
// 字模高度
#define NUM_MODE_H 14

enum ComImgIndex
{
	CIN_NoItem,     // 没有物品图片
	CIN_LockItem,   // 没有开启格子
	CIN_YaoBao,     // 药包
	CIN_ShouJi,     // 卡利亚手记
	CIN_FaXing,     // 巴力混沌发型包
	CIN_XingChen,   // 星辰之眼
	CIN_HuanMo,     // 幻魔晶石
	CIN_MoHun,      // 魔魂晶石
	CIN_LingHun,    // 灵魂晶石
	CIN_QingChi,    // 青螭礼包
	CIN_ZiMing,     // 紫冥礼包
	CIN_KaDi,       // 卡迪礼包
	CIN_YaEr,       // 亚尔礼包
	CIN_MuBa,       // 穆巴礼包
	CIN_LuDi,       // 鲁迪礼包
	CIN_XiangLian,  // 爱娜的项链
	CIN_InFB,       // 是否在副本
	CIN_MapOpen,    // 地图是否打开
	CIN_SureBtn,    // 弹框确定按钮
	CIN_CanLogin,   // 是否已到登录界面
	CIN_InTeamFlag, // 入队图标信息[组队信息左侧标题图标]
	CIN_InFBFlag,   // 进副本图标信息[愿意进副本信息左侧标题图标]
	CIN_MAX,
};

typedef struct com_pixel 
{
	char Key[32];      // 名称
	int  Width;        // 宽度
	int  Height;       // 高度
	COLORREF* Pixels;  // 像数数组
} ComPixel;

typedef struct compare_point 
{
	int x;
	int y;
} ComPoint;

// 对比图片信息
typedef struct compare_image
{
	INT       Width;  // 宽度
	INT       Height; // 高度
	COLORREF* Buffer; // 颜色
} CompareImageInfo;

// 共享读取像数信息
typedef struct share_read_pixel_data
{
	DWORD x;                  // 要读取的x开始位置
	DWORD x2;                 // 要读取的x结束位置
	DWORD y;                  // 要读取的y开始位置
	DWORD y2;                 // 要读取的y结束位置
	DWORD width;              // 宽
	DWORD height;             // 高
	DWORD Pixels[1920 * 1080];// 像数信息
	DWORD Flag;               // 0-不做任何操作 1-程序需要读取像数信息
	DWORD OK;                 // 是否可以使用此dll读取像数
} ShareReadPixelData;

class Game;
class LookImgNum;
class PrintScreen
{
public:
	// ...
	PrintScreen(Game* p, const char* pixel_file);
public:
	// 读取像数配置
	void ReadPixelConf(const char* pixel_file);
	// 获取ComPixel指针
	ComPixel* GetComPixel(const char* key);
	// 加载对比图片
	void LoadCompareImage(ComImgIndex index, wchar_t* path);

	// 是否是注入opengl截图
	bool IsOpenglPs();
	// 注入PID
	void InjectVBox(const char* path, DWORD pid);
	// 初始化
	void InitDC();
	// 截图
	HBITMAP CopyScreenToBitmap(HWND hWnd, int start_x=0, int start_y=0, int end_x=0, int end_y=0, DWORD sleep_ms=0, bool del=false);
	// 截图
	HBITMAP CopyScreenToBitmap(LPRECT lpRect, bool del=false);
	// 保存截图
	int SaveBitmapToFile(HBITMAP   hBitmap, LPCWSTR   lpFileName);

	// 获取位图像数
	COLORREF GetPixel(int x, int y);
	// 检查像数颜色
	bool CheckPixel(DWORD color, DWORD diff=0);
	// 获取像数位置
	int GetPixelPos(DWORD color, int& pos_x, int& pos_y, DWORD diff = 0);
	// 获取这种颜色的数量有多少
	int  GetPixelCount(DWORD color, DWORD diff=0, bool print=false);
	// 获取这种像数连续有多少数量
	int  GetPixelConCount(int& x_count, int& y_count, DWORD color, DWORD diff=0);
	// 获取灰色像素值的数量
	int  GetGrayPiexlCount(bool print = false);
	// 获取灰色像素值的数量(start_x=x开始位置, end_x=x结束位置）
	int  GetGrayPiexlCount(int start_x, int end_x, bool print = false);
	// 比较像数 返回符合要求数量
	int  ComparePixel(const char* key, ComPoint* save = nullptr, int length = 0);

	// 识别数字(color=颜色, diff=差值, start_x=x开始位置, end_x=x结束位置, start_y=y开始位置, end_y=y结束位置）
	int  LookNum(int start_x, int end_x, int start_y, int end_y, DWORD color, DWORD diff, int d_v=0, bool print = false);

	// 像数是否符合要求 pixel=原像数原色 color=比较的颜色值 diff=允许的差值
	bool IsThePixel(int pixel, int color, int diff);

	// 释放
	void Release();

	// 是否上锁
	bool IsLocked() { return m_bLocked;  }
	// 上锁
	void Lock() { m_bLocked = true; }
	// 解锁
	void UnLock() { m_bLocked = false; }
private:
	// 是否锁定了
	bool m_bLocked = false;
	// 比较像数
	bool  ComparePixel(int screen_x, int screen_y, ComPixel* p);
public:
	// ...
	Game* m_pGame;
	// 识别图中数字
	LookImgNum* m_pLookImgNum;
	// 相对于游戏的截图位置
	RECT m_GamePrintRect;

	HWND m_hDesktopWnd;
	HDC m_hScrDC;
	HDC m_hMemDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hScreen = NULL;
	HANDLE  m_hShareMap;               // 共享内存
	ShareReadPixelData* m_pShareBuffer;// 共享内存

	int m_xScrn = 0;
	int m_yScrn = 0;

	char* m_pBuffer;
	char* m_pTmp;
	bool m_bIsGetBuffer = false;
	LONG m_bmWidthBytes = 0;
	LONG m_bmWidth = 0;
	LONG m_bmHeight = 0;
public:
	// 比较物品数组
	CompareImageInfo m_ComImg[1];

	// 配置的像数列表
	ComPixel m_ComPixels[100];
	// 配置的像数个数
	int m_nComPixelCount;
};