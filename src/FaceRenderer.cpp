/**
 * 智能桌面伴侣 - 表情渲染器实现
 */

#include "FaceRenderer.h"

// ============================================================================
// 眼睛位图数据 (16x16 像素，每只眼睛)
// ============================================================================

// 正常睁眼
const uint8_t EYE_NORMAL_BITMAP[] PROGMEM = {
    0x00, 0x00, 0xC0, 0x03, 0x30, 0x0C, 0x08, 0x10,
    0x04, 0x20, 0xE4, 0x27, 0xE2, 0x47, 0xE2, 0x47,
    0xE2, 0x47, 0xE4, 0x27, 0x04, 0x20, 0x08, 0x10,
    0x30, 0x0C, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00
};

// 眨眼帧1（半闭）
const uint8_t EYE_BLINK_1[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03,
    0x30, 0x0C, 0x08, 0x10, 0xE4, 0x27, 0xE2, 0x47,
    0xE2, 0x47, 0xE4, 0x27, 0x08, 0x10, 0x30, 0x0C,
    0xC0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 眨眼帧2（更闭）
const uint8_t EYE_BLINK_2[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC0, 0x03, 0x30, 0x0C, 0xE8, 0x17,
    0xE8, 0x17, 0x30, 0x0C, 0xC0, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 眨眼帧3（闭眼）/ 睡眠闭眼
const uint8_t EYE_CLOSED[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F,
    0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 看左（瞳孔偏左）
const uint8_t EYE_LOOK_LEFT_BITMAP[] PROGMEM = {
    0x00, 0x00, 0xC0, 0x03, 0x30, 0x0C, 0x08, 0x10,
    0x04, 0x20, 0x9C, 0x23, 0x9E, 0x43, 0x9E, 0x43,
    0x9E, 0x43, 0x9C, 0x23, 0x04, 0x20, 0x08, 0x10,
    0x30, 0x0C, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00
};

// 看右（瞳孔偏右）
const uint8_t EYE_LOOK_RIGHT_BITMAP[] PROGMEM = {
    0x00, 0x00, 0xC0, 0x03, 0x30, 0x0C, 0x08, 0x10,
    0x04, 0x20, 0xC4, 0x39, 0xC2, 0x79, 0xC2, 0x79,
    0xC2, 0x79, 0xC4, 0x39, 0x04, 0x20, 0x08, 0x10,
    0x30, 0x0C, 0xC0, 0x03, 0x00, 0x00, 0x00, 0x00
};

// ============================================================================
// 嘴巴位图数据 (32x16 像素)
// ============================================================================

// 微笑
const uint8_t MOUTH_SMILE_BITMAP[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x60,
    0x0C, 0x00, 0x00, 0x30, 0x18, 0x00, 0x00, 0x18,
    0x30, 0x00, 0x00, 0x0C, 0xE0, 0x00, 0x00, 0x07,
    0xC0, 0x03, 0xC0, 0x03, 0x00, 0xFC, 0x3F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 中性
const uint8_t MOUTH_NEUTRAL_BITMAP[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFC, 0x3F, 0x00, 0x00, 0xFC, 0x3F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 惊讶（O形嘴）
const uint8_t MOUTH_SURPRISED_BITMAP[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00,
    0x00, 0x0C, 0x30, 0x00, 0x00, 0x02, 0x40, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00,
    0x00, 0x02, 0x40, 0x00, 0x00, 0x0C, 0x30, 0x00,
    0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 说话帧1
const uint8_t MOUTH_TALKING_1[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x00,
    0x00, 0x18, 0x18, 0x00, 0x00, 0x04, 0x20, 0x00,
    0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
    0x00, 0x18, 0x18, 0x00, 0x00, 0xE0, 0x07, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 说话帧2
const uint8_t MOUTH_TALKING_2[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x1F, 0x00, 0x00, 0x04, 0x20, 0x00,
    0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00,
    0x00, 0x04, 0x20, 0x00, 0x00, 0xF8, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// ============================================================================
// 默认表情布局
// ============================================================================
const FaceLayout DEFAULT_LAYOUT = {
    .leftEyeX = 24,
    .leftEyeY = 8,
    .rightEyeX = 88,
    .rightEyeY = 8,
    .mouthX = 48,
    .mouthY = 40
};

// ============================================================================
// FaceRenderer 实现
// ============================================================================

FaceRenderer::FaceRenderer()
    : eyeState(EYE_NORMAL)
    , mouthState(MOUTH_SMILE)
    , lastBlinkTime(0)
    , nextBlinkInterval(5000)
    , isBlinking(false)
    , blinkFrame(0)
    , blinkStartTime(0)
    , lastLookTime(0)
    , nextLookInterval(10000)
    , isReacting(false)
    , reactionStartTime(0)
    , isWakingUp(false)
    , wakeUpStartTime(0)
    , wakeUpFrame(0)
    , layout(DEFAULT_LAYOUT) {
}

void FaceRenderer::init() {
    // 初始化随机种子
    randomSeed(analogRead(0) + millis());
    
    // 设置初始眨眼间隔
    nextBlinkInterval = generateBlinkInterval();
    lastBlinkTime = millis();
    
    // 设置初始看左右间隔
    nextLookInterval = random(8000, 15000);
    lastLookTime = millis();
}

unsigned long FaceRenderer::generateBlinkInterval() {
    // 生成3000-8000ms范围内的随机间隔
    return random(BLINK_INTERVAL_MIN_MS, BLINK_INTERVAL_MAX_MS + 1);
}

void FaceRenderer::render(U8G2* display) {
    if (display == nullptr) return;
    
    // 绘制眼睛
    drawEyes(display);
    
    // 绘制嘴巴
    drawMouth(display);
}

void FaceRenderer::updateAnimation() {
    // 如果在睡眠状态，不执行动画更新
    if (eyeState == EYE_SLEEP && !isWakingUp) {
        return;
    }
    
    // 处理唤醒动画
    if (isWakingUp) {
        handleWakeUpAnimation();
        return;
    }
    
    // 处理触摸反应动画
    if (isReacting) {
        handleReactionAnimation();
        return;
    }
    
    // 处理眨眼动画
    handleBlinkAnimation();
    
    // 处理随机看左右
    if (!isBlinking) {
        handleRandomLook();
    }
}

void FaceRenderer::setExpression(Expression expr) {
    eyeState = expr.eyes;
    mouthState = expr.mouth;
}

void FaceRenderer::setEyeState(EyeState state) {
    eyeState = state;
}

void FaceRenderer::setMouthState(MouthState state) {
    mouthState = state;
}

EyeState FaceRenderer::getEyeState() const {
    return eyeState;
}

MouthState FaceRenderer::getMouthState() const {
    return mouthState;
}

void FaceRenderer::triggerBlink() {
    if (!isBlinking && eyeState != EYE_SLEEP) {
        isBlinking = true;
        blinkFrame = 0;
        blinkStartTime = millis();
    }
}

void FaceRenderer::triggerReaction() {
    if (!isReacting && eyeState != EYE_SLEEP) {
        isReacting = true;
        reactionStartTime = millis();
        // 显示惊讶表情
        mouthState = MOUTH_SURPRISED;
    }
}

void FaceRenderer::enterSleep() {
    eyeState = EYE_SLEEP;
    mouthState = MOUTH_NEUTRAL;
    isBlinking = false;
    isReacting = false;
}

void FaceRenderer::wakeUp() {
    isWakingUp = true;
    wakeUpStartTime = millis();
    wakeUpFrame = 0;
}

void FaceRenderer::drawEyes(U8G2* display) {
    const uint8_t* eyeBitmap = nullptr;
    
    // 根据当前状态选择眼睛位图
    if (isBlinking) {
        // 眨眼动画帧
        switch (blinkFrame) {
            case 0:
                eyeBitmap = EYE_BLINK_1;
                break;
            case 1:
                eyeBitmap = EYE_BLINK_2;
                break;
            case 2:
                eyeBitmap = EYE_CLOSED;
                break;
            case 3:
                eyeBitmap = EYE_BLINK_2;
                break;
            case 4:
                eyeBitmap = EYE_BLINK_1;
                break;
            default:
                eyeBitmap = EYE_NORMAL_BITMAP;
                break;
        }
    } else {
        switch (eyeState) {
            case EYE_NORMAL:
                eyeBitmap = EYE_NORMAL_BITMAP;
                break;
            case EYE_BLINK:
                eyeBitmap = EYE_CLOSED;
                break;
            case EYE_LOOK_LEFT:
                eyeBitmap = EYE_LOOK_LEFT_BITMAP;
                break;
            case EYE_LOOK_RIGHT:
                eyeBitmap = EYE_LOOK_RIGHT_BITMAP;
                break;
            case EYE_SLEEP:
                eyeBitmap = EYE_CLOSED;
                break;
            default:
                eyeBitmap = EYE_NORMAL_BITMAP;
                break;
        }
    }
    
    // 绘制左眼
    display->drawXBMP(layout.leftEyeX, layout.leftEyeY, 16, 16, eyeBitmap);
    
    // 绘制右眼
    display->drawXBMP(layout.rightEyeX, layout.rightEyeY, 16, 16, eyeBitmap);
}

void FaceRenderer::drawMouth(U8G2* display) {
    const uint8_t* mouthBitmap = nullptr;
    
    // 根据当前状态选择嘴巴位图
    switch (mouthState) {
        case MOUTH_SMILE:
            mouthBitmap = MOUTH_SMILE_BITMAP;
            break;
        case MOUTH_NEUTRAL:
            mouthBitmap = MOUTH_NEUTRAL_BITMAP;
            break;
        case MOUTH_SURPRISED:
            mouthBitmap = MOUTH_SURPRISED_BITMAP;
            break;
        case MOUTH_TALKING:
            // 说话动画：交替显示两帧
            if ((millis() / 200) % 2 == 0) {
                mouthBitmap = MOUTH_TALKING_1;
            } else {
                mouthBitmap = MOUTH_TALKING_2;
            }
            break;
        default:
            mouthBitmap = MOUTH_SMILE_BITMAP;
            break;
    }
    
    // 绘制嘴巴
    display->drawXBMP(layout.mouthX, layout.mouthY, 32, 16, mouthBitmap);
}

void FaceRenderer::handleBlinkAnimation() {
    unsigned long currentTime = millis();
    
    if (isBlinking) {
        // 眨眼动画进行中
        unsigned long elapsed = currentTime - blinkStartTime;
        uint8_t newFrame = elapsed / (BLINK_DURATION_MS / 5);
        
        if (newFrame != blinkFrame) {
            blinkFrame = newFrame;
        }
        
        // 眨眼动画结束
        if (elapsed >= BLINK_DURATION_MS) {
            isBlinking = false;
            blinkFrame = 0;
            lastBlinkTime = currentTime;
            nextBlinkInterval = generateBlinkInterval();
        }
    } else {
        // 检查是否需要触发眨眼
        if (currentTime - lastBlinkTime >= nextBlinkInterval) {
            triggerBlink();
        }
    }
}

void FaceRenderer::handleRandomLook() {
    unsigned long currentTime = millis();
    
    // 如果眼睛正在看左或看右，检查是否需要恢复正常
    if (eyeState == EYE_LOOK_LEFT || eyeState == EYE_LOOK_RIGHT) {
        if (currentTime - lastLookTime >= 1000) {  // 看1秒后恢复
            eyeState = EYE_NORMAL;
            lastLookTime = currentTime;
            nextLookInterval = random(8000, 15000);
        }
        return;
    }
    
    // 检查是否需要随机看左右
    if (currentTime - lastLookTime >= nextLookInterval) {
        // 随机决定看左还是看右
        if (random(2) == 0) {
            eyeState = EYE_LOOK_LEFT;
        } else {
            eyeState = EYE_LOOK_RIGHT;
        }
        lastLookTime = currentTime;
    }
}

void FaceRenderer::handleReactionAnimation() {
    unsigned long elapsed = millis() - reactionStartTime;
    
    // 反应动画持续500ms
    if (elapsed >= 500) {
        isReacting = false;
        mouthState = MOUTH_SMILE;  // 恢复微笑
    }
}

void FaceRenderer::handleWakeUpAnimation() {
    unsigned long elapsed = millis() - wakeUpStartTime;
    
    // 唤醒动画分为几个阶段，总共1.5秒
    if (elapsed < 300) {
        // 阶段1：眼睛半开
        eyeState = EYE_BLINK;
        mouthState = MOUTH_NEUTRAL;
    } else if (elapsed < 600) {
        // 阶段2：眼睛睁开，打哈欠
        eyeState = EYE_NORMAL;
        mouthState = MOUTH_SURPRISED;
    } else if (elapsed < 1000) {
        // 阶段3：伸懒腰（眼睛闭上）
        eyeState = EYE_BLINK;
        mouthState = MOUTH_TALKING;
    } else if (elapsed < 1500) {
        // 阶段4：恢复正常
        eyeState = EYE_NORMAL;
        mouthState = MOUTH_SMILE;
    } else {
        // 动画结束
        isWakingUp = false;
        eyeState = EYE_NORMAL;
        mouthState = MOUTH_SMILE;
        
        // 重置眨眼计时器
        lastBlinkTime = millis();
        nextBlinkInterval = generateBlinkInterval();
    }
}
