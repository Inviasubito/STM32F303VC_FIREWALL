#include <stm32f30x.h>
#include <math.h>
#include <stdio.h>

void es1();
void contatoreBinario();
void ledLampeggianti();
void cronometro();
void ledNVIC();
void LedNVIC_TIM2_IRQHandler();
void ONDAQUADRA_TIM2_IRQHandler();
void userTensAD();


int main()
{
  
  while(1);
}

void TIM2_IRQHandler()
{
  //LedNVIC_TIM2_IRQHandler();
  ONDAQUADRA_TIM2_IRQHandler();
}

//***********ONDA QUADRA NO DAC

float flag = 0;
void onda_quadra()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM7EN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  
  GPIOA->MODER |= GPIO_MODER_MODER0_0;  
  
  NVIC->ISER[0] |= (1<<28);
  NVIC->ISER[0] |= (1<<6);
  
  SYSCFG->CFGR1 &= ~SYSCFG_EXTICR1_EXTI0;       //PA0
  EXTI->IMR |= EXTI_IMR_MR0;
  EXTI->RTSR |= EXTI_RTSR_TR0;
  EXTI->FTSR |= EXTI_FTSR_TR0;
  
  TIM2->ARR = 40000;
  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CNT = 0;    
  TIM2->CR1 |= TIM_CR1_CEN;
  
  TIM7->ARR = 40000;
  TIM7->CNT = 0;    
  TIM7->CR1 |= TIM_CR1_CEN;
}

//Generazione
void ONDAQUADRA_TIM2_IRQHandler()
{
  TIM2->SR &= ~TIM_SR_UIF;
  
  if(flag == 0)
  {
    GPIOA->ODR &= ~GPIO_ODR_0;
    flag = 1;
  }
  else
  {
    GPIOA->ODR |= GPIO_ODR_0;
    flag = 0;
  }
}

//Verifica
int flags = 0;
int c = 0;
void EXTI0_IRQHandler()
{
  EXTI->PR &= ~EXTI_PR_PR0;
  
  if(flags == 1)
  {
   printf("fronte Salita"); 
    flags = 0;
  }
  else
  {
   printf("fronte discesa"); 
   
    flags = 1;
  }
  
  c = TIM7->CNT;
  TIM7->CNT = 0;
}

//***********DMA

#define N1 100;
int Vin[N1];
int Vout[N1];
void dma_sinosoide()
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN | RCC_AHBENR_DMA2EN | RCC_AHBENR_ADC12EN | RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN | RCC_APB1ENR_TIM2EN;
  
  //GPIOA..
  GPIOA->MODER |= GPIO_MODER_MODER2 | GPIO_MODER_MODER4;
  
  //TIM2..
  TIM2->ARR = 36000000;
  TIM2->CR2 |= TIM_CR2_MMS_1;
  
  //DAC...
  DAC->CR |= DAC_CR_DMAEN1;
  DAC->CR |= DAC_CR_TEN1;
  DAC->CR |= DAC_CR_TSEL1_2;
  DAC->CR |= DAC_CR_EN1;
  
  //ADC...
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i<1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);

  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->SQR1 = ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_0;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;
  
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;
  ADC1->CFGR |= (11<<6);
  ADC1->CFGR |= ADC_CFGR_DMAEN;
  
  //DAM1
  DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);    
  DMA1_Channel3->CMAR = (uint32_t)Vout;  
  DMA1_Channel1->CNDTR = N1;
  DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
  DMA1_Channel1->CCR |= DMA_CCR_MINC;
  DMA1_Channel1->CCR |= DMA_CCR_CIRC;
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
  DMA1_Channel1->CCR |= DMA_CCR_EN;
  
  //DAM2
  DMA2_Channel3->CPAR = (uint32_t)&(DAC1->DHR12R1);    
  DMA2_Channel3->CMAR = (uint32_t)Vin;  
  DMA2_Channel3->CNDTR = N1;
  DMA2_Channel3->CCR |= DMA_CCR_MSIZE_0;
  DMA2_Channel3->CCR |= DMA_CCR_PSIZE_0;
  DMA2_Channel3->CCR |= DMA_CCR_MINC;
  DMA2_Channel3->CCR |= DMA_CCR_CIRC;
  DMA2_Channel3->CCR |= DMA_CCR_DIR;
  DMA2_Channel3->CCR |= DMA_CCR_EN;
  
  ADC1->CR |= ADC_CR_ADSTART;
}

//*************

//***********EXTI

int parola_codice = 404;

