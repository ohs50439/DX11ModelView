#include "Xbox360.h"
#include "xinpututil.h"


#define INPUTDIGIT 20480

//-------------------------------------------//
//               新更
//-------------------------------------------//
void Xbox360::Update()
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		XinputSetDeadZone(i);
	}
}
//-------------------------------------------//
//               左スティック
//-------------------------------------------//
bool Xbox360::LeftStickCheck(int i)
{
	if (LeftStickUP(i)) return true;
	if (LeftStickDown(i)) return true;
	if (LeftStickLeft(i)) return true;
	if (LeftStickRight(i)) return true;
	return false;
}
// 上方向
bool Xbox360::LeftStickUP(int i)
{
	if (GetXinputThumbLY(i) > INPUTDIGIT) return true;
	return false;
}
// 下方向
bool Xbox360::LeftStickDown(int i)
{
	if (GetXinputThumbLY(i) < -INPUTDIGIT) return true;
	return false;
}
// 左方向
bool Xbox360::LeftStickLeft(int i)
{
	if (GetXinputThumbLX(i) > INPUTDIGIT) return true;
	return false;
}
// 右方向
bool Xbox360::LeftStickRight(int i)
{
	if (GetXinputThumbLX(i) < -INPUTDIGIT) return true;
	return false;

}
//-------------------------------------------//
//               右スティック
//-------------------------------------------//
bool Xbox360::RightStickCheck(int i)
{
	return false;
}
// 上方向
bool Xbox360::RightStickUP(int i)
{
	if (GetXinputThumbRY(i) > INPUTDIGIT) return true;
	return false;
}
// 下方向
bool Xbox360::RightStickDown(int i)
{
	if (GetXinputThumbRY(i) < -INPUTDIGIT) return true;
	return false;
}
// 左方向
bool Xbox360::RightStickLeft(int i)
{
	if (GetXinputThumbRX(i) > INPUTDIGIT) return true;
	return false;
}
// 右方向
bool Xbox360::RightStickRight(int i)
{
	if (GetXinputThumbRX(i) < -INPUTDIGIT) return true;
	return false;
}
//-------------------------------------------//
//               Xボタン
//-------------------------------------------//
bool Xbox360::Xbuttun(int i)
{
	if (GetXinputTriggerButton_X(i)) return true;
	return false;
}
//-------------------------------------------//
//               Yボタン
//-------------------------------------------//
bool Xbox360::Ybuttun(int i)
{
	if (GetXinputTriggerButton_Y(i)) return true;
	return false;
}
//-------------------------------------------//
//               Aボタン
//-------------------------------------------//
bool Xbox360::Abuttun(int i)
{
	if (GetXinputTriggerButton_A(i)) return true;
	return false;
}
//-------------------------------------------//
//               Bボタン
//-------------------------------------------//
bool Xbox360::Bbuttun(int i)
{
	if (GetXinputTriggerButton_B(i)) return true;
	return false;
}
//-------------------------------------------//
//               STARTボタン
//-------------------------------------------//
bool Xbox360::STARTbuttun(int i)
{
	if (GetXinputTriggerSTART(i)) return true;
	return false;
}
//-------------------------------------------//
//               BACKボタン
//-------------------------------------------//
bool Xbox360::BACKbuttun(int i)
{
	if (GetXinputTriggerBACK(i)) return true;
	return false;
}
//-------------------------------------------//
//               LBボタン
//-------------------------------------------//
bool Xbox360::LBbuttun(int i)
{
	if (GetXinputTriggerL_SHOULDER(i)) return true;
	return false;
}
//-------------------------------------------//
//               RBボタン
//-------------------------------------------//
bool Xbox360::RBbuttun(int i)
{
	if (GetXinputTriggerR_SHOULDER(i)) return true;
	return false;
}
//-------------------------------------------//
//               LTボタン
//-------------------------------------------//
bool Xbox360::LTbuttun(int i)
{
	if (GetXinputLeftTrigger(0) >= 100) return true;
	return false;
}
//-------------------------------------------//
//               RTボタン
//-------------------------------------------//
bool Xbox360::RTbuttun(int i)
{
	if (GetXinputRightTrigger(0) >= 100) return true;
	return false;
}
//-------------------------------------------//
//             十字キー上ボタン
//-------------------------------------------//
bool Xbox360::PADUPbuttun(int i)
{
	if (GetXinputButton(i) &XINPUT_GAMEPAD_DPAD_UP) return true;
	return false;
}
//-------------------------------------------//
//             十字キー下ボタン
//-------------------------------------------//
bool Xbox360::PADDownbuttun(int i)
{
	if (GetXinputButton(i) &XINPUT_GAMEPAD_DPAD_DOWN) return true;
	return false;
}
//-------------------------------------------//
//             十字キー左ボタン
//-------------------------------------------//
bool Xbox360::PADLeftbuttun(int i)
{
	if (GetXinputButton(i) &XINPUT_GAMEPAD_DPAD_LEFT) return true;
	return false;
}
//-------------------------------------------//
//             十字キー右ボタン
//-------------------------------------------//
bool Xbox360::PADRightbuttun(int i)
{
	if (GetXinputButton(i) &XINPUT_GAMEPAD_DPAD_RIGHT) return true;
	return false;
}