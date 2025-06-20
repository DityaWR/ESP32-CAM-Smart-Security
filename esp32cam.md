# Sistem Deteksi Gerakan ESP32 Camera

## Gambaran Umum

Proyek ini mengimplementasikan sistem monitoring ruangan pintar menggunakan modul ESP32-CAM yang dapat mendeteksi gerakan, mengambil foto, dan mengirim notifikasi melalui Telegram sambil juga mempublikasikan update status ke broker MQTT. Sistem ini secara cerdas melacak okupansi ruangan dan mengirim peringatan ketika gerakan terdeteksi di ruangan yang tidak digunakan.

## Fitur

- **Deteksi Gerakan**: Menggunakan sensor RCWL untuk mendeteksi pergerakan
- **Integrasi Kamera**: Mengambil foto ketika gerakan terdeteksi
- **Notifikasi Telegram**: Mengirim peringatan dan foto secara real-time via bot Telegram
- **Integrasi MQTT**: Mempublikasikan update status ke HiveMQ Cloud
- **Status Ruangan Pintar**: Melacak apakah ruangan sedang digunakan atau kosong
- **Indikator LED**: Feedback visual dengan lampu flash dan status LED
- **Optimasi Memori**: Dikonfigurasi untuk penggunaan memori yang efisien

## Kebutuhan Hardware

### Komponen
- Modul ESP32-CAM
- Sensor gerakan microwave RCWL-0516
- LED (Indikator status merah)
- LED Flash (Built-in atau eksternal)
- Power supply (5V direkomendasikan)

### Konfigurasi Pin
```
Pin 4  - LED Flash
Pin 15 - LED Status Merah
Pin 13 - Sensor Gerakan RCWL
```

## Dependensi Software

### Library yang Diperlukan
```cpp
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FastBot.h>        // Library bot Telegram
#include <PubSubClient.h>   // Library client MQTT
#include <ArduinoJson.h>    // Handling JSON
#include "camera.h"         // Fungsi kamera
```

### Instalasi
Instal library berikut via Arduino IDE Library Manager:
- FastBot
- PubSubClient  
- ArduinoJson

## Konfigurasi

### 1. Pengaturan WiFi
```cpp
#define WIFI_SSID "WifiIoT"
#define WIFI_PASS "12345678"
```

### 2. Setup Bot Telegram
```cpp
#define BOT_TOKEN "TOKEN_BOT_ANDA_DISINI"
#define CHAT_ID "CHAT_ID_ANDA_DISINI"
```

**Untuk mendapatkan kredensial Telegram:**
1. Buat bot dengan mengirim pesan ke @BotFather di Telegram
2. Dapatkan chat ID dengan mengirim pesan ke @userinfobot

### 3. Konfigurasi MQTT
```cpp
const char* mqtt_server = "a21d026aee9c471395ac1ab161eb8f48.s1.eu.hivemq.cloud";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "wololo";    
const char* mqtt_pass   = "Wololou6";
```

**Topik MQTT:**
- `perta/status` - Status okupansi ruangan
- `perta/motion` - Event deteksi gerakan
- `perta/notify` - Notifikasi umum

## Logika Sistem

### Status Ruangan
- **Kosong** (`ruang_tidak_dipakai`): Tidak ada gerakan yang terdeteksi baru-baru ini
- **Digunakan** (`ruang_sedang_dipakai`): Gerakan berkelanjutan terdeteksi

### Alur Deteksi Gerakan

1. **Gerakan Terdeteksi**
   - LED merah menyala
   - Jika ruangan kosong, mengambil dan mengirim foto via Telegram
   - Mempublikasikan event gerakan ke MQTT
   - Memulai timer okupansi

2. **Gerakan Berkelanjutan** (2+ menit)
   - Mengubah status ruangan menjadi "digunakan"
   - Mengirim update status via Telegram dan MQTT

3. **Tidak Ada Gerakan** (5+ menit setelah deteksi terakhir)
   - Mengubah status ruangan menjadi "kosong"
   - Mengirim update status via Telegram dan MQTT

### Parameter Timing
- **Deteksi Okupansi**: 120 detik gerakan
- **Deteksi Kosong**: 300 detik tanpa gerakan
- **Reset Status**: 180 detik timeout untuk perubahan status
- **Heartbeat MQTT**: Interval 30 detik

## Fungsi Utama

### `setup()`
- Menginisialisasi komponen hardware
- Mengkonfigurasi pengaturan kamera (resolusi VGA, kualitas 12)
- Menghubungkan ke WiFi dan MQTT
- Mengirim notifikasi startup

### `loop()`
- Memonitor sensor gerakan
- Mengelola logika okupansi ruangan
- Mempertahankan koneksi MQTT
- Mempublikasikan update status berkala

