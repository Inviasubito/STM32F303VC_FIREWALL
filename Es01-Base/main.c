//Gli indirizzi e gli offset si trovano nel ReferenceManual (RF)

//Indirizzo base + offset
//Abilita il registro AHB
//RCC_Reset and Clock Control
#define RCC_AHBENR 0x40021014

//Indirizzo base (pg. 42) + offset (pg. 43)
//0x48001000 + 0x00
//MODER8 - PIN [17, 16]
//Il MODER consente di...
#define GPIOE_MODER 0x48001000

//Indirizzo base (pg. 42) + offset (pg. 145)
//0x48001000 + 0x14
//Il GPIOE_ODR consente di..
#define GPIOE_ODR 0x48001014

//Sono registri e devo mettere unsigned
unsigned int* p;

int main()
{
  //Faccio il cast perchè il compilatore C, gestisce la define come intera
  p = (unsigned int*) RCC_AHBENR;
  
  //Metto 1 nel bit 21 (Shift)
  //Abilito quindi la periferica 21 del GPIOE
  //pg. 116 del ReferenceManual
  *p = (1<<21);
  
  p = (unsigned int*) GPIOE_MODER;
  
  //Voglio utilizzare la MODER8 utilizzando un segnale digitale.
  //Nel RF, pg. 143, è indicato che per abilitarlo bisogna settare il pin 17 = 0 e 16 = 1
  //Posso usare lo Swift come prima, oppure utilizzando una parola codice in esadecimale
  //Ricordiamo che ogni bit corrisponde a 4 cifre, quindi da sinistra, ogni 0 corrisponde a 4 cifre
  //0x0001 0000 -> Lo 0 più a sinistra corrisponde a MODER0 - MODER1, dove sta 1, abbiamo MODER8 - MODER9
  //Ricordandoci che in esadecimale 1 = 0001, abbiamo impostato il valore del pin 17 = 0 e 16 = 1
  *p = 0x00010000;
  
  //Devo inserire 1 nel registro 8 che corrisponde alla linea PE8
  p = (unsigned int*) GPIOE_ODR;
  
  //per lo stesso ragionamento di prima
  *p = 0x00000100;
  
  //Creo il loop perchè se esco non so dove finisce lo stato della macchina
  while(1);
  //return 0;
}
