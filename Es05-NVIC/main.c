#include <stm32f30x.h>
#include <stdio.h>
  
//Usa interruzione con  NVIC
//Ogni 0,5 secondi, abilita i led 

void init();
void logic();

int flag = 0;

int main()
{
  init();
  logic();
  
  while(1);
}

void init()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;           //TIM2
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;            //GPIOE
  
  GPIOE->MODER |= GPIO_MODER_MODER8_0;          //DIGITAL
  GPIOE->ODR = 0x0;                             //LED OFF
  
  TIM2->CR1 &= ~TIM_CR1_CEN;                    //TIM2 OFF
  TIM2->CR1 |= TIM_CR1_ARPE;                    //BUFFER
  TIM2->CR2 |= TIM_CR2_MMS2_1;                  //UPDATE
  TIM2->DIER |= TIM_DIER_UIE;                   //INTERRUPT ON
  TIM2->SR &= ~TIM_SR_UIF;                      //UIF 0
  TIM2->CNT = 0;                                //COUNT 0
  TIM2->PSC = 0;                                //PRESCALER
  TIM2->ARR = 36000000;                         //ARR - 0,5 s
  
  NVIC->ISER[0] = (1 << 28);                    //INTERRUZIONI TIM2
  
  flag = 0;                                     //0 -> led off
}

void logic()
{
  TIM2->CR1 |= TIM_CR1_CEN;                     //TIMER ON
  GPIOE->ODR = 0x0;                             //LED OFF
}

void TIM2_IRQHandler()
{
  if(flag == 0)
  {
    flag = 1;
    GPIOE->ODR = 0x0000FF00;                    //LED ON
  }
  else
  {
    flag = 0;
    GPIOE->ODR = 0x0;                           //LED OFF
  }
  
  TIM2->SR &= ~TIM_SR_UIF;                      //UIF 0
}
