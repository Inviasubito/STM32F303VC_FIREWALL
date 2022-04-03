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
  //Utilizzando un divisore di frequenza, PSC, però posso dividere 8 milioni per mille (999+1) e quindi
  //mettendo ARR a 4000, avrei un timer di 0.5 secondi.
  //Avrei potuto anche lasciare inalterato PSC ed impostare a 4 milioni ARR, per avere lo stesso risultato.
  TIM6->PSC = 999;
  TIM6->ARR = 4000;
  //Azzero il conteggio
  TIM6->CNT = 0;
  //Abilito il timer
  TIM6->CR1 = TIM_CR1_CEN;
  
  int ledCounter = 0;
  
  while(1)
  {
    //TIM6->SR Indica che si è verificato (1) o meno (0), un UE (Update Event).
    //Una volta che il valore è 1, dobbiamo mettere di nuovo 0, altrimenti non ci accorgeremo di un nuovo UE
    //Si è verificato un UE?
    if((TIM6->SR & 1) == 1)
    {
      //Si è verificato un UE, quindi imposto il bit a 0
      //In questo modo, al prossimo ciclo, mi accorgerò del cambiamento
      TIM6->SR = 0;
      
      if(count == 0)
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
