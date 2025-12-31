/**
 * 智能桌面伴侣 - 时钟显示渲染器实现
 */

#include "ClockRenderer.h"

// 迷你表情图标位图 (16x16 像素)
const uint8_t MINI_FACE[] PROGMEM = {
    0xE0, 0x07, 0x18, 0x18, 0x04, 0x20, 0x02, 0x40,
    0x32, 0x4C, 0x32, 0x4C, 0x01, 0x80, 0x01, 0x80,
    0x01, 0x80, 0x01, 0x80, 0x22, 0x44, 0xC2, 0x43,
    0x04, 0x20, 0x18, 0x18, 0xE0, 0x07, 0x00, 0x00
};

ClockRenderer::ClockRenderer()
    : currentHour(0)
    , currentMinute(0)
    , currentSecond(0)
    , currentYear(2024)
    , currentMonth(1)
    , currentDay(1)
    , isOffline(false) {
}

void ClockRenderer::init() {
    // 初始化完成
}

char* ClockRenderer::formatTime(uint8_t hour, uint8_t minute, uint8_t second, char* buffer) {
    // 格式化为 HH:MM:SS
    buffer[0] = '0' + (hour / 10);
    buffer[1] = '0' + (hour % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minute / 10);
    buffer[4] = '0' + (minute % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (second / 10);
    buffer[7] = '0' + (second % 10);
    buffer[8] = '\0';
    return buffer;
}

char* ClockRenderer::formatDate(uint16_t year, uint8_t month, uint8_t day, char* buffer) {
    // 格式化为 YYYY-MM-DD
    buffer[0] = '0' + (year / 1000);
    buffer[1] = '0' + ((year / 100) % 10);
    buffer[2] = '0' + ((year / 10) % 10);
    buffer[3] = '0' + (year % 10);
    buffer[4] = '-';
    buffer[5] = '0' + (month / 10);
    buffer[6] = '0' + (month % 10);
    buffer[7] = '-';
    buffer[8] = '0' + (day / 10);
    buffer[9] = '0' + (day % 10);
    buffer[10] = '\0';
    return buffer;
}

void ClockRenderer::render(U8G2* display) {
    if (display == nullptr) return;
    
    char timeBuffer[9];
    char dateBuffer[11];
    
    // 格式化时间和日期
    formatTime(currentHour, currentMinute, currentSecond, timeBuffer);
    formatDate(currentYear, currentMonth, currentDay, dateBuffer);
    
    // 绘制迷你表情图标（左上角）
    drawMiniFace(display, 4, 4);
    
    // 绘制时间（大字体，居中）
    display->setFont(u8g2_font_logisoso22_tn);
    int16_t timeWidth = display->getStrWidth(timeBuffer);
    int16_t timeX = (OLED_WIDTH - timeWidth) / 2;
    display->drawStr(timeX, 38, timeBuffer);
    
    // 绘制日期（小字体，居中）
    display->setFont(u8g2_font_6x10_tf);
    int16_t dateWidth = display->getStrWidth(dateBuffer);
    int16_t dateX = (OLED_WIDTH - dateWidth) / 2;
    display->drawStr(dateX, 56, dateBuffer);
    
    // 如果离线，显示离线指示器
    if (isOffline) {
        drawOfflineIndicator(display);
    }
}

void ClockRenderer::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    currentHour = hour;
    currentMinute = minute;
    currentSecond = second;
}

void ClockRenderer::setDate(uint16_t year, uint8_t month, uint8_t day) {
    currentYear = year;
    currentMonth = month;
    currentDay = day;
}

void ClockRenderer::setOffline(bool offline) {
    isOffline = offline;
}

void ClockRenderer::drawMiniFace(U8G2* display, int16_t x, int16_t y) {
    display->drawXBMP(x, y, 16, 16, MINI_FACE);
}

void ClockRenderer::drawOfflineIndicator(U8G2* display) {
    // 在右上角显示离线图标（一个小叉号）
    display->setFont(u8g2_font_6x10_tf);
    display->drawStr(110, 10, "OFF");
}
