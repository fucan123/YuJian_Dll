#pragma once
#include "GameData.h"
#include <Windows.h>

#define BUTTON_ID_ROLE      0x3ED         // 人物按钮ID
#define BUTTON_ID_BAG       0x3EF         // 背包按钮"物品"
#define BUTTON_ID_TEAM      0x3F3         // 队伍按钮"组队"
#define BUTTON_ID_SHEJIAO   0x402         // 社交按钮"好友"
#define BUTTON_ID_SAVE_MNY  0x405         // 仓库金钱存储"取"
#define BUTTON_ID_CRTTEAM   0x43B         // 创建队伍"1"
#define BUTTON_ID_CANCEL    0x44E         // 取消按钮"取消"
#define BUTTON_ID_CLOSESHOP 0x450         // 关闭商店按钮
#define BUTTON_ID_BAG_SET   0x460         // 整理背包"物品栏-排序按钮"
#define BUTTON_ID_FIX_ALL   0x454         // 修理所有装备"FIX_ALL"
#define BUTTON_ID_BAG_ITEM  0x46B         // 背包物品"MPC物品栏"
#define BUTTON_ID_CLOSEBAG  0x471         // 关闭背包按钮
#define BUTTON_ID_TIPSURE   0x477         // 提示确定按钮"确定"
#define BUTTON_ID_CUNQIAN   0x482         // 仓库确定存钱"存"
#define BUTTON_ID_CLOSECKIN 0x485         // 关闭仓库按钮
#define BUTTON_ID_CKIN_ITEM 0x486         // 仓库物品"存储物品栏"
#define BUTTON_ID_CKIN_PGUP 0x488         // 仓库向上翻页按钮"U"
#define BUTTON_ID_CKIN_PGDW 0x489         // 仓库向下翻页按钮"D"
#define BUTTON_ID_CLOSEMENU 0x49C         // 通用菜单关闭按钮"C"或'x'或'X'或关闭
#define BUTTON_ID_REBORN_AT 0x4A2         // 原地复活"XP1"
#define BUTTON_ID_REBORN    0x4A3         // 复活"XP2"
#define BUTTON_ID_ROLEVIP   0x5AD         // 点开人物上面VIP按钮ID
#define BUTTON_ID_BAG_PGUP  0x5C5         // 背包向上翻页按钮"up"
#define BUTTON_ID_BAG_PGDW  0x5C6         // 背包向下翻页按钮"down"
#define BUTTON_ID_FACE_BG   0x5F5         // 好友列表"FACE_BG"
#define BUTTON_ID_VIP       0x44F         // VIP按钮ID"VIP"
#define BUTTON_ID_LIAOTIANB 0x649         // 聊天框拖动大小按钮"高度"
#define BUTTON_ID_TEAMFLAG  0x713         // 同意入队那个旗帜按钮
#define BUTTON_ID_ITEM      0x751         // 物品快捷键"物品"
#define BUTTON_ID_MAGIC     0x752         // 技能快捷键"技能"
#define BUTTON_ID_INFB      0x7B2         // 同意进副本按钮"同意"
#define BUTTON_ID_JUJUE     0x7B3         // 同意进副本按钮"拒绝"
#define BUTTON_ID_INTEAM    0x45A         // 同意入队按钮"同意"
//#define BUTTON_ID_INTEAM    0x815         // 同意入队按钮
#define BUTTON_ID_SURE      0x8BA         // 确定按钮"接受"(副本邀请队员确定按钮)
#define BUTTON_ID_CHECKIN   0x960         // 物品仓库按钮ID"VIP界面---使用仓库"
#define BUTTON_ID_LEAVE_LGC 0x9F1         // 离开练功房按钮"离开练功场"
#define BUTTON_ID_XXX       0xAC9         // 关闭"X"
#define BUTTON_ID_LOGIN     0xA30         // 登入按钮
#define BUTTON_ID_ROLENO    0xD51         // 选择角色框[后面角色应该是+1递增]
#define BUTTON_ID_SHJLIST   0xE2B         // 社交展开列表"" 好友是45,62

#define BUTTON_ID_TASK_DUP  0x6B6         // 任务信息隐藏按钮"DUP"

#define BUTTON_ID_ACTY      0x00006568    // 显示隐藏地图下方活动列表按钮"BTN_ACTY_SHOW"
#define BUTTON_ID_FULI      0x000F4240    // 福利大厅按钮(0x000F4240-0x000F4246)"Worldcup_CloseBtn"

