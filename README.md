# 🎬 Mimi — Animated GIF + Audio Player on ESP32-S3

A lightweight embedded media player that plays a **synchronized GIF animation with audio** on a **1.44" ST7735 128×128 TFT display**, powered by an **ESP32-S3 N16R8**.

It uses delta-frame compression for the GIF and PWM-based audio playback driven by a hardware timer — all stored in flash (PROGMEM).

---

## 📺 Demo

> *(Add a photo or short video of your device here)*

---

## ✨ Features

- 🖼️ **Delta-compressed GIF playback** — only changed pixels are stored per frame, saving significant flash space
- 🔊 **8-bit PCM audio via PWM** — played through a hardware timer interrupt at 8 kHz
- 🔄 **Audio-video sync** — video frames wait on the audio pointer to stay in sync
- 💾 **PROGMEM storage** — all media lives in flash, not RAM
- ⚡ **ESP32-S3 + TFT_eSPI** — fast SPI display rendering at 27 MHz

---

## 🧰 Hardware

| Component | Details |
|-----------|---------|
| MCU | ESP32-S3 Dual Type C — N16R8 (16 MB flash, 8 MB PSRAM) |
| Display | 1.44" SPI TFT LCD — ST7735, 128×128 px |
| Speaker | Low-impedance speaker (lowest available ohm rating for louder PWM output) |

### Wiring

| TFT Pin | ESP32-S3 GPIO |
|---------|---------------|
| SDA (MOSI) | 11 |
| SCL (SCLK) | 12 |
| CS | 10 |
| DC / RS | 14 |
| RST | 9 |
| Speaker (PWM) | 7 |

> **Speaker tip:** Use the lowest-impedance speaker you have (e.g. 4Ω). PWM audio driven directly from a GPIO is weak — lower impedance draws more current and gives noticeably louder output. The sketch amplifies samples ×8 and clamps to prevent clipping.

---

## ⚠️ Partition Scheme — Critical

Because the GIF frames and audio are embedded directly in flash as PROGMEM arrays, the compiled binary can easily exceed 3 MB. You **must** set the partition scheme to **Huge APP** before flashing, or the upload will fail.

In Arduino IDE:

```
Tools → Partition Scheme → Huge APP (3MB No OTA / 1MB SPIFFS)
```

Also make sure you are using an ESP32-S3 with **at least 8 MB of flash** (e.g. N16R8 has 16 MB). Boards with only 4 MB will not have enough space for large media.

---

## 📁 Project Structure

```
mimi-esp32-display/
├── mimi/
│   └── mimi.ino          # Main Arduino sketch
├── tools/
│   ├── gif2h.py          # Convert GIF → gif_data.h (delta-compressed RGB565)
│   └── wav2h.py          # Convert WAV → audio_data.h (raw PCM hex)
├── assets/               # Put your GIF and WAV source files here
│   ├── FVB.gif           # (your animation)
│   └── audioo.wav        # (your audio, 8kHz mono recommended)
├── User_Setup.h          # TFT_eSPI config — copy to Arduino/libraries/TFT_eSPI/
└── README.md
```

> **Note:** `gif_data.h` and `audio_data.h` are generated files — do not commit them if they are large. They are listed in `.gitignore` by default.

---

## 🚀 Getting Started

### 1. Install Dependencies

- [Arduino IDE](https://www.arduino.cc/en/software) with ESP32 board support
- [TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI)
- Python 3 + Pillow:
  ```bash
  pip install Pillow
  ```

### 2. Configure TFT_eSPI

Copy `User_Setup.h` into your TFT_eSPI library folder:

```
Arduino/libraries/TFT_eSPI/User_Setup.h
```

> ⚠️ This overwrites the existing file. Back it up if needed.

### 3. Set Partition Scheme

In Arduino IDE, before flashing:

```
Tools → Board             → ESP32S3 Dev Module
Tools → Partition Scheme  → Huge APP (3MB No OTA / 1MB SPIFFS)
Tools → Flash Size        → 16MB   ← match your board
```

Skipping this step will cause an upload error if your binary exceeds the default app partition size.

### 4. Prepare Your Media

Place your source files in `assets/`, then run:

**Convert GIF:**
```bash
cd assets
python ../tools/gif2h.py
# Outputs: gif_data.h
```

**Convert WAV:**
```bash
cd assets
python ../tools/wav2h.py
# Outputs: audio_data.h
```

> **Audio tips:**
> - Use an **8 kHz, mono, 8-bit PCM** WAV for best results.
> - Convert with ffmpeg: `ffmpeg -i input.mp3 -ar 8000 -ac 1 -acodec pcm_u8 audioo.wav`

Move the generated `.h` files into `src/` alongside `mimi.ino`.

### 5. Flash

Open `src/mimi.ino` in Arduino IDE, select your **ESP32-S3** board, and upload.

---

## ⚙️ How It Works

### GIF → Delta Frames

`gif2h.py` converts each GIF frame to RGB565. Instead of storing every pixel for every frame, it only stores **pixels that changed** relative to the previous frame. Each changed pixel is stored as `(x, y, color)` — 3 × uint16_t, prepended with a pixel count.

### Audio Playback

`wav2h.py` dumps the raw WAV bytes into a PROGMEM array (skipping the 44-byte header). A hardware timer fires at **8000 Hz** and writes each sample to a PWM channel via `ledcWrite()`, amplified ×8 and clamped to prevent clipping.

### A/V Sync

In `loop()`, the sketch calculates `samplesPerFrame` and busy-waits on `audio_ptr` after each frame until the audio catches up — keeping video locked to audio for the full duration.

---

## 🛠️ Customization

| What | Where |
|------|-------|
| Audio amplification | `mimi.ino` — `int16_t amplified = raw * 8;` |
| Audio sample rate | Change `timerBegin(8000)` and re-encode WAV at that rate |
| Display rotation | `tft.setRotation(0)` in `setup()` |
| GIF source file | `GIF_FILE` constant in `gif2h.py` |
| WAV source file | `file_to_c_array("audioo.wav")` in `wav2h.py` |

---

## 📝 License

MIT — feel free to use, modify, and share.
