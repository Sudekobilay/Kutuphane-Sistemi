#define _CRT_SECURE_NO_WARNINGS // Güvenli olmayan fonksiyon uyarılarını kapatir
#include <stdio.h>               // Standart giris/cikis fonksiyonlari icin
#include <stdlib.h>              // Genel yardimci fonksiyonlar icin (orn: system)
#include <string.h>              // Dize isleme fonksiyonlari icin
#include <time.h>                // Zamanla ilgili fonksiyonlar icin

// Konsol ciktilari icin renk tanimlari (ANSI katis kodlari)
#define RED   "\x1B[31m" // Kirmizi
#define GRN   "\x1B[32m" // Yesil
#define YEL   "\x1B[33m" // Sari
#define BLU   "\x1B[34m" // Mavi
#define MAG   "\x1B[35m" // Magenta
#define CYN   "\x1B[36m" // Cyan
#define WHT   "\x1B[37m" // Beyaz
#define RESET "\x1B[0m"  // Rengi sifirla
#define BOLD  "\x1B[1m"  // Kalin yazi

// Sabit konsol genisligi, menu ve ciktilarin hizalanmasi icin
#define CONSOLE_WIDTH 60

// Maksimum kitap ve kullanici sayilari
#define MAX_BOOKS 500 // Kutphanedeki maksimum kitap kapasitesi
#define MAX_USERS 100 // Sistemdeki maksimum kullanici kapasitesi

// Kitaplarin gecikme suresi (gun cinsinden)
#define OVERDUE_DAYS 15

// Verilerin kaydedilecegi dosya adlari
#define BOOKS_FILE "books.dat" // Kitap verileri icin ikili dosya
#define USERS_FILE "users.dat" // Kullanici verileri icin ikili dosya

// Kitap yapisi tanimi
typedef struct {
    int id;                // Kitabin benzersiz kimligi
    char title[100];       // Kitabin basligi
    char author[100];      // Kitabin yazari
    char category[50];     // Kitabin kategorisi
    int is_borrowed;       // Kitap odunc alinmissa 1, raflardaysa 0
    time_t borrow_date;    // Kitabin odunc alindigi tarih (Unix zaman damgasi)
    int borrower_user_id;
} Book;

// Kullanici yapisi tanimi
typedef struct {
    int id;              // Kullanicinin benzersiz kimligi
    char username[50];   // Kullanici adi
    char password[50];   // Kullanici sifresi
    int borrowed_books_count; // Kullanicinin odunc aldigi kitap sayisi
} User;

// Yonetici yapisi tanimi
typedef struct {
    char username[50]; // Yonetici kullanici adi
    char password[50]; // Yonetici sifresi
} Admin;

// Global degiskenler
Book books[MAX_BOOKS];  // Kitaplari depolamak icin dizi
User users[MAX_USERS];  // Kullanicilari depolamak icin dizi
Admin admin = { "admin", "admin123" }; // Varsayilan yonetici bilgileri
int book_count = 0;   // Mevcut kitap sayisi
int user_count = 0;   // Mevcut kullanici sayisi

// Konsol ekranini temizleme fonksiyonu
void clear_screen() {
#ifdef _WIN32   // Eger isletim sistemi Windows ise
    system("cls"); // "cls" komutunu calistir
#else           // Diger isletim sistemleri (Linux/macOS) icin
    system("clear"); // "clear" komutunu calistir
#endif
}

// Menu kenarligi cizme fonksiyonu
void draw_menu_border() {
    printf(CYN "============================================================\n" RESET);
}

// Metni ortalamak icin yardimci fonksiyon
void print_centered(const char* text) {
    int len = strlen(text);          // Metnin uzunlugunu al
    int padding = (CONSOLE_WIDTH - len) / 2; // Ortalamak icin bosluk miktarini hesapla

    // Metnin soluna gerekli boslugu ekle
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }

    // Metni yazdir
    printf("%s", text);

    printf("\n"); // Yeni satira gec
}

