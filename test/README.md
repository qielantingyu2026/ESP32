# Test 目录

此目录用于存放单元测试和属性测试文件。

## 测试框架

- **C++测试**: Unity测试框架（PlatformIO内置）
- **Python测试**: 用于在没有硬件的情况下验证核心逻辑

## 测试文件命名规范

- `test_<模块名>.cpp` - C++单元测试文件
- `test_property_<属性名>.cpp` - C++属性测试文件
- `test_properties.py` - Python属性测试脚本

## 运行测试

### Python属性测试（推荐，无需硬件）

```bash
# 运行所有属性测试
python test/test_properties.py
```

### PlatformIO测试（需要ESP32硬件或native编译器）

```bash
# 在ESP32硬件上运行测试
pio test -e esp32c3

# 在本地环境运行测试（需要gcc/g++）
pio test -e native

# 运行特定测试
pio test -f test_<模块名>
```

## 属性测试覆盖

| Property | 描述 | 验证需求 |
|----------|------|----------|
| Property 1 | 时间日期格式化正确性 | 2.2, 2.3 |
| Property 2 | 显示模式循环切换 | 3.1 |
| Property 3 | 触摸输入防抖 | 3.3 |
| Property 4 | 眨眼间隔范围 | 4.4 |
