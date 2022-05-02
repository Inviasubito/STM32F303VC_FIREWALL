#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define tk 125*pow(10,-9)       //125 ns

//Quando tengo premuto il pulsante USER, avvio il conteggio

void init();
void logic();

int main()
{
  init();
  logic();
  
  while (1);
}

void init()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;                            //butUser
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                           //tim2
  
  GPIOA->MODER |= GPIO_MODER_MODER0_0;                          //userBut digital
  
  //timer2
  TIM2->CR1 &= ~TIM_CR1_ARPE;                                   //buffer off
  TIM2->CR1 &= ~TIM_CR1_CEN;                                    //TIMER OFF
  TIM2->CR2 = TIM_CR2_MMS2_1;                                   //UPDATE
  TIM2->DIER &= ~TIM_DIER_UIE;                                  //INTERRUPT OFF
  TIM2->SR |= TIM_SR_UIF;                                       //UPDATE FLAG
  TIM2->CNT = 0;                                                //COUNTER 0
  TIM2->PSC = 0;                                                //PRESCALER
  TIM2->ARR = 0;                                                //ARR
}

void logic()
{  
  while(1)
  {
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)                 //BUTTON ON
    {
      TIM2->CR1 |= TIM_CR1_CEN;                                 //TIMER OFF
      TIM2->CNT = 0;                                            //COUNTER 0
      
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);           //BUTTON ON
      
      TIM2->CR1 &= ~TIM_CR1_CEN;                                //TIMER OFF
      
      int count = TIM2->CNT;
      printf("Conteggio %f", count * tk);
    }
  }
}
