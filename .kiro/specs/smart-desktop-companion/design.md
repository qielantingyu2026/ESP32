# Design Document: 智能桌面伴侣

## Overview

智能桌面伴侣是一个基于ESP32-C3 Super Mini的嵌入式系统，使用Arduino框架开发。系统采用模块化设计，将显示、交互、网络和存储功能分离为独立组件。核心特色是OLED屏幕作为公仔脸部显示动态表情，通过触摸传感器实现用户交互。

### 技术选型

- **开发框架**: Arduino (PlatformIO)
- **显示驱动**: U8g2库（支持SSD1315 OLED，兼容SSD1306指令集）
- **WiFi管理**: WiFiManager库（简化配网流程）
- **时间同步**: NTPClient库
- **存储**: Preferences库（ESP32 NVS）
- **通信协议**: I2C（OLED）、GPIO（触摸传感器）

> 注意：SSD1315与SSD1306指令集兼容，U8g2库中使用SSD1306构造函数即可驱动SSD1315。

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Main Application                        │
│                     (状态机控制器)                            │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ DisplayMgr   │  │  TouchMgr    │  │  WiFiMgr     │       │
│  │ 显示管理器    │  │  触摸管理器   │  │  网络管理器   │       │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘       │
│         │                 │                 │               │
│  ┌──────┴───────┐  ┌──────┴───────┐  ┌──────┴───────┐       │
│  │ FaceRenderer │  │ TouchHandler │  │  NTPSync     │       │
│  │ 表情渲染器    │  │  触摸处理器   │  │  时间同步    │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
├─────────────────────────────────────────────────────────────┤
│                      ConfigManager                           │
│                     (配置持久化)                              │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────┴───────────────────────────────┐
│                      Hardware Layer                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ OLED SSD1315 │  │   TTP223     │  │  ESP32-C3    │       │
│  │   (I2C)      │  │   (GPIO)     │  │   WiFi       │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
```

### 状态机设计

```
┌─────────────┐     启动完成      ┌─────────────┐
│   BOOT      │ ───────────────> │    FACE     │
│   启动中    │                   │   表情模式   │
└─────────────┘                   └──────┬──────┘
                                         │
                    ┌────────────────────┼────────────────────┐
                    │ 短按               │ 短按               │ 短按
                    ▼                    ▼                    ▼
             ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
             │    CLOCK     │ ──> │  SYSINFO     │ ──> │    FACE      │
             │   时钟模式    │     │  系统信息    │     │   表情模式    │
             └──────────────┘     └──────────────┘     └──────────────┘
                    │                    │                    │
                    └────────────────────┴────────────────────┘
                                         │
                              5分钟无操作 │
                                         ▼
                                  ┌──────────────┐
                                  │    SLEEP     │
                                  │   睡眠模式    │
                                  └──────────────┘
```

## Components and Interfaces

### 1. DisplayManager（显示管理器）

负责OLED显示的统一管理，协调不同显示模式的切换。

```cpp
class DisplayManager {
public:
    void init();                          // 初始化显示
    void setMode(DisplayMode mode);       // 设置显示模式
    DisplayMode getMode();                // 获取当前模式
    void update();                        // 更新显示内容
    void setBrightness(uint8_t level);    // 设置亮度(0-255)
    
private:
    DisplayMode currentMode;
    FaceRenderer faceRenderer;
    ClockRenderer clockRenderer;
    SysInfoRenderer sysInfoRenderer;
};

enum DisplayMode {
    MODE_FACE,      // 表情模式
    MODE_CLOCK,     // 时钟模式
    MODE_SYSINFO,   // 系统信息模式
    MODE_SLEEP      // 睡眠模式
};
```

### 2. FaceRenderer（表情渲染器）

核心组件，负责渲染公仔的眼睛和嘴巴表情。

```cpp
class FaceRenderer {
public:
    void init();
    void render(U8G2* display);           // 渲染表情到显示器
    void setExpression(Expression expr);  // 设置表情
    void setEyeState(EyeState state);     // 设置眼睛状态
    void setMouthState(MouthState state); // 设置嘴巴状态
    void updateAnimation();               // 更新动画帧
    void triggerBlink();                  // 触发眨眼
    void triggerReaction();               // 触发触摸反应
    
private:
    EyeState eyeState;
    MouthState mouthState;
    uint8_t animationFrame;
    unsigned long lastBlinkTime;
    unsigned long nextBlinkInterval;
};

enum EyeState {
    EYE_NORMAL,     // 正常睁眼
    EYE_BLINK,      // 眨眼中
    EYE_LOOK_LEFT,  // 看左边
    EYE_LOOK_RIGHT, // 看右边
    EYE_SLEEP       // 闭眼睡觉
};

enum MouthState {
    MOUTH_SMILE,    // 微笑
    MOUTH_NEUTRAL,  // 中性
    MOUTH_SURPRISED,// 惊讶
    MOUTH_TALKING   // 说话
};

