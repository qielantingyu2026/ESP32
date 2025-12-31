# KKar Web 烧录工具

基于 ESP Web Tools 的在线固件烧录工具，让用户无需安装任何软件即可烧录 KKar 智能桌面伴侣固件。

## 功能特点

- 🌐 纯网页操作，无需安装软件
- 🔌 支持 Chrome / Edge 浏览器
- 📦 一键烧录完整固件
- 🔄 自动重启设备

## 使用方法

### 在线使用

1. 使用 Chrome 或 Edge 浏览器打开烧录页面
2. 用 USB 数据线连接 ESP32-C3 开发板
3. 点击「连接并烧录」按钮
4. 选择对应的串口
5. 等待烧录完成

### 本地部署

由于 Web Serial API 需要 HTTPS，本地测试需要启动 HTTPS 服务器：

```bash
# 方法1：使用 Python（需要生成证书）
python -m http.server 8000

# 方法2：使用 VS Code Live Server 插件

# 方法3：部署到 GitHub Pages（推荐）
```

## 部署到 GitHub Pages

1. 将 `kkar-flasher` 目录推送到 GitHub 仓库
2. 进入仓库 Settings → Pages
3. Source 选择 `main` 分支，目录选择 `/kkar-flasher`
4. 保存后等待部署完成
5. 访问 `https://你的用户名.github.io/仓库名/kkar-flasher/`

## 文件结构

```
kkar-flasher/
├── index.html          # 主页面
├── style.css           # 样式文件
├── manifest.json       # ESP Web Tools 配置
├── firmware/
│   └── kkar_v1.0.0.bin # 合并后的完整固件
├── assets/
│   └── logo.svg        # KKar Logo
└── README.md           # 说明文档
```

## 更新固件

当需要更新固件版本时：

1. 在主项目中编译新固件：
   ```bash
   python -m platformio run -e esp32c3
   ```

2. 合并固件文件：
   ```bash
   python -m platformio pkg exec -- esptool.py --chip esp32c3 merge_bin \
     -o kkar-flasher/firmware/kkar_vX.X.X.bin \
     --flash_mode dio --flash_size 4MB \
     0x0 .pio/build/esp32c3/bootloader.bin \
     0x8000 .pio/build/esp32c3/partitions.bin \
     0x10000 .pio/build/esp32c3/firmware.bin
   ```

3. 更新 `manifest.json` 中的版本号和文件路径

## 浏览器兼容性

| 浏览器 | 支持 |
|--------|------|
| Chrome 89+ | ✅ |
| Edge 89+ | ✅ |
| Opera 75+ | ✅ |
| Firefox | ❌ |
| Safari | ❌ |

## 硬件要求

- ESP32-C3 Super Mini 开发板
- 0.96寸 OLED 显示屏 (SSD1306, I2C)
- TTP223 触摸传感器
- USB Type-C 数据线

## 技术栈

- [ESP Web Tools](https://esphome.github.io/esp-web-tools/) - Web Serial 烧录库
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API) - 浏览器串口通信

## 许可证

MIT License
