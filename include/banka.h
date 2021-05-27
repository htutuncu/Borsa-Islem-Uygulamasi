#ifndef BANKA_H
#define BANKA_H

#include "hisse.h"
#include "portfoy.h"
#include "emir.h"

struct BANKA{
    int hisse_index;
    int portfoy_index;
    int emir_index;

    Hisse* hisseler;
    Portfoy* portfoyKaydi;
    Emir* emirler;

    void (*hisseEkle)(struct BANKA*,struct HISSE*);
    void (*portfoyEkle)(struct BANKA*,struct PORTFOY*);
    void (*emirEkle)(struct BANKA*,struct EMIR*);
    int (*hisseOku)(struct BANKA*,const char*);
    int (*portfoyOku)(struct BANKA*,const char*);
    int (*emirOku)(struct BANKA*,const char*);
    void (*bankaYoket)(struct BANKA*);
    void (*emirleriGerceklestir)(struct BANKA*);
};

typedef struct BANKA* Banka;

Banka BankaOlustur();
void HisseEkle(const Banka,const Hisse);
void PortfoyEkle(const Banka,const Portfoy);
void EmirEkle(const Banka,const Emir);

int HisseOku(const Banka,const char*);
int PortfoyOku(const Banka,const char*);
int EmirOku(const Banka,const char*);

void EmirleriGerceklestir(const Banka);

void BankaYoket(Banka);

#endif