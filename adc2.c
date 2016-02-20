#define F_CPU 16000000UL	
#include <avr/io.h>                                                 											   
#include <util/delay.h>	
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/sleep.h>							         
#define ADC_1    0    							   //LDR Pin
#define ADC_2    1 								  //Thermistor Pin
      					  

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
	set_sleep_mode(SLEEP_MODE_ADC);  //sleep in ADC Noise Reduction Mode	
	power_spi_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_twi_disable();
	power_usart0_disable();
}

int main(void)
{
	ADC_init();
	uint8_t ADC_LDR;               			//Variable used to store ADC output of LDR 
	uint8_t ADC_Th;                         //Variable used to store ADC output of Thermistor 
	DDRD |= (1 << PD0);
	PORTD ^= (0 << PD0);									//Set PDO as ground
	DDRB |= (1 << PB5);       							//Set DDR of PORTB of LED 1	as output	
	DDRB |= (1 << PB4);									//Set DDR of PORTB of LED 2 as output
	powermgmt(); 
	while(1)										 
	{ 
		sleep_cpu();
		ADC_LDR = read_ADC(0);
		ADC_Th = read_ADC(1);
		if((ADC_LDR < 40) & (ADC_Th > 100))
		{
			sleep_disable();
			PORTB |= (1 << PB5);
			PORTB |= (1 << PB4);
		}
		else if((ADC_LDR < 40) & (ADC_Th < 100))
		{
			sleep_disable();
			PORTB |= (1 << PB5);
			PORTB &= ~(1 << PB4);
		}
		else if((ADC_LDR > 50) & (ADC_Th > 100))
		{
			sleep_disable();
			PORTB &= ~(1 << PB5);
			PORTB |= (1 << PB4);
		}
		else
		{
			sleep_disable();
			PORTB &= ~(1 << PB5);
			PORTB &= ~(1 << PB4);
		}
 		
	}
	return 0;
}