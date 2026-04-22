#include "AudioService.h"
#include "audio/Audio.h"
#include "resource/Resource.h"
#include "core/EventManager.h"

namespace {
	uint8_t getPriority(SfxId id) noexcept {
		switch (id) {
		case SfxId::PlayerAttackSwing:
			return static_cast<uint8_t>(Priority::Low); // 挥刀音效优先级较低
		case SfxId::PlayerAttackHit:
			return static_cast<uint8_t>(Priority::High); // 攻击音效优先级较高
		case SfxId::PlayerJump:
		case SfxId::PlayerSprint:
			return static_cast<uint8_t>(Priority::Medium); // 跳跃和冲刺音效优先级中等
		case SfxId::PlayerHurt:
			return static_cast<uint8_t>(Priority::Max); // 受伤和死亡音效优先级最高
		default:
			return 0; // 默认优先级最低
		}
	}
}

AudioService::AudioService(Audio& aM, Resource& rM, EventManager& eM) noexcept : audio_(aM) , resource_(rM), eventManager_(eM){
	playBgmSubscriptionId_ = eventManager_.subscribe(EventType::Audio_PlayBgm, [this](const Event& event) {
		if (event.hasData<BgmId>()) {
			playBgm(event.getData<BgmId>());
		}
		});
	playSfxSubscriptionId_ = eventManager_.subscribe(EventType::Audio_PlaySfx, [this](const Event& event) {
		if (event.hasData<SfxId>()) {
			playSfx(event.getData<SfxId>());
		}
		});
	pauseBgmSubscriptionId_ = eventManager_.subscribe(EventType::Audio_PauseBgm, [this](const Event& event) {
		audio_.pauseBgm();
		});
	resumeBgmSubscriptionId_ = eventManager_.subscribe(EventType::Audio_ResumeBgm, [this](const Event& event) {
		audio_.resumeBgm();
		});
	stopBgmSubscriptionId_ = eventManager_.subscribe(EventType::Audio_StopBgm, [this](const Event& event) {
		currentBgmId_ = BgmId::None; // 停止背景音乐时重置当前BGM ID
		audio_.stopBgm(); 
		});
	currentBgmId_ = BgmId::None; // 初始背景音乐为菜单音乐
}

AudioService::~AudioService() noexcept{
	destroy();
}

void AudioService::playBgm(BgmId id) noexcept{
	if(id == currentBgmId_) {
		return; // 如果当前正在播放的BGM与请求的相同，则不重新加载和播放
	}
	currentBgmId_ = id; // 更新当前BGM ID
	audio_.playBgm(resource_.loadAudio(BgmFilePaths.at(id), audio_.getMixer()), true); // at方法会抛出std::out_of_range异常
}

void AudioService::playSfx(SfxId id) noexcept{
	if(id == SfxId::UIButtonClick) {
		audio_.playUISfx(resource_.loadAudio(SfxFilePaths.at(id), audio_.getMixer()));
	} else {
		audio_.playGameSfx(resource_.loadAudio(SfxFilePaths.at(id), audio_.getMixer()), getPriority(id)); // 
	}
}

void AudioService::destroy() noexcept{
	if (isDestory) {
		return;
	}
	eventManager_.unsubscribe(playBgmSubscriptionId_);
	eventManager_.unsubscribe(playSfxSubscriptionId_);
	eventManager_.unsubscribe(stopBgmSubscriptionId_);
	eventManager_.unsubscribe(resumeBgmSubscriptionId_);
	eventManager_.unsubscribe(pauseBgmSubscriptionId_);
	isDestory = true;
}