void exti_button_conversione()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_ADC12EN;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  GPIOA->MODER |= GPIO_MODER_MODER4 | GPIO_MODER_MODER2;
  
  DAC->CR |= DAC_CR_EN1;
  DAC->DHR12R1 = parola_codice;
  for(int i = 0; i<1000; i++);
  
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i<1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);

  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->SQR1 = ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_0;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;
  
  SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR1_EXTI0;   //LA LINEA 0 DI PA SONO ABILITATE LE EXTI
  EXTI->IMR |= EXTI_IMR_MR0;    //SMASCHERIAMO LA LINEA 0
  EXTI->RTSR |= EXTI_RTSR_TR0;  //SETTO IL CONTROLLO SU UN FRONTE DI SALITA
  NVIC->ISER[0] |= (1<<6);  
}

float ris;

void EXTI0_IRQHandler(){
  ADC1->CR |= ADC_CR_ADSTART;
  while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);
  
  ris = ADC1->DR;
}

//***********

//***********sinusoide

#define N 100
#define OFFSET 1.5
#define PI 3.14
float V1[N];
float in_TRIANGOLARE[N];
void gen_sinusoide(float ampiezza)
{
  float tensione = 0;
  for(int i = 0; i < N; i++)
  {
    tensione = OFFSET + ampiezza * sin(2*PI*(i/N));
    V1[i] = tensione * (4095.0/3.0);
  }
}

void gen_quadra(float ampiezza)
{  
  //tensione = parolacodice * 3 /4095
  for(int i = 0; i < N; i++)
  {
    if(i < N/2)
      V1[i] = 0;
    else
    {
      float p = ampiezza * 4095 / 3;
      V1[i] = p * 4095.0 / 3.0;
    }
  }
}

void gen_sega(float ampiezza)
{
  //tensione = parolacodice * 3 /4095
  for(int i = 0; i < N; i++)
  {
    float p = ampiezza * i /N;
    V1[i] = p * 4095.0 / 3.0;
  }
}

void gen_triangolare(float ampiezza)
{
  //tensione = parolacodice * 3 /4095
  for(int i = 0; i < N; i++)
  {
    float p;
     if(i < N/2)
     {
       p = ampiezza * i /N;;
     }
    else
    {
      p = ampiezza * (N-i) /N;
    }
    
    V1[i] = p * 4095.0 / 3.0;
  }
}

void sinusoide()
{
  RCC->AHBENR |= RCC_AHBENR_ADC12EN | RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_DACEN;
  
  GPIOA->MODER |= GPIO_MODER_MODER2 | GPIO_MODER_MODER4;
  
  //TIM2
  TIM2->ARR = 36000000;
  TIM2->CR2 |= TIM_CR2_MMS_1;
  
  //DAC
  DAC->CR |= DAC_CR_EN1;
  DAC->CR |= DAC_CR_TSEL1_2;
  DAC->CR |= DAC_CR_TEN1;
  
  //ADC
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i < 1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  
  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->SQR1 = ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;
  
  gen_sinusoide(1.5);
  
  //AVVIO TIM2
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->CNT = 0;
  
  float ten;
  
  for(int i = 0; i < N; i++)
  {
    DAC->DHR12R1 = V1[i]; //INSERISCO NEL DAC L'ELEMENTI I-ESIMO
    
    while((TIM2->SR & TIM_SR_UIF) != TIM_SR_UIF); //ASPETTO CHE SIA COMPLETO IL TRASFERIMENTO DA DHR A DOR ED INIZIA LA CONVERSIONE
    TIM2->SR &= ~TIM_SR_UIF;
    
    for(int i = 0; i < 1000; i++);      //ASPETTO CHE FINISCA LA CONVERSIONE
    
    ADC1->CR |= ADC_CR_ADSTART;
    while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);
    
    ten = ADC1->DR;
    
    printf("DAC: %f -> ADC: %f\n", V1[i], ten);
  } 
}

//***********

int parola_codice = 500;
void dac_adc()
{
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  RCC->AHBENR |= RCC_AHBENR_ADC12EN | RCC_AHBENR_GPIOAEN;
  
  GPIOA->MODER |= GPIO_MODER_MODER2 | GPIO_MODER_MODER4;
  
  //DAC
  DAC->CR |= DAC_CR_EN1;
  DAC->DHR12R1 = parola_codice;
  for (int i = 0; i < 1000; i++);
  
  //ADC
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i < 1000; i++);
  
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  
  ADC1->SQR1 = ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;        //PA2 -> ADC1_IN3
  ADC1->CFGR &= ~ADC_CFGR_CONT;         //SINGOLO PERCHè MI FERMO AD UNA SOLA CONVERSIONE
  
  ADC1->CR |= ADC_CR_ADSTART;
  while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);
    
  float var = (ADC1->DR * 3.0) / 4095.0;
  
  printf("parola codice in ingresso %d", parola_codice);
  printf("parola codice in uscita %f", var);
  
  //disabilitazione
  DAC->CR &= ~DAC_CR_EN1;
  ADC1->CR |= ADC_CR_ADDIS;
}

//************ADC

