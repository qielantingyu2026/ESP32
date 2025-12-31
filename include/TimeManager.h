/**
 * 智能桌面伴侣 - 时间管理器
 * 
 * 管理NTP时间同步和本地时间显示
 * 支持定期重新同步和离线指示
 */

#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

#ifndef UNIT_TEST
#include <WiFi.h>
#include <time.h>
#endif

#include "config.h"

// 时间同步状态枚举
enum TimeSyncState {
    TIME_NOT_SYNCED = 0,    // 未同步
    TIME_SYNCING,           // 同步中
    TIME_SYNCED,            // 已同步
    TIME_SYNC_FAILED        // 同步失败
};

// 时间同步回调函数类型
typedef void (*TimeSyncCallback)(TimeSyncState state);

/**
 * 时间管理器类
 * 
 * 功能：
 * - NTP时间同步
 * - 时间和日期格式化
 * - 定期重新同步
 * - 同步状态指示
 */
class TimeManager {
public:
    /**
     * 构造函数
     */
    TimeManager();
    
    /**
     * 初始化时间管理器
     */
    void init();
    
    /**
     * 同步NTP时间
     * @return true 同步成功，false 同步失败
     */
    bool syncNTP();
    
    /**
     * 更新时间管理器（在主循环中调用）
     * 处理定期重新同步
     */
    void update();
    
    /**
     * 检查时间是否已同步
     * @return true 已同步，false 未同步
     */
    bool isSynced();
    
    /**
     * 获取格式化的时间字符串
     * @return HH:MM:SS格式的时间字符串
     */
    String getTimeString();
    
    /**
     * 获取格式化的日期字符串
     * @return YYYY-MM-DD格式的日期字符串
     */
    String getDateString();
    
    /**
     * 获取当前小时
     * @return 小时 (0-23)
     */
    uint8_t getHour();
    
    /**
     * 获取当前分钟
     * @return 分钟 (0-59)
     */
    uint8_t getMinute();
    
    /**
     * 获取当前秒
     * @return 秒 (0-59)
     */
    uint8_t getSecond();
    
    /**
     * 获取当前年份
     * @return 年份
     */
    uint16_t getYear();
    
    /**
     * 获取当前月份
     * @return 月份 (1-12)
     */
    uint8_t getMonth();
    
    /**
     * 获取当前日期
     * @return 日期 (1-31)
     */
    uint8_t getDay();
    
    /**
     * 获取同步状态
     * @return TimeSyncState枚举值
     */
    TimeSyncState getState();
    
    /**
     * 设置同步状态回调函数
     * @param callback 回调函数指针
     */
    void setSyncCallback(TimeSyncCallback callback);
    
    /**
     * 获取上次同步时间（毫秒）
     * @return 上次同步的millis()值
     */
    unsigned long getLastSyncTime();
    
    /**
     * 强制重新同步
     */
    void forceSync();

private:
    TimeSyncState _state;               // 当前同步状态
    TimeSyncCallback _syncCallback;     // 同步状态回调函数
    
    unsigned long _lastSyncTime;        // 上次同步时间
    unsigned long _lastSyncAttempt;     // 上次同步尝试时间
    
    bool _synced;                       // 是否已同步
    
    // 缓存的时间值（用于离线显示）
    uint8_t _cachedHour;
    uint8_t _cachedMinute;
    uint8_t _cachedSecond;
    uint16_t _cachedYear;
    uint8_t _cachedMonth;
    uint8_t _cachedDay;
    unsigned long _cacheUpdateTime;     // 缓存更新时间
    
    /**
     * 更新同步状态并触发回调
     * @param newState 新状态
     */
    void setState(TimeSyncState newState);
    
    /**
     * 更新缓存的时间值
     */
    void updateCachedTime();
    
    /**
     * 格式化数字为两位字符串
     * @param num 数字
     * @return 两位格式的字符串（如 "05"）
     */
    String formatTwoDigits(uint8_t num);
};

#endif // TIME_MANAGER_H
