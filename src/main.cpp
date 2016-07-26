

#include <Windows.h>
#include <xnamath.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <./Device/WindowDevice.h>
#include <./Device/DirectXDevice.h>
#include <./Vertex.h>
#include <./Resource/Texture.h>


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

	// 頂点の生成
	Vertex4UV vertex[4] = {
		{  500.f,  500.f, 1.0f,0.0f, 1.0f, 0.0f },
		{ -500.f,  500.f, 1.0f,0.0f, 0.0f, 0.0f },
		{  500.f, -500.f, 1.0f,0.0f, 1.0f, 1.0f },
		{ -500.f, -500.f, 1.0f,0.0f, 0.0f, 1.0f },
	};

	// VetexBufferの格納先を宣言
	ID3D11Buffer *vertexbuffer = nullptr;

	D3D11_BUFFER_DESC bd; // 生成方法
	ZeroMemory(&bd, sizeof(bd)); // 中身をクリア
	// Bufferの生成方法の格納
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // なんのバッファですか？
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUからは書き込みのみ行います
	// Bufferの生成
	device.getDevice()->CreateBuffer(&bd, nullptr, &vertexbuffer);

	//　頂点情報を格納していく
	D3D11_MAPPED_SUBRESOURCE ms; // Bufferを格納する為にとりあえずロックをかけないといけない。どこまでロックをかける？
	device.getContext()->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // アクセス先ms
	memcpy(ms.pData, vertex, sizeof(vertex));// pData = vetexコピー　書き込み
	device.getContext()->Unmap(vertexbuffer, NULL); // ロック解除

	// パイプラインに頂点バッファのセット
	UINT stride = sizeof(Vertex4UV); // 頂点のサイズ
	UINT offset = 0;			   // ずれの調整
	device.getContext()->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);

	// シェーダーの生成
	ID3D11VertexShader *vs_buf = nullptr;  // shaderのbuffer　コンパイルしたシェーダーの格納先 
	ID3D11PixelShader  *ps_buf = nullptr;  // shaderのbuffer　コンパイルしたシェーダーの格納先


	// 
	ID3D10Blob *vsblob, *psblob;
	D3DX11CompileFromFile(TEXT("./Shader/VS.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PS.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblob, 0, 0);
	// blobを_bufに格納
	device.getDevice()->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr ,&vs_buf);
	device.getDevice()->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &ps_buf);
	// Shagerの設定
	device.getContext()->VSSetShader(vs_buf, nullptr, 0);
	device.getContext()->PSSetShader(ps_buf, nullptr, 0);

	//　インプットレイアウトを使うために必要なもの 
	D3D11_INPUT_ELEMENT_DESC element[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },// 位置情報
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 } // UV情報 
	};
	ID3D11InputLayout *inputlayout;
	device.getDevice()->CreateInputLayout(element, ARRAYSIZE(element), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout); // 格納
	//インプットレイアウトの設定
	device.getContext()->IASetInputLayout(inputlayout);
	// ラスライザの生成
	ID3D11RasterizerState* rasterizer = nullptr;
	D3D11_RASTERIZER_DESC rasterizerDesc = {
		D3D11_FILL_SOLID, // ワイヤーフレーム (見えるよフレーム)
		D3D11_CULL_FRONT, // 裏面ポリゴンをカリング
		FALSE,
		0,
		0.0f,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		FALSE
	};
	device.getDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizer); // ラスタライザーの生成
	// ラスタライザー設定
	device.getContext()->RSSetState(rasterizer);

	// 頂点の並び方の設定
	device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Shaderに送る行列の生成
	ConstantBuffer mtx;

	XMVECTOR hEye = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	XMVECTOR hAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR hUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mtx.mView = XMMatrixLookAtLH(hEye, hAt, hUp);
	mtx.mWorld = XMMatrixIdentity();
	mtx.mProjection = XMMatrixOrthographicLH(1280, 720, 1, 5000);

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
	// Bufferに更新をかける 
	device.getContext()->UpdateSubresource(constantbuffer, 0, NULL, &mtx, 0, 0);
	// Bufferをパイプラインにセット
	device.getContext()->VSSetConstantBuffers(0, 1, &constantbuffer);

	// テクスチャの読み込み
	Texture2D tex,tex2;
	tex.LoadFile("./Resource/Lenna.png");
	tex2.LoadFile("./Resource/lenna_normal.png");
	ID3D11ShaderResourceView *srv[] = {
		tex.getSRV(),
		tex2.getSRV()
	};
	device.getContext()->PSSetShaderResources(0, 2, srv);


	int ret = 0;
	while (ret != WM_QUIT){
		ret = window.MessageLoop();
		float clear[] = { 0.3f, 0.3f, 0.3f, 0.0f };
		device.getContext()->ClearRenderTargetView(device.getRTV(), clear);
		device.getContext()->ClearDepthStencilView(device.getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		// Boxの描画
		device.getContext()->Draw(4, 0);// 頂点数:何番目の頂点からやるか


		device.getSwapChain()->Present(0, 0);
	}


	SAFE_RELEASE(vertexbuffer);
	SAFE_RELEASE(vs_buf);
	SAFE_RELEASE(ps_buf);
	SAFE_RELEASE(inputlayout);
	SAFE_RELEASE(rasterizer);
	SAFE_RELEASE(constantbuffer);


	return ret;
}