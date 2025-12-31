/**
 * 智能桌面伴侣 - 配置管理器
 * 
 * 使用ESP32 NVS (Non-Volatile Storage) 存储配置数据
 * 支持配置的加载、保存和工厂重置功能
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include "config.h"

#ifndef UNIT_TEST
#include <Preferences.h>
#endif

class ConfigManager {
public:
    ConfigManager();
    
    /**
     * 初始化配置管理器
     * @return true 初始化成功，false 初始化失败
     */
    bool init();
    
    /**
     * 从NVS加载配置
     * @return true 加载成功，false 加载失败（将使用默认配置）
     */
    bool load();
    
    /**
     * 保存配置到NVS
     * @return true 保存成功，false 保存失败
     */
    bool save();
    
    /**
     * 执行工厂重置
     * 清除所有存储的配置，恢复默认值，并重启设备
     */
    void factoryReset();
    
    /**
     * 更新配置（在主循环中调用）
     * 检查是否有未保存的更改，如果超过5秒则自动保存
     */
    void update();
    
    /**
     * 标记配置已更改（需要保存）
     */
    void markDirty();
    
    /**
     * 检查是否有未保存的更改
     * @return true 有未保存的更改
     */
    bool isDirty() const;
    
    // ========================================================================
    // 配置项访问器
    // ========================================================================
    
    /**
     * 获取亮度设置
     * @return 亮度值 (0-255)
     */
    uint8_t getBrightness() const;
    
    /**
     * 设置亮度
     * @param value 亮度值 (0-255)
     */
    void setBrightness(uint8_t value);
    
    /**
     * 获取调暗超时时间
     * @return 超时时间（秒）
     */
    uint16_t getDimTimeout() const;
    
    /**
     * 设置调暗超时时间
     * @param seconds 超时时间（秒）
     */
    void setDimTimeout(uint16_t seconds);
    
    /**
     * 获取睡眠超时时间
     * @return 超时时间（秒）
     */
    uint16_t getSleepTimeout() const;
    
    /**
     * 设置睡眠超时时间
     * @param seconds 超时时间（秒）
     */
    void setSleepTimeout(uint16_t seconds);
    
    /**
     * 获取上次显示模式
     * @return 显示模式
     */
    DisplayMode getLastDisplayMode() const;
    
    /**
     * 设置上次显示模式
     * @param mode 显示模式
     */
    void setLastDisplayMode(DisplayMode mode);
    
    /**
     * 获取当前配置的只读引用
     * @return 配置结构体引用
     */
    const DeviceConfig& getConfig() const;

private:
#ifndef UNIT_TEST
    Preferences preferences;        // NVS存储对象
#endif
    DeviceConfig config;            // 当前配置
    bool dirty;                     // 是否有未保存的更改
    unsigned long lastChangeTime;   // 上次更改时间
    bool initialized;               // 是否已初始化
    
    // NVS命名空间
    static const char* NVS_NAMESPACE;
    
    // 自动保存延迟（毫秒）
    static const unsigned long AUTO_SAVE_DELAY_MS = 5000;
    
    // NVS键名
    static const char* KEY_VERSION;
    static const char* KEY_BRIGHTNESS;
    static const char* KEY_DIM_TIMEOUT;
    static const char* KEY_SLEEP_TIMEOUT;
    static const char* KEY_LAST_MODE;
};

#endif // CONFIG_MANAGER_H
