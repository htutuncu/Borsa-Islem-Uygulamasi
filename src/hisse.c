#include "hisse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Hisse HisseOlustur(const float fyt, char* smbl){
    Hisse this;
    this = (Hisse)malloc(sizeof(struct HISSE));
    this->fiyat = fyt;
    this->sembol = strdup(smbl);

    this->hisseYoket = &HisseYoket;
    
    return this;
}


void HisseYoket(Hisse h){
    if(h == NULL) return;
    free(h);
    h = NULL;
}