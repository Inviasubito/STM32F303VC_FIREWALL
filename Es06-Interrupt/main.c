#include <stm32f30x.h>

/*
Premo il pulsante
->Parte il lampeggio del timer

Premo il pulsante
-> Si interrompe il lampeggio
*/

int flag;

int main()
{
  //Abilito il clock
  //led
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
  //button user
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  //timer2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  //syscfg
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  //Uscita digitale PE8 - MODER8
  GPIOE->MODER = GPIO_MODER_MODER8_0;

  //Collego PA0 a EXTI0
  
  
  //Smaschero EXTI0
  
  //Rising Trigger su EXTI0
  
  //Accettazione Interrupt EXTI0 su NVIC
  
  flag = 0;
  
  //8Mhz/2000 = 4000
  TIM2->PSC = 1999;
  //Se in un secondo fa 4000 operazioni, allora a 2000, starò in 0.5 secondi
  TIM2->ARR = 2000;
  //counter
  TIM2->CNT = 0;
  //Abilito Update Interrupt Mode. Il timer può essere interrotto 
  TIM2->DIER |= TIM_DIER_UIE;
  
  //Accettazione di NVIC di Interrupt su Update di Tim2
  
  return 0;
}

/*

ISR EXTI0
  Ho premuto?
    No -> Avvio il timer
    Si -> Arresto il timer, imposto cnt = 0 e spengo led
*/

void EXTI0_IRQHandler()
{
  //Ho premuto?
  if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
  {
    //SI
    //Abilito il timer
    TIM2->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    //NO
    //Arresto il timer
    TIM2->CR1 |= ~TIM_CR1_CEN;
    //Imposto CNT a 0
    TIM2->CNT = 0;
    //Spengo i led
    GPIOE->ODR = 0;
  }
}
