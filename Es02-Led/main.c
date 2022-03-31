//PE8 LED Blue
//PE9 LED Red

//GPIOE_ODR_8  -> Il led Blue
//GPIOE_ODR_9  -> Il led Red
//RCC_AHBENR -> Abilito il clock sulla linea
//GPIOE_MODER -> Modalità di utilizzo della linea

//Indirizzi base + offset
//GPIOE_ODR_9 -> 0x48001000  + 0x14 -> ODR9
//GPIOE_ODR_8 -> 0x48001000  + 0x14 -> ODR8
//RCC_AHBENR -> 0x40021000 + 0x14
//GPIOE_MODER -> 0x48001000 + 0x00

#define RCC_AHBENR  0x40021014;
#define GPIOE_ODR  0x48001014;
#define GPIOE_MODER 0x48001000;

#define PIN_BLUE 0x00000100; 
#define PIN_RED 0x00000200; 

unsigned int* p;

int main()
{
  //Abilito il clock
  p = (unsigned int*)RCC_AHBENR;
  *p = 0x00200000;
  
  //Abilito MODER
  p =  (unsigned int*)GPIOE_MODER;
  *p = 0x00010000;
  
  //Abilito il pin Blue
  p =  (unsigned int*)GPIOE_ODR;
  *p = PIN_BLUE;
    
  //Abilito il pin Red
  //p =  (unsigned int*)GPIOE_ODR;
  *p = PIN_RED;
  
  //while (1);
  return 0;
}
