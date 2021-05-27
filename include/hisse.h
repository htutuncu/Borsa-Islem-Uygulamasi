#ifndef KISI_H
#define KISI_H


struct HISSE{
    float fiyat;
    char* sembol;

    void (*hisseYoket)(struct HISSE*);
};

typedef struct HISSE * Hisse;

Hisse HisseOlustur(const float fiyat, char* sembol);
void HisseYoket(const Hisse);

#endif