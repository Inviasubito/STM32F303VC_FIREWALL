#include <stm32f30x.h>

//Utilizza le interruzioni

int c = 0;
int main()
{
  //Abilito il clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  //Abilito la possibilità di interrompere il timer
  TIM2->DIER |= TIM_DIER_UIE;
  
  //Imposto il prescaler
  //2^16 = 65535, 2^16*125 (nanosecondi) = 8.191.875 (circa 8Mhz).
  //T=(PSC*Prelad)/MHz per avere il tempo T in secondi
  //TIM2->PSC = 65535;
  
  //L'Auto Reload Register a 36.000.000, perchè, aggiungendo il file system_stm32f30.c, alziamo la frequenza da 8 a 72 MHz
  TIM2->ARR = 36000000;
  
  //Interruzioni
  //Perchè nel reference manual, nella vector table (table 30), noto che il TIM2,
  //Sta nella posizione 28, inferiore a 31 (dimensione del primo ISER, ISER0, perchè sono a 32 bit ed inferiore ad ISER2 perchè non supero i 64 bit).
  //Quindi, mi rendo conto che 28-32 = 4 che è la posizione di ISER0
  NVIC->ISER[0] |= 1 << 28;
  
  //Conteggio a 0
  TIM2->CNT = 0;
  
  //Abilito il timer
  TIM2->CR1 |= TIM_CR1_CEN;
  
  while(1);
  
  return 0;
}

//Funzione di libreria che inizializza l'hardware
void TIM2_DAC_IRQHandler()
{
  TIM2->SR &= ~TIM_SR_UIF;
  c++;
}
