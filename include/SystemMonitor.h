/**
 * 智能桌面伴侣 - 系统监控器
 * 
 * 监控系统运行状态，包括内存使用、运行时间等
 */

#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <Arduino.h>

class SystemMonitor {
public:
    SystemMonitor();
    
    /**
     * 初始化系统监控器
     */
    void init();
    
    /**
     * 更新监控数据（在主循环中调用）
     */
    void update();
    
    /**
     * 获取空闲堆内存（字节）
     * @return 空闲堆内存大小
     */
    uint32_t getFreeHeap() const;
    
    /**
     * 获取空闲堆内存（KB）
     * @return 空闲堆内存大小（KB）
     */
    float getFreeHeapKB() const;
    
    /**
     * 获取设备运行时间（秒）
     * @return 运行时间（秒）
     */
    uint32_t getUptime() const;
    
    /**
     * 获取格式化的运行时间字符串
     * @return 格式化字符串 (HH:MM:SS 或 Xd HH:MM)
     */
    String getUptimeString() const;
    
    /**
     * 检查是否内存不足
     * @return true 如果空闲内存低于阈值
     */
    bool isLowMemory() const;
    
    /**
     * 获取最小空闲堆内存（自启动以来的最低值）
     * @return 最小空闲堆内存大小
     */
    uint32_t getMinFreeHeap() const;

private:
    unsigned long startTime;        // 启动时间戳
    uint32_t minFreeHeap;           // 最小空闲堆内存
    unsigned long lastCheckTime;    // 上次检查时间
    
    // 内存不足阈值（10KB）
    static const uint32_t LOW_MEMORY_THRESHOLD = 10240;
    
    // 检查间隔（毫秒）
    static const unsigned long CHECK_INTERVAL_MS = 1000;
};

#endif // SYSTEM_MONITOR_H
