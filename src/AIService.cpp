/**
 * 智能桌面伴侣 - AI 服务管理器实现
 */

#include "AIService.h"
#include <ArduinoJson.h>
#include <base64.h>

AIService::AIService() 
    : state(AI_IDLE)
    , lastError("") {
    config.aiPlatform = AI_BAIDU_ERNIE;
    config.asrPlatform = ASR_BAIDU;
    config.tokenExpireTime = 0;
}

bool AIService::begin() {
    Serial.println("AI 服务初始化...");
    
    // 检查 WiFi 连接
    if (WiFi.status() != WL_CONNECTED) {
        lastError = "WiFi 未连接";
        return false;
    }
    
    Serial.println("AI 服务初始化成功");
    return true;
}

void AIService::setBaiduCredentials(const String& apiKey, const String& secretKey) {
    config.baiduApiKey = apiKey;
    config.baiduSecretKey = secretKey;
    config.baiduAccessToken = "";
    config.tokenExpireTime = 0;
}

void AIService::setOpenAIKey(const String& apiKey) {
    config.openaiApiKey = apiKey;
}

void AIService::setAIPlatform(AIPlatform platform) {
    config.aiPlatform = platform;
}

void AIService::setASRPlatform(ASRPlatform platform) {
    config.asrPlatform = platform;
}

bool AIService::refreshBaiduToken() {
    // 检查 Token 是否有效
    if (config.baiduAccessToken.length() > 0 && millis() < config.tokenExpireTime) {
        return true;
    }
    
    Serial.println("刷新百度 Access Token...");
    
    HTTPClient http;
    String url = String(BAIDU_TOKEN_URL) + 
                 "?grant_type=client_credentials" +
                 "&client_id=" + config.baiduApiKey +
                 "&client_secret=" + config.baiduSecretKey;
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        
        // 解析 JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);
        
        if (!error && doc["access_token"].is<const char*>()) {
            config.baiduAccessToken = doc["access_token"].as<String>();
            int expiresIn = doc["expires_in"] | 2592000;  // 默认30天
            config.tokenExpireTime = millis() + (expiresIn - 3600) * 1000UL;  // 提前1小时刷新
            
            Serial.println("Token 获取成功");
            http.end();
            return true;
        }
    }
    
    lastError = "获取百度 Token 失败";
    Serial.println(lastError);
    http.end();
    return false;
}

String AIService::speechToText(const uint8_t* audioData, size_t audioLen) {
    state = AI_RECOGNIZING;
    
    switch (config.asrPlatform) {
        case ASR_BAIDU:
            return baiduASR(audioData, audioLen);
        default:
            lastError = "不支持的 ASR 平台";
            state = AI_ERROR;
            return "";
    }
}

String AIService::baiduASR(const uint8_t* audioData, size_t audioLen) {
    if (!refreshBaiduToken()) {
        state = AI_ERROR;
        return "";
    }
    
    Serial.printf("百度语音识别，音频大小: %d 字节\n", audioLen);
    
    HTTPClient http;
    http.begin(BAIDU_ASR_URL);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(15000);
    
    // 构建请求 JSON
    JsonDocument doc;
    doc["format"] = "pcm";
    doc["rate"] = MIC_SAMPLE_RATE;
    doc["channel"] = 1;
    doc["cuid"] = WiFi.macAddress();
    doc["token"] = config.baiduAccessToken;
    doc["speech"] = base64Encode(audioData, audioLen);
    doc["len"] = audioLen;
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    int httpCode = http.POST(requestBody);
    
    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response);
        
        if (!error && respDoc["err_no"] == 0) {
            String result = respDoc["result"][0].as<String>();
            Serial.printf("识别结果: %s\n", result.c_str());
            state = AI_IDLE;
            http.end();
            return result;
        } else {
            lastError = "ASR 识别失败: " + String(respDoc["err_msg"] | "未知错误");
        }
    } else {
        lastError = "ASR 请求失败: " + String(httpCode);
    }
    
    Serial.println(lastError);
    state = AI_ERROR;
    http.end();
    return "";
}

String AIService::chat(const String& message) {
    state = AI_THINKING;
    
    switch (config.aiPlatform) {
        case AI_BAIDU_ERNIE:
            return baiduErnieChat(message);
        case AI_OPENAI_GPT:
            return openaiChat(message);
        default:
            lastError = "不支持的 AI 平台";
            state = AI_ERROR;
            return "";
    }
}