// Kitap verilerini dosyaya kaydetme fonksiyonu
void save_books() {
    FILE* fp = fopen(BOOKS_FILE, "wb"); // Kitap dosyasini ikili yazma modunda ac
    if (fp == NULL) {                // Dosya acilamazsa hata mesaji ver
        perror(RED "Dosya acma hatasi (kitap kaydetme): " RESET);
        return;
    }
    // Tum kitaplari donguyle dosyaya yaz
    for (int i = 0; i < book_count; i++) {
        fwrite(&books[i], sizeof(Book), 1, fp);
    }
    fclose(fp); // Dosyayi kapat
    printf(GRN "Kitaplar basariyla kaydedildi.\n" RESET);
}

// Kullanici verilerini dosyaya kaydetme fonksiyonu
void save_users() {
    FILE* fp = fopen(USERS_FILE, "wb"); // Kullanici dosyasini ikili yazma modunda ac
    if (fp == NULL) {                // Dosya acilamazsa hata mesaji ver
        perror(RED "Dosya acma hatasi (kullanici kaydetme): " RESET);
        return;
    }
    fwrite(users, sizeof(User), user_count, fp); // Tum kullanicilari dosyaya yaz
    fclose(fp); // Dosyayi kapat
    printf(GRN "Kullanicilar basariyla kaydedildi.\n" RESET);
}

// Kitap verilerini dosyadan yukleme fonksiyonu
void load_books() {
    FILE* fp = fopen(BOOKS_FILE, "rb"); // Kitap dosyasini ikili okuma modunda ac
    if (fp == NULL) {                // Dosya yoksa veya acilamazsa, yeni dosya olusturulacagini belirt
        perror(YEL "Kitap dosyasi bulunamadi. Yeni bir kitap dosyasi olusturulacak.\n" RESET);
        return;
    }
    book_count = 0; // Kitap sayacini sifirla
    Book temp_book;  // Gecici kitap yapisi
    // Dosyadan kitaplari tek tek oku
    while (fread(&temp_book, sizeof(Book), 1, fp) == 1) {
        temp_book.id = book_count + 1; // Yukleme sirasina gore ID ata
        books[book_count++] = temp_book; // Kitabi diziye ekle ve sayaci artir
    }
    fclose(fp); // Dosyayi kapat
    printf(GRN "%d kitap basariyla yuklendi.\n" RESET, book_count);
}

// Kullanici verilerini dosyadan yukleme fonksiyonu
void load_users() {
    FILE* fp = fopen(USERS_FILE, "rb"); // Kullanici dosyasini ikili okuma modunda ac
    if (fp == NULL) {                // Dosya yoksa veya acilamazsa, yeni dosya olusturulacagini belirt
        perror(YEL "Kullanici dosyasi bulunamadi. Yeni bir kullanici dosyasi olusturulacak.\n" RESET);
        return;
    }
    user_count = fread(users, sizeof(User), MAX_USERS, fp); // Tum kullanicilari dosyadan oku
    fclose(fp); // Dosyayi kapat
    printf(GRN "%d kullanici basariyla yuklendi.\n" RESET, user_count);
}

