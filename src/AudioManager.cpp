/**
 * 智能桌面伴侣 - 音频管理器实现
 */

#include "AudioManager.h"
#include <math.h>

// I2S 端口号
#define I2S_PORT I2S_NUM_0

AudioManager::AudioManager() 
    : volume(DEFAULT_VOLUME)
    , muted(false)
    , playing(false)
    , initialized(false) {
}

bool AudioManager::begin() {
    // I2S 配置
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    // I2S 引脚配置
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRC_PIN,
        .data_out_num = I2S_DOUT_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    // 安装 I2S 驱动
    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S 驱动安装失败: %d\n", err);
        return false;
    }
    
    // 设置 I2S 引脚
    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("I2S 引脚配置失败: %d\n", err);
        return false;
    }
    
    // 清空 DMA 缓冲区
    i2s_zero_dma_buffer(I2S_PORT);
    
    initialized = true;
    Serial.println("音频管理器初始化成功");
    return true;
}

void AudioManager::playTone(uint16_t frequency, uint16_t duration) {
    if (!initialized || muted || frequency == 0) return;
    
    playing = true;
    
    // 计算需要的样本数
    uint32_t totalSamples = (I2S_SAMPLE_RATE * duration) / 1000;
    const size_t bufferSize = 256;
    int16_t buffer[bufferSize * 2];  // 立体声，左右声道
    
    uint32_t samplesWritten = 0;
    
    while (samplesWritten < totalSamples) {
        size_t samplesToWrite = min((size_t)(totalSamples - samplesWritten), bufferSize);
        
        // 生成正弦波
        for (size_t i = 0; i < samplesToWrite; i++) {
            float t = (float)(samplesWritten + i) / I2S_SAMPLE_RATE;
            float sample = sin(2.0f * M_PI * frequency * t);
            
            // 应用音量
            int16_t value = (int16_t)(sample * 32767.0f * volume / 100.0f);
            
            // 立体声：左右声道相同
            buffer[i * 2] = value;      // 左声道
            buffer[i * 2 + 1] = value;  // 右声道
        }
        
        // 写入 I2S
        size_t bytesWritten;
        i2s_write(I2S_PORT, buffer, samplesToWrite * 4, &bytesWritten, portMAX_DELAY);
        
        samplesWritten += samplesToWrite;
    }
    
    // 短暂静音，避免爆音
    memset(buffer, 0, sizeof(buffer));
    size_t bytesWritten;
    i2s_write(I2S_PORT, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
    
    playing = false;
}

void AudioManager::playMelody(const uint16_t* notes, const uint16_t* durations, uint8_t count) {
    if (!initialized || muted) return;
    
    for (uint8_t i = 0; i < count; i++) {
        if (notes[i] == Note::REST) {
            delay(durations[i]);
        } else {
            playTone(notes[i], durations[i]);
        }
        // 音符间短暂间隔
        delay(20);
    }
}

void AudioManager::playSound(SoundEffect effect) {
    if (!initialized || muted) return;
    
    switch (effect) {
        case SOUND_BOOT: {
            // 开机音效：上升音阶
            const uint16_t notes[] = {Note::C5, Note::E5, Note::G5, Note::C6};
            const uint16_t durations[] = {100, 100, 100, 200};
            playMelody(notes, durations, 4);
            break;
        }
        
        case SOUND_CLICK: {
            // 点击音效：短促高音
            playTone(Note::C6, 30);
            break;
        }
        
        case SOUND_SUCCESS: {
            // 成功音效：上升两音
            const uint16_t notes[] = {Note::G5, Note::C6};
            const uint16_t durations[] = {100, 150};
            playMelody(notes, durations, 2);
            break;
        }
        
        case SOUND_ERROR: {
            // 错误音效：下降两音
            const uint16_t notes[] = {Note::E5, Note::C5};
            const uint16_t durations[] = {150, 200};
            playMelody(notes, durations, 2);
            break;
        }
        
        case SOUND_NOTIFY: {
            // 通知音效：两声短促
            const uint16_t notes[] = {Note::A5, Note::REST, Note::A5};
            const uint16_t durations[] = {80, 50, 80};
            playMelody(notes, durations, 3);
            break;
        }
        
        case SOUND_SLEEP: {
            // 睡眠音效：下降音阶
            const uint16_t notes[] = {Note::G5, Note::E5, Note::C5};
            const uint16_t durations[] = {150, 150, 200};
            playMelody(notes, durations, 3);
            break;
        }
        
        case SOUND_WAKEUP: {
            // 唤醒音效：上升音阶
            const uint16_t notes[] = {Note::C5, Note::E5, Note::G5};
            const uint16_t durations[] = {100, 100, 150};
            playMelody(notes, durations, 3);
            break;
        }
    }
}

void AudioManager::setVolume(uint8_t vol) {
    volume = min(vol, (uint8_t)100);
}

void AudioManager::setMute(bool mute) {
    muted = mute;
    if (muted) {
        stop();
    }
}

void AudioManager::stop() {
    if (initialized) {
        i2s_zero_dma_buffer(I2S_PORT);
    }
    playing = false;
}
