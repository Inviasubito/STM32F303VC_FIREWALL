#include <stm32f30x.h>

//Utilizza un timer con le interruzioni per accender i led

void initialize();
void logic();
void logic_1();

int c = 0;
int main()
{
  //Inizializzo
  initialize();
  
  //la parte logica avviene grazie alle interruzioni
  //vedi TIM2_IRQHandler
  
  return 0;
}

void initialize()
{
  /*leds*/
  RCC->AHBENR |= RCC_AHBENR_GPIOEEN;    //bus
  GPIOE->MODER |= 0x55550000;           //abilito tutti i led
  GPIOE->ODR &= 0;                      //spengo tutti i led
  
  /*timer*/
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //bus
  TIM2->PSC = 1;                        //prescaler PSC = 1
  TIM2->ARR = 36000000;                 //auto preload = ARR = T * Mhz / PSC
  TIM2->CNT = 0;                        //conteggio a 0
  TIM2->DIER |= TIM_DIER_UIE;           //possibilità di associare una interruzione
  TIM2->CR1 |= TIM_CR1_CEN;             //abilito
  
  /*interruzioni*/
  //NVIC: Nested vectored interrupt controller
  NVIC->ISER[0] |= 1 << 22;
}

//NVIC ISER0 TIM2
void TIM2_IRQHandler()
{
  logic();
}


void logic()
{
  //Quando avviene una interruzione, accendo i led
  
  //ripristino lo stato di UIF (lo metto a 0 così mi accorgo del conteggio, quando termina nuovamente)
  TIM2->CR2 &= ~TIM_SR_UIF;
  
  //i led sono spenti?
  if(c == 0)
  {
    //accendo i led
    GPIOE->ODR |= 0x55550000;
    
    //ricordo che li ho accesi
    c = 1;
  }
  else
  {
    //spengo i led
    GPIOE->ODR |= 0;
    
    //ricordo di averli spenti
    c = 0;
  }
}

void logic_1()
{
  //Abilito il clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  //Abilito la possibilità di interrompere il timer
  TIM2->DIER |= TIM_DIER_UIE;
  
  //Imposto il prescaler
  //2^16 = 65535, 2^16*125 (nanosecondi) = 8.191.875 (circa 8Mhz).
  //T=(PSC*Prelad)/MHz per avere il tempo T in secondi
  //TIM2->PSC = 65535;
  
  //L'Auto Reload Register a 36.000.000, perchè, aggiungendo il file system_stm32f30.c, alziamo la frequenza da 8 a 72 MHz
  TIM2->ARR = 36000000;
  
  //Interruzioni
  //Perchè nel reference manual, nella vector table (table 30), noto che il TIM2,
  //Sta nella posizione 28, inferiore a 31 (dimensione del primo ISER, ISER0, perchè sono a 32 bit ed inferiore ad ISER2 perchè non supero i 64 bit).
  //Quindi, mi rendo conto che 28-32 = 4 che è la posizione di ISER0
  NVIC->ISER[0] |= 1 << 28;
  
  //Conteggio a 0
  TIM2->CNT = 0;
  
  //Abilito il timer
  TIM2->CR1 |= TIM_CR1_CEN;
  
  while(1);
  
}
