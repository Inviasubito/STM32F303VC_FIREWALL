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

#include <stm32f30x.h>

int count = 0;

int main()
{
  //Abilito clock per led (RCC_AHBENR_GPIOEEN) e per timer (RCC_APB1ENR)
  RCC->AHBENR = RCC_AHBENR_GPIOEEN;
  RCC->APB1ENR = RCC_APB1ENR_TIM6EN;
  
  //Uscita digitale per MODER 8, 9, 10, 11, 12, 13, 14, 15
  //Abilito tutti i led
  GPIOE->MODER = 0x55550000;
  //Accendo il led
  GPIOE->ODR = GPIO_ODR_8;
  
  //Imposto il tempo.
  //Essendo da 8MHz, fa 8 milioni di operazioni al secondo.
  //Quindi, mezzo secondo significa che il registro assuma 4 milioni come valore
  //8Mhz/1000, significa che in un secondo, il contatore arriva a 8000
  TIM6->PSC = 999;
  //Quando arrivo a 4000 (quindi in 0,5 secondi), il flag diverrà 1
  TIM6->ARR = 4000;
  //Azzero il conteggio
  TIM6->CNT = 0;
  //Abilito ARPE, in questo modo posso utilizzare il flag
  TIM6->CR1 |= TIM_CR1_ARPE;
  //Abilito il timer
  TIM6->CR1 |= TIM_CR1_CEN;
  
  int ledCounter = 0;
  
  while(1)
  {
    if((TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF)
    {
      TIM6->SR = 0;
      
      if(count == 0)    //Invece dell'if, avremmo potuto utilizzare un meccanismo delle interruzioni.
      {
        count++;
        GPIOE->ODR = (8 << ++ledCounter);
        
        if(ledCounter > 15)
          ledCounter = 0;
      }
      else
      {
        count--;
        GPIOE->ODR = 0;
      }
    }
  }
  
  return 0;
}
