/**
 * 智能桌面伴侣 - 配置管理器实现
 * 
 * 使用ESP32 Preferences库实现NVS存储
 */

#include <Arduino.h>
#include "ConfigManager.h"

// NVS命名空间和键名定义
const char* ConfigManager::NVS_NAMESPACE = "companion";
const char* ConfigManager::KEY_VERSION = "version";
const char* ConfigManager::KEY_BRIGHTNESS = "brightness";
const char* ConfigManager::KEY_DIM_TIMEOUT = "dimTimeout";
const char* ConfigManager::KEY_SLEEP_TIMEOUT = "sleepTimeout";
const char* ConfigManager::KEY_LAST_MODE = "lastMode";

ConfigManager::ConfigManager() 
    : config(DEFAULT_CONFIG)
    , dirty(false)
    , lastChangeTime(0)
    , initialized(false) {
}

bool ConfigManager::init() {
#ifndef UNIT_TEST
    // 打开NVS命名空间（读写模式）
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        Serial.println("[ConfigManager] 错误: 无法打开NVS命名空间");
        // 使用默认配置继续运行
        config = DEFAULT_CONFIG;
        initialized = true;
        return false;
    }
    
    Serial.println("[ConfigManager] NVS初始化成功");
#endif
    
    initialized = true;
    
    // 尝试加载配置
    if (!load()) {
        Serial.println("[ConfigManager] 使用默认配置");
        config = DEFAULT_CONFIG;
        // 保存默认配置到NVS
        save();
    }
    
    return true;
}

bool ConfigManager::load() {
    if (!initialized) {
        return false;
    }
    
#ifndef UNIT_TEST
    // 检查是否存在配置版本号
    uint8_t storedVersion = preferences.getUChar(KEY_VERSION, 0);
    
    if (storedVersion == 0) {
        // 没有存储的配置
        Serial.println("[ConfigManager] 未找到存储的配置");
        return false;
    }
    
    // 检查版本兼容性
    if (storedVersion != DEFAULT_CONFIG.version) {
        Serial.printf("[ConfigManager] 配置版本不匹配: 存储=%d, 当前=%d\n", 
                      storedVersion, DEFAULT_CONFIG.version);
        // 版本不匹配，使用默认配置
        return false;
    }
    
    // 加载各项配置
    config.version = storedVersion;
    config.brightness = preferences.getUChar(KEY_BRIGHTNESS, DEFAULT_CONFIG.brightness);
    config.dimTimeoutSec = preferences.getUShort(KEY_DIM_TIMEOUT, DEFAULT_CONFIG.dimTimeoutSec);
    config.sleepTimeoutSec = preferences.getUShort(KEY_SLEEP_TIMEOUT, DEFAULT_CONFIG.sleepTimeoutSec);
    config.lastDisplayMode = preferences.getUChar(KEY_LAST_MODE, DEFAULT_CONFIG.lastDisplayMode);
    
    // 验证配置值的有效性
    if (config.lastDisplayMode >= MODE_COUNT) {
        config.lastDisplayMode = MODE_FACE;
    }
    
    Serial.println("[ConfigManager] 配置加载成功");
    Serial.printf("  亮度: %d\n", config.brightness);
    Serial.printf("  调暗超时: %d秒\n", config.dimTimeoutSec);
    Serial.printf("  睡眠超时: %d秒\n", config.sleepTimeoutSec);
    Serial.printf("  上次模式: %d\n", config.lastDisplayMode);
#endif
    
    dirty = false;
    return true;
}

bool ConfigManager::save() {
    if (!initialized) {
        return false;
    }
    
#ifndef UNIT_TEST
    // 保存各项配置到NVS
    preferences.putUChar(KEY_VERSION, config.version);
    preferences.putUChar(KEY_BRIGHTNESS, config.brightness);
    preferences.putUShort(KEY_DIM_TIMEOUT, config.dimTimeoutSec);
    preferences.putUShort(KEY_SLEEP_TIMEOUT, config.sleepTimeoutSec);
    preferences.putUChar(KEY_LAST_MODE, config.lastDisplayMode);
    
    Serial.println("[ConfigManager] 配置已保存到NVS");
#endif
    
    dirty = false;
    return true;
}

void ConfigManager::factoryReset() {
#ifndef UNIT_TEST
    Serial.println("[ConfigManager] 执行工厂重置...");
    
    // 清除NVS中的所有数据
    preferences.clear();
    
    // 恢复默认配置
    config = DEFAULT_CONFIG;
    
    // 保存默认配置
    save();
    
    Serial.println("[ConfigManager] 工厂重置完成，设备将重启...");
    delay(1000);
    
    // 重启设备
    ESP.restart();
#else
    // 单元测试模式下只恢复默认配置
    config = DEFAULT_CONFIG;
    dirty = false;
#endif
}

void ConfigManager::update() {
    if (!initialized || !dirty) {
        return;
    }
    
    // 检查是否超过自动保存延迟时间
    unsigned long currentTime = millis();
    if (currentTime - lastChangeTime >= AUTO_SAVE_DELAY_MS) {
        Serial.println("[ConfigManager] 自动保存配置...");
        save();
    }
}

void ConfigManager::markDirty() {
    dirty = true;
    lastChangeTime = millis();
}

bool ConfigManager::isDirty() const {
    return dirty;
}

// ============================================================================
// 配置项访问器实现
// ============================================================================

uint8_t ConfigManager::getBrightness() const {
    return config.brightness;
}

void ConfigManager::setBrightness(uint8_t value) {
    if (config.brightness != value) {
        config.brightness = value;
        markDirty();
    }
}

uint16_t ConfigManager::getDimTimeout() const {
    return config.dimTimeoutSec;
}

void ConfigManager::setDimTimeout(uint16_t seconds) {
    if (config.dimTimeoutSec != seconds) {
        config.dimTimeoutSec = seconds;
        markDirty();
    }
}

uint16_t ConfigManager::getSleepTimeout() const {
    return config.sleepTimeoutSec;
}

void ConfigManager::setSleepTimeout(uint16_t seconds) {
    if (config.sleepTimeoutSec != seconds) {
        config.sleepTimeoutSec = seconds;
        markDirty();
    }
}

DisplayMode ConfigManager::getLastDisplayMode() const {
    return static_cast<DisplayMode>(config.lastDisplayMode);
}

void ConfigManager::setLastDisplayMode(DisplayMode mode) {
    uint8_t modeValue = static_cast<uint8_t>(mode);
    if (config.lastDisplayMode != modeValue) {
        config.lastDisplayMode = modeValue;
        markDirty();
    }
}

const DeviceConfig& ConfigManager::getConfig() const {
    return config;
}
