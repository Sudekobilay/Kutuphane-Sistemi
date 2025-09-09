# Kütüphane Yönetim Sistemi

Bu proje, C dilinde yazılmış kapsamlı bir kütüphane yönetim sistemidir. Sistem, kitap ve kullanıcı yönetimi, ödünç alma/iade işlemleri ve yönetici paneli gibi temel kütüphane işlevlerini destekler.

## 🚀 Özellikler

### Yönetici İşlevleri
- ✅ Yeni kullanici ekleme
- ✅ Yeni kitap ekleme
- ✅ Kitap bilgilerini güncelleme
- ✅ Kitap silme
- ✅ Geciken kitapları kontrol etme

### Kullanıcı İşlevleri
- ✅ Kullanıcı kaydı oluşturma
- ✅ Kitap arama (başlık ve yazar bazında)
- ✅ Kitap ödünç alma (maksimum 3 kitap)
- ✅ Kitap iade etme
- ✅ Ödünç alınan kitapları görüntüleme
- ✅ Kategori bazlı kitap listeleme

### Sistem Özellikleri
- 📁 Veri kalıcılığı (binary dosya formatı)
- 🎨 Renkli konsol arayüzü
- ⏰ Otomatik gecikme hesaplama (15 gün)
- 🔒 Güvenli giriş sistemi

## 📋 Gereksinimler

- **Derleyici**: GCC (GNU Compiler Collection)
- **İşletim Sistemi**: Windows/Linux/macOS
- **IDE**: Code::Blocks (önerilen)

## 🛠️ Kurulum ve Çalıştırma

### Code::Blocks ile
1. `kutuphane.cbp` dosyasını Code::Blocks ile açın
2. **Build** → **Build and Run** (F9) tuşuna basın

### Manuel Derleme
```bash
gcc -o kutuphane main.c -Wall
./kutuphane
```

## 📁 Dosya Yapısı

```
kutuphane/
├── main.c              # Ana kaynak kod dosyası
├── kutuphane.cbp       # Code::Blocks proje dosyası
├── books.dat           # Kitap verileri (binary)
├── users.dat           # Kullanıcı verileri (binary)
├── bin/                # Derlenmiş executable dosyalar
└── obj/                # Object dosyaları
```

## 🔐 Varsayılan Giriş Bilgileri

### Yönetici
- **Kullanıcı Adı**: `admin`
- **Şifre**: `admin123`

## 📖 Kullanım Kılavuzu

### İlk Çalıştırma
1. Programı çalıştırdığınızda ana menü görüntülenir
2. İlk kullanımda veri dosyaları otomatik oluşturulur
3. Yönetici girişi yaparak sistem kurulumunu tamamlayın

### Yönetici İşlemleri
1. Ana menüden "Yönetici Girişi" seçin
2. Varsayılan bilgilerle giriş yapın
3. Yönetici panelinden:
   - Yeni kullanıcılar ekleyin
   - Kitap koleksiyonunu yönetin
   - Geciken kitapları kontrol edin

### Kullanıcı İşlemleri
1. "Kayıt Ol" seçeneği ile hesap oluşturun
2. "Kullanıcı Girişi" ile sisteme girin
3. Kullanıcı panelinden:
   - Kitap arayın
   - Ödünç alma/iade işlemleri yapın
   - Kişisel kitap listenizi görüntüleyin

## ⚙️ Teknik Detaylar

### Veri Yapıları
```c
// Kitap yapısı
typedef struct {
    int id;
    char title[100];
    char author[100];
    char category[50];
    int is_borrowed;
    time_t borrow_date;
    int borrower_user_id;
} Book;

// Kullanıcı yapısı
typedef struct {
    int id;
    char username[50];
    char password[50];
    int borrowed_books_count;
} User;
```

### Sabitler
- **Maksimum kitap sayısı**: 500
- **Maksimum kullanıcı sayısı**: 100
- **Ödünç alma süresi**: 15 gün
- **Kullanıcı başına maksimum kitap**: 3

## 🎨 Arayüz Özellikleri

Sistem, ANSI renk kodları kullanarak renkli bir konsol arayüzü sunar:
- 🔴 Kırmızı: Hata mesajları
- 🟢 Yeşil: Başarı mesajları
- 🔵 Mavi: Bilgi mesajları
- 🟡 Sarı: Uyarı mesajları

## 🔧 Geliştirme Notları

### Güvenlik Özellikleri
- `_CRT_SECURE_NO_WARNINGS` direktifi ile güvenli olmayan fonksiyon uyarıları kapatılmıştır
- Kullanıcı girişleri buffer overflow'a karşı korunmuştur
- Dosya işlemleri hata kontrolü ile yapılmaktadır

### Platform Uyumluluğu
- Windows ve Unix-like sistemler için farklı ekran temizleme komutları
- Cross-platform zaman fonksiyonları kullanımı

## 🐛 Bilinen Sorunlar

- Türkçe karakter desteği terminal ayarlarına bağlıdır
- Binary dosya formatı farklı mimariler arası taşınabilir olmayabilir

## 📝 Lisans

Bu proje eğitim amaçlı geliştirilmiştir.

## 👨‍💻 Geliştirici



---

