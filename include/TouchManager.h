/**
 * 智能桌面伴侣 - 触摸管理器
 * 
 * 处理TTP223触摸传感器的输入，支持短按、长按和工厂重置检测
 * 实现防抖逻辑确保触摸输入的可靠性
 */

#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include "config.h"

// 触摸回调函数类型定义
typedef void (*TouchCallback)(TouchEvent event);

/**
 * 触摸管理器类
 * 
 * 负责：
 * - GPIO触摸状态读取
 * - 防抖处理（50ms）
 * - 按压时长检测（短按/长按/工厂重置）
 * - 触摸事件回调机制
 */
class TouchManager {
public:
    /**
     * 构造函数
     */
    TouchManager();
    
    /**
     * 初始化触摸管理器
     * @param pin 触摸传感器GPIO引脚
     */
    void init(uint8_t pin = TOUCH_PIN);
    
    /**
     * 更新触摸状态（需在主循环中调用）
     * 处理防抖和按压时长检测
     */
    void update();
    
    /**
     * 检测是否发生短按事件
     * @return true 如果检测到短按
     */
    bool isShortPress();
    
    /**
     * 检测是否发生长按事件
     * @return true 如果检测到长按（>2秒）
     */
    bool isLongPress();
    
    /**
     * 检测是否触发工厂重置
     * @return true 如果检测到超长按（>10秒）
     */
    bool isFactoryReset();
    
    /**
     * 获取当前触摸事件
     * @return 当前触摸事件类型
     */
    TouchEvent getEvent();
    
    /**
     * 清除当前事件（事件已处理后调用）
     */
    void clearEvent();
    
    /**
     * 设置触摸事件回调函数
     * @param callback 回调函数指针
     */
    void setCallback(TouchCallback callback);
    
    /**
     * 检查当前是否正在触摸
     * @return true 如果正在触摸
     */
    bool isTouching();
    
    /**
     * 获取当前按压持续时间
     * @return 按压时间（毫秒），未按压时返回0
     */
    unsigned long getPressDuration();

private:
    uint8_t _touchPin;              // 触摸传感器引脚
    bool _lastRawState;             // 上次原始状态
    bool _debouncedState;           // 防抖后的状态
    bool _lastDebouncedState;       // 上次防抖后的状态
    
    unsigned long _lastDebounceTime;    // 上次状态变化时间（用于防抖）
    unsigned long _pressStartTime;      // 按压开始时间
    
    TouchEvent _currentEvent;       // 当前触摸事件
    TouchCallback _callback;        // 回调函数指针
    
    bool _eventPending;             // 是否有待处理的事件
    bool _longPressTriggered;       // 长按是否已触发
    bool _factoryResetTriggered;    // 工厂重置是否已触发
    
    /**
     * 处理防抖逻辑
     * @param rawState 当前原始状态
     * @return 防抖后的状态
     */
    bool debounce(bool rawState);
    
    /**
     * 处理按压释放事件
     * @param pressDuration 按压持续时间
     */
    void handleRelease(unsigned long pressDuration);
    
    /**
     * 触发事件
     * @param event 事件类型
     */
    void triggerEvent(TouchEvent event);
};

#endif // TOUCH_MANAGER_H
