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

AudioService::AudioService(Audio& aM, Resource& rM, EventManager& eM) noexcept : m_audio(aM) , m_resource(rM), m_eventManager(eM){
	m_playBgmSubscriptionId = m_eventManager.subscribe(EventType::AUDIO_PLAY_BGM, [this](const Event& event) {
		if (event.hasData<BgmId>()) {
			playBgm(event.getData<BgmId>());
		}
		});
	m_playSfxSubscriptionId = m_eventManager.subscribe(EventType::AUDIO_PLAY_SFX, [this](const Event& event) {
		if (event.hasData<SfxId>()) {
			playSfx(event.getData<SfxId>());
		}
		});
	m_pauseBgmSubscriptionId = m_eventManager.subscribe(EventType::AUDIO_PAUSE_BGM, [this](const Event& event) {
		m_audio.pauseBgm();
		});
	m_resumeBgmSubscriptionId_ = m_eventManager.subscribe(EventType::AUDIO_RESUME_BGM, [this](const Event& event) {
		m_audio.resumeBgm();
		});
	m_stopBgmSubscriptionId = m_eventManager.subscribe(EventType::AUDIO_STOP_BGM, [this](const Event& event) {
		// 停止背景音乐时重置当前BGM ID
		m_currentBgmId = BgmId::NONE; 
		m_audio.stopBgm(); 
		});

	// 初始背景音乐为NONE
	m_currentBgmId = BgmId::NONE; 
}

AudioService::~AudioService() noexcept{
	destroy();
}

void AudioService::playBgm(BgmId id) noexcept{
	if(id == m_currentBgmId) {
		// 如果当前正在播放的BGM与请求的相同，则不重新加载和播放
		return; 
	}
	m_currentBgmId = id; 
	// at方法安全，因为const通过[]访问不安全
	m_audio.playBgm(m_resource.loadAudio(BgmFilePaths.at(id), m_audio.getMixer()), true); 
}

void AudioService::playSfx(SfxId id) noexcept{
	if(id == SfxId::UI_BUTTON_CLICK) {
		m_audio.playUISfx(m_resource.loadAudio(SfxFilePaths.at(id), m_audio.getMixer()));
	} else {
		m_audio.playGameSfx(m_resource.loadAudio(SfxFilePaths.at(id), m_audio.getMixer()), getPriority(id)); // 
	}
}

void AudioService::destroy() noexcept{
	if (m_isDestory) {
		return;
	}
	m_eventManager.unsubscribe(m_playBgmSubscriptionId);
	m_eventManager.unsubscribe(m_playSfxSubscriptionId);
	m_eventManager.unsubscribe(m_stopBgmSubscriptionId);
	m_eventManager.unsubscribe(m_resumeBgmSubscriptionId_);
	m_eventManager.unsubscribe(m_pauseBgmSubscriptionId);
	m_isDestory = true;
}