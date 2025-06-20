# Dashboard Monitoring Ruangan MQTT

## Gambaran Umum

Dashboard ini adalah aplikasi web real-time yang menampilkan data monitoring ruangan dari sistem ESP32-CAM melalui koneksi MQTT. Dashboard menyediakan interface yang user-friendly untuk memantau status ruangan, deteksi gerakan, dan menerima notifikasi secara langsung di browser web.

## Fitur Utama

- **Monitoring Real-time**: Menampilkan status ruangan dan gerakan secara langsung
- **Koneksi MQTT WebSocket**: Terhubung ke HiveMQ Cloud menggunakan WebSocket over TLS
- **Interface Responsif**: Desain yang menyesuaikan dengan berbagai ukuran layar
- **Animasi Visual**: Efek visual yang menarik untuk status dan notifikasi
- **Debug Console**: Tools debugging untuk troubleshooting koneksi
- **Riwayat Notifikasi**: Menampilkan 10 notifikasi terbaru
- **Auto-reconnect**: Otomatis mencoba koneksi ulang saat terputus

## Teknologi yang Digunakan

### Library dan Dependencies
- **Paho MQTT JavaScript Client**: Library untuk koneksi MQTT WebSocket
- **HTML5 + CSS3**: Interface dan styling modern
- **Vanilla JavaScript**: Logic aplikasi tanpa framework eksternal

### CDN Resources
```html
<script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js"></script>
```

## Konfigurasi

### 1. Konfigurasi MQTT
```javascript
const mqttConfig = {
    host: "a21d026aee9c471395ac1ab161eb8f48.s1.eu.hivemq.cloud",
    port: 8884,           // WebSocket over TLS port
    path: "/mqtt",        // WebSocket path
    username: "wololo",
    password: "Wololou6",
    clientId: "WebClient_" + Math.random().toString(16).substr(2, 8),
    useSSL: true
};
```

### 2. Topik MQTT
```javascript
const topics = {
    status: "perta/status",    // Status okupansi ruangan
    motion: "perta/motion",    // Data deteksi gerakan
    notify: "perta/notify"     // Notifikasi sistem
};
```

## Struktur Interface

### 1. Header Dashboard
- **Judul**: "🏠 Room Monitoring System"
- **Status Koneksi**: Indikator real-time koneksi MQTT
- **Debug Console**: Informasi debugging untuk troubleshooting

### 2. Kontrol Panel
- **Test Connection**: Tombol untuk menguji koneksi MQTT
- **Reconnect**: Tombol untuk koneksi ulang manual

### 3. Status Ruangan
- **Indikator Visual**: LED animasi dengan warna berbeda
  - 🔴 Merah: Ruangan sedang dipakai
  - 🟢 Hijau: Ruangan tidak dipakai
- **Text Status**: Menampilkan status dalam bahasa Indonesia
- **Timestamp**: Waktu update terakhir

### 4. Status Gerakan
- **Icon Animasi**: 
  - 🚨 Bergetar: Gerakan terdeteksi
  - 👤 Statis: Tidak ada gerakan
- **Text Deskripsi**: Keterangan status gerakan

### 5. Panel Notifikasi
- **Riwayat Real-time**: 10 notifikasi terbaru
- **Timestamp**: Waktu setiap notifikasi
- **Animasi Slide**: Efek masuk untuk notifikasi baru

## Fungsionalitas Utama

### Koneksi MQTT

#### `initMQTT()`
```javascript
// Inisialisasi koneksi ke broker MQTT
// - Membuat client Paho MQTT
// - Mengatur callback handlers
// - Melakukan koneksi dengan SSL/TLS
```

#### `onConnect()`
```javascript
// Callback saat koneksi berhasil
// - Update status interface
// - Subscribe ke semua topik
// - Mulai monitoring data
```

#### `onConnectionLost()` & `onConnectFailure()`
```javascript
// Handling koneksi terputus dan gagal
// - Update status error
// - Retry otomatis setelah 5 detik
// - Debug logging
```

