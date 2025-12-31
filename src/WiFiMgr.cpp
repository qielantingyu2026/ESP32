/**
 * 智能桌面伴侣 - WiFi管理器实现
 * 
 * 实现WiFi连接管理、AP配网模式和断线重连功能
 */

#include "WiFiMgr.h"

// AP配网热点名称
static const char* AP_NAME = "SmartCompanion";
// AP配网热点密码（空字符串表示开放网络）
static const char* AP_PASSWORD = "";

WiFiMgr::WiFiMgr() 
    : _state(WIFI_STATE_DISCONNECTED)
    , _stateCallback(nullptr)
    , _reconnectAttempts(0)
    , _lastReconnectTime(0)
    , _connectStartTime(0)
    , _apModeActive(false)
    , _apModeStartTime(0) {
}

void WiFiMgr::init() {
#ifndef UNIT_TEST
    // 设置WiFi模式为Station模式
    WiFi.mode(WIFI_STA);
    
    // 配置WiFiManager
    _wifiManager.setConfigPortalTimeout(AP_CONFIG_TIMEOUT_SEC);
    _wifiManager.setConnectTimeout(WIFI_CONNECT_TIMEOUT_SEC);
    
    Serial.println("WiFi管理器初始化完成");
#endif
    
    setState(WIFI_STATE_DISCONNECTED);
}

bool WiFiMgr::connect() {
#ifndef UNIT_TEST
    setState(WIFI_STATE_CONNECTING);
    _connectStartTime = millis();
    _reconnectAttempts = 0;
    
    Serial.println("正在连接WiFi...");
    
    // 尝试连接已保存的WiFi
    // WiFiManager会自动尝试连接已保存的凭据
    if (WiFi.begin() == WL_CONNECTED) {
        setState(WIFI_STATE_CONNECTED);
        Serial.print("WiFi已连接，IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    
    // 等待连接，最多等待WIFI_CONNECT_TIMEOUT_SEC秒
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > (WIFI_CONNECT_TIMEOUT_SEC * 1000UL)) {
            Serial.println("WiFi连接超时");
            setState(WIFI_STATE_FAILED);
            // 连接超时，启动AP配网模式
            startAPMode();
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    
    setState(WIFI_STATE_CONNECTED);
    Serial.println();
    Serial.print("WiFi已连接，IP: ");
    Serial.println(WiFi.localIP());
    return true;
#else
    // 单元测试模式
    return false;
#endif
}

void WiFiMgr::update() {
#ifndef UNIT_TEST
    // 如果在AP模式，检查是否超时
    if (_apModeActive) {
        if (millis() - _apModeStartTime > (AP_CONFIG_TIMEOUT_SEC * 1000UL)) {
            Serial.println("AP配网超时，重启设备...");
            ESP.restart();
        }
        return;
    }
    
    // 检查连接状态
    if (_state == WIFI_STATE_CONNECTED) {
        if (WiFi.status() != WL_CONNECTED) {
            // 连接断开，尝试重连
            Serial.println("WiFi连接断开");
            setState(WIFI_STATE_DISCONNECTED);
            _reconnectAttempts = 0;
        }
    }
    
    // 处理断线重连
    if (_state == WIFI_STATE_DISCONNECTED && !_apModeActive) {
        // 检查是否需要重连
        if (_reconnectAttempts < WIFI_MAX_RECONNECT_ATTEMPTS) {
            // 检查重连间隔
            if (millis() - _lastReconnectTime >= WIFI_RECONNECT_INTERVAL_MS) {
                tryReconnect();
            }
        } else {
            // 达到最大重连次数，进入AP模式
            Serial.println("达到最大重连次数，启动AP配网模式");
            startAPMode();
        }
    }
#endif
}

bool WiFiMgr::isConnected() {
#ifndef UNIT_TEST
    return WiFi.status() == WL_CONNECTED;
#else
    return _state == WIFI_STATE_CONNECTED;
#endif
}

void WiFiMgr::startAPMode() {
#ifndef UNIT_TEST
    if (_apModeActive) {
        return;
    }
    
    Serial.println("启动AP配网模式...");
    setState(WIFI_STATE_AP_MODE);
    _apModeActive = true;
    _apModeStartTime = millis();
    
    // 使用WiFiManager启动配网门户
    // autoConnect会阻塞直到配置完成或超时
    if (_wifiManager.autoConnect(AP_NAME, AP_PASSWORD)) {
        // 配网成功
        Serial.println("AP配网成功");
        _apModeActive = false;
        setState(WIFI_STATE_CONNECTED);
    } else {
        // 配网失败或超时
        Serial.println("AP配网失败");
        _apModeActive = false;
        setState(WIFI_STATE_FAILED);
    }
#endif
}

void WiFiMgr::stopAPMode() {
#ifndef UNIT_TEST
    if (!_apModeActive) {
        return;
    }
    
    Serial.println("停止AP配网模式");
    _wifiManager.stopConfigPortal();
    _apModeActive = false;
    setState(WIFI_STATE_DISCONNECTED);
#endif
}

int8_t WiFiMgr::getRSSI() {
#ifndef UNIT_TEST
    if (isConnected()) {
        return WiFi.RSSI();
    }
#endif
    return 0;
}

String WiFiMgr::getSSID() {
#ifndef UNIT_TEST
    if (isConnected()) {
        return WiFi.SSID();
    }
#endif
    return String("");
}

WiFiConnectionState WiFiMgr::getState() {
    return _state;
}

void WiFiMgr::setStateCallback(WiFiStateCallback callback) {
    _stateCallback = callback;
}

uint8_t WiFiMgr::getReconnectAttempts() {
    return _reconnectAttempts;
}

void WiFiMgr::resetReconnectAttempts() {
    _reconnectAttempts = 0;
}

void WiFiMgr::disconnect() {
#ifndef UNIT_TEST
    WiFi.disconnect();
#endif
    setState(WIFI_STATE_DISCONNECTED);
}

void WiFiMgr::setState(WiFiConnectionState newState) {
    if (_state != newState) {
        _state = newState;
        
        // 打印状态变化日志
        const char* stateNames[] = {
            "DISCONNECTED", "CONNECTING", "CONNECTED", "AP_MODE", "FAILED"
        };
        Serial.print("WiFi状态变化: ");
        Serial.println(stateNames[newState]);
        
        // 触发回调
        if (_stateCallback != nullptr) {
            _stateCallback(newState);
        }
    }
}

bool WiFiMgr::tryReconnect() {
#ifndef UNIT_TEST
    _reconnectAttempts++;
    _lastReconnectTime = millis();
    
    Serial.print("尝试重连WiFi (");
    Serial.print(_reconnectAttempts);
    Serial.print("/");
    Serial.print(WIFI_MAX_RECONNECT_ATTEMPTS);
    Serial.println(")");
    
    setState(WIFI_STATE_CONNECTING);
    
    // 尝试重新连接
    WiFi.reconnect();
    
    // 等待连接结果（最多等待5秒）
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > 5000) {
            Serial.println("重连超时");
            setState(WIFI_STATE_DISCONNECTED);
            return false;
        }
        delay(100);
    }
    
    Serial.println("重连成功");
    setState(WIFI_STATE_CONNECTED);
    _reconnectAttempts = 0;
    return true;
#else
    return false;
#endif
}