#define STATIC_ID_MAP       0x56D         // 地图名称"{1 0x00000000}阿拉玛的哭泣"
#define STATIC_ID_POS_X     0x56E         // 坐标X"{1 0x00000000}905"
#define STATIC_ID_POS_Y     0x56F         // 坐标Y"{1 0x00000000}1063"
#define STATIC_ID_HP        0x5A1         // 血量"HP:29696/29696神仙HP加成10%"
#define STATIC_ID_ROLE      0x5B1         // 角色名字"笑语嫣然"

#define STATIC_ID_PETLIFE_1 0xD45         // 宠物1血量"12587/12587"
#define STATIC_ID_PETLIFE_2 0xD49         // 宠物2血量"12587/12587"
#define STATIC_ID_PETLIFE_3 0xD4D         // 宠物3血量"12587/12587"
#define STATIC_ID_PETLIFE_4 0xD51         // 宠物4血量"12587/12587"

struct EnumWndInfo {
	int   Left;       // 距离左边位置, -1忽略
	int   Top;        // 距离顶部位置, -1忽略
	int   Width;      // 宽度, -1忽略
	int   Height;     // 高度, -1忽略
	int   WidthFlag;  // 0为=Width, -1为<Width, -2为>Width, 正数为允许范围
	int   HeightFlag; // 0为=Height, -1为<Height, -2为>Height, 正数为允许范围
	char* Text;       // 文本
	int   TextFlag;   // 0=Text, -1为匹配Text即可
	HWND GameWnd;    // 游戏窗口句柄
	HWND ReturnV;    // 返回值
}; // 要枚举的窗口信息

class Game;
class MCButton
{
public:
	MCButton(Game* p);
	// 隐藏活动列表
	bool HideActivity(HWND hwnd_own);
	// 按钮是否禁用
	bool IsDisabled(HWND hwnd_own, int button_id);
	// 窗口是否禁用
	bool IsDisabled(HWND hWnd);
	// 按键
	void Key(BYTE bVk);
	// 按键
	void KeyDown(BYTE bVk);
	// 按键
	void KeyUp(BYTE bVk);
	// 关闭按钮按钮
	bool CloseButton(HWND game, int button_id, const char* text=nullptr);
	// 点击屏幕坐标
	bool ClickScreen(int x, int y, int flag = 0xff, bool left_click = true);
	// 点击游戏画面
	bool ClickPic(HWND game, HWND pic, int x, int y, DWORD sleep_ms=50, bool left_click=true);
	// 点击对话选项
	bool ClickTalk(HWND pic, int x, int y, bool moumov);
	// 点击按钮
	bool Click(HWND hwnd, int x, int y, int flag = 0xff, bool left_click = true);
	// 点击按钮
	bool Click(HWND hwnd_own, int button_id, const char* text=nullptr);
	// 点击按钮
	bool Click(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag=0xff, bool left_click=true);
	// 点击按钮(坐标[屏幕的坐标])需要转换(相对于[hwnd_own的坐标])
	bool ClickRel(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag = 0xff, bool left_click = true);
	// 鼠标移动到指定位置
	void MouseMovePos(HWND hWnd, int x, int y);
	// 存入钱
	bool SaveMoney(HWND hwnd_own);
	// 打开仓库窗口
	bool OpenStorage(HWND hwnd_own);
	// 关闭仓库窗口
	bool CloseStorage(HWND hwnd_own);
	// 检查按钮
	bool CheckButton(HWND hwnd_own, int button_id, const char* text = nullptr);
	// 获取顶部宠物血量窗口
	int  FindPetLifeWndAtTop(HWND game_wnd, HWND wnds[]);
	// 获取顶部操作宠物父窗口
	HWND FindPetParentWndAtTop(HWND game_wnd);
	// 获取对话框窗口
	HWND FindTalkWnd(HWND pic);
	// 获取窗口句柄
	HWND FindButtonWnd(HWND hwnd_own, int button_id, const char* text = nullptr);
	// 获得窗口句柄
	bool FindButtonWnd(HWND hwnd_own, int button_id, HWND& hwnd, HWND& parent, const char* text = nullptr);
	// 获得游戏窗口
	HWND FindGameWnd(DWORD pid);
	// 计算相对于游戏窗口位置
	bool CalcRelGamePos(int& x, int& y, HWND game, int button_id, const char* text=nullptr);
	// 枚举对话框窗口
	static BOOL CALLBACK EnumProcByWndInfo(HWND hWnd, LPARAM lParam);
	// 枚举对话框窗口
	static BOOL CALLBACK EnumProcTalkWnd(HWND hWnd, LPARAM lParam);
	// 枚举窗口
	static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);
	// 枚举子窗口
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
public:
	Game* m_pGame;
};