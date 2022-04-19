/*
  AudioFileSourceHTTPStreamVOICEVOX
  Streaming HTTP source for voicevox

  This source is AudioFileSourceHTTPStream.cpp(https://github.com/earlephilhower/ESP8266Audio) 
  modified for WAV playback generated by Voicevox(https://voicevox.hiroshiba.jp/)

  Copyright (C) 2022 Takao Akaki 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(ESP32) || defined(ESP8266)

#include <UrlEncode.h>
#include "AudioFileSourceHTTPStreamVOICEVOX.h"


AudioFileSourceHTTPStreamVOICEVOX::AudioFileSourceHTTPStreamVOICEVOX()
{
  pos = 0;
  reconnectTries = 0;
  saveURL[0] = 0;
}

AudioFileSourceHTTPStreamVOICEVOX::AudioFileSourceHTTPStreamVOICEVOX(const char *url)
{
  saveURL[0] = 0;
  reconnectTries = 0;
  open(url);
}

bool AudioFileSourceHTTPStreamVOICEVOX::open(const char *text) {
  http.end();
  String base_str = "r_host/r_query?text=r_text&speaker=r_speaker";

  base_str.replace("r_host", text);
  base_str.replace("r_query", "audio_query");
  base_str.replace("r_text", encode_text);
  base_str.replace("r_speaker", speaker_no);
  http.begin(client, base_str.c_str());
  http.setReuse(true);
  http.addHeader("Content-Type", "text/plain");
  String payload = "";
  int code = http.POST(payload);
#ifndef ESP32
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
#endif
  if (code == HTTP_CODE_OK) {
    payload = http.getString();
    //Serial.println(payload);
    http.end();
  } else {
    http.end();
    cb.st(STATUS_HTTPFAIL, PSTR("Can't open HTTP request"));
    return false;
  }
  base_str = "r_host/r_query?speaker=r_speaker";
  base_str.replace("r_host", text);
  base_str.replace("r_query", "synthesis");
  base_str.replace("r_speaker", speaker_no);
  http.begin(client, base_str.c_str());
  http.addHeader("Content-Type", "application/json");
  code = http.POST(payload);
  if (code != HTTP_CODE_OK) {
    http.end();
    cb.st(STATUS_HTTPFAIL, PSTR("Can't open HTTP request"));
    return false;
  }
  size = http.getSize();
  strncpy(saveURL, base_str.c_str(), sizeof(saveURL));
  saveURL[sizeof(saveURL)-1] = 0;
  return true;
}

AudioFileSourceHTTPStreamVOICEVOX::~AudioFileSourceHTTPStreamVOICEVOX()
{
  http.end();
}

uint32_t AudioFileSourceHTTPStreamVOICEVOX::read(void *data, uint32_t len)
{
  if (data==NULL) {
    audioLogger->printf_P(PSTR("ERROR! AudioFileSourceHTTPStreamVOICEVOX::read passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, false);
}

uint32_t AudioFileSourceHTTPStreamVOICEVOX::readNonBlock(void *data, uint32_t len)
{
  if (data==NULL) {
    audioLogger->printf_P(PSTR("ERROR! AudioFileSourceHTTPStreamVOICEVOX::readNonBlock passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, true);
}

uint32_t AudioFileSourceHTTPStreamVOICEVOX::readInternal(void *data, uint32_t len, bool nonBlock)
{
retry:
  if (!http.connected()) {
    cb.st(STATUS_DISCONNECTED, PSTR("Stream disconnected"));
    http.end();
    for (int i = 0; i < reconnectTries; i++) {
      char buff[64];
      sprintf_P(buff, PSTR("Attempting to reconnect, try %d"), i);
      cb.st(STATUS_RECONNECTING, buff);
      delay(reconnectDelayMs);
      if (open(saveURL)) {
        cb.st(STATUS_RECONNECTED, PSTR("Stream reconnected"));
        break;
      }
    }
    if (!http.connected()) {
      cb.st(STATUS_DISCONNECTED, PSTR("Unable to reconnect"));
      return 0;
    }
  }
  if ((size > 0) && (pos >= size)) return 0;

  WiFiClient *stream = http.getStreamPtr();

  // Can't read past EOF...
  if ( (size > 0) && (len > (uint32_t)(pos - size)) ) len = pos - size;

  if (!nonBlock) {
    int start = millis();
    while ((stream->available() < (int)len) && (millis() - start < 500)) yield();
  }

  size_t avail = stream->available();
  if (!nonBlock && !avail) {
    cb.st(STATUS_NODATA, PSTR("No stream data available"));
    http.end();
    goto retry;
  }
  if (avail == 0) return 0;
  if (avail < len) len = avail;

  int read = stream->read(reinterpret_cast<uint8_t*>(data), len);
  pos += read;
  return read;
}

bool AudioFileSourceHTTPStreamVOICEVOX::seek(int32_t pos, int dir)
{
  audioLogger->printf_P(PSTR("ERROR! AudioFileSourceHTTPStreamVOICEVOX::seek not implemented!"));
  (void) pos;
  (void) dir;
  return false;
}

bool AudioFileSourceHTTPStreamVOICEVOX::close()
{
  http.end();
  return true;
}

bool AudioFileSourceHTTPStreamVOICEVOX::isOpen()
{
  return http.connected();
}

uint32_t AudioFileSourceHTTPStreamVOICEVOX::getSize()
{
  return size;
}

uint32_t AudioFileSourceHTTPStreamVOICEVOX::getPos()
{
  return pos;
}

void AudioFileSourceHTTPStreamVOICEVOX::setText(String text_str) {
    encode_text = urlEncode(text_str);
}

#endif