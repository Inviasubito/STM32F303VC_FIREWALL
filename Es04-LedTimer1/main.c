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

void initialize();
void logic();
void logic_1();

int count = 0;

int main()
{
  //inizializzo
  initialize();
  
  //logica
  logic();
  
  return 0;
}

void initialize()
{
  /*leds*/
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;    //bus
  GPIOE->MODER |= GPIO_MODER_MODER8_0;  //uscita digitale
  GPIOE->ODR = 0;                       //spengo leds
  
  /*Timer*/
  /*
  PSC = (T*Mhz)/Preload
  T   = (PSC*Preload)/Mhz
  */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //abilito tim2 (32bit)
  TIM2->PSC = 999;                      //prescaler 999
  TIM2->ARR = 4000;                     //auto reload a 4000 -> il contatore arriverà fino a questo valore e poi si fermerà
  TIM2->CNT = 0;                        //imposto il conteggio a 0
  TIM2->CR1 |= TIM_CR1_ARPE;            //bufferizzo -> se cambio arr durante il conteggio, aspetot prima il vecchio UIF (update event)
  TIM2->CR1 |= TIM_CR1_CEN;             //avvio il timer
}

void logic()
{
  int c = 0;
  
  while (1)
  {   
    //Se il timer raggiunge l'update event
    if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF)
    {
      //Accendo i led
      GPIOE->ODR |= (++c << 8);
      
      //Abbasso il led
      TIM2->SR |= ~TIM_SR_UIF;
    }
  }
}

void logic_1()
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
}