float tensioneIngresso;
void userTensAD()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->AHBENR |= RCC_AHBENR_ADC12EN;
  
  //ANALOGIC MODE perchè mi serve la tensione
  GPIOA->MODER |= GPIO_MODER_MODER0;
  
  //Tensione di rifeimento (generatore di tensione interno)
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i = 0; i < 1000; i++);
  
  //CLOCK/1
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  
  //MODALITà CONTINUA
  ADC1->CFGR |= ADC_CFGR_CONT;
  
  ADC1->SQR1 = ADC_SQR1_SQ1_0; //PA0 -> ADC1_IN1 e quindi L = 0 e SQR1 = 1. 
  //Se avessi avuto 2 conversioni da fare ad esempio anche PA3 -> ADC1_IN4 allora
  //L = 1 e SQR1 = 1 e SQR2 = 4. Questa cosa la vedi nel datasheet
  ADC1->SQR1 &= ~ADC_SQR1_L; 
  
  ADC1->SMPR1 |= ADC_SMPR1_SMP1; //->tempo di campionamento di 601.5 cicli di clock. Metti SMP1 perchè in PA0 hai ADC1_IN1
  
  ADC1->CR |= ADC_CR_ADSTART;
  
  while(1)
  {
    while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC); //attesa fine conversione
    tensioneIngresso = ADC1->DR * (3.0/(float)pow(2, 12)); //ADC1->DR -> Parola codice, 3.0 -> volt, (float)pow(2, 12)->2^n dove n è il numero di bit che utilizziamo per rappresentare la parola codice e adc ne usa 12
    
  }
}

//*************

int flag = 0;
void ledNVIC()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  GPIOE->MODER |= 0x55550000;
  TIM2->ARR = 7200000; //0,1s
  
  TIM2->DIER |= TIM_DIER_UIE; //abilito interruzioni
  
  NVIC->ISER[0] = (1 << 28); 
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->CNT = 0;  
}

//ledNVIC
void LedNVIC_TIM2_IRQHandler()
{
  TIM2->SR &= ~TIM_SR_UIF;

  if(flag == 0)
  {
    GPIOE->ODR |= 0x0000FF00;
    flag = 1;
  }
  else
  {
    GPIOE->ODR &= ~0x0000FF00;
    flag = 0;
  }
}

//Contatore
void cronometro()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM2->CNT = 0;
  
  int count = 0;
  float time = 0;
  float tau = 0;
  
  while(1)
  {
      if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
      {
        TIM2->CNT = 0;
        count = 0;
        
        while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
        
        count = TIM2->CNT;
        tau = (float)(125 * pow(10,-9));
        time = (float)count * tau;
        
        printf("Tempo (ns): %f", time);
      }
  }
}

//Base dei tempi
void ledLampeggianti()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;  
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  GPIOE->MODER |= 0x55550000;
  
  TIM6->ARR = 4000; //ARR = TIME * MHz  0,5s
  TIM6->PSC = 999; 
  TIM6->CR1 |= TIM_CR1_CEN;
  TIM6->CNT = 0;
  
  int isPush = 0;
  int count = 0;
  
  while(1)
  {
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);

      //Spento
      if(isPush == 0)
      {
        isPush = 1;
      }
      else
      {
        isPush = 0;
      }
    }
   
    if(isPush == 0 && (TIM6->SR & TIM_SR_UIF) == TIM_SR_UIF)
    {
      TIM6->SR &= ~TIM_SR_UIF;
      count++;
      
      GPIOE->ODR |= (count << 8);
    }
    else if(isPush == 1)
    {
      TIM6->CR1 &= ~TIM_CR1_CEN;
      TIM6->SR &= ~TIM_SR_UIF;
      TIM6->CNT = 0;
      GPIOA->ODR &= ~(count << 8);
      count = 0;
    }
  }
}

void contatoreBinario()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
  GPIOE->MODER |= 0x55550000;
  GPIOA->MODER &= 0xFFFFFFFC;
  
  int count = 0;
  
  while(1)
  {
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
      
      count++;
      GPIOE->ODR |= (count << 8);
    }
  }
}

void es1()
{
  int flag = 0; //0 spento, 1 acceso
  
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;
  GPIOE->MODER |= GPIO_MODER_MODER8_0;
  //GPIOE->ODR &= ~GPIO_ODR_8;
  GPIOA->MODER &= 0xFFFFFFFC; 
  
  while(1)
  {
    //Se clicco
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      //Aspetto che rilascia il pulsante
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);    
      
      //Se spento
      if(flag == 0)
      {
        //Accendo
        GPIOE->ODR |= GPIO_ODR_8; 
        flag = 1;
      }
      else //Se acceso
      {
        //Spengo
        GPIOE->ODR &= ~GPIO_ODR_8;
        flag = 0;
      }
    }
  }
}
