/**
 * 智能桌面伴侣 - 麦克风管理器
 * 
 * 使用 I2S 接口读取 INMP441 数字麦克风
 * 支持录音、VAD 静音检测
 */

#ifndef MIC_MANAGER_H
#define MIC_MANAGER_H

#include <Arduino.h>
#include "driver/i2s.h"
#include "config.h"

// 录音状态
enum RecordState {
    RECORD_IDLE,        // 空闲
    RECORD_ACTIVE,      // 录音中
    RECORD_DONE,        // 录音完成
    RECORD_ERROR        // 错误
};

class MicManager {
public:
    MicManager();
    ~MicManager();
    
    /**
     * 初始化麦克风
     * @return 初始化是否成功
     */
    bool begin();
    
    /**
     * 开始录音
     * @param maxDurationMs 最大录音时长 (毫秒)
     */
    void startRecording(uint32_t maxDurationMs = MIC_RECORD_SECONDS * 1000);
    
    /**
     * 停止录音
     */
    void stopRecording();
    
    /**
     * 更新录音状态（在 loop 中调用）
     */
    void update();
    
    /**
     * 获取录音数据
     * @return 录音数据指针
     */
    const uint8_t* getAudioData() const { return audioBuffer; }
    
    /**
     * 获取录音数据长度
     */
    size_t getAudioLength() const { return audioLength; }
    
    /**
     * 获取录音状态
     */
    RecordState getState() const { return state; }
    
    /**
     * 检查是否正在录音
     */
    bool isRecording() const { return state == RECORD_ACTIVE; }
    
    /**
     * 获取当前音量级别 (0-100)
     */
    uint8_t getVolumeLevel() const { return volumeLevel; }
    
    /**
     * 清空录音缓冲区
     */
    void clearBuffer();

private:
    uint8_t* audioBuffer;       // 录音缓冲区
    size_t audioLength;         // 当前录音长度
    size_t bufferSize;          // 缓冲区大小
    RecordState state;          // 录音状态
    uint32_t recordStartTime;   // 录音开始时间
    uint32_t maxRecordTime;     // 最大录音时长
    uint8_t volumeLevel;        // 当前音量级别
    bool initialized;           // 是否已初始化
    
    /**
     * 计算音量级别
     */
    uint8_t calculateVolume(int16_t* samples, size_t count);
};

#endif // MIC_MANAGER_H
