/**
 * 智能桌面伴侣 - 系统信息渲染器实现
 */

#include "SysInfoRenderer.h"

// ESP32-C3 总堆内存大约 320KB
static const uint32_t TOTAL_HEAP_ESTIMATE = 320 * 1024;

SysInfoRenderer::SysInfoRenderer()
    : freeHeapBytes(0)
    , uptimeSeconds(0)
    , wifiRSSI(0)
    , wifiConnected(false) {
}

void SysInfoRenderer::init() {
    // 初始化完成
}

char* SysInfoRenderer::formatUptime(uint32_t seconds, char* buffer) {
    uint32_t days = seconds / 86400;
    uint32_t hours = (seconds % 86400) / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    if (days > 0) {
        // 格式: Xd HH:MM:SS
        sprintf(buffer, "%lud %02lu:%02lu:%02lu", days, hours, minutes, secs);
    } else {
        // 格式: HH:MM:SS
        sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, secs);
    }
    
    return buffer;
}

void SysInfoRenderer::render(U8G2* display) {
    if (display == nullptr) return;
    
    display->setFont(u8g2_font_6x10_tf);
    
    // 标题
    display->drawStr(0, 10, "System Info");
    display->drawHLine(0, 12, OLED_WIDTH);
    
    // 内存信息
    char memBuffer[20];
    uint32_t freeKB = freeHeapBytes / 1024;
    sprintf(memBuffer, "Mem: %luKB", freeKB);
    display->drawStr(0, 26, memBuffer);
    
    // 绘制内存使用条
    drawMemoryBar(display, 70, 18, freeHeapBytes);
    
    // 运行时间
    char uptimeBuffer[20];
    formatUptime(uptimeSeconds, uptimeBuffer);
    display->drawStr(0, 40, "Up:");
    display->drawStr(24, 40, uptimeBuffer);
    
    // WiFi信号
    if (wifiConnected) {
        char rssiBuffer[16];
        sprintf(rssiBuffer, "WiFi: %ddBm", wifiRSSI);
        display->drawStr(0, 54, rssiBuffer);
        
        // 绘制信号图标
        drawSignalIcon(display, 100, 46, wifiRSSI);
    } else {
        display->drawStr(0, 54, "WiFi: --");
    }
}

void SysInfoRenderer::setFreeHeap(uint32_t bytes) {
    freeHeapBytes = bytes;
}

void SysInfoRenderer::setUptime(uint32_t seconds) {
    uptimeSeconds = seconds;
}

void SysInfoRenderer::setRSSI(int8_t rssi) {
    wifiRSSI = rssi;
}

void SysInfoRenderer::setWiFiConnected(bool connected) {
    wifiConnected = connected;
}

void SysInfoRenderer::drawSignalIcon(U8G2* display, int16_t x, int16_t y, int8_t rssi) {
    // 根据RSSI绘制信号强度条
    // RSSI范围通常是 -100 到 -30 dBm
    // -30 到 -50: 优秀 (4格)
    // -50 到 -60: 良好 (3格)
    // -60 到 -70: 一般 (2格)
    // -70 到 -80: 较弱 (1格)
    // < -80: 很弱 (0格)
    
    int bars = 0;
    if (rssi >= -50) bars = 4;
    else if (rssi >= -60) bars = 3;
    else if (rssi >= -70) bars = 2;
    else if (rssi >= -80) bars = 1;
    
    // 绘制4个信号条
    for (int i = 0; i < 4; i++) {
        int barHeight = 2 + i * 2;  // 2, 4, 6, 8
        int barX = x + i * 5;
        int barY = y + (8 - barHeight);
        
        if (i < bars) {
            // 填充的条
            display->drawBox(barX, barY, 3, barHeight);
        } else {
            // 空心的条
            display->drawFrame(barX, barY, 3, barHeight);
        }
    }
}

void SysInfoRenderer::drawMemoryBar(U8G2* display, int16_t x, int16_t y, uint32_t freeBytes) {
    // 绘制内存使用进度条
    const int barWidth = 50;
    const int barHeight = 6;
    
    // 计算使用百分比
    uint32_t usedBytes = TOTAL_HEAP_ESTIMATE - freeBytes;
    if (usedBytes > TOTAL_HEAP_ESTIMATE) usedBytes = TOTAL_HEAP_ESTIMATE;
    
    int usedWidth = (usedBytes * barWidth) / TOTAL_HEAP_ESTIMATE;
    
    // 绘制外框
    display->drawFrame(x, y, barWidth, barHeight);
    
    // 绘制已使用部分
    if (usedWidth > 0) {
        display->drawBox(x + 1, y + 1, usedWidth - 2, barHeight - 2);
    }
}
