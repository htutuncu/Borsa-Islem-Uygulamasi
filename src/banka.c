#include "banka.h"
#include "jsmn.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_SIZE 10000

// Yardimci fonksiyonlar
// C++ dilinde mevcut olan strndup() fonksiyonu implement edildi.
// Gonderilen deger kadar olan kismi kopyalayarak string geri dondurur. 
char *strndup(const char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}

// Bu fonksiyon jsmn.h kütüphanesinin bir fonksiyonudur. 
// Karsilastirma yapmak icin kullanilir. Eger gonderilen stringler eslesiyorsa 0 dondurur.
static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}


// Banka nesnesinin yapici fonksiyonudur. Yeni bir banka nesnesi olusturarak bu nesneyi dondurur.
// Banka nesnesi icerisinde hissler,portfoyKaydi ve emirler seklinde diziler tutar.
// Bu dizilerin bellekte yer ayrilma asamasi da yine bu yapici fonksiyon icerisinde yapilir.
Banka BankaOlustur(){
    Banka this;
    this = (Banka)malloc(sizeof(struct BANKA));
    this->hisseler = (Hisse*)malloc(MAX_SIZE*sizeof(struct HISSE));
    this->portfoyKaydi = (Portfoy*)malloc(MAX_SIZE*sizeof(struct PORTFOY));
    this->emirler = (Emir*)malloc(MAX_SIZE*sizeof(struct EMIR));
    
    // Fonksiyon baglamalari.
    this->hisseEkle = &HisseEkle;
    this->portfoyEkle = &PortfoyEkle;
    this->emirEkle = &EmirEkle;
    this->hisseOku = &HisseOku;
    this->portfoyOku = &PortfoyOku;
    this->emirOku = &EmirOku;
    this->bankaYoket = &BankaYoket;
    this->emirleriGerceklestir = &EmirleriGerceklestir;


    this->hisse_index = 0;
    this->portfoy_index = 0;
    this->emir_index = 0;

    return this;
}

// Bu fonksiyon gonderilen Banka nesnesi icerisindeki hisselere yeni bir hisse eklemesi yapar.
// Eklenecek yeni hisse de parametre olarak alinir.
void HisseEkle(const Banka this,const Hisse hisse){
    int idx = this->hisse_index;
    this->hisseler[idx] = hisse;
    
    this->hisse_index++;
}

// Gonderilen banka nesnesi icerisindeki portfoy kaydina yeni bir portfoy eklemesi yapar.
void PortfoyEkle(const Banka this,const Portfoy portfoy){
    int idx = this->portfoy_index;
    this->portfoyKaydi[idx] = portfoy;
    this->portfoy_index++;
}

// Gonderilen banka nesnesi icerisindeki emirlere yeni bir emir eklemesi yapar.
void EmirEkle(const Banka this, const Emir emir){
    int idx = this->emir_index;
    this->emirler[idx] = emir;
    this->emir_index++;
}

