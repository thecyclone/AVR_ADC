#define F_CPU 16000000UL	
#include <avr/io.h>                                                 											   
#include <util/delay.h>	
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/sleep.h>							         
#define ADC_1    0    							   //Defining the ADC PIN to which LDR is connected
#define ADC_2    1 								  //Defining the ADC PIN to which THERMISTOR is connected


uint8_t ADC_converted;               					     //Variable used to store the converted value read from the ADC									

void ADC_init(void)
{
	/*At Higher Frequency, conversion is fast but it is not very accurate.
	So, we trade accuracy for speed */
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //ADC Prescaler of 128 i.e 16Mhz/128 = 125Khz the ADC clock
	ADMUX  |= (1<<REFS0);                            // We are using Vcc for reference i.e Vref = Vcc.
	ADCSRA |= (1<<ADEN)|(1<<ADSC);                   //Turn on ADC and Do an initial conversion because this one is the slowest and to ensure that everything is up in running-Free running mode
}
 
uint8_t read_ADC(uint8_t channel)
{
	channel &= 0b00000111;
	ADMUX &= 0xF0;                             //Clear the older channel that was read-i.e MUX0-3 cleared without effecting REFS0-1 ,ADLAR and the reserved bit.
	ADMUX |= channel;                         //Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);                     //Starts a new conversion

	while(ADCSRA & (1<<ADSC));                //Wait until the conversion is done
	return ADC;                           //Returns the ADC converted value of the chosen channel
}

void powermgmt() //Code to turn off periphery and define the type of sleep mode
{
	set_sleep_mode(SLEEP_MODE_ADC);	 //Sleep mode is ADC Noise Reduction
	power_spi_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_twi_disable();
	power_usart0_disable();
}

int main(void)
{
	ADC_init();
	powermgmt(); 
	uint8_t i;
	DDRB |= (1 << PB5)|(1 << PB4);       							//Set DDR of PORTB of LED 1 & LED 2-High(output)		
	while(1)										 
	{ 
		sleep_cpu();
 		for(i=0; i<=1; i++)
		{	
 			if(i==0)
 			{
 				ADC_converted = read_ADC(0);                 //Read one ADC channel 
 				sleep_disable();
 				if(ADC_converted < 50)
 				{
 					PORTB |= (1<<PB5);    //If ADC value is Below "10"(i.e darkness) turn led on
 				}
 				else
 				{
 					PORTB &= ~(1<<PB5);    //Else turn led off
 				}
 			}
 			else if(i==1)
 			{
 				ADC_converted = read_ADC(1);                 //Read one ADC channel
 				sleep_disable();
 				if(ADC_converted > 120)
 				{
 					PORTB |= (1<<PB4);    //If ADC value is Above 121(i.e temperature higher than room temperature) turn led on
 				}
 				else if(ADC_converted < 120)
 				{
 					PORTB &= ~(1<<PB4);    //Else turn led off
 				}
 			}
 			_delay_ms(400);					 //0.4 second switching between LDR and NTC
 		}
	}
	return 0;
}