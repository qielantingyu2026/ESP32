# Requirements Document

## Introduction

智能桌面伴侣是一个基于ESP32-C3 Super Mini的嵌入式项目，配合0.96寸OLED屏幕和TTP223触摸开关，打造一个可爱的公仔形态桌面伴侣。OLED屏幕作为公仔的脸部，可以显示各种表情（眼睛和嘴巴），让公仔具有生命力。同时支持时间显示、系统信息等实用功能，通过触摸切换不同的显示模式。

## Glossary

- **ESP32_Controller**: ESP32-C3 Super Mini主控制器，负责运行主程序、WiFi连接和外设控制
- **OLED_Display**: 0.96寸OLED显示屏，作为公仔脸部显示表情和信息
- **Touch_Sensor**: TTP223触摸传感器，用于检测用户触摸输入
- **Display_Mode**: 显示模式，不同的信息展示界面
- **NTP_Client**: 网络时间协议客户端，用于同步网络时间
- **Face_Expression**: 脸部表情，包含眼睛和嘴巴的组合动画
- **Eye_Animation**: 眼睛动画，包括眨眼、看左右、睡眠等状态
- **Mouth_Animation**: 嘴巴动画，包括微笑、说话、惊讶等状态

## Requirements

### Requirement 1: WiFi连接管理

**User Story:** 作为用户，我希望设备能够自动连接WiFi网络，以便获取网络时间和天气信息。

#### Acceptance Criteria

1. WHEN the ESP32_Controller powers on, THE ESP32_Controller SHALL attempt to connect to the configured WiFi network
2. WHILE the ESP32_Controller is connecting to WiFi, THE OLED_Display SHALL show a connection status indicator
3. IF the WiFi connection fails after 30 seconds, THEN THE ESP32_Controller SHALL enter AP mode for configuration
4. WHEN the WiFi connection is established, THE ESP32_Controller SHALL maintain the connection and auto-reconnect if disconnected

### Requirement 2: 时间显示功能

**User Story:** 作为用户，我希望在屏幕上看到当前时间，以便随时了解时间。

#### Acceptance Criteria

1. WHEN WiFi is connected, THE NTP_Client SHALL synchronize time from NTP server
2. THE OLED_Display SHALL display current time in HH:MM:SS format
3. THE OLED_Display SHALL display current date in YYYY-MM-DD format
4. WHEN NTP sync fails, THE OLED_Display SHALL show the last known time with an offline indicator

### Requirement 3: 触摸交互功能

**User Story:** 作为用户，我希望通过触摸开关切换不同的显示模式，以便查看不同的信息。

#### Acceptance Criteria

1. WHEN the Touch_Sensor detects a short touch (less than 500ms), THE ESP32_Controller SHALL switch to the next Display_Mode
2. WHEN the Touch_Sensor detects a long touch (more than 2 seconds), THE ESP32_Controller SHALL enter settings mode
3. THE ESP32_Controller SHALL debounce touch inputs to prevent false triggers
4. WHEN switching Display_Mode, THE OLED_Display SHALL show a brief transition animation

### Requirement 4: 表情显示功能

**User Story:** 作为用户，我希望公仔能够显示各种可爱的表情，让它看起来有生命力。

#### Acceptance Criteria

1. THE OLED_Display SHALL display Face_Expression consisting of eyes and mouth graphics
2. THE Eye_Animation SHALL support at least 5 states: normal, blink, look_left, look_right, and sleep
3. THE Mouth_Animation SHALL support at least 4 states: smile, neutral, surprised, and talking
4. WHEN in idle state, THE Eye_Animation SHALL perform random blink animation every 3-8 seconds
5. WHEN in idle state, THE Eye_Animation SHALL occasionally look left or right randomly
6. THE Face_Expression SHALL transition smoothly between different states with animation frames
7. WHEN the Touch_Sensor is touched, THE Face_Expression SHALL show a happy or surprised reaction

### Requirement 5: 多模式显示

**User Story:** 作为用户，我希望设备有多种显示模式，以便查看不同类型的信息。

#### Acceptance Criteria

1. THE OLED_Display SHALL support at least 3 Display_Modes: Face, Clock, and System Info
2. WHEN in Face mode, THE OLED_Display SHALL show animated Face_Expression as default mode
3. WHEN in Clock mode, THE OLED_Display SHALL show time and date with a mini face icon
4. WHEN in System Info mode, THE OLED_Display SHALL show device uptime, memory usage, and WiFi signal strength
5. THE ESP32_Controller SHALL remember the last Display_Mode and restore it after power cycle
6. THE Face mode SHALL be the default Display_Mode after power on

### Requirement 6: 屏幕保护功能

**User Story:** 作为用户，我希望设备有屏幕保护功能，以延长OLED屏幕寿命。

#### Acceptance Criteria

1. WHEN no touch input is detected for 5 minutes, THE OLED_Display SHALL dim the brightness
2. WHEN no touch input is detected for 10 minutes, THE Face_Expression SHALL enter sleep state with closed eyes
3. WHEN the Touch_Sensor detects any touch during sleep state, THE Face_Expression SHALL wake up with a stretching animation
4. THE ESP32_Controller SHALL allow configuring screensaver timeout values

### Requirement 7: 系统状态监控

**User Story:** 作为用户，我希望能够了解设备的运行状态，以便确保设备正常工作。

#### Acceptance Criteria

1. THE ESP32_Controller SHALL monitor and report free heap memory
2. THE ESP32_Controller SHALL monitor and report WiFi signal strength (RSSI)
3. THE ESP32_Controller SHALL monitor and report device uptime
4. IF free heap memory falls below 10KB, THEN THE ESP32_Controller SHALL log a warning

### Requirement 8: 配置持久化

**User Story:** 作为用户，我希望设备能够保存我的配置，以便重启后不需要重新设置。

#### Acceptance Criteria

1. THE ESP32_Controller SHALL store WiFi credentials in non-volatile storage
2. THE ESP32_Controller SHALL store user preferences (brightness, timeout values) in non-volatile storage
3. WHEN configuration is changed, THE ESP32_Controller SHALL save it within 5 seconds
4. THE ESP32_Controller SHALL provide factory reset functionality via long touch (10 seconds)
