/**
 * 智能桌面伴侣 - 时钟显示渲染器
 * 
 * 负责渲染时钟界面，显示时间和日期
 */

#ifndef CLOCK_RENDERER_H
#define CLOCK_RENDERER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

class ClockRenderer {
public:
    ClockRenderer();
    
    /**
     * 初始化时钟渲染器
     */
    void init();
    
    /**
     * 渲染时钟界面到显示器
     * @param display U8g2显示对象指针
     */
    void render(U8G2* display);
    
    /**
     * 设置当前时间
     * @param hour 小时 (0-23)
     * @param minute 分钟 (0-59)
     * @param second 秒 (0-59)
     */
    void setTime(uint8_t hour, uint8_t minute, uint8_t second);
    
    /**
     * 设置当前日期
     * @param year 年份
     * @param month 月份 (1-12)
     * @param day 日 (1-31)
     */
    void setDate(uint16_t year, uint8_t month, uint8_t day);
    
    /**
     * 设置是否显示离线指示器
     * @param offline true表示离线
     */
    void setOffline(bool offline);
    
    /**
     * 格式化时间为字符串
     * @param hour 小时
     * @param minute 分钟
     * @param second 秒
     * @param buffer 输出缓冲区（至少9字节）
     * @return 格式化后的字符串指针
     */
    static char* formatTime(uint8_t hour, uint8_t minute, uint8_t second, char* buffer);
    
    /**
     * 格式化日期为字符串
     * @param year 年份
     * @param month 月份
     * @param day 日
     * @param buffer 输出缓冲区（至少11字节）
     * @return 格式化后的字符串指针
     */
    static char* formatDate(uint16_t year, uint8_t month, uint8_t day, char* buffer);

private:
    // 当前时间
    uint8_t currentHour;
    uint8_t currentMinute;
    uint8_t currentSecond;
    
    // 当前日期
    uint16_t currentYear;
    uint8_t currentMonth;
    uint8_t currentDay;
    
    // 是否离线
    bool isOffline;
    
    /**
     * 绘制迷你表情图标
     */
    void drawMiniFace(U8G2* display, int16_t x, int16_t y);
    
    /**
     * 绘制离线指示器
     */
    void drawOfflineIndicator(U8G2* display);
};

#endif // CLOCK_RENDERER_H
