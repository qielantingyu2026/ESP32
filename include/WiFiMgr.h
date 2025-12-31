/**
 * 智能桌面伴侣 - WiFi管理器
 * 
 * 管理WiFi连接，支持自动连接和AP配网模式
 * 实现断线自动重连和连接状态回调
 */

#ifndef WIFI_MGR_H
#define WIFI_MGR_H

#include <Arduino.h>

#ifndef UNIT_TEST
#include <WiFi.h>
#include <WiFiManager.h>
#endif

#include "config.h"

// WiFi连接状态枚举
enum WiFiConnectionState {
    WIFI_STATE_DISCONNECTED = 0,    // 未连接
    WIFI_STATE_CONNECTING,          // 连接中
    WIFI_STATE_CONNECTED,           // 已连接
    WIFI_STATE_AP_MODE,             // AP配网模式
    WIFI_STATE_FAILED               // 连接失败
};

// 连接状态回调函数类型
typedef void (*WiFiStateCallback)(WiFiConnectionState state);

/**
 * WiFi管理器类
 * 
 * 功能：
 * - 自动连接已保存的WiFi
 * - AP配网模式（连接超时后启动）
 * - 断线自动重连
 * - 连接状态指示器回调
 */
class WiFiMgr {
public:
    /**
     * 构造函数
     */
    WiFiMgr();
    
    /**
     * 初始化WiFi管理器
     */
    void init();
    
    /**
     * 尝试连接WiFi
     * @return true 连接成功，false 连接失败
     */
    bool connect();
    
    /**
     * 更新WiFi状态（在主循环中调用）
     * 处理断线重连逻辑
     */
    void update();
    
    /**
     * 检查WiFi是否已连接
     * @return true 已连接，false 未连接
     */
    bool isConnected();
    
    /**
     * 启动AP配网模式
     * 创建热点供用户配置WiFi
     */
    void startAPMode();
    
    /**
     * 停止AP配网模式
     */
    void stopAPMode();
    
    /**
     * 获取WiFi信号强度
     * @return RSSI值（dBm），未连接时返回0
     */
    int8_t getRSSI();
    
    /**
     * 获取当前连接的SSID
     * @return SSID字符串，未连接时返回空字符串
     */
    String getSSID();
    
    /**
     * 获取当前连接状态
     * @return WiFiConnectionState枚举值
     */
    WiFiConnectionState getState();
    
    /**
     * 设置状态变化回调函数
     * @param callback 回调函数指针
     */
    void setStateCallback(WiFiStateCallback callback);
    
    /**
     * 获取重连尝试次数
     * @return 当前重连尝试次数
     */
    uint8_t getReconnectAttempts();
    
    /**
     * 重置重连计数器
     */
    void resetReconnectAttempts();
    
    /**
     * 断开WiFi连接
     */
    void disconnect();

private:
    WiFiConnectionState _state;             // 当前连接状态
    WiFiStateCallback _stateCallback;       // 状态回调函数
    
    uint8_t _reconnectAttempts;             // 重连尝试次数
    unsigned long _lastReconnectTime;       // 上次重连时间
    unsigned long _connectStartTime;        // 连接开始时间
    
    bool _apModeActive;                     // AP模式是否激活
    unsigned long _apModeStartTime;         // AP模式启动时间
    
#ifndef UNIT_TEST
    WiFiManager _wifiManager;               // WiFiManager实例
#endif
    
    /**
     * 更新连接状态并触发回调
     * @param newState 新状态
     */
    void setState(WiFiConnectionState newState);
    
    /**
     * 尝试重新连接
     * @return true 重连成功，false 重连失败或达到最大次数
     */
    bool tryReconnect();
};

#endif // WIFI_MGR_H
