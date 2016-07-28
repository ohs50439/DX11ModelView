

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
// ��
#include <./Graphics/GraphicsPipeLine.h>
#include <./Graphics/DX11/Rasterizer.h>
#include <./Graphics/DX11/Sampler.h>


// Shader�ɑ���J�������
struct ConstantBuffer{
	XMMATRIX mWorld;		//���[���h�ϊ��s��
	XMMATRIX mView;			//�r���[�ϊ��s��
	XMMATRIX mProjection;	//�������e�ϊ��s��
};
 
void CrateDeferredPipeLine(GraphicsPipeLine &pipe); // �v���g�^�C�v�錾

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {

	//DX�������ɂ̓f�o�C�X�A�R���e�L�X�g�A�X���b�v�`�F�C����3�A�����ă����_�����O�^�[�Q�b�g
	WindowDevice &window = WindowDevice::getInstance();
	DX11Device &device = DX11Device::getInstance();
	window.Init(hInstance, nCmdShow, TEXT("test window"), 1280, 720, true);
	device.Init(window.getHandle(), window.getWidth(), window.getHeight(), window.getWindowMode());
	// IMGUI�̏�����
	ImGui_ImplDX11_Init(window.getHandle(), device.getDevice(), device.getContext());

	GraphicsPipeLine deferredGPL;
	//=====================================================//
	//     ���_�f�[�^���iFarstPass & FinalPass)  1.CreateBuffer�Ő��� 2.IASetVertexBuffers�Őݒ� 3.Draw�Ŏ��s(�`��)
	//=====================================================//
	//***************�錾********************//
	ID3D11Buffer *vertexbuffer = nullptr;		 // FirstPassBuffer
	ID3D11Buffer *finalvertexbuffer = nullptr;   // FinalPassBuffer
	D3D11_BUFFER_DESC bd;						 // �������@(�o�b�t�@�[ ���\�[�X)
	//***************���_����********************//
	// �ŏ��`��p�@  (FarstPass)
	Vertex4UV vertex[4] = {
		{  100.f,  100.f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ -100.f,  100.f, 1.0f, 1.0f, 0.0f, 0.0f },
		{  100.f, -100.f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ -100.f, -100.f, 1.0f, 1.0f, 0.0f, 1.0f },
	};
    // �ŏI�`��p�@�@�iFinalPass)
	Vertex4UV finalvertex[4] = {
		{  1.f,  1.f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ -1.f,  1.f, 1.0f, 1.0f, 0.0f, 0.0f },
		{  1.f, -1.f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ -1.f, -1.f, 1.0f, 1.0f, 0.0f, 1.0f },
	};
	//***************���_�o�b�t�@�ݒ�********************//(BufferDESC�ɐ������@���i�[��CreateBuffer�Ő����܂�)
	// FirstPass�ݒ�
	ZeroMemory(&bd, sizeof(bd)); // ���g���[���N���A
	bd.Usage = D3D11_USAGE_DYNAMIC; // �o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@������
	bd.ByteWidth = sizeof(vertex);  // �o�b�t�@�[�̃T�C�Y(�o�C�g�P��)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �Ȃ�̃o�b�t�@�ł����H
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU����͏������݂̂ݍs���܂�
	device.getDevice()->CreateBuffer(&bd, nullptr, &vertexbuffer); // �ݒ肵��Buffer�𐶐�
	// FinalPass�ݒ�
	ZeroMemory(&bd, sizeof(bd)); // ���g���[���N���A
	bd.Usage = D3D11_USAGE_DYNAMIC; // �o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@������
	bd.ByteWidth = sizeof(finalvertex);  // �o�b�t�@�[�̃T�C�Y(�o�C�g�P��)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �Ȃ�̃o�b�t�@�ł����H
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU����͏������݂̂ݍs���܂�
	device.getDevice()->CreateBuffer(&bd, nullptr, &finalvertexbuffer); // �ݒ肵��Buffer�𐶐�
	//***************���_�����i�[���Ă���********************//(BufferDESC�ɐ������@���i�[��CreateBuffer�Ő����܂�)
	// FirstPass�i�[
	D3D11_MAPPED_SUBRESOURCE ms; // Buffer���i�[����ׂɂƂ肠�������b�N�������Ȃ��Ƃ����Ȃ��B�ǂ��܂Ń��b�N��������H�T�u���\�[�X �f�[�^�ɃA�N�Z�X�ł���悤�ɂ���
	device.getContext()->Map(vertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // �A�N�Z�X��ms
	memcpy(ms.pData, vertex, sizeof(vertex));// pData = vetex�R�s�[�@��������
	device.getContext()->Unmap(vertexbuffer, NULL); // ���b�N����
	// FinalPass�i�[
	device.getContext()->Map(finalvertexbuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // �A�N�Z�X��ms
	memcpy(ms.pData, finalvertex, sizeof(finalvertex));// pData = vetex�R�s�[�@��������
	device.getContext()->Unmap(finalvertexbuffer, NULL); // ���b�N����

	//=====================================================//
	//       �V�F�[�_�[���iVertexShader&PixelShader)   1.Create(Vertex & Pixel)Shader�Ő���  2.(VS & PS)SetShader�Ŏ��s
	//=====================================================//
	//***************�錾********************//
	ID3D11VertexShader *vs_buf = nullptr;       // (first) shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[��
	ID3D11PixelShader  *ps_buf = nullptr;       // (first) shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[��
	ID3D11VertexShader *vsfinal_buf = nullptr;  // (final) shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[��
	ID3D11PixelShader  *psfinal_buf = nullptr;  // (final) shader��buffer�@�R���p�C�������V�F�[�_�[�̊i�[��
	ID3D10Blob *vsblob, *psblob, *vsblobfinal, *psblobfinal; // �C�Ӓ��̃f�[�^��Ԃ��ۂɎg�p
	//***************�V�F�[�_�[�R�[�h�i�[�t�@�C���R���p�C��********************//
	D3DX11CompileFromFile(TEXT("./Shader/VSDeferred.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PSDeferred.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblob, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/VSDeferredFinal.hlsl"), 0, 0, "main", "vs_5_0", 0, 0, 0, &vsblobfinal, 0, 0);
	D3DX11CompileFromFile(TEXT("./Shader/PSDeferredFinal.hlsl"), 0, 0, "main", "ps_5_0", 0, 0, 0, &psblobfinal, 0, 0);
	//***************�V�F�[�_�[�̐ݒ�(VS&PS)**********************************//
	device.getDevice()->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr, &vs_buf); // �R���p�C���ς݃V�F�[�_�[����A���_�V�F�[�_�[ �I�u�W�F�N�g���쐬
	device.getDevice()->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &ps_buf);  // �s�N�Z�� �V�F�[�_�[���쐬
	device.getDevice()->CreateVertexShader(vsblobfinal->GetBufferPointer(), vsblobfinal->GetBufferSize(), nullptr, &vsfinal_buf); // �R���p�C���ς݃V�F�[�_�[����A���_�V�F�[�_�[ �I�u�W�F�N�g���쐬
	device.getDevice()->CreatePixelShader(psblobfinal->GetBufferPointer(), psblobfinal->GetBufferSize(), nullptr, &psfinal_buf);  // �s�N�Z�� �V�F�[�_�[���쐬
	// ���_�V�F�[�_�[���f�o�C�X�ɐݒ�i���s�j
	device.getContext()->VSSetShader(vsfinal_buf, nullptr, 0); // ���_�V�F�[�_�[���f�o�C�X�ɐݒ�
	device.getContext()->PSSetShader(psfinal_buf, nullptr, 0); // �s�N�Z�� �V�F�[�_�[���f�o�C�X�ɐݒ�

	//=====================================================//
	///            ���_���C�A�E�g���
	//=====================================================//
	//�@���̓��C�A�E�g(���C�A�E�g�����R���p�C���ς�VertexShader���瓮�I�ɍ\�z)
	D3D11_INPUT_ELEMENT_DESC element[] = { // ���̓A�Z���u���[ �X�e�[�W�̒P��̗v�f( HLSL �Z�}���e�B�N�X,�v�f�̃Z�}���e�B�N�X �C���f�b�N�X,�v�f�f�[�^�̃f�[�^�^,���̓A�Z���u���[�����ʂ��鐮���l,�e�v�f�Ԃ̃I�t�Z�b�g (�o�C�g�P��),�P��̓��̓X���b�g�̓��̓f�[�^ �N���X������,�C���X�^���X�P�ʂ̓����f�[�^���g�p���ĕ`�悷��C���X�^���X�̐�)
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },// �ʒu���
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 } // UV��� 
	};
	ID3D11InputLayout *inputlayout; // ���̓A�Z���u���[ �X�e�[�W�̓��̓f�[�^�ɃA�N�Z�X
	device.getDevice()->CreateInputLayout(element, ARRAYSIZE(element), vsblobfinal->GetBufferPointer(), vsblobfinal->GetBufferSize(), &inputlayout); // �i�[(���̓A�Z���u���[ �X�e�[�W�Ŏg�p�������̓o�b�t�@�[ �f�[�^)
	device.getContext()->IASetInputLayout(inputlayout); //�C���v�b�g���C�A�E�g�̐ݒ� ���̓A�Z���u���[ �X�e�[�W�ɓ��̓��C�A�E�g �I�u�W�F�N�g���o�C���h
	device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // ���_�̕��ѕ��̐ݒ�(�v���~�e�B�u �^�C�v����уf�[�^�̏����Ɋւ�������o�C���h)
	

	// �ʏ�`��p�̃p�C�v���C��
	CrateDeferredPipeLine(deferredGPL);

	//=====================================================//
	//            �J�������
	//=====================================================//
	ConstantBuffer mtx; // Shader�ɑ���s��̐���
	XMVECTOR hEye = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	XMVECTOR hAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR hUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mtx.mView = XMMatrixLookAtLH(hEye, hAt, hUp);
	mtx.mWorld = XMMatrixIdentity();
	mtx.mProjection = XMMatrixOrthographicLH((float) window.getWidth(), (float) window.getHeight(), 1, 5000);

	//=====================================================//
	//            �R���X�^���g���
	//=====================================================//
	//�R���e�L�X�g�o�b�t�@�F�V�F�[�_�[�Ő錾�����萔���v���O����������ύX����(��ɐ����A�X�V�A�V�F�[�_�[�X�e�[�W�ւ̃Z�b�g�Ƃ����R�̃A�N�V����)
	// constant�o�b�t�@����
	ID3D11Buffer *constantbuffer = nullptr;
	ZeroMemory(&bd, sizeof(bd)); // ���g���N���A
	// Buffer�̐������@�̊i�[
	bd.ByteWidth = sizeof(ConstantBuffer); // size�̎w��
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �Ȃ�̃o�b�t�@�ł����H
	bd.CPUAccessFlags = 0; // CPU����͏������݂̂ݍs���܂�
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(float);
	device.getDevice()->CreateBuffer(&bd, NULL, &constantbuffer); // �o�b�t�@�̐���

	//=====================================================//
	//           �o�b�t�@���
	//=====================================================//
	// �e�N�X�`���̓ǂݍ���
	Texture2D tex,tex2;
	tex.LoadFile("./Resource/Lenna.png");
	tex2.LoadFile("./Resource/lenna_normal.png");
	ID3D11ShaderResourceView *srv[] = {
		tex.getSRV(),
		tex2.getSRV()
	};
	device.getContext()->PSSetShaderResources(0, 2, srv); // �s�N�Z�� �V�F�[�_�[ �X�e�[�W�ɃV�F�[�_�[ ���\�[�X�̔z����o�C���h

	// MRT�i�}���`�����_�[�^�[�Q�b�g�j
	//G�o�b�t�@�̐���      �W�I���g���o�b�t�@(1pass �����`��@2�������Ƃ��ĕۑ�:���_�V�F�[�_�ƃs�N�Z���V�F�[�_�̊ԂɎ��s)
	Texture2D GBuffer[4];
	UINT bindflg = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // ���\�[�X���p�C�v���C���Ƀo�C���h������@������(�A�o�C���h �t���O�͘_���a���g���đg�ݍ��킹�邱�Ƃ��ł���)
	GBuffer[0].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Albed
	GBuffer[1].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Normal
	GBuffer[2].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_FLOAT, bindflg);			//Depth
	GBuffer[3].Create(window.getWidth(), window.getHeight(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT, bindflg);	//Diffuse
	// �����_�����O���ɃV�F�[�_�[���A�N�Z�X�\�ȃT�u���\�[�X���w��(ShaderResourceView)
	ID3D11ShaderResourceView *GBufferSRV [] = { 
		GBuffer[0].getSRV(), //  Albed
		GBuffer[1].getSRV(), //  Normal
		GBuffer[2].getSRV(), //  Depth
		GBuffer[3].getSRV(), //  Diffuse
	};
	// �����_�����O���ɃA�N�Z�X�\�ȃ����_�[ �^�[�Q�b�g�̃T�u���\�[�X������(Render Target View)
	ID3D11RenderTargetView *GBufferRTV [] = { 
		GBuffer[0].getRTV(), //  Albed
		GBuffer[1].getRTV(), //  Normal
		GBuffer[2].getRTV(), //  Depth
		GBuffer[3].getRTV(), //  Diffuse
	};
	// �N���A�̍ۂɎg�p����
	ID3D11ShaderResourceView *NULLSRV[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	ID3D11RenderTargetView   *NULLRTV[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	// �p�C�v���C���̍\�z
	deferredGPL.setStatus();

	//=====================================================//
	//           ���[�v����
	//=====================================================//
	int ret = 0;
	while (ret != WM_QUIT){
		ret = window.MessageLoop();
		float clear [] = { 0.0f, 0.0f, 0.0f, 0.0f };
		// �p�C�v���C���̃N���A
		device.getContext()->OMSetRenderTargets(8, NULLRTV, nullptr); // �����_�[�^�[�Q�b�g�̃N���A
		device.getContext()->PSSetShaderResources(0,8, NULLSRV);      // �V�F�[�_�[���\�[�X�̃N���A
		//�o�b�N�o�b�t�@�̃N���A
		DEBUG(device.getAnotation()->BeginEvent(L"�o�b�t�@�̃N���A"));
		device.getContext()->ClearRenderTargetView(device.getRTV(), clear);
		device.getContext()->ClearDepthStencilView(device.getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//GBuffer�̃N���A
		for (auto v : GBufferRTV) {
			device.getContext()->ClearRenderTargetView(v, clear);
		}
		DEBUG(device.getAnotation()->EndEvent());
		//GUI�̃N���A
		ImGui_ImplDX11_NewFrame();
		// �f�o�b�O���̏o��
		ImGui::Text("Debug Text");
		ImGui::Text("Application.average %.3f ms/frame(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); // FPS
		//�ʏ�`��Ŏg���o�b�t�@�̍X�V
		// Buffer�ɍX�V�������� (�R���X�^���g�o�b�t�@���X�V�j
		device.getContext()->UpdateSubresource(constantbuffer, 0, NULL, &mtx, 0, 0); // CPU �ɂ���āA�}�b�s���O�s�\�ȃ������[���ɍ쐬���ꂽ�T�u���\�[�X�Ƀ������[����f�[�^���R�s�[�����
		// Buffer���p�C�v���C���ɃZ�b�g (�V�F�[�_�[�X�e�[�W�ւ̃Z�b�g)
		device.getContext()->VSSetConstantBuffers(0, 1, &constantbuffer); // ���_�V�F�[�_�[�̃p�C�v���C�� �X�e�[�W�Ŏg�p�����萔�o�b�t�@�[��ݒ�

		//�ʏ�`��̐ݒ�
		device.getContext()->OMSetRenderTargets(4, GBufferRTV, nullptr); // �o�͌����X�e�[�W�ɐ[�x�X�e���V�� �o�b�t�@�[���o�C���h
		UINT stride = sizeof(Vertex4UV); // ���_�̃T�C�Y
		UINT offset = 0;			   // ����̒���
		device.getContext()->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset); // ���̓A�Z���u���[ �X�e�[�W�ɒ��_�o�b�t�@�[�̔z����o�C���h
		device.getContext()->VSSetShader(vs_buf, nullptr, 0); // ���_�V�F�[�_�[���f�o�C�X�ɐݒ�
		device.getContext()->PSSetShader(ps_buf, nullptr, 0); // �s�N�Z�� �V�F�[�_�[���f�o�C�X�ɐݒ�

		device.getContext()->IASetInputLayout(inputlayout); // ���̓A�Z���u���[ �X�e�[�W�ɓ��̓��C�A�E�g �I�u�W�F�N�g���o�C���h

		//�e�N�X�`���̐ݒ�
		ID3D11ShaderResourceView *srv [] = { // �����_�����O���ɃV�F�[�_�[���A�N�Z�X�\�ȃT�u���\�[�X���w��
			tex.getSRV(),
			tex2.getSRV()
		};
		device.getContext()->PSSetShaderResources(0, 2, srv); // �s�N�Z�� �V�F�[�_�[ �X�e�[�W�ɃV�F�[�_�[ ���\�[�X�̔z����o�C���h
		device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �v���~�e�B�u �^�C�v����уf�[�^�̏����Ɋւ�������o�C���h
		//�ʏ�`��
		device.getContext()->Draw(4, 0);// ���_��:���Ԗڂ̒��_�����邩

		//�f�B�t�@�[�h�̍ŏI�`��̐ݒ�
		DEBUG(device.getAnotation()->BeginEvent(L"�f�B�t�@�[�h�̍ŏI����"));
		ID3D11RenderTargetView *finalrtv [] = {
			device.getRTV(),
		};
		device.getContext()->OMSetRenderTargets(1, finalrtv, nullptr); //  // �o�͌����X�e�[�W�ɐ[�x�X�e���V�� �o�b�t�@�[���o�C���h
		device.getContext()->IASetVertexBuffers(0, 1, &finalvertexbuffer, &stride, &offset);
		device.getContext()->VSSetShader(vsfinal_buf, nullptr, 0); // ���_�V�F�[�_�[���f�o�C�X�ɐݒ�
		device.getContext()->PSSetShader(psfinal_buf, nullptr, 0); // �s�N�Z�� �V�F�[�_�[���f�o�C�X�ɐݒ�

		device.getContext()->IASetInputLayout(inputlayout); // // ���̓A�Z���u���[ �X�e�[�W�ɓ��̓��C�A�E�g �I�u�W�F�N�g���o�C���h
		device.getContext()->PSSetShaderResources(0, 4, GBufferSRV);  // �s�N�Z�� �V�F�[�_�[ �X�e�[�W�ɃV�F�[�_�[ ���\�[�X�̔z����o�C���h
		device.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); //  // �v���~�e�B�u �^�C�v����уf�[�^�̏����Ɋւ�������o�C���h
		//�f�B�t�@�[�h�̍ŏI�`��
		device.getContext()->Draw(4, 0);// ���_��:���Ԗڂ̒��_�����邩
		DEBUG(device.getAnotation()->EndEvent());
		//Gui�̕`��
		ImGui::Render();
		//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̐؂�ւ�
		device.getSwapChain()->Present(0, 0);
	}
	//=====================================================//
	//         �@�@�@�������
	//=====================================================//
	//ImGui�̏I������
	ImGui_ImplDX11_Shutdown();
	// �}�N�������[�X
	SAFE_RELEASE(vertexbuffer);
	SAFE_RELEASE(finalvertexbuffer);
	SAFE_RELEASE(vs_buf);
	SAFE_RELEASE(ps_buf);
	SAFE_RELEASE(vsfinal_buf);
	SAFE_RELEASE(psfinal_buf);
	SAFE_RELEASE(inputlayout);
	SAFE_RELEASE(constantbuffer);
	deferredGPL.Release();

	return ret;
}

void CrateDeferredPipeLine(GraphicsPipeLine &GPL){
	//=====================================================//
	//         �@�@�@���X���C�U���
	//=====================================================//
	D3D11_RASTERIZER_DESC rasterizerDesc = {
		D3D11_FILL_SOLID, // ���C���[�t���[�� (�����_�����O���Ɏg�p����`�惂�[�h������)
		D3D11_CULL_FRONT, // ���ʃ|���S�����J�����O(�w��̕����������Ă���O�p�`���`�悳��Ȃ����Ƃ�����)
		FALSE,			  // �O�p�`���O�������������������肷��
		0,				  // �w��̃s�N�Z���ɉ��Z����[�x�l
		0.0f,             // �s�N�Z���̍ő�[�x�o�C�A�X
		FALSE,			  // �w��̃s�N�Z���̃X���[�v�ɑ΂���X�J��
		FALSE,			  // �����Ɋ�Â��ăN���b�s���O��L��
		FALSE,            // �V�U�[��`�J�����O��L��
		FALSE,			  // �}���`�T���v�����O�̃A���`�G�C���A�V���O��L��
		FALSE			  //�@���̃A���`�G�C���A�V���O��L��
	};
	Rasterizer *rast = new Rasterizer();// ���X�^���C�U�̐���
	rast->Create(&rasterizerDesc);

	GPL.Attach(rast); // �p�C�v���C���Ƀ��X�^���C�U��ǉ�
	//=====================================================//
	//         �@�@�@�T���v���[���
	//=====================================================//
	// �T���v���[�̐���
	D3D11_SAMPLER_DESC sampDesc;
	//�T���v���[�̐ݒ�
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
	Sampler *samp = new Sampler();
	samp->Create(0, &sampDesc);
	samp->Attach(1, samp, 0);
	samp->Attach(2, samp, 0);
	samp->Attach(3, samp, 0);

	GPL.Attach(samp); // �p�C�v���C���ɃT���v���[��ǉ�

}