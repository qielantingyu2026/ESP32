/**
 * 智能桌面伴侣 - 时间管理器实现
 * 
 * 实现NTP时间同步、时间格式化和定期重新同步功能
 */

#include <Arduino.h>
#include "TimeManager.h"

TimeManager::TimeManager()
    : _state(TIME_NOT_SYNCED)
    , _syncCallback(nullptr)
    , _lastSyncTime(0)
    , _lastSyncAttempt(0)
    , _synced(false)
    , _cachedHour(0)
    , _cachedMinute(0)
    , _cachedSecond(0)
    , _cachedYear(2024)
    , _cachedMonth(1)
    , _cachedDay(1)
    , _cacheUpdateTime(0) {
}

void TimeManager::init() {
#ifndef UNIT_TEST
    // 配置时区
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    Serial.println("时间管理器初始化完成");
    Serial.print("NTP服务器: ");
    Serial.println(NTP_SERVER);
    Serial.print("时区偏移: UTC+");
    Serial.println(GMT_OFFSET_SEC / 3600);
#endif
    
    setState(TIME_NOT_SYNCED);
}

bool TimeManager::syncNTP() {
#ifndef UNIT_TEST
    setState(TIME_SYNCING);
    _lastSyncAttempt = millis();
    
    Serial.println("正在同步NTP时间...");
    
    // 等待时间同步
    struct tm timeinfo;
    int retryCount = 0;
    const int maxRetries = 10;
    
    while (!getLocalTime(&timeinfo) && retryCount < maxRetries) {
        Serial.print(".");
        delay(500);
        retryCount++;
    }
    
    if (retryCount >= maxRetries) {
        Serial.println("\nNTP同步失败");
        setState(TIME_SYNC_FAILED);
        return false;
    }
    
    // 同步成功
    _synced = true;
    _lastSyncTime = millis();
    setState(TIME_SYNCED);
    
    // 更新缓存
    updateCachedTime();
    
    Serial.println("\nNTP同步成功");
    Serial.print("当前时间: ");
    Serial.print(getDateString());
    Serial.print(" ");
    Serial.println(getTimeString());
    
    return true;
#else
    // 单元测试模式
    return false;
#endif
}

void TimeManager::update() {
#ifndef UNIT_TEST
    // 更新缓存的时间（每秒更新一次）
    if (millis() - _cacheUpdateTime >= 1000) {
        updateCachedTime();
    }
    
    // 检查是否需要重新同步
    if (_synced) {
        if (millis() - _lastSyncTime >= NTP_SYNC_INTERVAL_MS) {
            Serial.println("定期重新同步NTP时间");
            syncNTP();
        }
    }
#endif
}

bool TimeManager::isSynced() {
    return _synced;
}

String TimeManager::getTimeString() {
    // 返回 HH:MM:SS 格式
    String result = formatTwoDigits(_cachedHour);
    result += ":";
    result += formatTwoDigits(_cachedMinute);
    result += ":";
    result += formatTwoDigits(_cachedSecond);
    return result;
}

String TimeManager::getDateString() {
    // 返回 YYYY-MM-DD 格式
    String result = String(_cachedYear);
    result += "-";
    result += formatTwoDigits(_cachedMonth);
    result += "-";
    result += formatTwoDigits(_cachedDay);
    return result;
}

uint8_t TimeManager::getHour() {
    return _cachedHour;
}

uint8_t TimeManager::getMinute() {
    return _cachedMinute;
}

uint8_t TimeManager::getSecond() {
    return _cachedSecond;
}

uint16_t TimeManager::getYear() {
    return _cachedYear;
}

uint8_t TimeManager::getMonth() {
    return _cachedMonth;
}

uint8_t TimeManager::getDay() {
    return _cachedDay;
}

TimeSyncState TimeManager::getState() {
    return _state;
}

void TimeManager::setSyncCallback(TimeSyncCallback callback) {
    _syncCallback = callback;
}

unsigned long TimeManager::getLastSyncTime() {
    return _lastSyncTime;
}

void TimeManager::forceSync() {
    syncNTP();
}

void TimeManager::setState(TimeSyncState newState) {
    if (_state != newState) {
        _state = newState;
        
        // 打印状态变化日志
        const char* stateNames[] = {
            "NOT_SYNCED", "SYNCING", "SYNCED", "SYNC_FAILED"
        };
        Serial.print("时间同步状态变化: ");
        Serial.println(stateNames[newState]);
        
        // 触发回调
        if (_syncCallback != nullptr) {
            _syncCallback(newState);
        }
    }
}

void TimeManager::updateCachedTime() {
#ifndef UNIT_TEST
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        _cachedHour = timeinfo.tm_hour;
        _cachedMinute = timeinfo.tm_min;
        _cachedSecond = timeinfo.tm_sec;
        _cachedYear = timeinfo.tm_year + 1900;
        _cachedMonth = timeinfo.tm_mon + 1;
        _cachedDay = timeinfo.tm_mday;
        _cacheUpdateTime = millis();
    }
#else
    // 单元测试模式：模拟时间流逝
    unsigned long elapsed = millis() - _cacheUpdateTime;
    if (elapsed >= 1000) {
        _cachedSecond++;
        if (_cachedSecond >= 60) {
            _cachedSecond = 0;
            _cachedMinute++;
            if (_cachedMinute >= 60) {
                _cachedMinute = 0;
                _cachedHour++;
                if (_cachedHour >= 24) {
                    _cachedHour = 0;
                }
            }
        }
        _cacheUpdateTime = millis();
    }
#endif
}

String TimeManager::formatTwoDigits(uint8_t num) {
    if (num < 10) {
        return "0" + String(num);
    }
    return String(num);
}