// Bu fonksiyon gonderilen dosya ismindeki dosyayi acarak okur.
// Okunan JSON dosyasi gerekli verilere gore parcalanir ve banka icerisindeki hisseler dizisine doldurulur.
// jsmn.h kutuphanesi orneklerinden uyarlanilmistir.
int HisseOku( Banka banka,const char* filename){
    int i,r;
    jsmn_parser p;
    jsmntok_t t[MAX_SIZE]; 

    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen(filename, "rb");
    if (!fp)
        perror(filename), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    // Dosya iceriginin tamami icin yer ayrilmasi yapiliyor. 
    buffer = calloc(1, lSize + 1);
    if (!buffer)
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    // Tum dosya icerigi buffer degiskenine ataniyor.
    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    char *JSON_STRING = buffer;

    
    // Eger dosyanin parcalanma asamasinda hatalar olusursa bu kisimda yakalaniyor.
    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING),t,sizeof(t) / sizeof(t[0]));
    if (r < 0){
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT){
        printf("Object expected\n");
        return 1;
    }


    char* semboller[MAX_SIZE];
    double fiyatlar[MAX_SIZE];
    int ix=0;
    // Dosya icerigi okunurken dosyayi dolasan dongu.
    for (i = 1; i < r; i++)
    {
        // Sadece sembol ismi ve fiyat bilgisi yeterli. Dolayisiyla diger bilgiler okunmasina gerek yok.
        char *str1 = " ";
        double fyt;
        if (jsoneq(JSON_STRING, &t[i], "Sembol") == 0){
            // Sembole karsilik gelen veri burada ayiklanarak atama yapiliyor.
            str1 = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);

            semboller[ix] = str1;
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "Fiyat") == 0){
            // Fiyata karsilik gelen veri burada ayiklaniyor.
            char *str = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            
            // String tipindeki veri double veri tipine donusturuluyor.
            fyt = strtod(str,NULL);
            fiyatlar[ix] = fyt;
            
            i++;
            
            ix++;
        }
    }

    // Dosyanin taramasi bittikten sonra doldurulmus olan diziler kullanilarak
    // hisseler olusturuluyor. Hisseler kaydedilmek uzere banka'ya gonderiliyor.
    for(i=0; i<ix; i++){
        Hisse hisse = HisseOlustur(fiyatlar[i],semboller[i]);
        banka->hisseEkle(banka,hisse);
    }
    fclose(fp);
    free(buffer);
}

// Bu fonksiyon gonderilen dosya ismindeki dosyayi acarak okur.
// Okunan JSON dosyasi gerekli verilere gore parcalanir ve banka icerisindeki portfoy kaydi dizisine doldurulur.
// jsmn.h kutuphanesi orneklerinden uyarlanilmistir.
int PortfoyOku(const Banka banka,const char* filename){
    int i,r;
    jsmn_parser p;
    jsmntok_t t[MAX_SIZE];

    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen(filename, "rb");
    if (!fp)
        perror(filename), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    // Dosya iceriginin tamami icin yer ayrilmasi yapiliyor. 
    buffer = calloc(1, lSize + 1);
    if (!buffer)
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    // Tum dosya icerigi buffer degiskenine ataniyor.
    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    char *JSON_STRING = buffer;

    
    // Eger dosyanin parcalanma asamasinda hatalar olusursa bu kisimda yakalaniyor.
    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING),t,sizeof(t) / sizeof(t[0]));
    if (r < 0){
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT){
        printf("Object expected\n");
        return 1;
    }


    char* semboller[MAX_SIZE];
    double maliyetler[MAX_SIZE];
    int adetler[MAX_SIZE];
    int ix=0;
    // Dosya icerigi okunurken dosyayi dolasan dongu.
    for (i = 1; i < r; i++)
    {
        // Sadece sembol, adet ve fiyat bilgisi yeterli. Dolayisiyla diger bilgiler okunmasina gerek yok.
        char *str1 = " ";
        double fyt;
        int adet;
        if (jsoneq(JSON_STRING, &t[i], "Sembol") == 0)
        {   // Sembole karsilik gelen veri burada ayiklanarak atama yapiliyor.
            str1 = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);

            semboller[ix] = str1;
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "Maliyet") == 0)
        {   // Maliyete karsilik gelen veri burada ayiklaniyor.
            char *str = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            // String tipindeki veri double veri tipine donusturuluyor.
            fyt = strtod(str,NULL);
           
            maliyetler[ix] = fyt;
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "Adet") == 0)
        {   // Adete karsilik gelen veri burada ayiklaniyor.
            char *str = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            // String tipindeki veri integer veri tipine donusturuluyor.
            adet = atoi(str);
            adetler[ix] = adet;
            i++;
            ix++;
        }
    }
    // Dosyanin taramasi bittikten sonra doldurulmus olan diziler kullanilarak
    // portfoyler olusturuluyor.Kaydedilmek uzere banka'ya gonderiliyor.
    for(i=0; i<ix; i++){
        Portfoy portfoy = PortfoyOlustur(semboller[i],maliyetler[i],adetler[i]);
        banka->portfoyEkle(banka,portfoy);
    }
    fclose(fp);
    free(buffer);
}

