#include <stdio.h>
#include <stm32f30x.h>

void initTimer();
void setTimer(float time);

int main()
{
  initTimer();
  setTimer(0.5);
  
  int count = 0;
  
  while(1)
  {
    //Se si verifica un update
    if((TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF)
    {
      //resetto il flag
      TIM6->SR &= ~TIM_SR_UIF;
      
      printf("Conteggio n. %d", ++count);
    }
  }
}

void initTimer()
{
  //Abilito il bus
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  
  //Disabilito
  TIM6->CR1 &= ~TIM_CR1_CEN;
  
  //Bufferizzato
  TIM6->CR1 |= TIM_CR1_ARPE;
  
  //Continua il conteggio
  TIM6->CR1 |= TIM_CR1_OPM;
  
  //Update
  TIM6->CR2 |= TIM_CR2_MMS_1;
  
  //Interrupt non in questo caso
  //TIM6->DIER &= ~TIM_DIER_UIE;
  
  //CNT = 0
  TIM6->CNT = 0x0;
}

void setTimer(float time)
{  
  int Mhz = 8000000;
  int maxTimer = 65535;
  int psc = 1;
  
  float arr = Mhz * time;
  
  while(arr > maxTimer)
  {
    psc += 999;
    arr = (Mhz / psc) * time;
  }
  
  //Prescaler
  TIM6->PSC = (unsigned int)psc;
  
  //ARR
  TIM6->ARR = (unsigned int)arr;
  
  //Abilito
  TIM6->CR1 |= TIM_CR1_CEN;
}
