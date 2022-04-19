
#if defined(ESP32) || defined(ESP8266)
#pragma once

#include <Arduino.h>
#ifdef ESP32
  #include <HTTPClient.h>
#else
  #include <ESP8266HTTPClient.h>
#endif
#include "AudioFileSource.h"

class AudioFileSourceHTTPStreamVOICEVOX : public AudioFileSource
{
  friend class AudioFileSourceICYStream;

  public:
    AudioFileSourceHTTPStreamVOICEVOX();
    AudioFileSourceHTTPStreamVOICEVOX(const char *url);
    virtual ~AudioFileSourceHTTPStreamVOICEVOX() override;
    
    virtual bool open(const char *url) override;
    virtual uint32_t read(void *data, uint32_t len) override;
    virtual uint32_t readNonBlock(void *data, uint32_t len) override;
    virtual bool seek(int32_t pos, int dir) override;
    virtual bool close() override;
    virtual bool isOpen() override;
    virtual uint32_t getSize() override;
    virtual uint32_t getPos() override;
    bool SetReconnect(int tries, int delayms) { reconnectTries = tries; reconnectDelayMs = delayms; return true; }
    void useHTTP10 () { http.useHTTP10(true); }
    void setText(String speech_text);
    void setSpeakerNo(String speaker_number) { speaker_no = speaker_number; }

    enum { STATUS_HTTPFAIL=2, STATUS_DISCONNECTED, STATUS_RECONNECTING, STATUS_RECONNECTED, STATUS_NODATA };

  private:
    virtual uint32_t readInternal(void *data, uint32_t len, bool nonBlock);
    WiFiClient client;
    HTTPClient http;
    int pos;
    int size;
    int reconnectTries;
    int reconnectDelayMs;
    char saveURL[128];
    String encode_text;
    String speaker_no;
};


#endif