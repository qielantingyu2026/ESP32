/**
 * 智能桌面伴侣 - 主程序入口
 * 
 * 基于ESP32-C3 Super Mini的智能桌面伴侣
 * 使用0.96寸OLED显示屏和TTP223触摸传感器
 */

#include <Arduino.h>
#include "config.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "WiFiMgr.h"
#include "TimeManager.h"
#include "ConfigManager.h"
#include "SystemMonitor.h"

// 全局对象实例
DisplayManager displayManager;
TouchManager touchManager;
WiFiMgr wifiMgr;
TimeManager timeManager;
ConfigManager configManager;
SystemMonitor systemMonitor;

// 系统状态
SystemState systemState = STATE_BOOT;

// 上次触摸时间（用于屏幕保护）
unsigned long lastTouchTime = 0;

// 屏幕保护状态
bool isDimmed = false;           // 是否已调暗
DisplayMode modeBeforeSleep;     // 睡眠前的显示模式

/**
 * 检查并处理空闲状态（屏幕保护）
 */
void checkIdleState() {
    unsigned long currentTime = millis();
    unsigned long idleTime = currentTime - lastTouchTime;
    
    // 获取超时配置（转换为毫秒）
    unsigned long dimTimeoutMs = configManager.getDimTimeout() * 1000UL;
    unsigned long sleepTimeoutMs = configManager.getSleepTimeout() * 1000UL;
    
    DisplayMode currentMode = displayManager.getMode();
    
    // 检查睡眠超时（优先级高于调暗）
    if (idleTime >= sleepTimeoutMs && currentMode != MODE_SLEEP) {
        Serial.println("空闲超时，进入睡眠模式");
        // 保存睡眠前的模式
        modeBeforeSleep = currentMode;
        // 进入睡眠模式
        displayManager.setMode(MODE_SLEEP);
        isDimmed = true;
    }
    // 检查调暗超时
    else if (idleTime >= dimTimeoutMs && !isDimmed && currentMode != MODE_SLEEP) {
        Serial.println("空闲超时，调暗屏幕");
        // 降低亮度到原来的30%
        uint8_t dimBrightness = configManager.getBrightness() * 0.3;
        displayManager.setBrightness(dimBrightness);
        isDimmed = true;
    }
}

/**
 * 重置空闲状态（有触摸输入时调用）
 */
void resetIdleState() {
    if (isDimmed) {
        // 恢复正常亮度
        displayManager.setBrightness(configManager.getBrightness());
        isDimmed = false;
        Serial.println("恢复正常亮度");
    }
}

/**
 * WiFi状态变化回调函数
 */
void onWiFiStateChange(WiFiConnectionState state) {
    switch (state) {
        case WIFI_STATE_CONNECTED:
            Serial.println("WiFi已连接，开始同步时间");
            timeManager.syncNTP();
            break;
        case WIFI_STATE_DISCONNECTED:
            Serial.println("WiFi断开连接");
            break;
        case WIFI_STATE_AP_MODE:
            Serial.println("进入AP配网模式");
            break;
        default:
            break;
    }
}

/**
 * 时间同步状态回调函数
 */
void onTimeSyncStateChange(TimeSyncState state) {
    switch (state) {
        case TIME_SYNCED:
            Serial.println("时间同步成功");
            break;
        case TIME_SYNC_FAILED:
            Serial.println("时间同步失败");
            break;
        default:
            break;
    }
}

/**
 * 触摸事件回调函数
 * 处理短按、长按和工厂重置事件
 */
