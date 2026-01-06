/**
 * 智能桌面伴侣 - AI 服务管理器
 * 
 * 集成语音识别(ASR)、语音合成(TTS)、AI大模型对话
 * 支持百度、讯飞、OpenAI等多平台
 */

#ifndef AI_SERVICE_H
#define AI_SERVICE_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "config.h"

// AI 平台枚举
enum AIPlatform {
    AI_BAIDU_ERNIE,     // 百度文心一言
    AI_ALIYUN_QWEN,     // 阿里通义千问
    AI_XFYUN_SPARK,     // 讯飞星火
    AI_OPENAI_GPT,      // OpenAI GPT
    AI_ZHIPU_GLM        // 智谱 ChatGLM
};

// ASR 平台枚举
enum ASRPlatform {
    ASR_BAIDU,          // 百度语音识别
    ASR_XFYUN,          // 讯飞语音识别
    ASR_ALIYUN          // 阿里语音识别
};

// AI 服务状态
enum AIState {
    AI_IDLE,            // 空闲
    AI_RECORDING,       // 录音中
    AI_RECOGNIZING,     // 语音识别中
    AI_THINKING,        // AI 思考中
    AI_SPEAKING,        // 语音播放中
    AI_ERROR            // 错误状态
};

// AI 配置结构体
struct AIConfig {
    // 百度 API 配置
    String baiduApiKey;
    String baiduSecretKey;
    String baiduAccessToken;
    unsigned long tokenExpireTime;
    
    // 其他平台配置 (按需添加)
    String openaiApiKey;
    String qwenApiKey;
    String sparkApiKey;
    
    // 当前使用的平台
    AIPlatform aiPlatform;
    ASRPlatform asrPlatform;
};

class AIService {
public:
    AIService();
    
    /**
     * 初始化 AI 服务
     */
    bool begin();
    
    /**
     * 设置百度 API 密钥
     */
    void setBaiduCredentials(const String& apiKey, const String& secretKey);
    
    /**
     * 设置 OpenAI API 密钥
     */
    void setOpenAIKey(const String& apiKey);
    
    /**
     * 设置当前 AI 平台
     */
    void setAIPlatform(AIPlatform platform);
    
    /**
     * 设置当前 ASR 平台
     */
    void setASRPlatform(ASRPlatform platform);
    
    /**
     * 语音识别 - 将音频数据转为文字
     * @param audioData PCM 音频数据
     * @param audioLen 数据长度
     * @return 识别结果文字
     */
    String speechToText(const uint8_t* audioData, size_t audioLen);
    
    /**
     * AI 对话 - 发送文字获取回复
     * @param message 用户消息
     * @return AI 回复
     */
    String chat(const String& message);
    
    /**
     * 语音合成 - 将文字转为音频
     * @param text 要合成的文字
     * @param audioBuffer 输出音频缓冲区
     * @param maxLen 缓冲区最大长度
     * @return 实际音频数据长度
     */
    size_t textToSpeech(const String& text, uint8_t* audioBuffer, size_t maxLen);
    
    /**
     * 获取当前状态
     */
    AIState getState() const { return state; }
    
    /**
     * 获取最后的错误信息
     */
    String getLastError() const { return lastError; }
    
    /**
     * 清除对话历史
     */
    void clearHistory();

private:
    AIConfig config;
    AIState state;
    String lastError;
    String conversationHistory;  // 简单的对话历史
    
    /**
     * 获取百度 Access Token
     */
    bool refreshBaiduToken();
    
    /**
     * 百度语音识别
     */
    String baiduASR(const uint8_t* audioData, size_t audioLen);
    
    /**
     * 百度文心一言对话
     */
    String baiduErnieChat(const String& message);
    
    /**
     * OpenAI GPT 对话
     */
    String openaiChat(const String& message);
    
    /**
     * 百度语音合成
     */
    size_t baiduTTS(const String& text, uint8_t* audioBuffer, size_t maxLen);
    
    /**
     * Base64 编码
     */
    String base64Encode(const uint8_t* data, size_t len);
    
    /**
     * URL 编码
     */
    String urlEncode(const String& str);
};

#endif // AI_SERVICE_H
