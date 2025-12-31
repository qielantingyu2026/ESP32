/**
 * 智能桌面伴侣 - 显示管理器
 * 
 * 负责OLED显示的统一管理，协调不同显示模式的切换
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"
#include "FaceRenderer.h"
#include "ClockRenderer.h"
#include "SysInfoRenderer.h"

class DisplayManager {
public:
    DisplayManager();
    
    /**
     * 初始化显示管理器
     * @return true 初始化成功，false 初始化失败
     */
    bool init();
    
    /**
     * 更新显示内容（在主循环中调用）
     */
    void update();
    
    /**
     * 设置显示模式
     * @param mode 目标显示模式
     */
    void setMode(DisplayMode mode);
    
    /**
     * 获取当前显示模式
     * @return 当前显示模式
     */
    DisplayMode getMode() const;
    
    /**
     * 切换到下一个显示模式（循环切换）
     */
    void nextMode();
    
    /**
     * 设置显示亮度
     * @param level 亮度值 (0-255)
     */
    void setBrightness(uint8_t level);
    
    /**
     * 获取当前亮度
     * @return 当前亮度值
     */
    uint8_t getBrightness() const;
    
    /**
     * 获取U8g2显示对象指针（供渲染器使用）
     * @return U8g2对象指针
     */
    U8G2* getDisplay();
    
    /**
     * 显示启动画面
     */
    void showBootScreen();
    
    /**
     * 显示连接状态
     * @param message 状态消息
     */
    void showConnectionStatus(const char* message);
    
    /**
     * 显示通用消息
     * @param message 要显示的消息
     */
    void showMessage(const char* message);
    
    /**
     * 清空显示
     */
    void clear();
    
    /**
     * 获取表情渲染器引用
     */
    FaceRenderer& getFaceRenderer();
    
    /**
     * 获取时钟渲染器引用
     */
    ClockRenderer& getClockRenderer();
    
    /**
     * 获取系统信息渲染器引用
     */
    SysInfoRenderer& getSysInfoRenderer();

private:
    // U8g2显示对象 - SSD1306 128x64 I2C模式
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;
    
    // 当前显示模式
    DisplayMode currentMode;
    
    // 上一个显示模式（用于过渡动画）
    DisplayMode previousMode;
    
    // 当前亮度
    uint8_t brightness;
    
    // 是否正在进行模式切换动画
    bool isTransitioning;
    
    // 过渡动画开始时间
    unsigned long transitionStartTime;
    
    // 过渡动画持续时间（毫秒）
    static const unsigned long TRANSITION_DURATION_MS = 200;
    
    // 渲染器实例
    FaceRenderer faceRenderer;
    ClockRenderer clockRenderer;
    SysInfoRenderer sysInfoRenderer;
    
    /**
     * 渲染过渡动画
     */
    void renderTransition();
    
    /**
     * 根据当前模式渲染内容
     */
    void renderCurrentMode();
};

#endif // DISPLAY_MANAGER_H