String AIService::baiduErnieChat(const String& message) {
    if (!refreshBaiduToken()) {
        state = AI_ERROR;
        return "";
    }
    
    Serial.printf("文心一言对话: %s\n", message.c_str());
    
    HTTPClient http;
    // 使用 ERNIE-Speed 模型（免费）
    String url = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k?access_token=" + config.baiduAccessToken;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(AI_TIMEOUT_MS);
    
    // 构建请求
    JsonDocument doc;
    JsonArray messages = doc["messages"].to<JsonArray>();
    
    // 添加系统提示
    JsonObject sysMsg = messages.add<JsonObject>();
    sysMsg["role"] = "user";
    sysMsg["content"] = "你是 KKar，一个可爱的智能桌面伴侣。请用简短、友好的方式回答，每次回复不超过50个字。";
    
    JsonObject sysReply = messages.add<JsonObject>();
    sysReply["role"] = "assistant";
    sysReply["content"] = "好的，我是 KKar，你的桌面小伙伴！有什么我能帮你的吗？";
    
    // 添加用户消息
    JsonObject userMsg = messages.add<JsonObject>();
    userMsg["role"] = "user";
    userMsg["content"] = message;
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    int httpCode = http.POST(requestBody);
    
    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response);
        
        if (!error && respDoc["result"].is<const char*>()) {
            String result = respDoc["result"].as<String>();
            Serial.printf("AI 回复: %s\n", result.c_str());
            state = AI_IDLE;
            http.end();
            return result;
        } else if (respDoc["error_msg"].is<const char*>()) {
            lastError = "AI 错误: " + respDoc["error_msg"].as<String>();
        }
    } else {
        lastError = "AI 请求失败: " + String(httpCode);
    }
    
    Serial.println(lastError);
    state = AI_ERROR;
    http.end();
    return "";
}

String AIService::openaiChat(const String& message) {
    if (config.openaiApiKey.length() == 0) {
        lastError = "OpenAI API Key 未设置";
        state = AI_ERROR;
        return "";
    }
    
    Serial.printf("OpenAI 对话: %s\n", message.c_str());
    
    HTTPClient http;
    http.begin("https://api.openai.com/v1/chat/completions");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + config.openaiApiKey);
    http.setTimeout(AI_TIMEOUT_MS);
    
    // 构建请求
    JsonDocument doc;
    doc["model"] = "gpt-3.5-turbo";
    doc["max_tokens"] = 100;
    
    JsonArray messages = doc["messages"].to<JsonArray>();
    
    JsonObject sysMsg = messages.add<JsonObject>();
    sysMsg["role"] = "system";
    sysMsg["content"] = "你是 KKar，一个可爱的智能桌面伴侣。请用简短、友好的方式回答，每次回复不超过50个字。";
    
    JsonObject userMsg = messages.add<JsonObject>();
    userMsg["role"] = "user";
    userMsg["content"] = message;
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    int httpCode = http.POST(requestBody);
    
    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response);
        
        if (!error) {
            String result = respDoc["choices"][0]["message"]["content"].as<String>();
            Serial.printf("AI 回复: %s\n", result.c_str());
            state = AI_IDLE;
            http.end();
            return result;
        }
    } else {
        lastError = "OpenAI 请求失败: " + String(httpCode);
    }
    
    Serial.println(lastError);
    state = AI_ERROR;
    http.end();
    return "";
}

size_t AIService::textToSpeech(const String& text, uint8_t* audioBuffer, size_t maxLen) {
    state = AI_SPEAKING;
    return baiduTTS(text, audioBuffer, maxLen);
}

size_t AIService::baiduTTS(const String& text, uint8_t* audioBuffer, size_t maxLen) {
    if (!refreshBaiduToken()) {
        state = AI_ERROR;
        return 0;
    }
    
    Serial.printf("百度语音合成: %s\n", text.c_str());
    
    HTTPClient http;
    String url = String(BAIDU_TTS_URL) +
                 "?tex=" + urlEncode(text) +
                 "&tok=" + config.baiduAccessToken +
                 "&cuid=" + WiFi.macAddress() +
                 "&ctp=1&lan=zh&spd=5&pit=5&vol=10&per=4&aue=6";  // aue=6 是 PCM 格式
    
    http.begin(url);
    http.setTimeout(15000);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        // 检查返回类型
        String contentType = http.header("Content-Type");
        
        if (contentType.indexOf("audio") >= 0) {
            // 返回的是音频数据
            WiFiClient* stream = http.getStreamPtr();
            size_t totalRead = 0;
            
            while (http.connected() && totalRead < maxLen) {
                size_t available = stream->available();
                if (available) {
                    size_t toRead = min(available, maxLen - totalRead);
                    size_t read = stream->readBytes(audioBuffer + totalRead, toRead);
                    totalRead += read;
                }
                delay(1);
            }
            
            Serial.printf("TTS 音频大小: %d 字节\n", totalRead);
            state = AI_IDLE;
            http.end();
            return totalRead;
        } else {
            // 返回的是错误 JSON
            String response = http.getString();
            lastError = "TTS 失败: " + response;
        }
    } else {
        lastError = "TTS 请求失败: " + String(httpCode);
    }
    
    Serial.println(lastError);
    state = AI_ERROR;
    http.end();
    return 0;
}

void AIService::clearHistory() {
    conversationHistory = "";
}

String AIService::base64Encode(const uint8_t* data, size_t len) {
    return base64::encode(data, len);
}

String AIService::urlEncode(const String& str) {
    String encoded = "";
    char c;
    char code0;
    char code1;
    
    for (size_t i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9) code0 = c - 10 + 'A';
            encoded += '%';
            encoded += code0;
            encoded += code1;
        }
    }
    return encoded;
}