void onTouchEvent(TouchEvent event) {
    // 更新最后触摸时间
    lastTouchTime = millis();
    
    // 重置空闲状态（恢复亮度）
    resetIdleState();
    
    switch (event) {
        case TOUCH_SHORT:
            Serial.println("触摸事件: 短按");
            
            // 如果在睡眠模式，唤醒设备
            if (displayManager.getMode() == MODE_SLEEP) {
                displayManager.getFaceRenderer().wakeUp();
                // 恢复到表情模式
                displayManager.setMode(MODE_FACE);
                configManager.setLastDisplayMode(MODE_FACE);
            } else {
                // 切换到下一个显示模式
                displayManager.nextMode();
                
                // 保存当前模式到配置
                configManager.setLastDisplayMode(displayManager.getMode());
                
                // 触发表情反应（如果当前是表情模式）
                if (displayManager.getMode() == MODE_FACE) {
                    displayManager.getFaceRenderer().triggerReaction();
                }
            }
            break;
            
        case TOUCH_LONG:
            Serial.println("触摸事件: 长按");
            // TODO: 进入设置模式（预留）
            // 目前仅触发表情反应
            if (displayManager.getMode() == MODE_FACE) {
                displayManager.getFaceRenderer().triggerReaction();
            }
            break;
            
        case TOUCH_FACTORY_RESET:
            Serial.println("触摸事件: 工厂重置");
            // 显示重置提示
            displayManager.showMessage("工厂重置中...");
            delay(500);
            // 执行工厂重置（清除NVS配置并重启设备）
            configManager.factoryReset();
            break;
            
        default:
            break;
    }
}

void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(1000);
    Serial.println("智能桌面伴侣启动中...");
    
    // 初始化配置管理器（优先初始化，其他模块可能依赖配置）
    if (configManager.init()) {
        Serial.println("配置管理器初始化成功");
    } else {
        Serial.println("配置管理器初始化失败，使用默认配置");
    }
    
    // 初始化显示管理器
    if (displayManager.init()) {
        Serial.println("显示管理器初始化成功");
        displayManager.showBootScreen();
        delay(1500);  // 显示启动画面1.5秒
    } else {
        Serial.println("显示管理器初始化失败!");
    }
    
    // 初始化触摸管理器
    touchManager.init(TOUCH_PIN);
    touchManager.setCallback(onTouchEvent);
    Serial.println("触摸管理器初始化成功");
    
    // 初始化WiFi管理器
    wifiMgr.init();
    wifiMgr.setStateCallback(onWiFiStateChange);
    Serial.println("WiFi管理器初始化成功");
    
    // 初始化时间管理器
    timeManager.init();
    timeManager.setSyncCallback(onTimeSyncStateChange);
    Serial.println("时间管理器初始化成功");
    
    // 初始化系统监控器
    systemMonitor.init();
    Serial.println("系统监控器初始化成功");
    
    // 尝试连接WiFi
    Serial.println("正在连接WiFi...");
    displayManager.showConnectionStatus("Connecting WiFi...");
    wifiMgr.connect();
    
    // 记录启动时间
    lastTouchTime = millis();
    
    // 设置系统状态为运行中
    systemState = STATE_RUNNING;
    
    // 从配置中恢复上次的显示模式，默认为表情模式
    DisplayMode lastMode = configManager.getLastDisplayMode();
    displayManager.setMode(lastMode);
    
    // 应用保存的亮度设置
    displayManager.setBrightness(configManager.getBrightness());
    
    Serial.println("系统启动完成");
}

void loop() {
    // 更新触摸管理器
    touchManager.update();
    
    // 更新WiFi管理器（处理断线重连）
    wifiMgr.update();
    
    // 更新时间管理器（处理定期同步）
    timeManager.update();
    
    // 更新配置管理器（处理自动保存）
    configManager.update();
    
    // 更新系统监控器
    systemMonitor.update();
    
    // 更新系统信息渲染器的数据
    SysInfoRenderer& sysInfo = displayManager.getSysInfoRenderer();
    sysInfo.setFreeHeap(systemMonitor.getFreeHeap());
    sysInfo.setUptime(systemMonitor.getUptime());
    sysInfo.setWiFiConnected(wifiMgr.isConnected());
    if (wifiMgr.isConnected()) {
        sysInfo.setRSSI(wifiMgr.getRSSI());
    }
    
    // 更新显示管理器
    displayManager.update();
    
    // 检查空闲状态（屏幕保护）
    checkIdleState();
    
    // 短暂延时，避免过度占用CPU
    delay(10);
}