// Bu fonksiyon gonderilen dosya ismindeki dosyayi acarak okur.
// Okunan JSON dosyasi gerekli verilere gore parcalanir ve banka icerisindeki emirler dizisine doldurulur.
// jsmn.h kutuphanesi orneklerinden uyarlanilmistir.
int EmirOku(const Banka banka,const char* filename){
    int i,r;
    jsmn_parser p;
    jsmntok_t t[MAX_SIZE]; 

    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen(filename, "rb");
    if (!fp)
        perror(filename), exit(1);

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    // Dosya iceriginin tamami icin yer ayrilmasi yapiliyor. 
    buffer = calloc(1, lSize + 1);
    if (!buffer)
        fclose(fp), fputs("memory alloc fails", stderr), exit(1);

    // Tum dosya icerigi buffer degiskenine ataniyor.
    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

    char *JSON_STRING = buffer;

    
    // Eger dosyanin parcalanma asamasinda hatalar olusursa bu kisimda yakalaniyor.
    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING),t,sizeof(t) / sizeof(t[0]));
    if (r < 0){
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT){
        printf("Object expected\n");
        return 1;
    }


    char* semboller[MAX_SIZE];
    char* islemler[MAX_SIZE];
    int adetler[MAX_SIZE];
    int ix=0;
    // Dosya icerigi okunurken dosyayi dolasan dongu.
    for (i = 1; i < r; i++)
    {
        // Sadece sembol, adet ve islem bilgisi yeterli. Dolayisiyla diger bilgiler okunmasina gerek yok.
        char *str1 = " ";
        char *str2 = " ";
        int adet;
        if (jsoneq(JSON_STRING, &t[i], "Sembol") == 0)
        {   // Sembole karsilik gelen veri burada ayiklanarak atama yapiliyor.
            str1 = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            
            semboller[ix] = str1;
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "Islem") == 0)
        {   // Isleme karsilik gelen veri burada ayiklanarak atama yapiliyor.
            char *str = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            islemler[ix] = str;
            
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "Adet") == 0)
        {   // Adete karsilik gelen veri burada ayiklanarak atama yapiliyor.
            char *str = strndup(JSON_STRING + t[i + 1].start,t[i + 1].end - t[i + 1].start);
            // String tipindeki veri integer tipine donusturulup kaydediliyor.
            adet = atoi(str);
            
            adetler[ix] = adet;
            
            i++;
            
            ix++;
        }
    }

    // Dosyanin taramasi bittikten sonra doldurulmus olan diziler kullanilarak
    // emirler olusturuluyor.Kaydedilmek uzere banka'ya gonderiliyor.
    for(i=0; i<ix; i++){
        Emir emir = EmirOlustur(semboller[i],islemler[i],adetler[i]);
        banka->emirEkle(banka,emir);
    }
    fclose(fp);
    free(buffer);
}


