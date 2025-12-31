# Implementation Plan: 智能桌面伴侣

## Overview

基于ESP32-C3 Super Mini的智能桌面伴侣实现计划。使用PlatformIO + Arduino框架开发，采用模块化设计逐步实现各个功能组件。

## Tasks

- [x] 1. 项目初始化和硬件配置
  - [x] 1.1 创建PlatformIO项目结构
    - 创建platformio.ini配置文件，配置ESP32-C3开发板
    - 添加依赖库：U8g2、WiFiManager、NTPClient、Preferences
    - 创建src、include、test目录结构
    - _Requirements: 1.1_

  - [x] 1.2 定义硬件引脚和全局配置
    - 创建include/config.h定义I2C引脚（SDA=GPIO8、SCL=GPIO9）
    - 定义触摸传感器GPIO引脚（GPIO2）
    - 定义默认配置常量（亮度、超时时间等）
    - _Requirements: 1.1, 3.1, 8.2_

- [x] 2. 显示模块实现
  - [x] 2.1 实现OLED显示管理器
    - 创建src/DisplayManager.cpp和include/DisplayManager.h
    - 初始化U8g2库（SSD1306_128X64_NONAME，I2C模式）
    - 实现亮度控制功能（setContrast）
    - 实现显示模式枚举（MODE_FACE, MODE_CLOCK, MODE_SYSINFO, MODE_SLEEP）
    - _Requirements: 5.1, 5.2, 5.3, 5.4_

  - [x] 2.2 实现表情渲染器
    - 创建src/FaceRenderer.cpp和include/FaceRenderer.h
    - 定义眼睛位图数据（16x16像素：正常、眨眼帧、看左、看右、睡眠）
    - 定义嘴巴位图数据（32x16像素：微笑、中性、惊讶、说话帧）
    - 实现表情状态切换（EyeState、MouthState枚举）
    - 实现眨眼动画逻辑（3-8秒随机间隔）
    - 实现触摸反应动画
    - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7_

  - [x] 2.3 编写眨眼间隔属性测试

    - **Property 4: 眨眼间隔范围**
    - 验证随机眨眼间隔在3000-8000ms范围内
    - **Validates: Requirements 4.4**

  - [x] 2.4 实现时钟显示渲染器
    - 创建src/ClockRenderer.cpp和include/ClockRenderer.h
    - 实现时间格式化函数（返回HH:MM:SS格式字符串）
    - 实现日期格式化函数（返回YYYY-MM-DD格式字符串）
    - 实现时钟界面渲染（包含迷你表情图标）
    - _Requirements: 2.2, 2.3, 5.3_

  - [ ]* 2.5 编写时间日期格式化属性测试
    - **Property 1: 时间日期格式化正确性**
    - 验证时间格式化输出长度为8，包含两个冒号
    - 验证日期格式化输出长度为10，包含两个连字符
    - **Validates: Requirements 2.2, 2.3**

  - [x] 2.6 实现系统信息渲染器
    - 创建src/SysInfoRenderer.cpp和include/SysInfoRenderer.h
    - 显示空闲堆内存（KB）
    - 显示设备运行时间
    - 显示WiFi信号强度（RSSI）
    - _Requirements: 5.4, 7.1, 7.2, 7.3_

  - [x] 2.7 实现显示模式切换逻辑
    - 在DisplayManager中实现nextMode()方法
    - 实现模式循环：FACE → CLOCK → SYSINFO → FACE
    - 实现模式切换过渡动画
    - _Requirements: 3.1, 3.4, 5.1_

  - [ ]* 2.8 编写显示模式切换属性测试
    - **Property 2: 显示模式循环切换**
    - 验证模式切换顺序正确，最后一个模式切换回第一个
    - **Validates: Requirements 3.1**

- [x] 3. Checkpoint - 显示模块验证
  - 确保所有显示相关测试通过
  - 验证OLED显示效果（如有硬件）
  - 如有问题请提出

- [x] 4. 触摸交互模块实现
  - [x] 4.1 实现触摸管理器
    - 创建src/TouchManager.cpp和include/TouchManager.h
    - 实现GPIO触摸状态读取
    - 实现防抖逻辑（DEBOUNCE_MS = 50ms）
    - _Requirements: 3.3_

  - [ ]* 4.2 编写触摸防抖属性测试
    - **Property 3: 触摸输入防抖**
    - 验证防抖时间窗口内只产生一次有效事件
    - **Validates: Requirements 3.3**

  - [x] 4.3 实现按压时长检测
    - 实现短按检测（pressTime < 500ms）
    - 实现长按检测（pressTime > 2000ms）
    - 实现工厂重置检测（pressTime > 10000ms）
    - _Requirements: 3.1, 3.2, 8.4_

  - [x] 4.4 实现触摸事件回调机制
    - 定义TouchEvent枚举（TOUCH_SHORT, TOUCH_LONG, TOUCH_FACTORY_RESET）
    - 实现回调函数注册（setCallback）
    - 在主程序中连接触摸事件到显示模式切换和表情反应
    - _Requirements: 3.1, 4.7_

