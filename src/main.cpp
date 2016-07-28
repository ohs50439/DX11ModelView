

#include <Windows.h>
#include <xnamath.h>
#include <d3dcompiler.h>
#include <./Device/WindowDevice.h>
#include <./Device/DirectXDevice.h>
#include <./Vertex.h>
#include <./Resource/Texture.h>
#include <./GUI/imgui.h>
#include <./GUI/imgui_internal.h>
#include <./GUI/imgui_impl_dx11.h>
// 仮
#include <./Graphics/DX11/Rasterizer.h>


// Shaderに送るカメラ情報
struct ConstantBuffer{
	XMMATRIX mWorld;		//ワールド変換行列
	XMMATRIX mView;			//ビュー変換行列
	XMMATRIX mProjection;	//透視投影変換行列
};

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {

	WindowDevice &window = WindowDevice::getInstance();
	DX11Device &device = DX11Device::getInstance();
	window.Init(hInstance, nCmdShow, TEXT("test window"), 1280, 720, true);
	device.Init(window.getHandle(), window.getWidth(), window.getHeight(), window.getWindowMode());
	ImGui_ImplDX11_Init(window.getHandle(), device.getDevice(), device.getContext());

	// 頂点の生成
	Vertex4UV vertex[4] = {
		{  100.f,  100.f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ -100.f,  100.f, 1.0f, 1.0f, 0.0f, 0.0f },
		{  100.f, -100.f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ -100.f, -100.f, 1.0f, 1.0f, 0.0f, 1.0f },
	};
    // 最終描画用（final)
	Vertex4UV finalvertex[4] = {
		{  1.f,  1.f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ -1.f,  1.f, 1.0f, 1.0f, 0.0f, 0.0f },
		{  1.f, -1.f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ -1.f, -1.f, 1.0f, 1.0f, 0.0f, 1.0f },
	};

	// VetexBufferの格納先を宣言
	ID3D11Buffer *vertexbuffer = nullptr;
	ID3D11Buffer *finalvertexbuffer = nullptr;

	D3D11_BUFFER_DESC bd; // 生成方法(バッファー リソース)
	ZeroMemory(&bd, sizeof(bd)); // 中身をゼロクリア
	// Bufferの生成方法の格納
	bd.Usage = D3D11_USAGE_DYNAMIC; // バッファーで想定されている読み込みおよび書き込みの方法を識別
	bd.ByteWidth = sizeof(vertex);  // バッファーのサイズ(バイト単位)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // なんのバッファですか？
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUからは書き込みのみ行います
	// Bufferの生成
	device.getDevice()->CreateBuffer(&bd, nullptr, &vertexbuffer);

	//
	ZeroMemory(&bd, sizeof(bd)); // 中身をゼロクリア
	// Bufferの生成方法の格納
	bd.Usage = D3D11_USAGE_DYNAMIC; // バッファーで想定されている読み込みおよび書き込みの方法を識別
	bd.ByteWidth = sizeof(finalvertex);  // バッファーのサイズ(バイト単位)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // なんのバッファですか？
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUからは書き込みのみ行います
	// Bufferの生成
	device.getDevice()->CreateBuffer(&bd, nullptr, &finalvertexbuffer);

	// IMGUIの初期化
	ImGui_ImplDX11_Init(window.getHandle(), device.getDevice(), device.getContext());



	//　頂点情報を格納していく
	D3D11_MAPPED_SUBRESOURCE ms; // Bufferを格納する為にとりあえずロックをかけないといけない。どこまでロックをかける？サブリソース データにアクセスできるようにする
	device.getContext()->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // アクセス先ms
	memcpy(ms.pData, vertex, sizeof(vertex));// pData = vetexコピー　書き込み
	device.getContext()->Unmap(vertexbuffer, NULL); // ロック解除

	device.getContext()->Map(finalvertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // アクセス先ms
	memcpy(ms.pData, finalvertex, sizeof(finalvertex));// pData = vetexコピー　書き込み
	device.getContext()->Unmap(finalvertexbuffer, NULL); // ロック解除


	// シェーダーの生成
	ID3D11VertexShader *vs_buf = nullptr;       // shaderのbuffer　コンパイルしたシェーダーの格納先
	ID3D11PixelShader  *ps_buf = nullptr;       // shaderのbuffer　コンパイルしたシェーダーの格納先
	ID3D11VertexShader *vsfinal_buf = nullptr;  // shaderのbuffer　コンパイルしたシェーダーの格納先
	ID3D11PixelShader  *psfinal_buf = nullptr;  // shaderのbuffer　コンパイルしたシェーダーの格納先


	// 
	ID3D10Blob *vsblob, *psblob, *vsblobfinal, *psblobfinal; // 任意長のデータを返す際に使用
	// ファイルを元にエフェクトをコンパイル
	D3DX11CompileFromFile(TEXT("./Shader/VSDeferred.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PSDeferred.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/VSDeferredFinal.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblobfinal, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PSDeferredFinal.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblobfinal, 0, 0);
	// blobを_bufに格納
	device.getDevice()->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr, &vs_buf); // コンパイル済みシェーダーから、頂点シェーダー オブジェクトを作成
	device.getDevice()->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &ps_buf);  // ピクセル シェーダーを作成
	device.getDevice()->CreateVertexShader(vsblobfinal->GetBufferPointer(), vsblobfinal->GetBufferSize(), nullptr, &vsfinal_buf); // コンパイル済みシェーダーから、頂点シェーダー オブジェクトを作成
	device.getDevice()->CreatePixelShader(psblobfinal->GetBufferPointer(), psblobfinal->GetBufferSize(), nullptr, &psfinal_buf);  // ピクセル シェーダーを作成
	// Shagerの設定
	device.getContext()->VSSetShader(vsfinal_buf, nullptr, 0); // 頂点シェーダーをデバイスに設定
	device.getContext()->PSSetShader(psfinal_buf, nullptr, 0); // ピクセル シェーダーをデバイスに設定

	//　インプットレイアウトを使うために必要なもの 
	D3D11_INPUT_ELEMENT_DESC element[] = { // 入力アセンブラー ステージの単一の要素( HLSL セマンティクス,要素のセマンティクス インデックス,要素データのデータ型,入力アセンブラーを識別する整数値,各要素間のオフセット (バイト単位),単一の入力スロットの入力データ クラスを識別,インスタンス単位の同じデータを使用して描画するインスタンスの数)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },// 位置情報
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 } // UV情報 
	};
	ID3D11InputLayout *inputlayout; // 入力アセンブラー ステージの入力データにアクセス
	device.getDevice()->CreateInputLayout(element, ARRAYSIZE(element), vsblobfinal->GetBufferPointer(), vsblobfinal->GetBufferSize(), &inputlayout); // 格納(入力アセンブラー ステージで使用される入力バッファー データ)
	//インプットレイアウトの設定
	device.getContext()->IASetInputLayout(inputlayout); // 入力アセンブラー ステージに入力レイアウト オブジェクトをバインド
	// ラスライザの生成
	D3D11_RASTERIZER_DESC rasterizerDesc = {
		D3D11_FILL_SOLID, // ワイヤーフレーム (レンダリング時に使用する描画モードを決定)
		D3D11_CULL_FRONT, // 裏面ポリゴンをカリング(指定の方向を向いている三角形が描画されないことを示す)
		FALSE,			  // 三角形が前向きか後ろ向きかを決定する
		0,				  // 指定のピクセルに加算する深度値
		0.0f,             // ピクセルの最大深度バイアス
		FALSE,			  // 指定のピクセルのスロープに対するスカラ
		FALSE,			  // 距離に基づいてクリッピングを有効
		FALSE,            // シザー矩形カリングを有効
		FALSE,			  // マルチサンプリングのアンチエイリアシングを有効
		FALSE			  //　線のアンチエイリアシングを有効
	};
	Rasterizer rast;
	rast.Create(&rasterizerDesc);
	rast.SetStatus();

	// サンプラーの生成
	ID3D11SamplerState *samp;
	D3D11_SAMPLER_DESC sampDesc;
	//サンプラーの設定
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.BorderColor[0] = 0.0f; 
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 2;
	sampDesc.MinLOD = FLT_MAX * -1;
	sampDesc.MaxLOD = FLT_MAX;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	device.getDevice()->CreateSamplerState(&sampDesc,&samp);
	device.getContext()->PSSetSamplers(0, 1, &samp);  //  Albed
	device.getContext()->PSSetSamplers(1, 1, &samp);  //  Normal
	device.getContext()->PSSetSamplers(2, 1, &samp);  //  Depth
	device.getContext()->PSSetSamplers(3, 1, &samp);  //  Diffuse


	device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 頂点の並び方の設定(プリミティブ タイプおよびデータの順序に関する情報をバインド)

	// Shaderに送る行列の生成
	ConstantBuffer mtx;

	XMVECTOR hEye = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	XMVECTOR hAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR hUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mtx.mView = XMMatrixLookAtLH(hEye, hAt, hUp);
	mtx.mWorld = XMMatrixIdentity();
	mtx.mProjection = XMMatrixOrthographicLH((float) window.getWidth(), (float) window.getHeight(), 1, 5000);

	//コンテキストバッファ：シェーダーで宣言した定数をプログラム側から変更する(主に生成、更新、シェーダーステージへのセットという３つのアクション)
	// constantバッファ生成
	ID3D11Buffer *constantbuffer = nullptr;
	ZeroMemory(&bd, sizeof(bd)); // 中身をクリア
	// Bufferの生成方法の格納
	bd.ByteWidth = sizeof(ConstantBuffer); // sizeの指定
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // なんのバッファですか？
	bd.CPUAccessFlags = 0; // CPUからは書き込みのみ行います
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(float);
	device.getDevice()->CreateBuffer(&bd, NULL, &constantbuffer); // バッファの生成

	// テクスチャの読み込み
	Texture2D tex,tex2;
	tex.LoadFile("./Resource/Lenna.png");
	tex2.LoadFile("./Resource/lenna_normal.png");
	ID3D11ShaderResourceView *srv[] = {
		tex.getSRV(),
		tex2.getSRV()
	};
	device.getContext()->PSSetShaderResources(0, 2, srv); // ピクセル シェーダー ステージにシェーダー リソースの配列をバインド

	// MRT（マルチレンダーターゲット）
	//Gバッファの生成      ジオメトリバッファ(1pass 複数描画　2次元情報として保存:頂点シェーダとピクセルシェーダの間に実行)
	Texture2D GBuffer[4];
	UINT bindflg = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // リソースをパイプラインにバインドする方法を識別(、バインド フラグは論理和を使って組み合わせることができる)
	GBuffer[0].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Albed
	GBuffer[1].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Normal
	GBuffer[2].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_FLOAT, bindflg);			//Depth
	GBuffer[3].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Diffuse
	// レンダリング時にシェーダーがアクセス可能なサブリソースを指定(ShaderResourceView)
	ID3D11ShaderResourceView *GBufferSRV [] = { 
		GBuffer[0].getSRV(), //  Albed
		GBuffer[1].getSRV(), //  Normal
		GBuffer[2].getSRV(), //  Depth
		GBuffer[3].getSRV(), //  Diffuse
	};
	// レンダリング時にアクセス可能なレンダー ターゲットのサブリソースを識別(Render Target View)
	ID3D11RenderTargetView *GBufferRTV [] = { 
		GBuffer[0].getRTV(), //  Albed
		GBuffer[1].getRTV(), //  Normal
		GBuffer[2].getRTV(), //  Depth
		GBuffer[3].getRTV(), //  Diffuse
	};
	int ret = 0;
	// クリアの際に使用する
	ID3D11ShaderResourceView *NULLSRV[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	ID3D11RenderTargetView   *NULLRTV[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	while (ret != WM_QUIT){
		ret = window.MessageLoop();
		float clear [] = { 0.0f, 0.0f, 0.0f, 0.0f };
		// パイプラインのクリア
		device.getContext()->OMSetRenderTargets(8, NULLRTV, nullptr); // レンダーターゲットのクリア
		device.getContext()->PSSetShaderResources(0,8, NULLSRV);      // シェーダーリソースのクリア
		//バックバッファのクリア
		DEBUG(device.getAnotation()->BeginEvent(L"バッファのクリア"));
		device.getContext()->ClearRenderTargetView(device.getRTV(), clear);
		device.getContext()->ClearDepthStencilView(device.getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GBufferのクリア
		for (auto v : GBufferRTV) {
			device.getContext()->ClearRenderTargetView(v, clear);
		}
		DEBUG(device.getAnotation()->EndEvent());
		//GUIのクリア
		ImGui_ImplDX11_NewFrame();

		ImGui::Text("Debug Text");

		//通常描画で使うバッファの更新
		// Bufferに更新をかける (コンスタントバッファを更新）
		device.getContext()->UpdateSubresource(constantbuffer, 0, NULL, &mtx, 0, 0); // CPU によって、マッピング不可能なメモリー内に作成されたサブリソースにメモリーからデータがコピーされる
		// Bufferをパイプラインにセット (シェーダーステージへのセット)
		device.getContext()->VSSetConstantBuffers(0, 1, &constantbuffer); // 頂点シェーダーのパイプライン ステージで使用される定数バッファーを設定

		//通常描画の設定
		device.getContext()->OMSetRenderTargets(4, GBufferRTV, nullptr); // 出力結合ステージに深度ステンシル バッファーをバインド
		UINT stride = sizeof(Vertex4UV); // 頂点のサイズ
		UINT offset = 0;			   // ずれの調整
		device.getContext()->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset); // 入力アセンブラー ステージに頂点バッファーの配列をバインド
		device.getContext()->VSSetShader(vs_buf, nullptr, 0); // 頂点シェーダーをデバイスに設定
		device.getContext()->PSSetShader(ps_buf, nullptr, 0); // ピクセル シェーダーをデバイスに設定

		device.getContext()->IASetInputLayout(inputlayout); // 入力アセンブラー ステージに入力レイアウト オブジェクトをバインド

		//テクスチャの設定
		ID3D11ShaderResourceView *srv [] = { // レンダリング時にシェーダーがアクセス可能なサブリソースを指定
			tex.getSRV(),
			tex2.getSRV()
		};
		device.getContext()->PSSetShaderResources(0, 2, srv); // ピクセル シェーダー ステージにシェーダー リソースの配列をバインド
		device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // プリミティブ タイプおよびデータの順序に関する情報をバインド
		//通常描画
		device.getContext()->Draw(4, 0);// 頂点数:何番目の頂点からやるか

		//ディファードの最終描画の設定
		DEBUG(device.getAnotation()->BeginEvent(L"ディファードの最終合成"));
		ID3D11RenderTargetView *finalrtv [] = {
			device.getRTV(),
		};
		device.getContext()->OMSetRenderTargets(1, finalrtv, nullptr); //  // 出力結合ステージに深度ステンシル バッファーをバインド
		device.getContext()->IASetVertexBuffers(0, 1, &finalvertexbuffer, &stride, &offset);
		device.getContext()->VSSetShader(vsfinal_buf, nullptr, 0); // 頂点シェーダーをデバイスに設定
		device.getContext()->PSSetShader(psfinal_buf, nullptr, 0); // ピクセル シェーダーをデバイスに設定

		device.getContext()->IASetInputLayout(inputlayout); // // 入力アセンブラー ステージに入力レイアウト オブジェクトをバインド
		device.getContext()->PSSetShaderResources(0, 4, GBufferSRV);  // ピクセル シェーダー ステージにシェーダー リソースの配列をバインド
		device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); //  // プリミティブ タイプおよびデータの順序に関する情報をバインド
		//ディファードの最終描画
		device.getContext()->Draw(4, 0);// 頂点数:何番目の頂点からやるか
		DEBUG(device.getAnotation()->EndEvent());
		//Guiの描画
		ImGui::Render();
		//バックバッファとフロントバッファの切り替え
		device.getSwapChain()->Present(0, 0);
	}
	//ImGuiの終了処理
	ImGui_ImplDX11_Shutdown();

	// マクロリリース
	SAFE_RELEASE(vertexbuffer);
	SAFE_RELEASE(finalvertexbuffer);
	SAFE_RELEASE(vs_buf);
	SAFE_RELEASE(ps_buf);
	SAFE_RELEASE(vsfinal_buf);
	SAFE_RELEASE(psfinal_buf);
	SAFE_RELEASE(inputlayout);
	SAFE_RELEASE(constantbuffer);
	SAFE_RELEASE(samp);


	return ret;
}