// Bu fonksiyon islemleri gerceklestirir ve hesaplamalari yapar. 
// Emirler dosyasindaki verilere gore alim ve satim islemleri gerceklestirilir.
// Hesaplanan kar/zarar ekranda gosterilir. Son olarak da guncel portfoy ekrana yazilir.
void EmirleriGerceklestir(const Banka this){
    // Turkce karakterler icin kullanilir.
    SetConsoleOutputCP(65001);
    int i,j,portfoydeBulundu = 0;
    float maliyet,guncelFiyat,toplam,sonToplam = 0.0;

    // Emirler dosyasindaki verileri tek tek ele alarak alim satimlari gerceklestirir.
    for(i=0; i<this->emir_index; i++){
        maliyet = 0.0;
        guncelFiyat = 0.0;
        toplam = 0.0;
        if( !strcmp(this->emirler[i]->islem,"SATIS")){
            for(j=0; j<this->hisse_index; j++){
                if( !strcmp(this->emirler[i]->sembol,this->hisseler[j]->sembol)){
                    guncelFiyat = this->hisseler[j]->fiyat;
                    break;
                }
            }
            for(j=0; j<this->portfoy_index; j++){
                if( !strcmp(this->emirler[i]->sembol,this->portfoyKaydi[j]->sembol)){
                    maliyet = this->portfoyKaydi[j]->maliyet;
                    // Eger portfoyde olmayan bir satis yapilmaya calisilirsa hata mesaji ekrana gelir.
                    if( this->portfoyKaydi[j]->adet < this->emirler[i]->adet){
                        printf("Satis gerceklestirilemedi. %s \n\n", this->emirler[i]->sembol);
                        break;
                    }else{
                        this->portfoyKaydi[j]->adet -= this->emirler[i]->adet;
                        toplam = this->emirler[i]->adet * (guncelFiyat - maliyet);
                        sonToplam +=toplam;
                        if(toplam < 0 ){
                            toplam = toplam * -1;
                            printf("%s : %.1f",this->emirler[i]->sembol,toplam);
                            printf(" TL Zarar\n");
                        }else{
                            printf("%s : %.1f",this->emirler[i]->sembol,toplam);
                            printf(" TL Kar\n");
                        }
                    }
                    break;
                }
                
            }    
        }
        else{
            float eskiAdet = 0.0;
            float yeniAdet = 0.0;
            int portfoy_idx = 0;
            for(j=0; j<this->hisse_index; j++){
                if( !strcmp(this->emirler[i]->sembol,this->hisseler[j]->sembol)){
                    guncelFiyat = this->hisseler[j]->fiyat;
                    break;
                }
            }
            portfoydeBulundu = 0;
            for(j=0; j<this->portfoy_index; j++){
                if( !strcmp(this->emirler[i]->sembol,this->portfoyKaydi[j]->sembol)){
                    maliyet = this->portfoyKaydi[j]->maliyet;
                    eskiAdet = this->portfoyKaydi[j]->adet;
                    portfoy_idx = j;
                    portfoydeBulundu = 1;
                    break;
                }
                else
                    portfoydeBulundu = 0;   
            }
            yeniAdet = this->emirler[i]->adet;
            float yeniMaliyet =0.0;
            if( portfoydeBulundu == 1){
                
                yeniMaliyet = (maliyet*eskiAdet) + (yeniAdet*guncelFiyat);
                yeniMaliyet /= (eskiAdet+yeniAdet);
                this->portfoyKaydi[portfoy_idx]->adet = eskiAdet+yeniAdet;
                this->portfoyKaydi[portfoy_idx]->maliyet = yeniMaliyet;

            }
            else{
                yeniMaliyet = yeniAdet*guncelFiyat;
                Portfoy portfoy = PortfoyOlustur(this->emirler[i]->sembol,guncelFiyat,yeniAdet);
                this->portfoyEkle(this,portfoy);
            }
        }
    }

    // Islemler sonuclandiktan sonra gereken bilgiler ekrana yazdirilir ve program sonlanir.
    printf("Satışlar Kar/Zarar:\n");
    if(sonToplam < 0)
        printf("Toplam Kar/Zarar: - %.1f TL\n\n",sonToplam*-1);
    else
        printf("Toplam Kar/Zarar: + %.1f TL\n\n",sonToplam);

    printf("\nGüncel Portföy:\n");
    for(i=0; i<this->portfoy_index; i++){
        if( this->portfoyKaydi[i]->adet > 0){
            
            printf("Hisse: %s\n",this->portfoyKaydi[i]->sembol);
            printf("Adet: %d\n",this->portfoyKaydi[i]->adet);
            printf("Maliyet: %.2f\n",this->portfoyKaydi[i]->maliyet);
            if( i != this->portfoy_index-1 )
                printf("--------------------------\n");
        }
        
    }
}

void BankaYoket(Banka this){
    if(this == NULL) return;
    int i;
    for(i=0; i<this->hisse_index; i++);{
        HisseYoket(this->hisseler[i]); 
    }
    for(i=0; i<this->portfoy_index; i++);{
        PortfoyYoket(this->portfoyKaydi[i]); 
    }
    for(i=0; i<this->emir_index; i++);{
        EmirYoket(this->emirler[i]); 
    }
    
    free(this);
    this = NULL;
}