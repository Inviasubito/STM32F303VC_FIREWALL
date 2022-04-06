#include <stm32f30x.h>

void initialize();
void logic();
void logic_1();

//soluzione del prof
void Prof();

void main(void)
{
  //Inizializzo
  initialize();
  
  //Logica
  logic();
}

void initialize()
{
  /*led*/
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;    //bus
  GPIOE->MODER = 0x55550000;            //da 8 a 15 in digitale
  GPIOE->ODR = 0x0;                     //led spenti
  
  /*butUser*/
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    //bus
  GPIOA->MODER |= GPIO_MODER_MODER0_0;  //uscita digitale per il button
}

void logic()
{
  //ogni volta che clicco, accendo un led, fino a 255. Poi spengo e ricomincio
  int count = 0;
  
  while (1)
  {
    //se clicco
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {
      //Se sto ancora cliccando, mi fermo ed aspetto il rilascio del bottone
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
      
      //attivo il led PE8 (001)...PE9 (010)...PE10(011)...PE11(100) ecc...
      GPIOE->ODR |= (++count << 8);
    }
  }
}

void logic_1()
{
  //Ogni volta che premo il bottone, si accende un led da 8 a 15
  //Però non devono esserci ripetizioni. Es:
  //No-> 001, 010, 011
  //Si-> 001, 010, 100
  
  //Posizione di PE8
  int tmp = 0;
  int count = 0;
  
  while (1)
  {
    //Se premo il pulsante
    if((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
    {             
      //Aspeto che venga rilasciato
      while((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0);
      
      //accendo il led (prima PE8)
      GPIOE->ODR |= tmp;

      //se non ho led accesi
      if(tmp == 0)
      {
        //imposto il led 8 per l'accensione
        tmp = GPIO_ODR_8;
      }
      else if(tmp > GPIO_ODR_15)        //Se ho acceso tutti i led
      {
        //allora imposto tmp al valore di default
        tmp = 0;
      }
      else                              //Se sto ancora incrementando
      {
        //incremento tmp di 2
        //001*2 = 010*2 = 100 ecc
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
