#include "AudioService.h"
#include "audio/Audio.h"
#include "resource/Resource.h"
#include "core/EventManager.h"

namespace {
	uint8_t getPriority(SfxId id) noexcept {
		switch (id) {
		case SfxId::PLAYER_ATTACK_SWING:
			return static_cast<uint8_t>(Priority::Low); 
		case SfxId::PLAYER_ATTACK_HIT:
			return static_cast<uint8_t>(Priority::High); 
		case SfxId::PLAYER_JUMP:
		case SfxId::PLAYER_SPRINT:
			return static_cast<uint8_t>(Priority::Medium);
		case SfxId::PLAYER_HURT:
			return static_cast<uint8_t>(Priority::Max); 
		default:
			return 1; 
		}
	}
}

AudioService::AudioService(Audio& aM, Resource& rM, EventManager& eM) noexcept : audio_(aM) , resource_(rM), eventManager_(eM){
	playBgmSubscriptionId_ = eventManager_.subscribe(EventType::AUDIO_PLAY_BGM, [this](const Event& event) {
		if (event.hasData<BgmId>()) {
			playBgm(event.getData<BgmId>());
		}
		});
	playSfxSubscriptionId_ = eventManager_.subscribe(EventType::AUDIO_PLAY_SFX, [this](const Event& event) {
		if (event.hasData<SfxId>()) {
			playSfx(event.getData<SfxId>());
		}
		});
	pauseBgmSubscriptionId_ = eventManager_.subscribe(EventType::AUDIO_PAUSE_BGM, [this](const Event& event) {
		audio_.pauseBgm();
		});
	resumeBgmSubscriptionId_ = eventManager_.subscribe(EventType::AUDIO_RESUME_BGM, [this](const Event& event) {
		audio_.resumeBgm();
		});
	stopBgmSubscriptionId_ = eventManager_.subscribe(EventType::AUDIO_STOP_BGM, [this](const Event& event) {
		// 停止背景音乐时重置当前BGM ID
		currentBgmId_ = BgmId::NONE; 
		audio_.stopBgm(); 
		});

	// 初始背景音乐为NONE
	currentBgmId_ = BgmId::NONE; 
}

AudioService::~AudioService() noexcept{
	destroy();
}

void AudioService::playBgm(BgmId id) noexcept{
	if(id == currentBgmId_) {
		// 如果当前正在播放的BGM与请求的相同，则不重新加载和播放
		return; 
	}
	currentBgmId_ = id; 
	// at方法安全，因为const通过[]访问不安全
	audio_.playBgm(resource_.loadAudio(BgmFilePaths.at(id), audio_.getMixer()), true); 
}

void AudioService::playSfx(SfxId id) noexcept{
	if(id == SfxId::UI_BUTTON_CLICK) {
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