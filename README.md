# M5Unified_Voicevox_TTS
PC上でVoicevoxエンジンを利用して音声合成したWAVファイルをM5Stackで再生します。


# 概要
PC（Ubuntu20.04で動作確認しました。）上でVoicevoxエンジンを動かし、音声合成したWAVファイルを再生します。
M5Stack/M5Stack Core2で動作確認しています。

# 設定箇所
下記の項目を設定してください。
- SSID
- WiFiルータのパスワード
- Voiceboxエンジンを実行しているPCのIPアドレスとポート（デフォルトは50021です。）
- Voicevoxの話者番号
```
#define STASSID "YOUR_SSID"
#define STAPSK  "YOUR_PASSWORD"

// Voicevoxを起動しているPCのIPアドレスとポート番号を設定します。
const char *URL="http://192.168.0.222:50021";

// Voicevoxの話者番号を指定します。
const char* SPEAKER_NO = "1";

```

# インストール

## Voicevox
 Ubuntuの場合、インストーラーではなくtar.gz版をダウンロードして解凍してください。解凍した後、フォルダに移動して下記のようにIPアドレスを指定してVoicevoxエンジンを起動します。
 
 ※ IPアドレスを指定しないと127.0.0.1で起動するのでM5Stackからアクセスできません。

 ```
 ./run --host 192.168.0.222
 ```

## M5Stackのビルド

 PlatformIO用なので、ArduinoIDEで利用する場合は、srcフォルダとmain.cppの名前を揃えて変更してください。

### 例

 src -> M5Unified_Voicevox_TTS

 main.cpp -> M5Unified_Voicecox_TTS

### 必要なライブラリ
詳しい情報はplatformio.iniを参照してください。
- [M5Unified](https://github.com/m5stack/M5Unified)
- [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)
- [UrlEncode](https://github.com/plageoj/urlencode)

# 参考（引用）
- [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)のサンプル(StreamMP3FromHTTP)
- [M5Unified](https://github.com/m5stack/M5Unified)のサンプル(WebRadio_with_ESP8266Audio)

# Author
[Takao Akaki](https://github.com/mongonta0716)

# Licence
[AGPL-3.0](https://github.com/mongonta0716/M5Unified_Voicevox_TTS/blob/main/LICENSE)