struct Expression {
    EyeState eyes;
    MouthState mouth;
};
```

### 3. TouchManager（触摸管理器）

处理TTP223触摸传感器的输入，支持短按和长按检测。

```cpp
class TouchManager {
public:
    void init(uint8_t pin);
    void update();                        // 更新触摸状态
    bool isShortPress();                  // 检测短按
    bool isLongPress();                   // 检测长按(2秒)
    bool isFactoryReset();                // 检测工厂重置(10秒)
    void setCallback(TouchCallback cb);   // 设置回调函数
    
private:
    uint8_t touchPin;
    bool lastState;
    unsigned long pressStartTime;
    bool debouncing;
    static const unsigned long DEBOUNCE_MS = 50;
    static const unsigned long SHORT_PRESS_MAX = 500;
    static const unsigned long LONG_PRESS_MIN = 2000;
    static const unsigned long FACTORY_RESET_MIN = 10000;
};

typedef void (*TouchCallback)(TouchEvent event);

enum TouchEvent {
    TOUCH_SHORT,        // 短按
    TOUCH_LONG,         // 长按
    TOUCH_FACTORY_RESET // 工厂重置
};
```

### 4. WiFiManager（网络管理器）

管理WiFi连接，支持自动连接和AP配网模式。

```cpp
class WiFiMgr {
public:
    void init();
    bool connect();                       // 连接WiFi
    bool isConnected();                   // 检查连接状态
    void startAPMode();                   // 启动AP配网模式
    int8_t getRSSI();                     // 获取信号强度
    String getSSID();                     // 获取当前SSID
    
private:
    bool apModeActive;
    unsigned long lastReconnectAttempt;
};
```

### 5. TimeManager（时间管理器）

管理NTP时间同步和本地时间。

```cpp
class TimeManager {
public:
    void init();
    void syncNTP();                       // 同步NTP时间
    bool isSynced();                      // 是否已同步
    String getTimeString();               // 获取时间字符串 HH:MM:SS
    String getDateString();               // 获取日期字符串 YYYY-MM-DD
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    
private:
    bool synced;
    unsigned long lastSyncTime;
    static const char* NTP_SERVER;
    static const long GMT_OFFSET_SEC;     // 时区偏移(中国+8)
};
```

### 6. ConfigManager（配置管理器）

使用ESP32 NVS存储配置数据。

```cpp
class ConfigManager {
public:
    void init();
    void load();                          // 加载配置
    void save();                          // 保存配置
    void factoryReset();                  // 恢复出厂设置
    
    // 配置项
    uint8_t brightness;                   // 亮度 0-255
    uint16_t dimTimeout;                  // 调暗超时(秒)
    uint16_t sleepTimeout;                // 睡眠超时(秒)
    DisplayMode lastMode;                 // 上次显示模式
    
private:
    Preferences preferences;
    bool dirty;                           // 是否有未保存的更改
    unsigned long lastChangeTime;
};
```

### 7. SystemMonitor（系统监控）

监控系统运行状态。

```cpp
class SystemMonitor {
public:
    void init();
    void update();
    uint32_t getFreeHeap();               // 获取空闲堆内存
    uint32_t getUptime();                 // 获取运行时间(秒)
    bool isLowMemory();                   // 是否内存不足
    
private:
    unsigned long startTime;
    static const uint32_t LOW_MEMORY_THRESHOLD = 10240; // 10KB
};
```

## Data Models

### 表情位图数据

表情使用位图数组存储，每个表情状态对应一组位图数据。

```cpp
// 眼睛位图 (每只眼睛 16x16 像素)
const uint8_t EYE_NORMAL[] PROGMEM = { /* 位图数据 */ };
const uint8_t EYE_BLINK_1[] PROGMEM = { /* 眨眼帧1 */ };
const uint8_t EYE_BLINK_2[] PROGMEM = { /* 眨眼帧2 */ };
const uint8_t EYE_BLINK_3[] PROGMEM = { /* 眨眼帧3 - 闭眼 */ };
const uint8_t EYE_LOOK_LEFT[] PROGMEM = { /* 看左 */ };
const uint8_t EYE_LOOK_RIGHT[] PROGMEM = { /* 看右 */ };
const uint8_t EYE_SLEEP[] PROGMEM = { /* 睡眠闭眼 */ };

// 嘴巴位图 (32x16 像素)
const uint8_t MOUTH_SMILE[] PROGMEM = { /* 微笑 */ };
const uint8_t MOUTH_NEUTRAL[] PROGMEM = { /* 中性 */ };
const uint8_t MOUTH_SURPRISED[] PROGMEM = { /* 惊讶 */ };
const uint8_t MOUTH_TALKING_1[] PROGMEM = { /* 说话帧1 */ };
const uint8_t MOUTH_TALKING_2[] PROGMEM = { /* 说话帧2 */ };

// 表情布局配置
struct FaceLayout {
    int16_t leftEyeX, leftEyeY;    // 左眼位置
    int16_t rightEyeX, rightEyeY;  // 右眼位置
    int16_t mouthX, mouthY;        // 嘴巴位置
};