### Pemrosesan Data

#### `onMessageArrived()`
```javascript
// Memproses pesan MQTT yang diterima
// - Parse JSON payload
// - Route ke handler yang sesuai
// - Error handling untuk data corrupt
```

#### `updateRoomStatus(data)`
```javascript
// Update tampilan status ruangan
// - Ubah warna indikator
// - Update text status
// - Timestamp update terakhir
```

#### `updateMotionStatus(data)`
```javascript
// Update tampilan deteksi gerakan
// - Animasi icon
// - Perubahan warna background
// - Text deskriptif
```

#### `addNotification(data)`
```javascript
// Menambah notifikasi baru
// - Animasi slide-in
// - Format timestamp
// - Batasi maksimal 10 item
```

## Format Data MQTT

### 1. Status Ruangan (`perta/status`)
```json
{
  "status": "occupied|empty",
  "timestamp": 1234567890
}
```

**Mapping:**
- `"occupied"` → "RUANGAN SEDANG DIPAKAI" (Merah)
- `"empty"` → "RUANGAN TIDAK DIPAKAI" (Hijau)

### 2. Deteksi Gerakan (`perta/motion`)
```json
{
  "motion": true|false,
  "timestamp": 1234567890
}
```

**Mapping:**
- `true` → "Gerakan terdeteksi!" (🚨 Animasi)
- `false` → "Tidak ada gerakan" (👤 Normal)

### 3. Notifikasi (`perta/notify`)
```json
{
  "message": "Teks notifikasi",
  "timestamp": 1234567890
}
```

**Contoh pesan:**
- "Perhatian. Terdeteksi gerakan pada [tanggal] [waktu]"
- "Ruangan sedang dipakai"
- "Ruangan tidak dipakai"

## Fitur Visual & Animasi

### 1. Animasi CSS
```css
/* Pulse effect untuk indikator status */
@keyframes pulse {
    0% { box-shadow: 0 0 0 0 rgba(255, 255, 255, 0.7); }
    70% { box-shadow: 0 0 0 10px rgba(255, 255, 255, 0); }
    100% { box-shadow: 0 0 0 0 rgba(255, 255, 255, 0); }
}

/* Shake effect untuk deteksi gerakan */
@keyframes shake {
    0%, 100% { transform: translateX(0); }
    25% { transform: translateX(-10px); }
    75% { transform: translateX(10px); }
}

/* Slide-in effect untuk notifikasi */
@keyframes slideIn {
    from { opacity: 0; transform: translateX(-20px); }
    to { opacity: 1; transform: translateX(0); }
}
```

### 2. Responsive Design
- **Desktop**: Layout penuh dengan semua elemen
- **Mobile**: Elemen disusun vertikal, font disesuaikan
- **Breakpoint**: 600px untuk optimasi mobile

## Setup dan Instalasi

### 1. Persiapan File
```html
<!DOCTYPE html>
<html lang="id">
<!-- Simpan sebagai file .html -->
```

### 2. Hosting
**Opsi 1: Local Development**
- Buka file HTML langsung di browser
- Tidak memerlukan web server untuk testing

**Opsi 2: Web Server**
- Upload ke hosting web
- Akses via domain/subdomain
- Lebih stabil untuk production

### 3. Konfigurasi MQTT
- Pastikan kredensial MQTT benar
- Verifikasi topik sesuai dengan ESP32
- Test koneksi menggunakan MQTT client lain

## Troubleshooting

### Masalah Koneksi MQTT

#### "MQTT library not loaded"
```javascript
// Solusi: Periksa koneksi internet dan CDN
if (typeof Paho === 'undefined') {
    console.error("❌ Paho MQTT library not loaded!");
}
```

#### "Bad username or password" (Error Code 7)
- Verifikasi username dan password MQTT
- Periksa case-sensitive credential
- Coba regenerate password di HiveMQ Console

