/**
 * Hikmet Tutuncu
 * 01.05.21
 * 
*/
#include <stdio.h>
#include "banka.h"
#include "hisse.h"
#include "emir.h"
#include "portfoy.h"


int main(){

    Banka banka = BankaOlustur();

    banka->hisseOku(banka,"./doc/hisseler.json");
    banka->portfoyOku(banka,"./doc/portfoy.json");
    banka->emirOku(banka,"./doc/emirler.json");

    
    banka->emirleriGerceklestir(banka);

    banka->bankaYoket(banka);
    return 0;
}