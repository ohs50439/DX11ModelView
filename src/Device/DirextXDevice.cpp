#include <./Device/DirectXDevice.h>

DX11Device::DX11Device() {

}
DX11Device::~DX11Device() {
}
unsigned long DX11Device::Init(HWND handle, unsigned long width, unsigned long height, bool windowmode) {
	unsigned long ret = 0;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Direct3D リソースとの相互運用性を Direct2D で実現するために必要
	DEBUG(creationFlags |= D3D11_CREATE_DEVICE_DEBUG);	   // デバッグ レイヤーをサポートするデバイスを作成
	//------bufferの設定--------//
	DXGI_SWAP_CHAIN_DESC desc = {}; // バック バッファーの表示モードを表す 
	desc.BufferCount = 1; // スワップ チェーンのバッファー数を表す値(フロント バッファーを含む)
	desc.BufferDesc.Width = width;   // 解像度の幅
	desc.BufferDesc.Height = height; // 解像度の高さ
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 4 成分、32 ビット符号なし整数フォーマット
	//リフレッシュ レート (Hz 単位) 
	desc.BufferDesc.RefreshRate.Numerator = 60;    // 有理数の最大値を表す符号なし整数値【分母】
	desc.BufferDesc.RefreshRate.Denominator = 1;   // 有理数の最小値を表す符号なし整数値【分子】
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // サーフェスまたはリソースを出力レンダー ターゲットとして使用
	// リソースのマルチサンプリング パラメーター(アンチエイリアシングなしのデフォルト サンプラ モード)
	desc.SampleDesc.Count = 1;   // ピクセル単位のマルチサンプリングの数
	desc.SampleDesc.Quality = 0; // イメージの品質レベル
	desc.Windowed = windowmode;  // 出が力ウィンドウ モードの場合は TRUE それ以外の場合は FALSE
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // IDXGISwapChain::ResizeTarget を呼び出してモードを切り替えられるようにするために設定
	desc.OutputWindow = handle; // 出力ウィンドウへの HWND ハンドル
	// 環境に応じて切り替える構造体を造る
	D3D_FEATURE_LEVEL features[] = { // verごとに設定（優先度は上から）Direct3D デバイスのターゲットとなる機能セット
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	// ドライバーの種類のオプション GPUを使うかCPUを使うか
	D3D_DRIVER_TYPE devicetype[] = {
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, // ハードウェア アブストラクション レイヤー (HAL) 
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_SOFTWARE, // ソフトウェア ドライバー
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE // リファレンス ラスタライザー(機能のテスト、デモ、デバッグ用）
	};
	// DX11必要変数の初期化群
	ID3D11Device *device = nullptr;         // レンダリングおよびリソースの作成に使用
	ID3D11DeviceContext *context = nullptr; // レンダリング コマンドを生成するデバイス コンテキスト
	IDXGISwapChain *swapchain = nullptr;    // レンダリングされたデータを出力に表示する前に格納するための 1 つまたは複数のサーフェス
	for (auto dt : devicetype) {
		if (SUCCEEDED(
			D3D11CreateDeviceAndSwapChain(  // ディスプレイ アダプターを表すデバイスとレンダリングに使用するスワップ チェーンを作成
			nullptr,						// デバイスの作成時に使用するビデオ アダプターへのポインター(NULLで規定のアダプター)
			dt,								// 作成するデバイスの種類
			nullptr,						// ソフトウェア ラスタライザーを実装する DLL のハンドル
			creationFlags,					// 有効にするランタイム レイヤー(値はビット単位の OR 演算で指定)
			features,						// 作成を試みる機能レベルの順序を指定する D3D_FEATURE_LEVEL の配列へのポインター
			ARRAYSIZE(features),			// pFeatureLevels の要素数
			D3D11_SDK_VERSION,				// SDK のバージョン D3D11_SDK_VERSION を指定
			&desc,							// スワップ チェーンの初期化パラメーターを格納するポインタ
			&swapchain,						// レンダリングに使用するスワップ チェーンのポインタアドレスを返す
			&device,						// 作成されたデバイスへのポインタのアドレスを返す
			&this->feature,					// 機能レベルの配列にある最初の要素を表すポインタを返す
			&context						// デバイス コンテキストのポインタのアドレスを返す
			)
			)) { // 上記初期化された各機能を代入
			this->device.Attach(device);
			this->context.Attach(context);
			this->swapchain.Attach(swapchain);
			this->drivertype = dt;
			break;
		}
	}
	// デバイスNULLチェック
	if (!device) {
		ret = -1;
		return ret;
	}

	//デバッグ用デバイスの生成
	DEBUG(SUCCEEDED(this->context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&this->anotation)));

	//BuckBufferの生成
	D3D11_TEXTURE2D_DESC descDepth;            // 2D テクスチャー
	ID3D11Texture2D* pBackBuffer = NULL;	   // 構造化されたメモリーであるテクセル データを管理
	ID3D11RenderTargetView*		rendertargetview; // レンダーターゲットビュー(レンダリング時にアクセス可能なレンダー ターゲットのサブリソースを識別)
	ID3D11DepthStencilView*		depthstencilview; // ステンシルビュー(深度ステンシル テスト中にテクスチャー リソースにアクセス)
	// comptrみたなもの)GetBuffer(参照カウンタ1増える
	ret = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(ret)) {
		return ret;
	}
	// BackBufferの取得方法（CreateRenderTargetView)
	ret = this->device->CreateRenderTargetView(pBackBuffer, NULL, &rendertargetview);
	if (FAILED(ret)) {
		return ret;
	}
	// retでCreateを格納したので参照カウンタを0にする（２になるから）
	pBackBuffer->Release();
	pBackBuffer = nullptr;

	//ステンシルビューの生成
	ZeroMemory(&descDepth, sizeof(descDepth));	// 構造体中身を０初期化
	descDepth.Width = width; descDepth.Height = height; // テクスチャーの幅と高さ
	descDepth.MipLevels = 1; // テクスチャー内のミップマップ レベルの最大数(1:マルチサンプリングされたテクスチャー 0:サブテクスチャーの全セットを生成する)
	descDepth.ArraySize = 1; // テクスチャー配列内のテクスチャーの数
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 深度チャンネルに 24 ビット、ステンシル チャンネルに 8 ビットを使用する 32 ビット Z バッファー フォーマット
	descDepth.SampleDesc.Count = 1; descDepth.SampleDesc.Quality = 0; // ピクセル単位のマルチサンプリングの数とイメージの品質レベル
	descDepth.Usage = D3D11_USAGE_DEFAULT; // テクスチャーの読み込みおよび書き込み方法を識別する値
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // テクスチャーを出力結合ステージの深度ステンシル ターゲットとしてバインド
	ret = this->device->CreateTexture2D(&descDepth, NULL, &pBackBuffer); // 2D テクスチャーの配列

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthStencilView; // 深度ステンシル ビューからアクセス可能なテクスチャーのサブリソースを指定
	ZeroMemory(&descDepthStencilView, sizeof(descDepthStencilView)); // 構造体中身を０初期化
	descDepthStencilView.Format = descDepth.Format; // リソース データのフォーマット
	descDepthStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // (2D テクスチャーとしてアクセス)深度ステンシル リソースへのアクセス方法を指定
	descDepthStencilView.Texture2D.MipSlice = 0; // (最初に使用するミップマップ レベルのインデックス)2D テクスチャーのサブリソースを指定

	// リソース データへのアクセス用に深度ステンシル ビューを作成
	ret = this->device->CreateDepthStencilView(pBackBuffer, &descDepthStencilView, &depthstencilview);
	if (pBackBuffer) { // 使用後の解放
		pBackBuffer->Release();
		pBackBuffer = nullptr;
	}
	// // 上記初期化された各機能を代入
	this->rendertargetview.Attach(rendertargetview);
	this->depthstencilview.Attach(depthstencilview);
	// ビューポート
	D3D11_VIEWPORT vp; // ビューポートの寸法を定義
	vp.Width = (float)width;		// ビューポートの幅
	vp.Height = (float)height;		// ビューポートの高さ
	vp.MinDepth = 0.0f;				// ビューポートの最小深度
	vp.MaxDepth = 1.0f;				// ビューポートの最大深度
	vp.TopLeftX = 0;				// ビューポートの左側の X 位置
	vp.TopLeftY = 0;				// ビューポートの上部の Y 位置
	this->context->RSSetViewports(1, &vp); // パイプラインのラスタライザー ステージにビューポートの配列をバインド

	//レンダーターゲットの設定
	ID3D11RenderTargetView *rtv[] = { // レンダリング時にアクセス可能なレンダー ターゲットのサブリソースを識別
		this->getRTV(),
	};
	this->context->OMSetRenderTargets(1, rtv, nullptr); // 1 つ以上のレンダー ターゲットをアトミックにバインドし、出力結合ステージに深度ステンシル バッファーをバインド
	return ret;
}

