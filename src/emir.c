#include "emir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Emir EmirOlustur(const char* sembol, const char* islem, int adet){
    Emir this;
    this = (Emir)malloc(sizeof(struct EMIR));

    this->emirYoket = &EmirYoket;

    this->adet = adet;
    this->sembol = strdup(sembol);
    this->islem = strdup(islem);

}

void EmirYoket(Emir this){
    if(this == NULL) return;
    free(this);
    this = NULL;
}