#include <stdint.h>
#include "reg.h"

void init_usart1(void)
{
//PB6 PB7
	//Enable RCC AHB1 匯流排
	SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIO_PORTB);

//GPIO Configurations
	//Modeler: Set PB6 Modeler: 10
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_1_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_0_BIT(6));
	
	//Modeler: Set PB7 Modeler: 10
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_1_BIT(7));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_0_BIT(7));

	//SET GPIOB OTYPER to 0
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OTYPER_OFFSET, OTy_BIT(6)); 
	//SET GPIOB OTYPER to 0
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OTYPER_OFFSET, OTy_BIT(7)); 

	//Output speed medium: 10
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_1_BIT(6));
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_0_BIT(6));


	//Output speed medium: 10
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_1_BIT(7));
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_0_BIT(7));

	//PUPDR: PB6:00
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB)+ GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB)+ GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(6));

	//PUPDR: PB7:00
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB)+ GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(7));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB)+ GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(7));

	WRITE_BITS(GPIO_BASE(GPIO_PORTB)+ GPIOx_AFRL_OFFSET, AFRLy_3_BIT(6), AFRLy_0_BIT(6), 7);
	WRITE_BITS(GPIO_BASE(GPIO_PORTB)+ GPIOx_AFRL_OFFSET, AFRLy_3_BIT(7), AFRLy_0_BIT(7), 7);

	//RCC EN USART1
	SET_BIT(RCC_BASE + RCC_APB2ENR_OFFSET, USART1EN);

	//Baud
	const unsigned int BAUD = 115200; //BAUD RATE
	const unsigned int SYSCLK_MHZ = 168; //System clock
	const double USARTDIV = SYSCLK_MHZ * 1.0e6 / 16 / BAUD;

	const uint32_t DIV_MANTISSA = (uint32_t)USARTDIV; //取出USARTDIV的整數部份(直接round)
	const uint32_t DIV_FRACTION = (uint32_t)(USARTDIV-DIV_MANTISSA * 16);//取出USARTDIV的小數部份

	//USART  Configurations
	WRITE_BITS(USART1_BASE + USART_BRR_OFFSET, DIV_MANTISSA_11_BIT, DIV_MANTISSA_0_BIT, DIV_MANTISSA);
	WRITE_BITS(USART1_BASE + USART_BRR_OFFSET, DIV_FRACTION_3_BIT, DIV_FRACTION_0_BIT, DIV_FRACTION);

	SET_BIT(USART1_BASE + USART_CR1_OFFSET, UE_BIT);

	SET_BIT(USART1_BASE + USART_CR1_OFFSET, TE_BIT);

	SET_BIT(USART1_BASE + USART_CR1_OFFSET, RE_BIT);

}

//Send character
void usart1_send_char(const char ch)
{

//While TXE_BIT!=1
while(!READ_BIT(USART1_BASE + USART_SR_OFFSET, TXE_BIT))
;

REG(USART1_BASE+USART_DR_OFFSET)=ch;

}

//Receive character
char usart1_receive_char(void)
{
	while(!READ_BIT(USART1_BASE + USART_SR_OFFSET, RXNE_BIT))
	;
	return (char)REG(USART1_BASE + USART_DR_OFFSET);
}
void ustart1_handler(void){
	//receive the char and echo to screen
	if(READ_BIT(USART1_BASE + USART_SR_OFFSET, RXNE_BIT)==1){
		char ch = usart1_receive_char();

		if(ch =='\r')
			usart1_send_char('\r\n');

			else
			{
				usart1_send_char(ch);
			}
	}


}



int main(void)
{
	//Initialization the USART
	init_usart1();

	char *hello = "Hello world!\r\n"; // \r 是 carriage return

	//Send Hello world
	while (*hello != '\0')
		usart1_send_char(*hello++);//一個character傳送


	//Receive char and resend it
	char ch;
	while (1)
	{
		ch = usart1_receive_char();

		if (ch == '\r')
			usart1_send_char('\n');

		usart1_send_char(ch);
	}

	

}
