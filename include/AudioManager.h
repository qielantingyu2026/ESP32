/**
 * 智能桌面伴侣 - 音频管理器
 * 
 * 使用 I2S 接口驱动 MAX98357A 功放播放音频
 * 支持播放提示音、简单旋律
 */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include "driver/i2s.h"
#include "config.h"

// 音效类型枚举
enum SoundEffect {
    SOUND_BOOT,         // 开机音效
    SOUND_CLICK,        // 点击音效
    SOUND_SUCCESS,      // 成功音效
    SOUND_ERROR,        // 错误音效
    SOUND_NOTIFY,       // 通知音效
    SOUND_SLEEP,        // 睡眠音效
    SOUND_WAKEUP        // 唤醒音效
};

class AudioManager {
public:
    AudioManager();
    
    /**
     * 初始化 I2S 音频
     * @return 初始化是否成功
     */
    bool begin();
    
    /**
     * 播放预设音效
     * @param effect 音效类型
     */
    void playSound(SoundEffect effect);
    
    /**
     * 播放指定频率的音调
     * @param frequency 频率 (Hz)
     * @param duration 持续时间 (ms)
     */
    void playTone(uint16_t frequency, uint16_t duration);
    
    /**
     * 播放旋律（音符序列）
     * @param notes 音符频率数组
     * @param durations 音符时长数组 (ms)
     * @param count 音符数量
     */
    void playMelody(const uint16_t* notes, const uint16_t* durations, uint8_t count);
    
    /**
     * 设置音量
     * @param vol 音量 (0-100)
     */
    void setVolume(uint8_t vol);
    
    /**
     * 获取当前音量
     */
    uint8_t getVolume() const { return volume; }
    
    /**
     * 静音/取消静音
     */
    void setMute(bool mute);
    bool isMuted() const { return muted; }
    
    /**
     * 停止播放
     */
    void stop();
    
    /**
     * 检查是否正在播放
     */
    bool isPlaying() const { return playing; }

private:
    uint8_t volume;         // 音量 (0-100)
    bool muted;             // 是否静音
    bool playing;           // 是否正在播放
    bool initialized;       // 是否已初始化
    
    /**
     * 生成正弦波样本
     */
    void generateSineWave(int16_t* buffer, size_t samples, uint16_t frequency);
    
    /**
     * 写入 I2S 数据
     */
    void writeI2S(int16_t* buffer, size_t samples);
};

// 常用音符频率定义 (Hz)
namespace Note {
    const uint16_t C4  = 262;
    const uint16_t CS4 = 277;
    const uint16_t D4  = 294;
    const uint16_t DS4 = 311;
    const uint16_t E4  = 330;
    const uint16_t F4  = 349;
    const uint16_t FS4 = 370;
    const uint16_t G4  = 392;
    const uint16_t GS4 = 415;
    const uint16_t A4  = 440;
    const uint16_t AS4 = 466;
    const uint16_t B4  = 494;
    const uint16_t C5  = 523;
    const uint16_t D5  = 587;
    const uint16_t E5  = 659;
    const uint16_t F5  = 698;
    const uint16_t G5  = 784;
    const uint16_t A5  = 880;
    const uint16_t B5  = 988;
    const uint16_t C6  = 1047;
    const uint16_t REST = 0;    // 休止符
}

#endif // AUDIO_MANAGER_H
