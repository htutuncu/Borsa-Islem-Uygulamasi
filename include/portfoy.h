#ifndef PORTFOY_H
#define PORTFOY_H

struct PORTFOY{
    char* sembol;
    double maliyet;
    int adet;

    void (*portfoyYoket)(struct PORTFOY*);
};

typedef struct PORTFOY * Portfoy;


Portfoy PortfoyOlustur(const char* sembol, double maliyet, int adet);
void PortfoyYoket(Portfoy);


#endif