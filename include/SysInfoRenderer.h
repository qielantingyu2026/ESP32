/**
 * 智能桌面伴侣 - 系统信息渲染器
 * 
 * 负责渲染系统信息界面，显示内存、运行时间和WiFi信号
 */

#ifndef SYSINFO_RENDERER_H
#define SYSINFO_RENDERER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

class SysInfoRenderer {
public:
    SysInfoRenderer();
    
    /**
     * 初始化系统信息渲染器
     */
    void init();
    
    /**
     * 渲染系统信息界面到显示器
     * @param display U8g2显示对象指针
     */
    void render(U8G2* display);
    
    /**
     * 设置空闲堆内存
     * @param bytes 空闲内存字节数
     */
    void setFreeHeap(uint32_t bytes);
    
    /**
     * 设置设备运行时间
     * @param seconds 运行时间（秒）
     */
    void setUptime(uint32_t seconds);
    
    /**
     * 设置WiFi信号强度
     * @param rssi 信号强度（dBm）
     */
    void setRSSI(int8_t rssi);
    
    /**
     * 设置WiFi连接状态
     * @param connected true表示已连接
     */
    void setWiFiConnected(bool connected);
    
    /**
     * 格式化运行时间为可读字符串
     * @param seconds 运行时间（秒）
     * @param buffer 输出缓冲区（至少16字节）
     * @return 格式化后的字符串指针
     */
    static char* formatUptime(uint32_t seconds, char* buffer);

private:
    // 空闲堆内存（字节）
    uint32_t freeHeapBytes;
    
    // 运行时间（秒）
    uint32_t uptimeSeconds;
    
    // WiFi信号强度（dBm）
    int8_t wifiRSSI;
    
    // WiFi是否已连接
    bool wifiConnected;
    
    /**
     * 绘制信号强度图标
     */
    void drawSignalIcon(U8G2* display, int16_t x, int16_t y, int8_t rssi);
    
    /**
     * 绘制内存使用条
     */
    void drawMemoryBar(U8G2* display, int16_t x, int16_t y, uint32_t freeBytes);
};

#endif // SYSINFO_RENDERER_H
