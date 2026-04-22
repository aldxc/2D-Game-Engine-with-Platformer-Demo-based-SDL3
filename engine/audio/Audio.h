#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <list>
#include <SDL3_mixer/SDL_mixer.h>

class Audio {
public:
	explicit Audio() noexcept = default;
	~Audio() noexcept;

	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;
	// 初始化音频系统，sfxSize指定预定义的音效轨道数量
	bool init(size_t sfxSize) noexcept; 
	// 播放背景音乐，支持循环播放
	void playBgm(const std::shared_ptr<MIX_Audio>& bgm, bool loop = true) noexcept; 
	// 停止背景音乐
	void stopBgm() noexcept; 
	// 暂停背景音乐
	void pauseBgm() noexcept; 
	// 恢复背景音乐
	void resumeBgm() noexcept; 
	// 播放UI音效，通常不循环
	void playUISfx(const std::shared_ptr<MIX_Audio>& sfx) noexcept; 
	// 播放游戏音效和优先级，通常不循环
	void playGameSfx(const std::shared_ptr<MIX_Audio>& sfx, uint8_t priovity) noexcept; 
	// 销毁音频系统，释放资源
	void destroy() noexcept; 
	// 获取混音器实例
	MIX_Mixer* getMixer() const noexcept { return mixer_.get(); } 
private:
	// 标记音频系统是否已销毁，防止重复销毁或访问已销毁的资源
	bool isDestroyed_ = false; 
	// 自定义删除器
	struct MIX_MixerDeleter {
		void operator()(MIX_Mixer* mixer) const {
			MIX_DestroyMixer(mixer);
		}
	};
	struct MIX_TrackDeleter {
		void operator()(MIX_Track* track) const {
			MIX_DestroyTrack(track);
		}
	};
	// 音轨槽位结构体，管理单个音轨的状态和资源
	struct TrackSlot { 
		// 音轨资源，使用智能指针管理生命周期
		std::unique_ptr<MIX_Track, MIX_TrackDeleter> track; 
		// 音轨优先级
		uint8_t priority = 0; 
		// 标记音轨是否需要循环播放
		bool loop = false;
		// 缓存当前音轨使用的音频资源
		std::shared_ptr<MIX_Audio> audio; 
	};  // 后续设计内存对齐
	// SDL_mixer实例，负责管理音频系统
	std::unique_ptr<MIX_Mixer, MIX_MixerDeleter> mixer_; 
	// 当前播放的背景音乐轨道 
	std::unique_ptr<MIX_Track, MIX_TrackDeleter> bgmTrack_; 
	// 当前播放的UI音效轨道
	std::unique_ptr<MIX_Track, MIX_TrackDeleter> uiTrack_; 
	// 预定义的音效轨道数组，提供固定数量的音效轨道
	std::list<TrackSlot> sfxTracks_; 
};