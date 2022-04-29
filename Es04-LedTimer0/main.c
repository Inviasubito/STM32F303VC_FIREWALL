#include <stdio.h>
#include <stm32f30x.h>

//Contatore ogni secondo 
//Clicchi il bottone ed i led si accendono ogni secondo in intermittenza
//Clicchi di nuovo ed i led si spengono

void init();
void timerLogic(int);
void logic();

int main()
{
  init();
  logic();
}

void init()
{
  //Bus led e bottone
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
  
  //Bus Timer6
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  
  //moder -> digital
  GPIOA->MODER |= GPIO_MODER_MODER0_0;  //bottone in digitale
  GPIOE->MODER |= 0x55550000;           //led in digitale
  
  //Spengo i led
  GPIOE->ODR = 0x0;
  
  //Timer
  TIM6->CR1 &= ~TIM_CR1_CEN;            //Disattivo
  TIM6->CR1 |= TIM_CR1_ARPE;            //Bufferizzato
  TIM6->CNT = 0x0;                      //Conteggio 0
  TIM6->PSC = 999;                      //PSC -> 1000
  TIM6->ARR = 1600;                     //0.2s -> mhz/psc*0.2
}

void logic()
{
  int flag = 0;
  
  while(1)
  {
     //Se clicco il bottone
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      //Se il timer è disattivato
      if(flag == 0)
      {
        //Attivo il timer
        TIM6->CR1 |= TIM_CR1_CEN;
        
        flag = 1;
      }
      else
      {
        //Disattivo il timer
        TIM6->CR1 &= ~TIM_CR1_CEN;
        TIM6->CNT = 0x0;
        
        flag = 0;
      }        
    }
    
    timerLogic(flag); 
  }
}

void timerLogic(int flag)
{
  int f_led = 0;
  
  //Fino a quando il timer è in azione
  while((TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF)
  {
    if(f_led == 0)
    {
      //Accendo tutti i led
      GPIOE->ODR |= 0x0000FF00;
      f_led = 1;
    }
    else
    {
      //Spengo tutti i led
      GPIOE->ODR |= 0x0;
      f_led = 0;
    }
    
    TIM6->SR &= ~TIM_SR_UIF;
  }
}
