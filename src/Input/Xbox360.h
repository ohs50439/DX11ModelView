#ifndef _Xbox360_
#define _Xbox360_
#include <Windows.h>

class Xbox360
{
public:
	// シングルトン
	static Xbox360* GetInstance()
	{
		static Xbox360 instance;
		return &instance;
	}

	void Update(); // 更新
	// 左スティック入力
	bool LeftStickCheck(int i);   // 左スティック押されているかチェック
	bool LeftStickUP(int i);
	bool LeftStickDown(int i);
	bool LeftStickLeft(int i);
	bool LeftStickRight(int i);
	// 右スティック入力
	bool RightStickCheck(int i);  // 右スティック押されているかチェック
	bool RightStickUP(int i);
	bool RightStickDown(int i);
	bool RightStickLeft(int i);
	bool RightStickRight(int i);
	// ボタン入力
	bool Xbuttun(int i);
	bool Ybuttun(int i);
	bool Abuttun(int i);
	bool Bbuttun(int i);
	bool STARTbuttun(int i);
	bool BACKbuttun(int i);
	bool LBbuttun(int i);
	bool RBbuttun(int i);
	bool LTbuttun(int i);
	bool RTbuttun(int i);
	bool PADUPbuttun(int i);
	bool PADDownbuttun(int i);
	bool PADLeftbuttun(int i);
	bool PADRightbuttun(int i);
private:
	Xbox360(){}  // コンストラクタ
};


#endif // !_Xbox360_
