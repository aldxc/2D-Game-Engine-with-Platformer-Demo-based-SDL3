#pragma once
#include <unordered_map>
#include "audio/Audio.h"
#include "resource/Resource.h"
#include "core/SubscriptionId.h"
#include "AudioId.h"

class Audio;
class Resource;
class EventManager;

class AudioService {
public:
	explicit AudioService(Audio& aM, Resource& rM, EventManager& eM) noexcept;

	~AudioService() noexcept;
	// 播放背景音乐
	void playBgm(BgmId id) noexcept; 
	// 播放音效
	void playSfx(SfxId id) noexcept;
	// 销毁音频服务，取消订阅事件等清理工作
	void destroy() noexcept; 
private:
	bool isDestory = false;
	Audio& audio_;
	Resource& resource_;
	EventManager& eventManager_;
	// 当前播放的背景音乐ID，方便在暂停和恢复时使用
	BgmId currentBgmId_ = BgmId::NONE; 

	// 事件订阅ID，方便在销毁时取消订阅
	SubscriptionId playBgmSubscriptionId_; 
	SubscriptionId playSfxSubscriptionId_; 
	SubscriptionId pauseBgmSubscriptionId_; 
	SubscriptionId resumeBgmSubscriptionId_; 
	SubscriptionId stopBgmSubscriptionId_; 
};