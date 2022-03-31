#include <stm32f30x.h>

//I led vanno da PE8 a PE15
#define PE8_LED 0x100

void My();
void Prof();

void main(void)
{
  //Abilito bus
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
  
  //Abilito MODER
  GPIOE->MODER = 0x55550000;
  
  Prof();
}

void My()
{
  GPIOA->MODER = 0x00000000;
  GPIOE->ODR = 0x0;
  
  int tmp = PE8_LED;
  int count = 0;
  
  while (1)
  {
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {             
      GPIOE->ODR |= tmp;

      if(++count > 8)
      {
        count = 1;
        GPIOE->ODR =0x0;  
      }
      else
      {  
        tmp *= 2;
      }
    }
  }
}

int _count = 0;
void Prof()
{
  GPIOA->MODER &= ~GPIO_MODER_MODER0_0;
  
  while(1)
  {
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
     //while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
     _count++;
     GPIOE->ODR = (_count << 8);
    }
  }
}
