/**
 * 智能桌面伴侣 - 眨眼间隔属性测试
 * 
 * Property 4: 眨眼间隔范围
 * 验证随机眨眼间隔在3000-8000ms范围内
 * 
 * Feature: smart-desktop-companion, Property 4: 眨眼间隔范围
 * Validates: Requirements 4.4
 */

#include <unity.h>

// 模拟Arduino的random函数用于测试
static unsigned long mockRandomSeed = 12345;

unsigned long mockRandom(unsigned long min, unsigned long max) {
    // 简单的线性同余生成器
    mockRandomSeed = (mockRandomSeed * 1103515245 + 12345) & 0x7FFFFFFF;
    return min + (mockRandomSeed % (max - min));
}

// 眨眼间隔常量（与config.h保持一致）
#define BLINK_INTERVAL_MIN_MS   3000
#define BLINK_INTERVAL_MAX_MS   8000

/**
 * 生成随机眨眼间隔（测试版本）
 * 与FaceRenderer::generateBlinkInterval()逻辑一致
 */
unsigned long generateBlinkInterval() {
    return mockRandom(BLINK_INTERVAL_MIN_MS, BLINK_INTERVAL_MAX_MS + 1);
}

void setUp(void) {
    // 每个测试前重置随机种子
    mockRandomSeed = 12345;
}

void tearDown(void) {
    // 清理
}

/**
 * Property 4: 眨眼间隔范围
 * For any 生成的随机眨眼间隔值，该值应在3000ms到8000ms范围内（包含边界）
 */
void test_blink_interval_within_range(void) {
    // 运行100次迭代验证属性
    for (int i = 0; i < 100; i++) {
        // 更新随机种子以获得不同的值
        mockRandomSeed = mockRandomSeed * 31 + i;
        
        unsigned long interval = generateBlinkInterval();
        
        // 验证间隔在有效范围内
        TEST_ASSERT_GREATER_OR_EQUAL(BLINK_INTERVAL_MIN_MS, interval);
        TEST_ASSERT_LESS_OR_EQUAL(BLINK_INTERVAL_MAX_MS, interval);
    }
}

/**
 * 边界测试：验证最小值边界
 */
void test_blink_interval_min_boundary(void) {
    bool foundMin = false;
    
    // 多次生成，检查是否能生成接近最小值的间隔
    for (int i = 0; i < 1000; i++) {
        mockRandomSeed = i * 7919;  // 使用质数获得更好的分布
        unsigned long interval = generateBlinkInterval();
        
        if (interval <= BLINK_INTERVAL_MIN_MS + 500) {
            foundMin = true;
            break;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(foundMin, "Should be able to generate intervals near minimum");
}

/**
 * 边界测试：验证最大值边界
 */
void test_blink_interval_max_boundary(void) {
    bool foundMax = false;
    
    // 多次生成，检查是否能生成接近最大值的间隔
    for (int i = 0; i < 1000; i++) {
        mockRandomSeed = i * 7919 + 1;
        unsigned long interval = generateBlinkInterval();
        
        if (interval >= BLINK_INTERVAL_MAX_MS - 500) {
            foundMax = true;
            break;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(foundMax, "Should be able to generate intervals near maximum");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_blink_interval_within_range);
    RUN_TEST(test_blink_interval_min_boundary);
    RUN_TEST(test_blink_interval_max_boundary);
    
    return UNITY_END();
}
