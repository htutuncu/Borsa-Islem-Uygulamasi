#ifndef EMIR_H
#define EMIR_H

struct EMIR{
    char *sembol;
    char *islem;
    int adet;

    void (*emirYoket)(struct EMIR*);
};

typedef struct EMIR * Emir;

Emir EmirOlustur(const char* sembol, const char* islem, int adet);
void EmirYoket(Emir);


#endif