#### "Not authorized" (Error Code 8)
- Periksa permission user di broker
- Verifikasi ACL (Access Control List)
- Pastikan user bisa subscribe ke topik

#### "Connection timeout"
- Periksa firewall yang memblokir port 8884
- Verifikasi SSL/TLS certificate
- Coba dari jaringan berbeda

### Masalah Data

#### "Parse error" pada pesan
```javascript
// Data JSON tidak valid dari ESP32
try {
    const payload = JSON.parse(message.payloadString);
} catch(error) {
    console.error("Error parsing message:", error);
}
```

#### Data tidak update
- Periksa ESP32 masih terkoneksi ke MQTT
- Verifikasi topik yang dipublish ESP32
- Check debug console untuk pesan error

### Debug Tools

#### Browser Console
```javascript
// Buka Developer Tools (F12)
// Tab Console untuk melihat log
console.log("=== MQTT Debug Info ===");
```

#### Debug Panel dalam Dashboard
- Real-time connection log
- Message tracking
- Error reporting

## Fitur Testing

### 1. Test Connection
```javascript
function testConnection() {
    if (client && client.isConnected()) {
        // Kirim test message
        const testMessage = new Paho.MQTT.Message(
            JSON.stringify({test: true, time: new Date().toISOString()})
        );
        testMessage.destinationName = "perta/test";
        client.send(testMessage);
    }
}
```

### 2. Manual Reconnect
```javascript
function reconnect() {
    if (client && client.isConnected()) {
        client.disconnect();
    }
    setTimeout(() => initMQTT(), 1000);
}
```

## Keamanan

### 1. Koneksi Aman
- **WebSocket over TLS**: Port 8884 dengan SSL
- **Autentikasi**: Username/password authentication
- **Certificate Validation**: Validasi SSL certificate

### 2. Client Security
- **Random Client ID**: Mencegah konflik connection
- **Clean Session**: Tidak menyimpan data session
- **Timeout Protection**: Timeout 10 detik untuk koneksi

## Performance Optimization

### 1. Memory Management
- **Clean Session**: `cleanSession: true`
- **Limited Notifications**: Maksimal 10 item di history
- **Efficient DOM Updates**: Update hanya elemen yang berubah

### 2. Network Optimization
- **Keep Alive**: Interval 30 detik
- **Auto Reconnect**: Otomatis reconnect saat terputus
- **Compression**: Menggunakan JSON compact format

## Integrasi dengan ESP32

### Sinkronisasi Data
Dashboard ini dirancang untuk bekerja dengan ESP32 code yang telah didokumentasikan sebelumnya:

1. **Status Publikasi**: ESP32 publish ke `perta/status` setiap 30 detik
2. **Motion Events**: ESP32 publish ke `perta/motion` saat ada perubahan
3. **Notifications**: ESP32 publish ke `perta/notify` untuk alert

### Kompatibilitas Format
- JSON format sama antara ESP32 dan dashboard
- Timestamp handling otomatis
- Error tolerance untuk data corrupt

## Pengembangan Lanjutan

### Fitur Tambahan yang Bisa Ditambahkan
1. **Historical Charts**: Grafik data dalam periode waktu
2. **Mobile App**: Progressive Web App (PWA)
3. **Push Notifications**: Browser notification API
4. **Data Export**: Export data ke CSV/JSON
5. **User Authentication**: Login system untuk keamanan
6. **Multi-room Support**: Monitoring beberapa ruangan
7. **Custom Alerts**: Alert berdasarkan kondisi tertentu
8. **Dark Mode**: Theme switching
9. **Offline Mode**: Service worker untuk offline capability
10. **Real-time Charts**: Live data visualization

### Arsitektur Scalable
```
ESP32 → MQTT Broker → Web Dashboard
                  ↘ Mobile App
                  ↘ API Server
                  ↘ Database Logger
```

