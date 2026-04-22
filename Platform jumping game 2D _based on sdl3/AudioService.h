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

	void playBgm(BgmId id) noexcept; // 播放背景音乐
	void playSfx(SfxId id) noexcept; // 播放音效

	void destroy() noexcept; // 销毁音频服务，取消订阅事件等清理工作
private:
	bool isDestory = false;
	Audio& audio_;
	Resource& resource_;
	EventManager& eventManager_;

	BgmId currentBgmId_ = BgmId::None; // 当前播放的背景音乐ID，方便在暂停和恢复时使用

	SubscriptionId playBgmSubscriptionId_; // 播放背景音乐事件订阅ID
	SubscriptionId playSfxSubscriptionId_; // 播放音效事件订阅ID
	SubscriptionId pauseBgmSubscriptionId_; // 暂停背景音乐事件订阅ID
	SubscriptionId resumeBgmSubscriptionId_; // 恢复背景音乐事件订阅ID
	SubscriptionId stopBgmSubscriptionId_; // 停止背景音乐事件订阅ID
};