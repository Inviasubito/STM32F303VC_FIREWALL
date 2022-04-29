#include <stm32f30x.h>

//Premo un pulsante per accendere un led
//Se lo ripremo, accendo anche un altro led fino ad accenderli tutti

void init();
void logic();

int main()
{
  init();
  logic();
}

void init()
{
  //Abilito il bus per i led e per il pulsante
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOAEN;
  
  //Imposto il modo digitale per i led
  GPIOE->MODER |= 0x55550000;
  
  //Spengo tutti i led
  GPIOE->ODR = 0x0;
  
  //Imposto il modo di input
  GPIOA->MODER |= GPIO_MODER_MODER0_0;
}

void logic()
{
  int count = 0;
  
  while(1)
  {
    //Se il pulsante è premuto
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      //Accendo il led
      GPIOE->ODR |= (++count << 8);
      
      //attendo che il pulsante venga rilasciato
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
    }
    else
    {
      //Spengo tutti i led
      GPIOE->ODR = 0x0;
    }
  }
}
