#include <stm32f30x.h>

//Accensione led blu

int main()
{
  //Abilitiamo il bus
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
  
  //Uscita digitale PE8
  GPIOE->MODER |= GPIO_MODER_MODER8_0;
  
  //Accendo il led
  GPIOE->ODR |= GPIO_ODR_8;
  
  while(1);
}
