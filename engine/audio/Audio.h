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

	bool init(size_t sfxSize) noexcept; // 初始化音频系统，加载必要的资源
	
	void playBgm(const std::shared_ptr<MIX_Audio>& bgm, bool loop = true) noexcept; // 播放背景音乐，支持循环播放
	void stopBgm() noexcept; // 停止背景音乐
	void pauseBgm() noexcept; // 暂停背景音乐
	void resumeBgm() noexcept; // 恢复背景音乐
	void playUISfx(const std::shared_ptr<MIX_Audio>& sfx) noexcept; // 播放UI音效，通常不循环

	void playGameSfx(const std::shared_ptr<MIX_Audio>& sfx, uint8_t priovity) noexcept; // 播放游戏音效和优先级，通常不循环

	// pause resume

	void destroy() noexcept; // 销毁音频系统，释放资源

	MIX_Mixer* getMixer() const noexcept { return mixer_.get(); } // 获取混音器实例
private:
	bool isDestroyed_ = false; // 标记音频系统是否已销毁，防止重复销毁或访问已销毁的资源
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

	struct TrackSlot { // 音轨槽位结构体，管理单个音轨的状态和资源
		std::unique_ptr<MIX_Track, MIX_TrackDeleter> track; // 音轨资源，使用智能指针管理生命周期
		uint8_t priority = 0; // 音轨优先级，控制音效的播放顺序和覆盖关系
		bool loop = false; // 标记音轨是否需要循环播放，控制播放行为
		//bool reserving = false; // 标记音轨是否正在预留，避免在预留过程中被其他音效占用
		std::shared_ptr<MIX_Audio> audio; // 当前音轨关联的音频资源，方便在播放时访问和管理音频数据
	}; // 后续设计内存对齐

	std::unique_ptr<MIX_Mixer, MIX_MixerDeleter> mixer_; // SDL_mixer实例，负责管理音频系统

	std::unique_ptr<MIX_Track, MIX_TrackDeleter> bgmTrack_; // 当前播放的背景音乐轨道 
	std::unique_ptr<MIX_Track, MIX_TrackDeleter> uiTrack_; // 当前播放的UI音效轨道

	std::list<TrackSlot> sfxTracks_; // 预定义的音效轨道数组，提供固定数量的音效轨道
};