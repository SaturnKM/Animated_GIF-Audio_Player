#include <TFT_eSPI.h>
#include "gif_data.h"
#include "audio_data.h"

#define AUDIO_PIN 7
#define Y_OFFSET ((128 - FRAME_HEIGHT) / 2)

TFT_eSPI tft = TFT_eSPI();
volatile uint32_t audio_ptr = 44; 
hw_timer_t * audioTimer = NULL;

void IRAM_ATTR onAudioTimer() {
  if (audio_ptr < AUDIO_LEN) {
    int8_t raw = (int8_t)pgm_read_byte(&audio_wav[audio_ptr++]);
    
    // نضرب في 4 أو 5 لرفع مستوى الصوت الضعيف في التعليق الصوتي
    int16_t amplified = raw * 8; 
    
    // منع التشويه الرقمي (Clipping)
    if (amplified > 127) amplified = 127;
    if (amplified < -128) amplified = -128;
    
    uint8_t sample = (uint8_t)(amplified + 128);  
    ledcWrite(AUDIO_PIN, sample);
  } else {
    audio_ptr = 44; 
  }
}

void setup() {
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  ledcAttach(AUDIO_PIN, 100000, 8); 

  audioTimer = timerBegin(8000); // تردد 8000 هرتز
  timerAttachInterrupt(audioTimer, &onAudioTimer);
  timerAlarm(audioTimer, 1, true, 0);
}

void loop() {
  // إعادة التصفير لبداية الدورة
  audio_ptr = 44;
  
  // حساب عدد البكسلات التي تمثل إطاراً كاملاً في الصوت
  // (8000 عينة في الثانية * 25 ثانية) / عدد الإطارات
  uint32_t samplesPerFrame = (AUDIO_LEN - 44) / FRAME_COUNT;

  for (int i = 0; i < FRAME_COUNT; i++) {
    // رسم الإطار الحالي
    uint16_t* frameData = (uint16_t*)pgm_read_ptr(&(frames[i]));
    uint16_t numPixels = pgm_read_word(frameData);
    
    for (uint16_t p = 0; p < numPixels; p++) {
      uint16_t x = pgm_read_word(frameData + 1 + p * 3);
      uint16_t y = pgm_read_word(frameData + 2 + p * 3);
      uint16_t color = pgm_read_word(frameData + 3 + p * 3);
      tft.drawPixel(x, y + Y_OFFSET, color);
    }

    // الانتظار حتى يصل مؤشر الصوت للنقطة التي تتوافق مع الإطار القادم
    // هذا يضمن بقاء الصورة مع الصوت لممدة الـ 25 ثانية كاملة
    uint32_t targetSample = 44 + ((i + 1) * samplesPerFrame);
    while (audio_ptr < targetSample) {
      yield(); // انتظر الصوت ليلحق بالصورة
    }
  }
}