### `sendPhotoTelegram()`
- Sementara memutus koneksi MQTT untuk membebaskan memori
- Mengambil beberapa foto (dua foto pertama dibuang untuk kualitas)
- Mengirim foto ke Telegram dengan error handling
- Menghubungkan kembali ke MQTT

### Fungsi MQTT
- `publishStatusToMQTT()`: Status okupansi ruangan
- `publishMotionToMQTT()`: Event deteksi gerakan
- `publishNotificationToMQTT()`: Notifikasi umum

## Format Pesan JSON

### Pesan Status
```json
{
  "status": "occupied|empty",
  "timestamp": 1234567890
}
```

### Pesan Gerakan
```json
{
  "motion": true|false,
  "timestamp": 1234567890
}
```

### Pesan Notifikasi
```json
{
  "message": "Teks notifikasi",
  "timestamp": 1234567890
}
```

## Fitur Keamanan

- **TLS/SSL**: Koneksi MQTT aman menggunakan sertifikat root CA
- **Autentikasi Token**: Perlindungan token bot Telegram
- **Manajemen Kredensial**: Konstanta konfigurasi terpusat

## Optimasi Memori

Kode ini mencakup beberapa teknik optimasi memori:

- Ukuran paket MQTT dikurangi menjadi 512 bytes
- Buffer dokumen JSON yang lebih kecil
- Pemutusan MQTT sementara selama pengambilan foto
- Resolusi kamera dibatasi ke VGA
- Manajemen buffer untuk frame kamera

## Langkah Instalasi

1. **Setup Hardware**
   - Hubungkan komponen sesuai konfigurasi pin
   - Pastikan power supply stabil

2. **Konfigurasi Software**
   - Update kredensial WiFi
   - Konfigurasi token bot Telegram dan chat ID
   - Set kredensial broker MQTT

3. **Upload Kode**
   - Pilih "ESP32 Wrover Module" sebagai board
   - Set partition scheme ke "Huge APP"
   - Upload sketch

4. **Testing**
   - Monitor output serial untuk status koneksi
   - Test deteksi gerakan
   - Verifikasi fungsionalitas Telegram dan MQTT

## Troubleshooting

### Masalah Umum

**Error Kamera**
- Periksa stabilitas power supply
- Verifikasi koneksi modul kamera
- Pastikan pencahayaan yang memadai

**Koneksi WiFi Gagal**
- Verifikasi SSID dan password
- Periksa kekuatan sinyal
- Sistem akan auto-restart setelah 20 detik

**Masalah Koneksi MQTT**
- Verifikasi kredensial broker
- Periksa konektivitas internet
- Monitor output serial untuk kode error

**Masalah Memori**
- Pengambilan foto mungkin gagal dengan memori rendah
- Sistem sementara memutus MQTT selama pengambilan foto
- Pertimbangkan mengurangi kualitas gambar lebih lanjut jika diperlukan

## Catatan Performa

- **Waktu Startup**: ~3 detik setelah inisialisasi kamera
- **Pengambilan Foto**: ~2-3 detik termasuk flash
- **Reconnection MQTT**: ~5 detik saat gagal
- **Penggunaan Memori**: Dioptimalkan untuk batasan ESP32-CAM

## Pengembangan Masa Depan

Peningkatan potensial untuk sistem ini:
- Menambah penjadwalan berbasis waktu
- Implementasi multiple zone ruangan
- Menambah deteksi suara
- Interface web untuk konfigurasi
- Data logging dan analytics
- Dukungan battery backup

## Penggunaan dan Monitoring

### Pesan Telegram yang Diterima
- **Startup**: "Bot Ready!"
- **Deteksi Gerakan**: "Perhatian. Terdeteksi gerakan pada [tanggal] [waktu]"
- **Status Berubah**: "status_ruangan : ruang_sedang_dipakai!" atau "status_ruangan : ruang_tidak_dipakai!"

### Monitoring via Serial
```
Motion detected!
Motion stopped!
status_ruangan : ruang_sedang_dipakai!
Status published to MQTT
Photo size: 12345
Foto Telah Terkirim
```

### Data MQTT
Sistem mempublikasikan data ke tiga topik berbeda dengan format JSON yang dapat diintegrasikan dengan sistem home automation atau dashboard monitoring.

## Tips Penggunaan

1. **Penempatan Sensor**: Posisikan sensor RCWL pada ketinggian 1-2 meter untuk deteksi optimal
2. **Pencahayaan**: Pastikan pencahayaan ruangan cukup untuk kualitas foto yang baik
3. **Jaringan**: Gunakan WiFi dengan signal kuat untuk stabilitas koneksi
4. **Power**: Gunakan adaptor 5V 2A minimum untuk performa optimal
5. **Testing**: Lakukan testing komprehensif sebelum deployment final