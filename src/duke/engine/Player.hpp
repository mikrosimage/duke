#pragma once

#include <duke/engine/cache/LoadedTextureCache.hpp>
#include <duke/engine/Timeline.hpp>
#include <duke/time/FrameUtils.hpp>
#include <duke/base/NonCopyable.hpp>

namespace duke {

struct CmdLineParameters;

struct Player : public noncopyable {
  Player(const CmdLineParameters& parameters);

  enum Mode {
    CONTINUE,
    LOOP,
    STOP
  };

  // setup
  void load(const Timeline& timeline, const FrameDuration& duration);
  void setFrameDuration(const FrameDuration& duration);

  // frame based
  void stop();
  void cue(uint32_t frame);
  void cueRelative(int32_t frame);

  // time based
  void setPlaybackSpeed(int speed);
  void setPlaybackTime(const Time time);
  void offsetPlaybackTime(const Time time);
  void setPlaybackMode(const Mode mode);

  // accessors
  FrameIndex getCurrentFrame() const;
  Time getPlaybackTime() const;
  FrameDuration getFrameDuration() const;
  int getPlaybackSpeed() const;
  Mode getPlaybackMode() const;

  // subobjects
  const Timeline& getTimeline() const;
  LoadedTextureCache& getTextureCache();

 private:
  LoadedTextureCache m_TextureCache;
  Range m_TimelineRange = Range::EMPTY;
  Time m_FirstFrameTime;
  Time m_LastFrameTime;
  Time m_EndFrameTime;
  Time m_PlaybackTime;
  FrameDuration m_FrameDuration = FrameDuration::PAL;
  int m_PlaybackSpeed = 0;
  Mode m_PlaybackMode = LOOP;
};

}  // namespace duke
