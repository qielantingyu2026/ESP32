/**
 * 智能桌面伴侣 - 麦克风管理器实现
 */

#include "MicManager.h"

// I2S 端口号（ESP32-C3 只有一个 I2S 端口，与功放分时复用）
#define I2S_MIC_PORT I2S_NUM_0

// 录音缓冲区大小（16kHz * 5秒 * 2字节 = 160KB）
#define MAX_AUDIO_BUFFER_SIZE (MIC_SAMPLE_RATE * 5 * sizeof(int16_t))

MicManager::MicManager() 
    : audioBuffer(nullptr)
    , audioLength(0)
    , bufferSize(0)
    , state(RECORD_IDLE)
    , recordStartTime(0)
    , maxRecordTime(0)
    , volumeLevel(0)
    , initialized(false) {
}

MicManager::~MicManager() {
    if (audioBuffer) {
        free(audioBuffer);
        audioBuffer = nullptr;
    }
    if (initialized) {
        i2s_driver_uninstall(I2S_MIC_PORT);
    }
}

bool MicManager::begin() {
    Serial.println("麦克风初始化...");
    
    // 分配录音缓冲区
    bufferSize = MAX_AUDIO_BUFFER_SIZE;
    audioBuffer = (uint8_t*)malloc(bufferSize);
    if (!audioBuffer) {
        Serial.println("录音缓冲区分配失败");
        return false;
    }
    
    // I2S 配置（接收模式）
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = MIC_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // INMP441 单声道
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    // I2S 引脚配置
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_MIC_SCK_PIN,
        .ws_io_num = I2S_MIC_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_MIC_SD_PIN
    };
    
    // 安装 I2S 驱动
    esp_err_t err = i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S 麦克风驱动安装失败: %d\n", err);
        return false;
    }
    
    // 设置 I2S 引脚
    err = i2s_set_pin(I2S_MIC_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("I2S 麦克风引脚配置失败: %d\n", err);
        return false;
    }
    
    initialized = true;
    Serial.println("麦克风初始化成功");
    return true;
}

void MicManager::startRecording(uint32_t maxDurationMs) {
    if (!initialized) {
        Serial.println("麦克风未初始化");
        state = RECORD_ERROR;
        return;
    }
    
    // 清空缓冲区
    clearBuffer();
    
    maxRecordTime = maxDurationMs;
    recordStartTime = millis();
    state = RECORD_ACTIVE;
    
    Serial.printf("开始录音，最大时长: %d ms\n", maxDurationMs);
}

void MicManager::stopRecording() {
    if (state == RECORD_ACTIVE) {
        state = RECORD_DONE;
        Serial.printf("录音结束，数据长度: %d 字节\n", audioLength);
    }
}

void MicManager::update() {
    if (state != RECORD_ACTIVE) return;
    
    // 检查是否超时
    if (millis() - recordStartTime >= maxRecordTime) {
        stopRecording();
        return;
    }
    
    // 检查缓冲区是否已满
    if (audioLength >= bufferSize) {
        stopRecording();
        return;
    }
    
    // 读取麦克风数据
    const size_t readSize = 512;
    int16_t tempBuffer[readSize / 2];
    size_t bytesRead = 0;
    
    esp_err_t err = i2s_read(I2S_MIC_PORT, tempBuffer, readSize, &bytesRead, 10);
    
    if (err == ESP_OK && bytesRead > 0) {
        // 计算音量级别
        volumeLevel = calculateVolume(tempBuffer, bytesRead / 2);
        
        // 复制到录音缓冲区
        size_t toCopy = min(bytesRead, bufferSize - audioLength);
        memcpy(audioBuffer + audioLength, tempBuffer, toCopy);
        audioLength += toCopy;
    }
}

void MicManager::clearBuffer() {
    audioLength = 0;
    volumeLevel = 0;
    if (audioBuffer) {
        memset(audioBuffer, 0, bufferSize);
    }
}

uint8_t MicManager::calculateVolume(int16_t* samples, size_t count) {
    if (count == 0) return 0;
    
    // 计算 RMS 音量
    int64_t sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += (int64_t)samples[i] * samples[i];
    }
    
    double rms = sqrt((double)sum / count);
    
    // 映射到 0-100
    // INMP441 输出范围约 -32768 到 32767
    uint8_t level = (uint8_t)min(100.0, rms / 327.67);
    
    return level;
}
