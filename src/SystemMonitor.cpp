/**
 * 智能桌面伴侣 - 系统监控器实现
 */

#include <Arduino.h>
#include "SystemMonitor.h"

#ifndef UNIT_TEST
#include <esp_system.h>
#endif

SystemMonitor::SystemMonitor()
    : startTime(0)
    , minFreeHeap(UINT32_MAX)
    , lastCheckTime(0) {
}

void SystemMonitor::init() {
    startTime = millis();
    lastCheckTime = startTime;
    
#ifndef UNIT_TEST
    // 记录初始空闲堆内存
    minFreeHeap = esp_get_free_heap_size();
    
    Serial.println("[SystemMonitor] 系统监控器初始化");
    Serial.printf("  初始空闲堆内存: %u bytes (%.1f KB)\n", 
                  minFreeHeap, minFreeHeap / 1024.0f);
#endif
}

void SystemMonitor::update() {
    unsigned long currentTime = millis();
    
    // 定期检查内存状态
    if (currentTime - lastCheckTime >= CHECK_INTERVAL_MS) {
        lastCheckTime = currentTime;
        
#ifndef UNIT_TEST
        uint32_t currentFreeHeap = esp_get_free_heap_size();
        
        // 更新最小空闲堆内存
        if (currentFreeHeap < minFreeHeap) {
            minFreeHeap = currentFreeHeap;
        }
        
        // 检查内存不足警告
        if (isLowMemory()) {
            Serial.printf("[SystemMonitor] 警告: 内存不足! 空闲: %u bytes (%.1f KB)\n",
                          currentFreeHeap, currentFreeHeap / 1024.0f);
        }
#endif
    }
}

uint32_t SystemMonitor::getFreeHeap() const {
#ifndef UNIT_TEST
    return esp_get_free_heap_size();
#else
    return 50000;  // 测试模式返回模拟值
#endif
}

float SystemMonitor::getFreeHeapKB() const {
    return getFreeHeap() / 1024.0f;
}

uint32_t SystemMonitor::getUptime() const {
    return (millis() - startTime) / 1000;
}

String SystemMonitor::getUptimeString() const {
    uint32_t totalSeconds = getUptime();
    uint32_t days = totalSeconds / 86400;
    uint32_t hours = (totalSeconds % 86400) / 3600;
    uint32_t minutes = (totalSeconds % 3600) / 60;
    uint32_t seconds = totalSeconds % 60;
    
    char buffer[16];
    
    if (days > 0) {
        // 超过一天显示: Xd HH:MM
        snprintf(buffer, sizeof(buffer), "%ud %02u:%02u", days, hours, minutes);
    } else {
        // 不足一天显示: HH:MM:SS
        snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", hours, minutes, seconds);
    }
    
    return String(buffer);
}

bool SystemMonitor::isLowMemory() const {
    return getFreeHeap() < LOW_MEMORY_THRESHOLD;
}

uint32_t SystemMonitor::getMinFreeHeap() const {
    return minFreeHeap;
}