const FaceLayout DEFAULT_LAYOUT = {
    .leftEyeX = 16, .leftEyeY = 8,
    .rightEyeX = 80, .rightEyeY = 8,
    .mouthX = 48, .mouthY = 40
};
```

### 配置数据结构

```cpp
struct DeviceConfig {
    uint8_t version;              // 配置版本号
    uint8_t brightness;           // 亮度 (0-255)
    uint16_t dimTimeoutSec;       // 调暗超时 (默认300秒)
    uint16_t sleepTimeoutSec;     // 睡眠超时 (默认600秒)
    uint8_t lastDisplayMode;      // 上次显示模式
    char wifiSSID[32];            // WiFi SSID
    char wifiPassword[64];        // WiFi 密码
};

const DeviceConfig DEFAULT_CONFIG = {
    .version = 1,
    .brightness = 200,
    .dimTimeoutSec = 300,
    .sleepTimeoutSec = 600,
    .lastDisplayMode = MODE_FACE,
    .wifiSSID = "",
    .wifiPassword = ""
};
```



## Correctness Properties

*正确性属性是系统在所有有效执行中都应保持为真的特征或行为。属性作为人类可读规范和机器可验证正确性保证之间的桥梁。*

### Property 1: 时间日期格式化正确性

*For any* 有效的时间值（0-23时，0-59分，0-59秒），格式化函数应返回符合HH:MM:SS格式的字符串（长度为8，包含两个冒号分隔符）。

*For any* 有效的日期值，格式化函数应返回符合YYYY-MM-DD格式的字符串（长度为10，包含两个连字符分隔符）。

**Validates: Requirements 2.2, 2.3**

### Property 2: 显示模式循环切换

*For any* 当前显示模式，执行模式切换操作后，应切换到下一个模式；当处于最后一个模式时，应循环回到第一个模式。

模式顺序: FACE → CLOCK → SYSINFO → FACE

**Validates: Requirements 3.1**

### Property 3: 触摸输入防抖

*For any* 在防抖时间窗口（50ms）内的连续触摸输入序列，系统应只产生一次有效的触摸事件。

**Validates: Requirements 3.3**

### Property 4: 眨眼间隔范围

*For any* 生成的随机眨眼间隔值，该值应在3000ms到8000ms范围内（包含边界）。

**Validates: Requirements 4.4**

### Property 5: 配置持久化Round-Trip

*For any* 有效的配置值（亮度0-255，超时值>0），保存到存储后再读取，应得到与原始值相等的配置。

这是一个round-trip属性：save(config) → load() == config

**Validates: Requirements 5.5, 6.4, 8.1, 8.2**

### Property 6: 运行时间计算

*For any* 两个时间点t1和t2（t2 > t1），运行时间的增量应等于t2 - t1。

运行时间应单调递增，不应出现负值或回退。

**Validates: Requirements 7.3**

## Error Handling

### WiFi连接错误

- 连接超时（30秒）：自动进入AP配网模式
- 连接断开：自动尝试重连，最多3次，间隔5秒
- AP模式超时（5分钟无配置）：重启设备

### NTP同步错误

- 同步失败：使用上次同步的时间，显示离线指示器
- 重试策略：每5分钟尝试一次同步

### 存储错误

- NVS初始化失败：使用默认配置运行
- 保存失败：记录错误日志，下次启动重试

### 内存不足

- 堆内存低于10KB：记录警告日志
- 堆内存低于5KB：重启设备

## Testing Strategy

### 单元测试

使用Unity测试框架（PlatformIO内置）进行单元测试：

1. **时间格式化测试**
   - 测试边界值：00:00:00, 23:59:59
   - 测试中间值：12:30:45
   - 测试日期格式化

2. **模式切换测试**
   - 测试每个模式的切换
   - 测试循环回到第一个模式

3. **触摸检测测试**
   - 测试短按检测（<500ms）
   - 测试长按检测（>2000ms）
   - 测试工厂重置检测（>10000ms）

4. **配置管理测试**
   - 测试默认值
   - 测试保存和加载
   - 测试工厂重置

### 属性测试

使用自定义属性测试框架（基于随机输入生成）：

1. **Property 1测试**: 生成随机时间值，验证格式化输出
   - 最少100次迭代
   - **Feature: smart-desktop-companion, Property 1: 时间日期格式化正确性**

2. **Property 2测试**: 遍历所有模式，验证切换逻辑
   - **Feature: smart-desktop-companion, Property 2: 显示模式循环切换**

3. **Property 3测试**: 生成随机触摸序列，验证防抖效果
   - 最少100次迭代
   - **Feature: smart-desktop-companion, Property 3: 触摸输入防抖**

4. **Property 4测试**: 生成多个随机眨眼间隔，验证范围
   - 最少100次迭代
   - **Feature: smart-desktop-companion, Property 4: 眨眼间隔范围**

5. **Property 5测试**: 生成随机配置值，验证round-trip
   - 最少100次迭代
   - **Feature: smart-desktop-companion, Property 5: 配置持久化Round-Trip**

6. **Property 6测试**: 生成随机时间增量，验证运行时间计算
   - 最少100次迭代
   - **Feature: smart-desktop-companion, Property 6: 运行时间计算**

### 集成测试

在实际硬件上进行：

1. WiFi连接和NTP同步
2. 触摸传感器响应
3. OLED显示效果
4. 完整的模式切换流程
