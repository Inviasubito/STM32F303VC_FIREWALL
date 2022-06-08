#include <stm32f30x.h>
#include <math.h>
#include <stdio.h>

/*
generare un segnale quadra con valore 1V->2V con passo di 100 millivolt e periodo di 300ms.
per ogni valore di tensione 3 valori generati
*/

unsigned int parola;
float conta;
float AMPIEZZA = 100;

int main()
{
  RCC->APB1ENR |= RCC_APB1ENR_DACEN | RCC_APB1ENR_TIM2EN;
  
  NVIC->ISER[0] = (1 << 28); 
  TIM2->ARR = 2160000;        //0,03s
  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->CNT = 0;  
  
  DAC->CR |= DAC_CR_EN1;
  
  conta = 1;
  float p = AMPIEZZA * (float)conta;
  parola = (unsigned short int)p * 4095.0 / 3000.0;
  
  while(1);
}

void TIM2_IRQHandler()
{
  TIM2->SR &= ~TIM_SR_UIF;
  
  //2*4095/3000.0 = 2,730
  if(parola >= 2,730)
  {
    TIM2->CR1 &= ~TIM_CR1_CEN;
    DAC->CR &= ~DAC_CR_EN1;
  }
  else
  {
    conta++;
    float p = AMPIEZZA * (float)conta;
    parola = (unsigned short int)p * 4095.0 / 3000.0;
    
    DAC->DHR12R1 = parola;
    for(int i = 0; i<1000; i++);
  }
}
