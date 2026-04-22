#include "Audio.h"


Audio::~Audio() noexcept{
	destroy(); // 确保在销毁Audio实例时正确关闭音频系统，释放资源
}

bool Audio::init(size_t sfxSize) noexcept {
	// 初始化音频系统，加载必要的资源
	if (!MIX_Init()) {
		SDL_Log("Failed to init SDL_mixer: %s", SDL_GetError());
		return false;
	}
	MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if(mixer == nullptr) {
		SDL_Log("Failed to create mixer device: %s", SDL_GetError());
		return false;
	}
	mixer_.reset(mixer);

	MIX_Track* bgmTrack = MIX_CreateTrack(mixer_.get());
	if (bgmTrack == nullptr) {
		SDL_Log("Failed to create bgm track: %s", SDL_GetError());
		return false;
	}
	bgmTrack_.reset(bgmTrack);

	MIX_Track* uiTrack = MIX_CreateTrack(mixer_.get());
	if (uiTrack == nullptr) {
		SDL_Log("Failed to create ui track: %s", SDL_GetError());
		return false;
	}
	uiTrack_.reset(uiTrack);

	for(size_t i = 0; i < sfxSize; ++i) {
		MIX_Track* sfxTrack = MIX_CreateTrack(mixer_.get());
		if (sfxTrack == nullptr) {
			SDL_Log("Failed to create sfx track: %s", SDL_GetError());
			return false;
		}
		sfxTracks_.emplace_back(TrackSlot{ std::unique_ptr<MIX_Track, MIX_TrackDeleter>(sfxTrack), 0, false, nullptr });
	}

	return true;
}

void Audio::playBgm(const std::shared_ptr<MIX_Audio>& bgm, bool loop) noexcept{
	// 播放背景音乐，支持循环播放
	MIX_SetTrackAudio(bgmTrack_.get(), bgm.get());
	SDL_PropertiesID options = SDL_CreateProperties();
	if(options == 0) {
		SDL_Log("Failed to create properties: %s", SDL_GetError());
		return;
	}
	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0);
	MIX_SetTrackGain(bgmTrack_.get(), 0.5f); // 设置背景音乐音量，范围为0.0f（静音）到1.0f（最大音量）
	MIX_PlayTrack(bgmTrack_.get(), options);

	SDL_DestroyProperties(options);
}

void Audio::stopBgm() noexcept{
	MIX_StopTrack(bgmTrack_.get(), 2);
}

void Audio::pauseBgm() noexcept{
	MIX_PauseTrack(bgmTrack_.get());
}

void Audio::resumeBgm() noexcept{
	MIX_ResumeTrack(bgmTrack_.get());
}

void Audio::playUISfx(const std::shared_ptr<MIX_Audio>& sfx) noexcept{
	MIX_SetTrackAudio(uiTrack_.get(), sfx.get());

	MIX_PlayTrack(uiTrack_.get(), 0);
}

void Audio::playGameSfx(const std::shared_ptr<MIX_Audio>& sfx, uint8_t priovity) noexcept{
	// 先检查是否有空闲的音效轨道，如果有则直接使用；如果没有，则根据优先级和上次使用时间选择一个轨道进行替换(类LRU)
	// 找空闲轨道
	for (auto it = sfxTracks_.begin(); it != sfxTracks_.end(); ++it) {
		if(!MIX_TrackPlaying(it->track.get())) { // 未播放空闲轨道
			MIX_SetTrackAudio(it->track.get(), sfx.get());
			MIX_SetTrackGain(bgmTrack_.get(), 0.5f);
			MIX_PlayTrack(it->track.get(), 0);
			it->priority = priovity;
			it->audio = sfx;
			sfxTracks_.splice(sfxTracks_.begin(), sfxTracks_, it); // 将使用的轨道移动到列表前面，表示最近使用
			return;
		}
	}

	// 没有空闲轨道，类LRU替换，从后往前找优先级 <= 当前优先级且最久未使用的轨道进行替换
	for(auto it = sfxTracks_.rbegin(); it != sfxTracks_.rend(); ++it) {
		if(it->priority <= priovity) {
			MIX_SetTrackAudio(it->track.get(), sfx.get());
			MIX_SetTrackGain(bgmTrack_.get(), 0.5f);
			MIX_PlayTrack(it->track.get(), 0);
			it->priority = priovity;
			it->audio = sfx;

			auto fwdIt = std::next(it).base(); // 将reverse_iterator转换为正向iterator
			sfxTracks_.splice(sfxTracks_.begin(), sfxTracks_, fwdIt); // 将使用的轨道移动到列表前面，表示最近使用
			return;
		}
	}
	// 所有轨道优先级都高于当前音效，无法播放当前音效
}

void Audio::destroy() noexcept{
	if (isDestroyed_) {
		return; // 已经销毁，避免重复销毁
	}
	mixer_.reset(); // 销毁混音器实例，释放资源
	MIX_Quit(); // 关闭SDL_mixer
	isDestroyed_ = true; // 标记为已销毁
}
