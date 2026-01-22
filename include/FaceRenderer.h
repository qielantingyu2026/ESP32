/**
 * 智能桌面伴侣 - 表情渲染器
 * 
 * 核心组件，负责渲染公仔的眼睛和嘴巴表情
 */

#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

// 表情结构体
struct Expression {
    EyeState eyes;
    MouthState mouth;
};

// 表情布局配置
struct FaceLayout {
    int16_t leftEyeX, leftEyeY;    // 左眼位置
    int16_t rightEyeX, rightEyeY;  // 右眼位置
    int16_t mouthX, mouthY;        // 嘴巴位置
};

class FaceRenderer {
public:
    FaceRenderer();
    
    /**
     * 初始化表情渲染器
     */
    void init();
    
    /**
     * 渲染表情到显示器
     * @param display U8g2显示对象指针
     */
    void render(U8G2* display);
    
    /**
     * 更新动画状态（在主循环中调用）
     */
    void updateAnimation();
    
    /**
     * 设置完整表情
     * @param expr 表情结构体
     */
    void setExpression(Expression expr);
    
    /**
     * 设置眼睛状态
     * @param state 眼睛状态
     */
    void setEyeState(EyeState state);
    
    /**
     * 设置嘴巴状态
     * @param state 嘴巴状态
     */
    void setMouthState(MouthState state);
    
    /**
     * 获取当前眼睛状态
     */
    EyeState getEyeState() const;
    
    /**
     * 获取当前嘴巴状态
     */
    MouthState getMouthState() const;
    
    /**
     * 触发眨眼动画
     */
    void triggerBlink();
    
    /**
     * 触发触摸反应动画
     */
    void triggerReaction();
    
    /**
     * 进入睡眠状态
     */
    void enterSleep();
    
    /**
     * 唤醒（播放伸懒腰动画）
     */
    void wakeUp();
    
    /**
     * 生成随机眨眼间隔
     * @return 眨眼间隔（毫秒）
     */
    static unsigned long generateBlinkInterval();

private:
    // 当前眼睛状态
    EyeState eyeState;
    
    // 当前嘴巴状态
    MouthState mouthState;
    
    // 眨眼动画相关
    unsigned long lastBlinkTime;
    unsigned long nextBlinkInterval;
    bool isBlinking;
    uint8_t blinkFrame;
    unsigned long blinkStartTime;
    
    // 随机看左右相关
    unsigned long lastLookTime;
    unsigned long nextLookInterval;
    
    // 触摸反应相关
    bool isReacting;
    unsigned long reactionStartTime;
    
    // 唤醒动画相关
    bool isWakingUp;
    unsigned long wakeUpStartTime;
    uint8_t wakeUpFrame;
    
    // 表情布局
    FaceLayout layout;
    
    /**
     * 绘制眼睛
     */
    void drawEyes(U8G2* display);
    
    /**
     * 绘制嘴巴
     */
    void drawMouth(U8G2* display);
    
    /**
     * 绘制腮红
     */
    void drawBlush(U8G2* display);
    
    /**
     * 处理眨眼动画
     */
    void handleBlinkAnimation();
    
    /**
     * 处理随机看左右
     */
    void handleRandomLook();
    
    /**
     * 处理触摸反应动画
     */
    void handleReactionAnimation();
    
    /**
     * 处理唤醒动画
     */
    void handleWakeUpAnimation();
};

#endif // FACE_RENDERER_H
