#include "Manager/EffectManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
EffectManager::EffectManager(Dx12Wrapper& dx12) :_dx12(dx12)
{
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;

	_efkRenderer = EffekseerRendererDX12::Create(
		_dx12.Device(),
		_dx12.CommandQueue(),
		2,
		&format,
		1,
		DXGI_FORMAT_D32_FLOAT,
		false,
		10000);

	_efkManager = Effekseer::Manager::Create(10000);

	_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	_efkManager->SetSpriteRenderer(_efkRenderer->CreateSpriteRenderer());
	_efkManager->SetRibbonRenderer(_efkRenderer->CreateRibbonRenderer());
	_efkManager->SetRingRenderer(_efkRenderer->CreateRingRenderer());
	_efkManager->SetTrackRenderer(_efkRenderer->CreateTrackRenderer());
	_efkManager->SetModelRenderer(_efkRenderer->CreateModelRenderer());

	_efkManager->SetTextureLoader(_efkRenderer->CreateTextureLoader());
	_efkManager->SetModelLoader(_efkRenderer->CreateModelLoader());

	_efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(_efkRenderer->GetGraphicsDevice());
	_efkCmdList = EffekseerRenderer::CreateCommandList(_efkRenderer->GetGraphicsDevice(), _efkMemoryPool);

	_efkRenderer->SetCommandList(_efkCmdList);

	_effect = Effekseer::Effect::Create(
		_efkManager,
		(const EFK_CHAR*)L"Asset/effect/TriggerLaser.efkefc",
		5.0f,
		(const EFK_CHAR*)L"Asset/effect"
	);
}

/// <summary>
/// エフェクトを開始する関数
/// </summary>
void
EffectManager::CallEffect()
{
	_efkHandle = _efkManager->Play(_effect, 0, 0, 0);
}

/// <summary>
/// エフェクトを更新する関数
/// </summary>
void
EffectManager::Draw()
{
	_efkManager->Update();
	_efkMemoryPool->NewFrame();

	BeginCommandList(_efkCmdList, _dx12.CommandList());

	_efkRenderer->BeginRendering();
	_efkManager->Draw();
	_efkRenderer->EndRendering();

	EndCommandList(_efkCmdList);

	Effekseer::Matrix44 fkViewMat;
	Effekseer::Matrix44 fkProjMat;

	auto view = _dx12.ViewMatrix();
	auto proj = _dx12.ProjMatrix();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fkViewMat.Values[i][j] = view.r[i].m128_f32[j];
			fkProjMat.Values[i][j] = proj.r[i].m128_f32[j];
		}
	}

	_efkRenderer->SetCameraMatrix(fkViewMat);
	_efkRenderer->SetProjectionMatrix(fkProjMat);
}