- [x] 5. 网络和时间模块实现
  - [x] 5.1 实现WiFi管理器
    - 创建src/WiFiMgr.cpp和include/WiFiMgr.h
    - 实现自动连接已保存的WiFi
    - 实现AP配网模式（连接超时30秒后启动）
    - 实现断线自动重连（最多3次，间隔5秒）
    - 实现连接状态指示器回调
    - _Requirements: 1.1, 1.2, 1.3, 1.4_

  - [x] 5.2 实现时间管理器
    - 创建src/TimeManager.cpp和include/TimeManager.h
    - 实现NTP时间同步（服务器：pool.ntp.org）
    - 配置中国时区（GMT_OFFSET_SEC = 8 * 3600）
    - 实现同步失败时显示离线指示器
    - 实现定期重新同步（每5分钟）
    - _Requirements: 2.1, 2.4_

- [x] 6. 配置和存储模块实现
  - [x] 6.1 实现配置管理器
    - 创建src/ConfigManager.cpp和include/ConfigManager.h
    - 使用Preferences库实现NVS存储
    - 定义DeviceConfig结构体
    - 实现load()和save()方法
    - 实现配置变更后5秒内自动保存
    - _Requirements: 8.1, 8.2, 8.3_

  - [ ]* 6.2 编写配置持久化属性测试
    - **Property 5: 配置持久化Round-Trip**
    - 验证保存后读取得到相同配置值
    - **Validates: Requirements 5.5, 6.4, 8.1, 8.2**

  - [x] 6.3 实现工厂重置功能
    - 清除NVS中所有存储的配置
    - 恢复DEFAULT_CONFIG默认值
    - 重启设备
    - _Requirements: 8.4_

- [x] 7. 系统监控模块实现
  - [x] 7.1 实现系统监控器
    - 创建src/SystemMonitor.cpp和include/SystemMonitor.h
    - 实现getFreeHeap()获取空闲堆内存
    - 实现getUptime()计算运行时间（秒）
    - 实现isLowMemory()检测（阈值10KB）
    - 实现低内存警告日志
    - _Requirements: 7.1, 7.2, 7.3, 7.4_

  - [ ]* 7.2 编写运行时间计算属性测试
    - **Property 6: 运行时间计算**
    - 验证运行时间单调递增，不出现负值或回退
    - **Validates: Requirements 7.3**

- [x] 8. 屏幕保护功能实现
  - [x] 8.1 实现空闲检测
    - 在主程序中跟踪lastTouchTime
    - 实现调暗超时检测（默认5分钟）
    - 实现睡眠超时检测（默认10分钟）
    - 调暗时降低OLED亮度
    - _Requirements: 6.1, 6.2, 6.4_

  - [x] 8.2 实现睡眠和唤醒动画
    - 睡眠时切换到MODE_SLEEP，显示闭眼表情
    - 唤醒时播放伸懒腰动画序列
    - 唤醒后恢复之前的显示模式
    - _Requirements: 6.2, 6.3_

- [x] 9. 主程序集成
  - [x] 9.1 实现主程序框架
    - 创建src/main.cpp
    - 实现setup()初始化所有模块
    - 实现loop()主循环
    - 定义系统状态枚举（BOOT, RUNNING, SLEEP）
    - _Requirements: All_

  - [x] 9.2 连接所有模块交互
    - 触摸短按 → DisplayManager.nextMode()
    - 触摸短按 → FaceRenderer.triggerReaction()
    - 触摸长按 → 进入设置模式（预留）
    - 触摸超长按 → ConfigManager.factoryReset()
    - 空闲超时 → 屏幕保护流程
    - WiFi连接成功 → TimeManager.syncNTP()
    - _Requirements: All_

  - [x] 9.3 实现启动流程
    - 显示启动画面
    - 初始化WiFi并显示连接状态
    - 同步NTP时间
    - 加载上次显示模式（默认FACE模式）
    - _Requirements: 1.2, 5.5, 5.6_

- [x] 10. Final Checkpoint - 完整功能验证
  - 确保所有测试通过
  - 在实际硬件上验证完整功能
  - 如有问题请提出

## Notes

- 标记 `*` 的任务为可选测试任务，可跳过以加快MVP开发
- 每个任务都引用了对应的需求编号以便追溯
- Checkpoint任务用于阶段性验证
- 属性测试验证设计文档中定义的正确性属性
- 使用Unity测试框架（PlatformIO内置）进行单元测试和属性测试
