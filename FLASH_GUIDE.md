# ESP32-C3 Super Mini 烧录指南

## 硬件准备

### 所需材料
- ESP32-C3 Super Mini 开发板
- USB Type-C 数据线（确保是数据线，不是仅充电线）
- 0.96寸 OLED 显示屏 (SSD1306/SSD1315, I2C接口)
- TTP223 触摸传感器模块

### 接线说明

| ESP32-C3 引脚 | 连接设备 | 说明 |
|--------------|---------|------|
| GPIO8 (SDA)  | OLED SDA | I2C 数据线 |
| GPIO9 (SCL)  | OLED SCL | I2C 时钟线 |
| GPIO2        | TTP223 OUT | 触摸传感器输出 |
| 3.3V         | OLED VCC, TTP223 VCC | 电源正极 |
| GND          | OLED GND, TTP223 GND | 电源负极 |

## 烧录方法

### 方法一：使用 PlatformIO（推荐）

1. **安装 PlatformIO**
   ```bash
   pip install platformio
   ```

2. **连接开发板**
   - 用 USB 线连接 ESP32-C3 Super Mini 到电脑
   - 首次连接可能需要安装 USB 驱动（CH340 或 CP2102）

3. **进入下载模式**（如果自动烧录失败）
   - 按住 BOOT 按钮
   - 按一下 RST 按钮
   - 松开 BOOT 按钮

4. **烧录固件**
   ```bash
   # 在项目目录下执行
   python -m platformio run -e esp32c3 --target upload
   ```

5. **查看串口输出**
   ```bash
   python -m platformio device monitor -b 115200
   ```

### 方法二：使用 esptool.py 手动烧录

1. **安装 esptool**
   ```bash
   pip install esptool
   ```

2. **查找串口**
   - Windows: 设备管理器 → 端口 (COM和LPT) → 找到 COM 端口号
   - Linux: `ls /dev/ttyUSB*` 或 `ls /dev/ttyACM*`
   - macOS: `ls /dev/cu.*`

3. **烧录命令**
   ```bash
   # Windows 示例 (替换 COM3 为实际端口)
   esptool.py --chip esp32c3 --port COM3 --baud 921600 \
     --before default_reset --after hard_reset write_flash \
     -z --flash_mode dio --flash_freq 80m --flash_size 4MB \
     0x0 .pio/build/esp32c3/bootloader.bin \
     0x8000 .pio/build/esp32c3/partitions.bin \
     0x10000 .pio/build/esp32c3/firmware.bin
   ```

### 方法三：使用 ESP Flash Download Tool（Windows 图形界面）

1. 下载 [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)

2. 打开工具，选择 ESP32-C3

3. 配置烧录文件：
   | 文件路径 | 地址 |
   |---------|------|
   | `.pio/build/esp32c3/bootloader.bin` | 0x0 |
   | `.pio/build/esp32c3/partitions.bin` | 0x8000 |
   | `.pio/build/esp32c3/firmware.bin` | 0x10000 |

4. 选择正确的 COM 端口，点击 START

## 首次使用

1. **WiFi 配网**
   - 首次启动或 WiFi 连接失败 30 秒后，设备会进入 AP 配网模式
   - 用手机搜索名为 `SmartCompanion` 的 WiFi 热点
   - 连接后会自动弹出配网页面
   - 选择你的 WiFi 并输入密码

2. **操作说明**
   - **短按触摸**：切换显示模式（表情 → 时钟 → 系统信息 → 表情）
   - **长按 2 秒**：触发表情反应
   - **长按 10 秒**：工厂重置（清除所有配置）

3. **屏幕保护**
   - 5 分钟无操作：屏幕自动调暗
   - 10 分钟无操作：进入睡眠模式（闭眼表情）
   - 触摸唤醒：播放伸懒腰动画

## 常见问题

### Q: 烧录失败，提示找不到设备
A: 
1. 检查 USB 线是否为数据线
2. 安装 CH340 或 CP2102 驱动
3. 尝试进入下载模式（按住 BOOT + 按 RST）

### Q: 烧录成功但屏幕不亮
A:
1. 检查 I2C 接线是否正确
2. 确认 OLED 的 I2C 地址是 0x3C
3. 检查电源连接

### Q: WiFi 连接失败
A:
1. 确保 WiFi 是 2.4GHz（不支持 5GHz）
2. 检查 WiFi 密码是否正确
3. 长按 10 秒重置后重新配网

## 固件文件位置

编译成功后，固件文件位于：
```
.pio/build/esp32c3/
├── bootloader.bin    # 引导程序
├── partitions.bin    # 分区表
└── firmware.bin      # 主程序
```
