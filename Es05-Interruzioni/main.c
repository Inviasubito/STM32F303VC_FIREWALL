#include <stm32f30x.h>

int c = 0;
int main()
{
  //Abilito il clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  
  //Abilito la possibilità di interrompere il timer
  TIM6->DIER |= TIM_DIER_UIE;
  
  //Imposto il prescaler
  //2^16 = 65535, 2^16*125 (nanosecondi) = 8.191.875 (circa 8Mhz).
  //T=(PSC*Prelad)/MHz per avere il tempo T in secondi
  TIM6->PSC = 65535;
  
  //L'Auto Reload Register a 1000, quindi ogni secondo
  TIM6->ARR = 1000;
  
  //Interruzioni
  //Perchè nel reference manual, nella vector table (table 30), noto che il TIM6 (condiviso anche con DACUNDER),
  //Sta nella posizione 54, superiore a 31 (dimensione del primo ISER, ISER0, perchè sono a 32 bit ed inferiore ad ISER2 perchè non supero i 64 bit).
  //Quindi, mi rendo conto che 54-32 = 22 che è la posizione di ISER1
  NVIC->ISER[1] |= 1 << 22;
  
  //Abilito il timer
  TIM6->CR1 |= TIM_CR1_CEN;
  
  while(1);
  
  return 0;
}

//Funzione di libreria che inizializza l'hardware
void TIM6_DAC_IRQHandler()
{
  TIM6->SR &= ~TIM_SR_UIF;
  c++;
}
