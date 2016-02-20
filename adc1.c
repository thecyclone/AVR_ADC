#define F_CPU 16000000UL	
#include <avr/io.h>                                                 											   
#include <util/delay.h>	
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>							         
#define ADC_1    0    							   //Defining the ADC PIN to which LDR is connected
#define ADC_2    1 								  //Defining the ADC PIN to which THERMISTOR is connected


uint8_t ADC_converted;               					     //Variable used to store the converted value read from the ADC
uint8_t i=ADC_1;            					  

void ADC_init()
{
	/*At Higher Frequency, conversion is fast but it is not very accurate.
	So, we trade accuracy for speed */
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE));    //ADC Prescaler of 128 i.e 16Mhz/128 = 125Khz the ADC clock
	ADMUX  |= (1<<REFS0);                            			// We are using Vcc for reference i.e Vref = Vcc.
	ADCSRA |= (1<<ADEN)|(1<<ADSC);                   			//Turn on ADC and Do an initial conversion because this one is the slowest and to ensure that everything is up in running-Free running mode
}
 
uint8_t read_ADC(uint8_t channel)
{
	channel &= 0b00000111;
	ADMUX &= 0xF0;                              //Clear the older channel that was read-i.e MUX0-3 cleared without effecting REFS0-1 ,ADLAR and the reserved bit.
	ADMUX |= channel;                          //Defines the new ADC channel to be read
	//ADCSRA = 0x8F;							  // Enable the ADC and its interrupt feature
	//ADCSRA |= (1<<ADSC);                     //Starts a new conversion
	while(ADCSRA & (1<<ADSC));                //Wait until the conversion is done
	return ADC;                           //Returns the ADC converted value of the chosen channel
}

ISR(ADC_vect)
{
 	sleep_disable();
 	
	for(i=0; i<=1; i++)
		{	
			
 			if(i==ADC_1)
 			{
 				if(ADC_converted < 50)
 				{
 					PORTB |= (1<<PB5);    //If ADC value is Below "10"(i.e darkness) turn led on
 				}
 				else
 				{
 					PORTB &= ~(1<<PB5);    //Else turn led off
 				}
 			}
 			else if(i==ADC_2)
 			{
 				if(ADC_converted > 80)
 				{
 					PORTB |= (1<<PB4);    //If ADC value is Above 80(i.e temperature higher than room temperature) turn led on
 				}
 				else if(ADC_converted < 80)
 				{
 					PORTB &= ~(1<<PB4);    //Else turn led off
 				}
 			}
 		}
}

void powermgmt()
{
	set_sleep_mode(SLEEP_MODE_ADC);
	sleep_cpu();
	power_spi_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_twi_disable();
}

int main(void)
{
		 
	powermgmt();
	ADC_init();
	 
	DDRB |= (1 << PB5)|(1 << PB4);       							//Set DDR of PORTB of LED 1 & LED 2-High(output)
	sei();				// Enable Global Interrupts
	ADCSRA |= 1<<ADSC;		// Start Conversion		
	while(1)										 
	{ 
 	ADC_converted = read_ADC(i);                 //Read one ADC channel 	
	}
	return 0;
}