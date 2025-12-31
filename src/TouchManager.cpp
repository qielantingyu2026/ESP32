/**
 * 智能桌面伴侣 - 触摸管理器实现
 * 
 * 实现TTP223触摸传感器的输入处理
 * 包含防抖逻辑和按压时长检测
 */

#include "TouchManager.h"

TouchManager::TouchManager() 
    : _touchPin(TOUCH_PIN)
    , _lastRawState(false)
    , _debouncedState(false)
    , _lastDebouncedState(false)
    , _lastDebounceTime(0)
    , _pressStartTime(0)
    , _currentEvent(TOUCH_NONE)
    , _callback(nullptr)
    , _eventPending(false)
    , _longPressTriggered(false)
    , _factoryResetTriggered(false)
{
}

void TouchManager::init(uint8_t pin) {
    _touchPin = pin;
    
    // 配置GPIO为输入模式
    pinMode(_touchPin, INPUT);
    
    // 读取初始状态
    _lastRawState = digitalRead(_touchPin) == HIGH;
    _debouncedState = _lastRawState;
    _lastDebouncedState = _lastRawState;
    _lastDebounceTime = millis();
    
    // 重置所有状态
    _currentEvent = TOUCH_NONE;
    _eventPending = false;
    _longPressTriggered = false;
    _factoryResetTriggered = false;
    _pressStartTime = 0;
}

bool TouchManager::debounce(bool rawState) {
    unsigned long currentTime = millis();
    
    // 如果原始状态发生变化，重置防抖计时器
    if (rawState != _lastRawState) {
        _lastDebounceTime = currentTime;
        _lastRawState = rawState;
    }
    
    // 如果状态稳定超过防抖时间，更新防抖后的状态
    if ((currentTime - _lastDebounceTime) >= TOUCH_DEBOUNCE_MS) {
        _debouncedState = rawState;
    }
    
    return _debouncedState;
}

void TouchManager::update() {
    // 读取当前原始状态（TTP223高电平表示触摸）
    bool rawState = digitalRead(_touchPin) == HIGH;
    
    // 应用防抖处理
    bool currentState = debounce(rawState);
    
    unsigned long currentTime = millis();
    
    // 检测状态变化
    if (currentState != _lastDebouncedState) {
        if (currentState) {
            // 按下：记录开始时间
            _pressStartTime = currentTime;
            _longPressTriggered = false;
            _factoryResetTriggered = false;
        } else {
            // 释放：计算按压时长并处理
            if (_pressStartTime > 0) {
                unsigned long pressDuration = currentTime - _pressStartTime;
                handleRelease(pressDuration);
            }
            _pressStartTime = 0;
        }
        _lastDebouncedState = currentState;
    }
    
    // 持续按压时检测长按和工厂重置
    if (currentState && _pressStartTime > 0) {
        unsigned long pressDuration = currentTime - _pressStartTime;
        
        // 检测工厂重置（优先级最高）
        if (pressDuration >= FACTORY_RESET_MIN_MS && !_factoryResetTriggered) {
            _factoryResetTriggered = true;
            _longPressTriggered = true;  // 防止后续触发长按
            triggerEvent(TOUCH_FACTORY_RESET);
        }
        // 检测长按
        else if (pressDuration >= LONG_PRESS_MIN_MS && !_longPressTriggered) {
            _longPressTriggered = true;
            triggerEvent(TOUCH_LONG);
        }
    }
}

void TouchManager::handleRelease(unsigned long pressDuration) {
    // 如果已经触发了长按或工厂重置，不再处理短按
    if (_longPressTriggered || _factoryResetTriggered) {
        return;
    }
    
    // 检测短按
    if (pressDuration < SHORT_PRESS_MAX_MS) {
        triggerEvent(TOUCH_SHORT);
    }
}

void TouchManager::triggerEvent(TouchEvent event) {
    _currentEvent = event;
    _eventPending = true;
    
    // 如果设置了回调函数，立即调用
    if (_callback != nullptr) {
        _callback(event);
    }
}

bool TouchManager::isShortPress() {
    if (_eventPending && _currentEvent == TOUCH_SHORT) {
        return true;
    }
    return false;
}

bool TouchManager::isLongPress() {
    if (_eventPending && _currentEvent == TOUCH_LONG) {
        return true;
    }
    return false;
}

bool TouchManager::isFactoryReset() {
    if (_eventPending && _currentEvent == TOUCH_FACTORY_RESET) {
        return true;
    }
    return false;
}

TouchEvent TouchManager::getEvent() {
    return _currentEvent;
}

void TouchManager::clearEvent() {
    _currentEvent = TOUCH_NONE;
    _eventPending = false;
}

void TouchManager::setCallback(TouchCallback callback) {
    _callback = callback;
}

bool TouchManager::isTouching() {
    return _debouncedState;
}

unsigned long TouchManager::getPressDuration() {
    if (_debouncedState && _pressStartTime > 0) {
        return millis() - _pressStartTime;
    }
    return 0;
}
