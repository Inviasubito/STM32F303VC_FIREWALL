/*

Accendere ogni led ogni secondo

definisco lo stato del led = 0
abilito GPIOE e TIMER a 16 bit
PE8 in uscita digitale 
PSC 999
ARR 4000
CNT 0
CEN 1

ciclo
  uif == 1
    stato? (decido se accendere o spegnere il led)
    uif = 0 così resetto e parte un nuovo conteggio

*/
