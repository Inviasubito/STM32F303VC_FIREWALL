#include <stm32f30x.h>

#define N 100   //campioni

short int w_v[N];
short int t_v[N];

float amp;

void genSign();
void init();
void logic();

int main()
{
  //Genero il segnale ed aggiungo gli elementi in w_v
  genSign();
  
  //inizializzo
  init();
  
  //logica
  logic();
  
  while(1);
  //return 0;
}

//segnale sega
void genSign()
{
  //Per ogni campione
  for (int i = 0; i < N; i++)
  {
    w_v[i] = (short int)amp * (i/N) * (3.0/4095.0);
  }
}

void init()
{
  //GPIOA perchè nel datashet vediamo che ADC1 è su PA
  //ADC12EN perchè abilito la periferica
  //abilito il TIM2EN
  //abilito il DAC
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_ADC12EN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_DACEN;
  
  GPIOA->MODER |= GPIO_MODER_MODER2 | GPIO_MODER_MODER4;
  
  /*timer*/
  TIM2->ARR = 36000000;                                 //0.5 sec
  TIM2->CR2 |= TIM_CR2_MMS2_1;                          //010 update event trigger
  
  /*DAC*/
  DAC->CR |= DAC_CR_TEN1;                               //abilito il trigger
  DAC->CR |= DAC_CR_TSEL1_2;                            //aggancio tim2 al dac
  DAC->CR |= DAC_CR_EN1;                                //abilito
  
  /*ADC*/
  //-->inizializzazione
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;                       //01->00
  ADC1->CR |= ADC_CR_ADVREGEN_0;                        //00->10
  for(int i = 0; i < 1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;                    //divide il clock per 1
  ADC1->CR |= ADC_CR_ADCAL;                             //inizio la calibrazione
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);     //attendo che finisca. 0 -> completato
  
  ADC1->CR |= ADC_CR_ADEN;                              //abilitazione
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);    //attendo
  
  //-->config
  ADC1->CFGR &= ~ADC_CFGR_CONT;                         //cont = 0 -> conversione singola
  ADC1->SQR1 |= (3 << 6);                               //0011 in 6 abilito pa2
  ADC1->SQR1 &= ~ADC_SQR1_L;                            //ho un canale da convertire
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;                        //601.5 periodi di clock per caricare il condensatore
}

void logic()
{
  //ABILITO IL TIMER
  TIM2->CNT = 0;
  TIM2-> CR1 |= TIM_CR1_CEN;
  
  for(int i = 0; i < 1000; i++)
  {
    DAC->DHR12L1 = w_v[i];
    while((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF);
    TIM2->SR &= ~TIM_SR_UIF;
    
    for(int i = 0; i < 1000; i++);
    
    ADC1->CR |= ADC_CR_ADSTART;
    while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);
    
    t_v[i]=ADC1->DR;
  }
  
  ADC1->CR |= ADC_CR_ADDIS;
  DAC->CR &= ~DAC_CR_EN1;
  TIM2->CR1 &= ~TIM_CR1_CEN;
}
