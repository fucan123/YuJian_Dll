#pragma once
#include <Windows.h>

#define ISCMD 0

#define IS_READ_MEM         0x0000001     // 是否开启MEM模式

#define ADDR_ACCOUNT_NAME   0x1DFC0A0     // 登录帐号名称
#define ADDR_ROLE_NAME      0x1E39760     // 游戏角色名称 
#define ADDR_SERVER_NAME    0x1DCC668     // 游戏区服名称
#define ADDR_COOR_X_OFFSET  0x1E7E274     // X坐标地址在模块里面的偏移[MOD_3drole]
#define ADDR_COOR_Y_OFFSET  0x1E7E270     // Y坐标地址在模块里面的偏移[MOD_3drole]
#define ADDR_LIFE_OFFSET    0x1E39688     // 血量地址在模块里面的偏移[MOD_3drole]
#define ADDR_LIFEMAX_OFFSET 0x1E39690     // 血量上限地址在模块里面的偏移[MOD_3drole]

#define SCREEN_X 1440 // 屏幕的宽度
#define SCREEN_Y 900  // 屏幕的高度

/// 游戏物品类型
enum ITEM_TYPE
{
	未知物品 = 0x00,       // 不知道什么物品
	特制经验球礼包 = 0x000B1FC8,
	白羊星穆巴礼包 = 0x000B21B0,
	白羊星卡迪礼包 = 0x000B21B1,
	金牛星亚尔礼包 = 0x000B21BB,
	金牛星鲁迪礼包 = 0x000B21BC,
	速效治疗包 = 0x000B5593, // 可以开出几瓶速效治疗药水
	亚特速报 = 0x000B84E9,
	燧石钥匙 = 0x000B55FA,
	莎顿的宝库钥匙 = 0x000B5545,
	圣光炼金卷轴 = 0x000B55F8,
	清凉的圣水 = 0x000B55F9,
	卡利亚堡钥匙 = 0x000B55FB,
	蓝色祝福碎片 = 0x000B561E,
	女伯爵的铜镜 = 0x000B5797,
	卡利亚手记一 = 0x000B578C,
	卡利亚手记二 = 0x000B578D,
	卡利亚手记三 = 0x000B578E,
	卡利亚手记四 = 0x000B578F,
	卡利亚手记五 = 0x000B5790,
	卡利亚手记六 = 0x000B5791,
	卡利亚手记七 = 0x000B5792,
	卡利亚手记八 = 0x000B5793,
	卡利亚手记九 = 0x000B5794,
	卡利亚手记十 = 0x000B5795,
	炎魔督军之杖 = 0x000B5798,
	巴力混沌发型包 = 0x000B579D,
	爱娜祈祷项链 = 0x000B5B24,
	神恩治疗药水 = 0x000B783C,
	四百点图鉴卡一 = 0x000C5D77,
	四百点图鉴卡二 = 0x000C614D,
	O礼包6星   = 0x000C67C9,
	XO礼包12星 = 0x000C67C8,
	XO礼包25星 = 0x000C6FA4,
	双子星青螭礼包 = 0x000C6FD7,
	双子星紫冥礼包 = 0x000C6FD8,
	速效治疗药水 = 0x000F6982, // +2500生命值
	速效圣兽灵药 = 0x000F943E, // 复活宝宝的
	三十星神兽碎片加1 = 0x000F90E4,
	三十星神兽碎片加2 = 0x000F90E5,
	三十星神兽碎片加3 = 0x000F90E6,
	魔魂晶石 = 0x000FD35E,
	灵魂晶石 = 0x000FD368,
	幻魔晶石 = 0x000FD372,
};

// 自动捡拾物品信息
typedef struct conf_item_info
{
	CHAR      Name[32];
	DWORD     Type;
	DWORD     Extra[2];   
} ConfItemInfo;

typedef struct game_data_addr
{
	DWORD Player;        // 人物首地址
	DWORD Account;       // 帐号
	DWORD Role;          // 角色的名字
	DWORD CoorX;         // X坐标
	DWORD CoorY;         // Y坐标
	DWORD MoveX;         // X坐标(目的地)
	DWORD MoveY;         // Y坐标(目的地)
	DWORD ScreenX;       // 人物在屏幕坐标X
	DWORD ScreenY;       // 人物在屏幕坐标Y
	DWORD Life;          // 血量
	DWORD LifeMax;       // 血量最大值
	DWORD PicScale;      // 画面缩放数值
	DWORD QuickMagicNum; // 技能快捷栏上面物品显示数量
	DWORD QuickItemNum;  // 物品栏上面物品显示数量
	DWORD Bag;           // 背包物品地址
	DWORD Storage;       // 仓库物品地址
} GameDataAddr;