// Yeni kitap ekleme fonksiyonu
void add_book() {
    // Kitap sayisi maksimum sinira ulastiysa hata ver
    if (book_count >= MAX_BOOKS) {
        printf(RED "Kitap sayisi sinirini astiniz. (Maksimum: %d)\n" RESET, MAX_BOOKS);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar(); // Bekletme icin
        (void)getchar(); // \n karakterini temizlemek icin
        return;
    }
    Book new_book;                  // Yeni kitap yapisi olustur
    new_book.id = book_count + 1; // Yeni kitaba mevcut sayi + 1 olarak ID ata

    printf(CYN "Kitap basligini girin: " RESET);
    fgets(new_book.title, sizeof(new_book.title), stdin); // Basligi oku
    new_book.title[strcspn(new_book.title, "\n")] = 0;     // Yeni satir karakterini kaldir

    printf(CYN "Yazar adini girin: " RESET);
    fgets(new_book.author, sizeof(new_book.author), stdin); // Yazar adini oku
    new_book.author[strcspn(new_book.author, "\n")] = 0;     // Yeni satir karakterini kaldir

    printf(CYN "Kategoriyi girin: " RESET);
    fgets(new_book.category, sizeof(new_book.category), stdin); // Kategoriyi oku
    new_book.category[strcspn(new_book.category, "\n")] = 0;     // Yeni satir karakterini kaldir

    new_book.is_borrowed = 0;    // Yeni kitap varsayilan olarak mevcut (odunc alinmamis)
    books[book_count] = new_book; // Kitabi diziye ekle
    book_count++;                  // Kitap sayisini artir
    printf(GRN "Kitap basariyla eklendi. Toplam kitap sayisi: %d, Yeni ID: %d\n" RESET, book_count, new_book.id);

    save_books(); // Kitaplari dosyaya kaydet

    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Yeni kullanici kaydi olusturma fonksiyonu
void register_user() {
    // Kullanici sayisi maksimum sinira ulastiysa hata ver
    if (user_count >= MAX_USERS) {
        printf(RED "Kullanici sayisi sinirini astiniz. (Maksimum: %d)\n" RESET, MAX_USERS);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        (void)getchar();
        return;
    }

    User new_user; // Yeni kullanici yapisi olustur
    printf(CYN "Yeni kullanici adi: " RESET);
    (void)scanf(" %49s", new_user.username); // Kullanici adini oku
    while ((getchar()) != '\n'); // Geri kalan karakterleri temizle

    // Kullanici adinin zaten kullanilip kullanilmadigini kontrol et
    for (int i = 0; i < user_count; i++) {
        if (strcmp(new_user.username, users[i].username) == 0) {
            printf(RED "Bu kullanici adi zaten kullaniliyor. Lutfen baska bir kullanici adi deneyin.\n" RESET);
            printf("Devam etmek icin bir tusa basin...\n");
            (void)getchar();
            return;
        }
    }

    printf(CYN "Yeni sifre: " RESET);
    (void)scanf(" %49s", new_user.password); // Sifreyi oku
    while ((getchar()) != '\n'); // Geri kalan karakterleri temizle

    new_user.id = user_count + 1; // Yeni kullaniciya ID ata
    new_user.borrowed_books_count = 0; // Baslangicta odunc aldigi kitap sayisi 0

    users[user_count] = new_user; // Kullaniciyi diziye ekle
    user_count++;                  // Kullanici sayisini artir
    printf(GRN "Kayit Basarili! Hos geldiniz, " BOLD CYN "%s" RESET GRN "!\n" RESET, new_user.username);

    save_users(); // Kullanicilari dosyaya kaydet

    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Yalnizca yonetici tarafindan yeni kullanici ekleme fonksiyonu (kayittan farkli)
void add_user() {
    // Kullanici sayisi maksimum sinira ulastiysa hata ver
    if (user_count >= MAX_USERS) {
        printf(RED "Kullanici sayisi sinirini astiniz. (Maksimum: %d)\n" RESET, MAX_USERS);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        (void)getchar();
        return;
    }

    User new_user; // Yeni kullanici yapisi olustur
    printf(CYN "Kullanici adi: " RESET);
    (void)scanf(" %49s", new_user.username); // Kullanici adini oku
    while ((getchar()) != '\n'); // Geri kalan karakterleri temizle
    printf(CYN "Sifre: " RESET);
    (void)scanf(" %49s", new_user.password); // Sifreyi oku
    while ((getchar()) != '\n'); // Geri kalan karakterleri temizle
    new_user.id = user_count + 1; // Yeni kullaniciya ID ata
    new_user.borrowed_books_count = 0; // Baslangicta odunc aldigi kitap sayisi 0

    users[user_count] = new_user; // Kullaniciyi diziye ekle
    user_count++;                  // Kullanici sayisini artir
    printf(GRN "Kullanici basariyla eklendi. Toplam kullanici sayisi: %d\n" RESET, user_count);

    save_users(); // Kullanicilari dosyaya kaydet

    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
    (void)getchar();
}

// Yonetici girisi fonksiyonu
int admin_login() {
    char username[50]; // Kullanici adi icin tampon
    char password[50]; // Sifre icin tampon

    printf(CYN "Yonetici kullanici adi: " RESET);
    (void)scanf(" %49s", username);      // Kullanici adini oku
    while ((getchar()) != '\n');         // Geri kalan karakterleri temizle
    printf(CYN "Yonetici sifre: " RESET);
    (void)scanf(" %49s", password);      // Sifreyi oku
    while ((getchar()) != '\n');         // Geri kalan karakterleri temizle

    // Girilen kullanici adi ve sifreyi varsayilan yonetici bilgileriyle karsilastir
    if (strcmp(username, admin.username) == 0 && strcmp(password, admin.password) == 0) {
        printf(GRN "Yonetici girisi basarili.\n" RESET);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        return 1; // Basarili giris
    } else {
        printf(RED "Gecersiz kullanici adi veya sifre.\n" RESET);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        return 0; // Basarisiz giris
    }
}


// Geciken kitaplari kontrol etme fonksiyonu
void check_overdue_books() {
    time_t now = time(NULL); // Guncel zamani al
    printf(CYN "\n--- Geciken Kitaplar ---\n" RESET);
    int overdue_found = 0; // Geciken kitap bulunup bulunmadigini izleyen bayrak

    // Tum kullanicilari ve tum kitaplari kontrol et
    for (int i = 0; i < user_count; i++) {
        for (int j = 0; j < book_count; j++) {
            // Eger kitap odunc alinmissa ve odunc alma tarihi ile guncel zaman arasindaki fark gecikme suresinden fazlaysa
            if (books[j].is_borrowed && difftime(now, books[j].borrow_date) > OVERDUE_DAYS * 24 * 3600) {
                printf(RED "Kullanici: %s, Geciken Kitap: %s (ID: %d)\n" RESET,users[i].username, books[j].title, books[j].id);
                overdue_found = 1; // Geciken kitap bulundu
            }
        }
    }
    if (!overdue_found) { // Geciken kitap bulunamadiysa
        printf(GRN "Geciken kitap bulunmamaktadir.\n" RESET);
    }
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Yonetici menusu fonksiyonu
void admin_menu() {
    int choice; // Kullanicinin menu secimi

    while (1) { // Menuden cikis yapilana kadar donguyu surdur
        clear_screen();      // Ekrani temizle
        draw_menu_border();  // Menu kenarligini ciz
        print_centered(BOLD CYN "Yonetici Menusu" RESET); // Basligi ortala
        draw_menu_border();  // Menu kenarligini ciz
        printf(GRN "1." WHT " Kullanici Ekle\n" RESET);
        printf(GRN "2." WHT " Kitap Ekle\n" RESET);
        printf(GRN "3." WHT " Geciken Kitaplari Kontrol Et\n" RESET);
        printf(GRN "4." WHT " Kitap Guncelle\n" RESET);
        printf(GRN "5." WHT " Kitap Sil\n" RESET);
        printf(GRN "0." WHT " Cikis\n" RESET);
        draw_menu_border();
        printf(YEL "Seciminizi yapin: " RESET);
        int result;
        result = scanf("%d", &choice); // Kullanicinin secimini al
        while ((getchar()) != '\n');    // Giris tamponunu temizle

        // Secime gore ilgili fonksiyonu cagir
        switch (choice) {
        case 1:
            clear_screen();
            add_user();
            break;
        case 2:
            clear_screen();
            add_book();
            break;
        case 3:
            clear_screen();
            check_overdue_books();
            break;
        case 4:
            clear_screen();
            update_book(); // Kitap guncelleme fonksiyonunu cagir
            break;
        case 5:
            clear_screen();
            delete_book(); // Kitap silme fonksiyonunu cagir
            break;
        case 0:
            clear_screen();
            return; // Yonetici menusunden cik
        default:
            printf(RED "Gecersiz secim. Lutfen tekrar deneyin.\n" RESET);
            printf("Devam etmek icin bir tusa basin...\n");
            (void)getchar();
            break;
        }
    }
}

// Kitap bilgilerini guncelleme fonksiyonu
void update_book() {
    char title[100]; // Aranacak kitap basligi icin tampon
    printf(CYN "Guncellemek istediginiz kitabin basligini girin: " RESET);
    fgets(title, sizeof(title), stdin);        // Basligi oku
    title[strcspn(title, "\n")] = 0;           // Yeni satir karakterini kaldir

    int found = 0; // Kitap bulunup bulunmadigini izleyen bayrak

    // Kitaplari donguyle ara
    for (int i = 0; i < book_count; i++) {
        // Baslik buyuk/kucuk harf duyarsiz olarak eslesirse
        if (strcasecmp(books[i].title, title) == 0) {
            found = 1; // Kitap bulundu

            // Yeni bilgileri al
            printf(CYN "Yeni kitap basligini girin (eski: %s): " RESET, books[i].title);
            fgets(books[i].title, sizeof(books[i].title), stdin);
            books[i].title[strcspn(books[i].title, "\n")] = 0; // Yeni satir karakterini sil (/N)

            printf(CYN "Yeni yazar adini girin (eski: %s): " RESET, books[i].author);
            fgets(books[i].author, sizeof(books[i].author), stdin);
            books[i].author[strcspn(books[i].author, "\n")] = 0;

            printf(CYN "Yeni kategori girin (eski: %s): " RESET, books[i].category);
            fgets(books[i].category, sizeof(books[i].category), stdin);
            books[i].category[strcspn(books[i].category, "\n")] = 0;

            printf(GRN "Kitap bilgileri basariyla guncellendi.\n" RESET);
            break; // Guncelleme yapildi, donguden cik
        }
    }

    if (!found) { // Kitap bulunamadiysa mesaj ver
        printf(RED "Belirtilen baslikta kitap bulunamadi: %s\n" RESET, title);
    }

    save_books(); // Guncel kitaplari dosyaya kaydet

    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Kitap silme fonksiyonu
void delete_book() {
    char title[100]; // Silinecek kitap basligi icin tampon
    printf(CYN "Silmek istediginiz kitap basligini girin: " RESET);
    fgets(title, sizeof(title), stdin);        // Basligi oku
    title[strcspn(title, "\n")] = 0;           // Yeni satir karakterini kaldir

    int found = 0; // Kitap bulunup bulunmadigini izleyen bayrak

    // Kitaplari donguyle ara
    for (int i = 0; i < book_count; i++) {
        // Baslik buyuk/kucuk harf duyarsiz olarak eslesirse
        if (strcasecmp(books[i].title, title) == 0) {
            found = 1; // Kitap bulundu
            // Kitabi diziden silmek icin kaydirma islemi yap
            for (int j = i; j < book_count - 1; j++) {
                books[j] = books[j + 1];
            }
            book_count--; // Kitap sayisini azalt
            printf(GRN "'%s' baslikli kitap basariyla silindi.\n" RESET, title);
            break; // Silme yapildi, donguden cik
        }
    }

    if (!found) { // Kitap bulunamadiysa mesaj ver
        printf(RED "Belirtilen baslikta kitap bulunamadi: %s\n" RESET, title);
    }

    save_books(); // Guncel kitaplari dosyaya kaydet

    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Kullanici girisi fonksiyonu
int user_login() {
    char username[50]; // Kullanici adi icin tampon
    char password[50]; // Sifre icin tampon

    printf(CYN "Kullanici adi: " RESET);
    (void)scanf(" %49s", username); // Kullanici adini oku
    while ((getchar()) != '\n');    // Geri kalan karakterleri temizle
    printf(CYN "Sifre: " RESET);
    (void)scanf(" %49s", password); // Sifreyi oku
    while ((getchar()) != '\n');    // Geri kalan karakterleri temizle

    // Kullanicilari donguyle ara
    for (int i = 0; i < user_count; i++) {
        // Girilen kullanici adi ve sifre eslesirse
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            printf(GRN "Kullanici girisi basarili.\n" RESET);
            printf("Devam etmek icin bir tusa basin...\n");
            (void)getchar();
            return i; // Giris basariliysa kullanicinin indeksini dondur
        }
    }
    printf(RED "Gecersiz kullanici adi veya sifre.\n" RESET);
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
    return -1; // Kullanici bulunamadi
}

// Kitap arama fonksiyonu
void search_book() {
    char search_term[100]; // Arama terimi icin tampon
    printf(CYN "Aramak istediginiz kitap basligini veya yazar adini girin: " RESET);
    fgets(search_term, sizeof(search_term), stdin);    // Arama terimini oku
    search_term[strcspn(search_term, "\n")] = 0;        // Yeni satir karakterini kaldir

    int found = 0; // Kitap bulunup bulunmadigini izleyen bayrak
    printf(CYN "\n--- Arama SonucLari ---\n" RESET);
    // Tum kitaplari donguyle kontrol et
    for (int i = 0; i < book_count; i++) {
        // Kitap basliginda veya yazar adinda arama terimi bulunursa
        if (strstr(books[i].title, search_term) != NULL || strstr(books[i].author, search_term) != NULL) {
            // Kitap bilgilerini yazdir
            printf(BLU "Kitap ID: " WHT "%d\n" RESET, books[i].id);
            printf(BLU "Baslik: " WHT "%s\n" RESET, books[i].title);
            printf(BLU "Yazar: " WHT "%s\n" RESET, books[i].author);
            printf(BLU "Kategori: " WHT "%s\n" RESET, books[i].category);
            printf(BLU "Durum: %s\n" RESET, books[i].is_borrowed ? RED "Oduncte" RESET : GRN "Raflarda" RESET);
            printf("\n");
            found = 1; // Kitap bulundu
        }
    }
    if (!found) { // Kitap bulunamadiysa mesaj ver
        printf(RED "Aradiginiz kriterlere uygun kitap bulunamadi.\n" RESET);
    }
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

// Kategoriye gore kitaplari listeleme fonksiyonu
void list_books_by_category() {
    char category[50]; // Kategori adi icin tampon
    printf(CYN "Listelemek istediginiz kategori: " RESET);
    (void)scanf(" %49s", category); // Kategori adini oku
    while ((getchar()) != '\n');    // Geri kalan karakterleri temizle

    int found = 0; // Kitap bulunup bulunmadigini izleyen bayrak
    printf(CYN "\n--- %s Kategorisindeki Kitaplar ---\n" RESET, category);
    // Tum kitaplari donguyle kontrol et
    for (int i = 0; i < book_count; i++) {
        // Kitabin kategorisi eslesirse
        if (strcmp(books[i].category, category) == 0) {
            // Kitap bilgilerini yazdir
            printf(BLU "Kitap ID: " WHT "%d\n" RESET, books[i].id);
            printf(BLU "Baslik: " WHT "%s\n" RESET, books[i].title);
            printf(BLU "Yazar: " WHT "%s\n" RESET, books[i].author);
            printf(BLU "Durum: %s\n" RESET, books[i].is_borrowed ? RED "Oduncte" RESET : GRN "Raflarda" RESET);
            printf("\n");
            found = 1; // Kitap bulundu
        }
    }
    if (!found) { // Kitap bulunamadiysa mesaj ver
        printf(RED "Bu kategoriye ait kitap bulunamadi.\n" RESET);
    }
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();
}

void borrow_book(int user_index) {
    if (users[user_index].borrowed_books_count >= 3) {
        printf(RED "En fazla 3 kitap odunc alabilirsiniz.\n" RESET);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        return;
    }

    int book_id;// Odünc alinacak kitabın ID'si için değişken
    printf(CYN "Odunc almak istediginiz kitap ID'sini girin: " RESET);
    int result;
    result = scanf("%d", &book_id);// Kullanicinin girdiği ID'yi oku
    while ((getchar()) != '\n');
    // kitap ID'sinin gecerli olup olmadigini veya kitabin odunc alinip alinmadigini kontrol eder
    if (book_id < 1 || book_id > book_count || books[book_id - 1].is_borrowed) {
        printf(RED "Gecersiz kitap ID'si veya kitap zaten oduncte.\n" RESET);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        return;
    }

    books[book_id - 1].is_borrowed = 1;//kitabin durumunu odunc alindi olarak guncelle
    books[book_id - 1].borrow_date = time(NULL);//kitabin odunc alinma tarihini simdiki zaman olarak ayarla
    // Kitabı ödünç alan kullanicinin ID'sini kaydet
    books[book_id - 1].borrower_user_id = users[user_index].id;
    users[user_index].borrowed_books_count++;// kullanicinin odunc aldigi kitap sayisini arttir
    printf(GRN "Kitap basariyla odunc alindi.\n" RESET);
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();

    // Değişiklikleri kaydetmeyi unutmayın
    save_books();
}
void return_book(int user_index) {
    int book_id;
    printf(CYN "Iade etmek istediginiz kitap ID'sini girin: " RESET);
    int result;
    result = scanf("%d", &book_id);
    while ((getchar()) != '\n');

    // Hata ayıklama için eklenen satırlar (daha sonra kaldırabilirsiniz)
    printf(YEL "Iade Edilmeye Calisilan Kitap ID: %d\n" RESET, book_id);
    if (book_id >= 1 && book_id <= book_count) {
        printf(YEL "Kitap '%s' Odunc Durumu: %d, Odunc Alan Kullanici ID: %d\n" RESET, books[book_id - 1].title, books[book_id - 1].is_borrowed, books[book_id - 1].borrower_user_id);
    }

    // Kitap ID'sinin gecerli olup olmadigini, odunc alinip alinmadigini VE mevcut kullanicinin o kitabı ödünc alip almadigini kontrol et
    if (book_id < 1 || book_id > book_count || !books[book_id - 1].is_borrowed || books[book_id - 1].borrower_user_id != users[user_index].id) {
        printf(RED "Gecersiz kitap ID'si, kitap rafta veya bu kitap sizin uzerinizde degil.\n" RESET);
        printf("Devam etmek icin bir tusa basin...\n");
        (void)getchar();
        return;
    }

    books[book_id - 1].is_borrowed = 0;//kitabin durumunu odunc alinmamis (raflarda) olarak guncelle
    books[book_id - 1].borrower_user_id = 0; // Odunç alan ID'sini sıfırla
    users[user_index].borrowed_books_count--;//kullanicinin odunc aldigi kitap sayisini azalt
    printf(GRN "Kitap basariyla iade edildi.\n" RESET);
    printf("Devam etmek icin bir tusa basin...\n");
    (void)getchar();

    // Değişiklikleri kaydetmeyi unutmayın
    save_books();
}
void view_borrowed_books(int user_index) {
    printf(CYN "\n--- Uzerinizdeki Kitaplar ---\n" RESET); // Baslik yazdir
    int borrowed_found = 0; // Bu degisken, kullanicinin odunc aldigi bir kitap bulunup bulunmadigini takip eder
    time_t now = time(NULL); // Simdiki zamani al, gecikme hesaplamasi icin kullanilacak

    // Tum kitaplari dongu ile kontrol et
    for (int i = 0; i < book_count; i++) {
        // Her bir kitabi iki kosula gore filtrele:
        // 1. Kitap odunc alinmis mi? (books[i].is_borrowed == 1)
        // 2. Kitabi odunc alan kullanicinin ID'si, o anki oturum acmis kullanicinin ID'si ile eslesiyor mu?
        if (books[i].is_borrowed && books[i].borrower_user_id == users[user_index].id) {
            // Eger kosullar saglanirsa, kitabin bilgilerini yazdir
            printf(BLU "Kitap ID: " WHT "%d\n" RESET, books[i].id);
            printf(BLU "Baslik: " WHT "%s\n" RESET, books[i].title);
            printf(BLU "Yazar: " WHT "%s\n" RESET, books[i].author);
            printf(BLU "Kategori: " WHT "%s\n" RESET, books[i].category);

            // Kitabin ne kadar suredir odunc alindigini saniye cinsinden hesapla
            double seconds_borrowed = difftime(now, books[i].borrow_date);
            // Saniyeleri gune cevir
            int days_borrowed = (int)(seconds_borrowed / (60 * 60 * 24));
            // Gecikme gun sayisini hesapla (eger varsa)
            int overdue_days = days_borrowed - OVERDUE_DAYS; // OVERDUE_DAYS 15 olarak tanimlanmis

            // Iade edilmesi gereken tarihi hesapla (odunc alma tarihi + izin verilen gun sayisi)
            time_t return_date = books[i].borrow_date + (OVERDUE_DAYS * 24 * 3600);
            // Iade tarihini okunabilir formatta yazdir
            printf(GRN "Iade tarihi: %s", ctime(&return_date));

            // Eger gecikme varsa, gecikme uyarisi yazdir
            if (overdue_days > 0) {
                printf(RED "!!! Gecikme: %d gun\n" RESET, overdue_days);
            }
            printf("\n"); // Kitap bilgileri arasina bosluk birak
            borrowed_found = 1; // En az bir odunc alinmis kitap bulundugunu isaretle
        }
    }
    // Eger hicbir odunc alinmis kitap bulunamadiysa ilgili mesaji yazdir
    if (!borrowed_found) {
        printf(GRN "Uzerinizde odunc alinmis kitap bulunmamaktadir.\n" RESET);
    }
    printf("Devam etmek icin bir tusa basin...\n"); // Kullanicidan devam etmesini bekle
    (void)getchar(); // Tusa basilana kadar bekler
}


// Kullanici menusu fonksiyonu
void user_menu(int user_index) {
    int choice; // Kullanicinin menu secimi
    while (1) { // Menuden cikis yapilana kadar donguyu surdur
        clear_screen();      // Ekrani temizle
        draw_menu_border();  // Menu kenarligini ciz
        print_centered(BOLD CYN "Kullanici Menusu" RESET); // Basligi ortala
        draw_menu_border();  // Menu kenarligini ciz
        printf(GRN "1." WHT " Kitap Ara\n" RESET);
        printf(GRN "2." WHT " Kitap Odunc Al\n" RESET);
        printf(GRN "3." WHT " Kitap Iade Et\n" RESET);
        printf(GRN "4." WHT " Uzerimdeki Kitaplari Goruntule\n" RESET);
        printf(GRN "5." WHT " Kategori Bazli Kitaplari Listele\n" RESET);
        printf(GRN "0." WHT " Cikis\n" RESET);
        draw_menu_border();
        printf(YEL "Seciminizi yapin: " RESET);
        int result;
        result = scanf("%d", &choice); // Kullanicinin secimini al
        while ((getchar()) != '\n');    // Giris tamponunu temizle

        // Secime gore ilgili fonksiyonu cagir
        switch (choice) {
        case 1:
            clear_screen();
            search_book();
            break;
        case 2:
            clear_screen();
            borrow_book(user_index);
            break;
        case 3:
            clear_screen();
            return_book(user_index);
            break;
        case 4:
            clear_screen();
            view_borrowed_books(user_index);
            break;
        case 5:
            clear_screen();
            list_books_by_category();
            break;
        case 0:
            clear_screen();
            return; // Kullanici menusunden cik
        default:
            printf(RED "Gecersiz secim. Lutfen tekrar deneyin.\n" RESET);
            printf("Devam etmek icin bir tusa basin...\n");
            (void)getchar();
            break;
        }
    }
}

// Ana fonksiyon
int main() {
    int choice; // Ana menu secimi

    load_books(); // Uygulama baslangicinda kitaplari yukle
    load_users(); // Uygulama baslangicinda kullanicilari yukle

    while (1) { // Ana menuden cikis yapilana kadar donguyu surdur
        clear_screen();      // Ekrani temizle
        draw_menu_border();  // Menu kenarligini ciz
        print_centered(BOLD CYN "Kutuphane Sistemine Hos Geldiniz" RESET); // Basligi ortala
        draw_menu_border();  // Menu kenarligini ciz
        printf(GRN "1." WHT " Yonetici Girisi\n" RESET);
        printf(GRN "2." WHT " Kullanici Girisi\n" RESET);
        printf(GRN "3." WHT " Kayit Ol\n" RESET);
        printf(GRN "0." WHT " Cikis\n" RESET);
        draw_menu_border();
        printf(YEL "Seciminizi yapin: " RESET);
        int result;
        result = scanf("%d", &choice); // Kullanicinin secimini al
        while ((getchar()) != '\n');    // Giris tamponunu temizle

        // Secime gore ilgili menuye yonlendir
        switch (choice) {
        case 1:
            clear_screen();
            if (admin_login()) { // Yonetici girisi basariliysa
                clear_screen();
                admin_menu();  // Yonetici menusune git
            }
            break;
        case 2: {
            clear_screen();
            int user_index = user_login(); // Kullanici girisi yap ve indeksini al
            if (user_index != -1) { // Kullanici girisi basariliysa
                clear_screen();
                user_menu(user_index); // Kullanici menusune git
            }
            break;
        }
        case 3:
            clear_screen();
            register_user(); // Yeni kullanici kaydi yap
            break;
        case 0:
            clear_screen();
            printf(CYN "Cikis yapiliyor...\n" RESET);
            save_books(); // Cikis yapmadan once kitaplari kaydet
            save_users(); // Cikis yapmadan once kullanicilari kaydet
            return 0;      // Programi sonlandir
        default:
            printf(RED "Gecersiz secim. Lutfen tekrar deneyin.\n" RESET);
            printf("Devam etmek icin bir tusa basin...\n");
            (void)getchar();
            break;
        }
    }

    return 0;
}
