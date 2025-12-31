/**
 * 智能桌面伴侣 - 显示管理器实现
 */

#include "DisplayManager.h"
#include <Wire.h>

DisplayManager::DisplayManager()
    : display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE)
    , currentMode(MODE_FACE)
    , previousMode(MODE_FACE)
    , brightness(DEFAULT_BRIGHTNESS)
    , isTransitioning(false)
    , transitionStartTime(0)
    , faceRenderer()
    , clockRenderer()
    , sysInfoRenderer() {
}

bool DisplayManager::init() {
    // 初始化I2C引脚
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    // 初始化U8g2显示
    if (!display.begin()) {
        return false;
    }
    
    // 设置默认亮度
    display.setContrast(brightness);
    
    // 设置字体
    display.setFont(u8g2_font_6x10_tf);
    
    // 清空显示
    display.clearBuffer();
    display.sendBuffer();
    
    // 初始化渲染器
    faceRenderer.init();
    clockRenderer.init();
    sysInfoRenderer.init();
    
    return true;
}

void DisplayManager::update() {
    // 更新表情动画（即使不在表情模式也需要更新，以便切换时动画连贯）
    if (currentMode == MODE_FACE || currentMode == MODE_SLEEP) {
        faceRenderer.updateAnimation();
    }
    
    // 检查是否正在进行过渡动画
    if (isTransitioning) {
        unsigned long elapsed = millis() - transitionStartTime;
        if (elapsed >= TRANSITION_DURATION_MS) {
            // 过渡动画结束
            isTransitioning = false;
            // 恢复正常亮度
            display.setContrast(brightness);
        } else {
            // 渲染过渡动画
            renderTransition();
            return;
        }
    }
    
    // 渲染当前模式内容
    renderCurrentMode();
}

void DisplayManager::setMode(DisplayMode mode) {
    if (mode == currentMode) {
        return;
    }
    
    // 如果从睡眠模式唤醒，触发唤醒动画
    if (currentMode == MODE_SLEEP && mode != MODE_SLEEP) {
        faceRenderer.wakeUp();
    }
    
    // 如果进入睡眠模式，设置表情为睡眠状态
    if (mode == MODE_SLEEP) {
        faceRenderer.enterSleep();
    }
    
    // 保存上一个模式
    previousMode = currentMode;
    currentMode = mode;
    
    // 开始过渡动画
    isTransitioning = true;
    transitionStartTime = millis();
}

DisplayMode DisplayManager::getMode() const {
    return currentMode;
}

void DisplayManager::nextMode() {
    // 计算下一个模式（跳过睡眠模式，睡眠模式由系统自动进入）
    DisplayMode next;
    switch (currentMode) {
        case MODE_FACE:
            next = MODE_CLOCK;
            break;
        case MODE_CLOCK:
            next = MODE_SYSINFO;
            break;
        case MODE_SYSINFO:
        case MODE_SLEEP:
        default:
            next = MODE_FACE;
            break;
    }
    
    setMode(next);
}

void DisplayManager::setBrightness(uint8_t level) {
    brightness = level;
    display.setContrast(brightness);
}

uint8_t DisplayManager::getBrightness() const {
    return brightness;
}

U8G2* DisplayManager::getDisplay() {
    return &display;
}

void DisplayManager::showBootScreen() {
    display.clearBuffer();
    
    // 显示启动画面
    display.setFont(u8g2_font_helvB12_tf);
    const char* title = "Smart";
    int16_t titleWidth = display.getStrWidth(title);
    display.drawStr((OLED_WIDTH - titleWidth) / 2, 24, title);
    
    const char* subtitle = "Companion";
    int16_t subtitleWidth = display.getStrWidth(subtitle);
    display.drawStr((OLED_WIDTH - subtitleWidth) / 2, 44, subtitle);
    
    // 显示版本号
    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(44, 60, "v1.0.0");
    
    display.sendBuffer();
}

void DisplayManager::showConnectionStatus(const char* message) {
    display.clearBuffer();
    
    // 显示WiFi图标区域
    display.setFont(u8g2_font_open_iconic_www_2x_t);
    display.drawGlyph(56, 24, 0x0048);  // WiFi图标
    
    // 显示状态消息
    display.setFont(u8g2_font_6x10_tf);
    int16_t msgWidth = display.getStrWidth(message);
    display.drawStr((OLED_WIDTH - msgWidth) / 2, 48, message);
    
    display.sendBuffer();
}

void DisplayManager::showMessage(const char* message) {
    display.clearBuffer();
    
    // 居中显示消息
    display.setFont(u8g2_font_helvB10_tf);
    int16_t msgWidth = display.getStrWidth(message);
    int16_t x = (OLED_WIDTH - msgWidth) / 2;
    int16_t y = OLED_HEIGHT / 2 + 4;  // 垂直居中
    display.drawStr(x, y, message);
    
    display.sendBuffer();
}

void DisplayManager::clear() {
    display.clearBuffer();
    display.sendBuffer();
}

FaceRenderer& DisplayManager::getFaceRenderer() {
    return faceRenderer;
}

ClockRenderer& DisplayManager::getClockRenderer() {
    return clockRenderer;
}

SysInfoRenderer& DisplayManager::getSysInfoRenderer() {
    return sysInfoRenderer;
}

void DisplayManager::renderTransition() {
    // 简单的淡入淡出过渡效果
    unsigned long elapsed = millis() - transitionStartTime;
    float progress = (float)elapsed / TRANSITION_DURATION_MS;
    
    // 前半段：淡出
    if (progress < 0.5f) {
        uint8_t fadeLevel = brightness * (1.0f - progress * 2);
        display.setContrast(fadeLevel);
    }
    // 后半段：淡入
    else {
        uint8_t fadeLevel = brightness * ((progress - 0.5f) * 2);
        display.setContrast(fadeLevel);
        // 在淡入阶段渲染新内容
        renderCurrentMode();
    }
}

void DisplayManager::renderCurrentMode() {
    display.clearBuffer();
    
    switch (currentMode) {
        case MODE_FACE:
            // 使用表情渲染器
            faceRenderer.render(&display);
            break;
            
        case MODE_CLOCK:
            // 使用时钟渲染器
            clockRenderer.render(&display);
            break;
            
        case MODE_SYSINFO:
            // 使用系统信息渲染器
            sysInfoRenderer.render(&display);
            break;
            
        case MODE_SLEEP:
            // 睡眠模式：显示闭眼表情
            faceRenderer.render(&display);
            break;
            
        default:
            break;
    }
    
    display.sendBuffer();
}
