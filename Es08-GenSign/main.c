#include <stm32f30x.h>

#define N 100

short int vett1[N];
short int vett2[N];

void gen_sign(float);
void DACsetup();
void ADCsetup();
void TIMsetup();

int main()
{
  gen_sign(1.0);
  
  RCC->AHBENR |= RCC_AHBENR_ADC12EN | RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_DACEN;
    
  GPIOA->MODER |= GPIO_MODER_MODER2 | GPIO_MODER_MODER4;
  
  DACsetup();
  ADCsetup();
  TIMsetup();
  
  TIM2->CNT = 0;
  TIM2->CR1 |= TIM_CR1_CEN;
  
  for(int i = 0; i<N; i++){
    DAC->DHR12R1 = vett1[i];
    while((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF);
    TIM2->SR &= ~TIM_SR_UIF;
    for(int i = 0; i<1000; i++);
    
    ADC1->CR |= ADC_CR_ADSTART;
    while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);
    
    vett2[i]=ADC1->DR;
    
  }
  
  ADC1->CR |= ADC_CR_ADDIS;
  DAC->CR &= ~DAC_CR_EN1;
  TIM2->CR1 &= ~TIM_CR1_CEN;
  
  
  
  while(1);
}

void gen_sign(float ampiezza){
  for(int i=0; i<N; i++){
    vett1[i]=ampiezza*(i/N)*(3.0/4095.0);
  }
}

void TIMsetup(){
  TIM2->ARR = 36000000;
  TIM2->CR2 |= TIM_CR2_MMS_1; //MMS=010, stiamo abilitando il trigger
}

void DACsetup(){
  DAC->CR |= DAC_CR_TEN1; //abilitare i trigger
  DAC->CR |= DAC_CR_TSEL1_2; //per il timer 2
  DAC->CR |= DAC_CR_EN1;
}

void ADCsetup(){
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i<1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  
  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->SQR1 = (3<<6); //canale 3 (PA2);
  ADC1->SQR1 &= ~ADC_SQR1_L; //L=0000, significa 1 canale
  ADC1->SMPR1 |= ADC_SMPR1_SMP3; //SMP3 = 111
}
