#include <windows.h>
#include <xinput.h>
#include "xinpututil.h"

#include <stdio.h>

// ライブラリをリンク
#pragma comment(lib,"xinput.lib")

// XBOXコントローラ情報（ＭＡＸ４台）
CONTROLLER_STATE g_Controllers[MAX_CONTROLLERS];

// 不正な値を補正
void XinputSetDeadZone(int idx){
	// Zero value if thumbsticks are within the dead zone 
	if ((g_Controllers[idx].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
		g_Controllers[idx].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
		(g_Controllers[idx].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
		g_Controllers[idx].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
	{
		g_Controllers[idx].state.Gamepad.sThumbLX = 0;
		g_Controllers[idx].state.Gamepad.sThumbLY = 0;
	}

	if ((g_Controllers[idx].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
		g_Controllers[idx].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
		(g_Controllers[idx].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
		g_Controllers[idx].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
	{
		g_Controllers[idx].state.Gamepad.sThumbRX = 0;
		g_Controllers[idx].state.Gamepad.sThumbRY = 0;
	}
	

}

// idx番目のボタン情報（デジタルデータ）を取得
WORD GetXinputButton(int idx){
	

	return(g_Controllers[idx].state.Gamepad.wButtons);
}

bool GetXinputTriggerButton_Y(int idx){
	
	static  bool NowKey = false;

	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;


	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x8000) == 0x8000)  NowKey = true;
	else		NowKey = false;
	
	TriggerKey = (OldKey ^ NowKey) & NowKey;
	return TriggerKey;
}

bool GetXinputTriggerButton_X(int idx){
	
	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x4000) == 0x4000)  NowKey = true;
	else	NowKey = false;
	
	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}


bool GetXinputTriggerButton_B(int idx){
	//WORD NowButton;
	////= g_Controllers[idx].state.Gamepad.wButtons;
	//WORD OldButton;
	//WORD Trigger;

	//memcpy(&OldButton, &NowButton, sizeof(OldButton));

	//NowButton = g_Controllers[idx].state.Gamepad.wButtons & 0x2000;

	//Trigger = (NowButton ^ OldButton) & NowButton;
	static  bool NowKey = false;

	 bool OldKey = false;
	 bool TriggerKey = false;


	OldKey = NowKey;

	/*char str[256];

	sprintf_s(str, "xinput wButtons=%x\n", g_Controllers[idx].state.Gamepad.wButtons);
	OutputDebugString(str);*/

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x2000) == 0x2000){//Bボタン

		NowKey = true;
	}
	else{

		NowKey = false;
	}

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

	//return(g_Controllers[idx].state.Gamepad.wButtons);
}

bool GetXinputTriggerButton_A(int idx){

	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x1000) == 0x1000)  NowKey = true;
	else	NowKey = false;

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}


bool GetXinputTriggerR_SHOULDER(int idx){

	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x0200) == 0x0200)  NowKey = true;
	else	NowKey = false;

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}

bool GetXinputTriggerL_SHOULDER(int idx){

	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x0100) == 0x0100)  NowKey = true;
	else	NowKey = false;

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}



bool GetXinputTriggerSTART(int idx){

	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x0010) == 0x0010)  NowKey = true;
	else	NowKey = false;

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}

bool GetXinputTriggerBACK(int idx){

	static  bool NowKey = false;
	bool OldKey = false;
	bool TriggerKey = false;

	OldKey = NowKey;

	if ((g_Controllers[idx].state.Gamepad.wButtons & 0x0020) == 0x0020)  NowKey = true;
	else	NowKey = false;

	TriggerKey = (OldKey ^ NowKey) & NowKey;

	return TriggerKey;

}


// idx番目のＸＢＯＸコントローラが接続されているかを確認
bool XinputConnected(int idx){

	return g_Controllers[idx].bConnected;
}

// XINPUT初期化
void InitXinputState(){
	// Init state
	memset(g_Controllers, 0, sizeof(CONTROLLER_STATE)* MAX_CONTROLLERS);
}

// XINPUT　ＵＰＤＡＴＥ
HRESULT UpdateXinputControllerState()
{
	unsigned int  dwResult;
	for (unsigned int i = 0; i < MAX_CONTROLLERS; i++)
	{
		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &g_Controllers[i].state);

		if (dwResult == ERROR_SUCCESS)
			g_Controllers[i].bConnected = true;
		else
			g_Controllers[i].bConnected = false;
	}

	


	return S_OK;
}

// ｉｄｘ番目の左トリガー値（アナログ０－２５５）を取得
int GetXinputLeftTrigger(int idx){
	return g_Controllers[idx].state.Gamepad.bLeftTrigger;
}

// ｉｄｘ番目の右トリガー値（アナログ０－２５５）を取得
int GetXinputRightTrigger(int idx){
	return g_Controllers[idx].state.Gamepad.bRightTrigger;
}

// ｉｄｘ番目の左ＪＯＹスティック（アナログＸ値）を取得
int GetXinputThumbLX(int idx){
	return g_Controllers[idx].state.Gamepad.sThumbLX;
}

// ｉｄｘ番目の左ＪＯＹスティック（アナログＹ値）を取得
int GetXinputThumbLY(int idx){
	return g_Controllers[idx].state.Gamepad.sThumbLY;
}

// ｉｄｘ番目の右ＪＯＹスティック（アナログＸ値）を取得
int GetXinputThumbRX(int idx){
	return g_Controllers[idx].state.Gamepad.sThumbRX;
}

// ｉｄｘ番目の右ＪＯＹスティック（アナログＹ値）を取得
int GetXinputThumbRY(int idx){
	return g_Controllers[idx].state.Gamepad.sThumbRY;
}


