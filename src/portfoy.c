#include "portfoy.h"
#include <string.h>
#include <stdlib.h>


Portfoy PortfoyOlustur(const char* sembol, double maliyet, int adet){
    Portfoy this;
    this = (Portfoy)malloc(sizeof(struct PORTFOY));

    this->portfoyYoket = &PortfoyYoket;

    this->adet = adet;
    this->maliyet = maliyet;
    this->sembol = strdup(sembol);
}

void PortfoyYoket(Portfoy this){
    if(this == NULL ) return;
    free(this);
    this = NULL;
}