// 共享写入目的地xy地址数据
typedef struct share_write_xy_data
{
	DWORD InDll;    // 是否已注入DLL
	DWORD AddrX;    // X地址
	DWORD AddrY;    // Y地址
	DWORD AddrPic;  // 画面缩放地址
	DWORD X;        // X数值
	DWORD Y;        // Y数值
	DWORD PicScale; // 画面缩放数值
	DWORD Flag;     // 0-主程序写入数据 1-Dll程序写入数据 2-写入画面缩放数值
} ShareWriteXYData;

class Game;
class GameData
{
public:
	GameData(Game* p);

	// 获取角色字符
	bool GetRoleByPid(_account_* account, char* out, int len);
	// 监听游戏
	int  WatchGame(bool first=true);
	// 获取游戏窗口
	void FindGameWnd();
	// 获取游戏信息
	void FindGameInfo(_account_* account);
	// 枚举窗口
	static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);

	// 是否在指定区域坐标 allow=误差
	bool IsInArea(int x, int y, int allow, _account_* account=nullptr);
	// 是否不在指定区域坐标 allow=误差
	bool IsNotInArea(int x, int y, int allow, _account_* account=nullptr);

	// 是否在神殿
	bool IsInShenDian(_account_* account=nullptr);
	// 是否在遗忘神域
	bool IsInShenYu(_account_* account = nullptr);
	// 是否在副本门口
	bool IsInFBDoor(_account_* account=nullptr);

	// 是否是那地图
	bool IsTheMap(const char* map, _account_* account = nullptr);

	// 获取快捷栏上面数量
	bool FindQuickAddr(int flag=0);
	// 获取血量地址
	bool FindLifeAddr();
	// 获取背包物品地址
	bool FindBagAddr();
	// 获取仓库物品地址
	bool FindStorageAddr();
	// 获取人物屏幕坐标
	bool FindScreenPos();
	// 获取画面缩放数值地址
	bool FindPicScale();

	// 读取角色
	bool ReadName(char* name, _account_* account=nullptr);
	// 读取坐标
	bool ReadCoor(DWORD* x=nullptr, DWORD* y=nullptr, _account_* account=nullptr);
	// 读取坐标
	bool ReadCoorByWnd(DWORD* x=nullptr, DWORD* y=nullptr, _account_* account=nullptr);
	// 解析坐标数据
	int  FormatCoor(HWND hWnd);
	// 获取屏幕坐标
	bool ReadScreenPos(int& x, int& y, _account_* account = nullptr);
	// 读取生命值
	DWORD ReadLife(DWORD* life=nullptr, DWORD* life_max=nullptr, _account_* account=nullptr);

	// 搜索特征码
	DWORD SearchCode(DWORD* codes, DWORD length, DWORD* save, DWORD save_length = 1, DWORD step = 4);
	// 搜索特征码
	DWORD SearchByteCode(BYTE* codes, DWORD length);
	// 读取四字节内容
	bool ReadDwordMemory(DWORD addr, DWORD& v, _account_* account=nullptr);
	// 读取内存
	bool ReadMemory(PVOID addr, PVOID save, DWORD length, _account_* account=nullptr);
	// 读取游戏内存
	bool ReadGameMemory(DWORD flag = 0x01);

public:
	Game* m_pGame;

	// 当前X坐标
	DWORD m_dwX = 0;
	// 当前Y坐标
	DWORD m_dwY = 0;
	// 当前血量
	DWORD m_dwLife = 0;
	// 血量最大值
	DWORD m_dwLifeMax = 0;

	// 数据
	GameDataAddr m_DataAddr;
	// 数据大号
	GameDataAddr m_DataAddrBig;
public:
	// 游戏大号窗口
	HWND m_hWndBig = NULL;
	// 游戏句柄
	HANDLE m_hProcessBig = NULL;
	// 大号
	_account_* m_pAccountBig = NULL;
	// 临时
	_account_* m_pAccountTmp = NULL;

	// 游戏权限句柄
	HANDLE   m_hGameProcess = NULL;
	// 是否读取完毕
	bool  m_bIsReadEnd;
	// 读取内存块的大小
	DWORD m_dwReadSize;
	// 读取基地址
	DWORD m_dwReadBase;
	// 读取内容临时内存
	BYTE* m_pReadBuffer;

	HANDLE m_hShareMap;    // 共享内存
	ShareWriteXYData* m_pShareBuffer; // 共享内存
	bool m_bInDll = false;
};