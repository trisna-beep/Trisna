#include <avr/io.h>
#include <util/delay.h>
#include "LCD_16x2_H_file.h" // Include the LCD library

#define F_CPU 16000000UL  // Define CPU frequency for delay

// Stepper motor control pins
#define IN1 PD3
#define IN2 PD4
#define IN3 PD5
#define IN4 PD6

// Initialize ADC
void ADC_Init()
{
	ADMUX = (1<<REFS0); // AVcc with external capacitor at AREF pin
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1); // Enable ADC and set prescaler to 64
}

// Read ADC value from the specified channel
uint16_t ADC_Read(uint8_t channel)
{
	channel &= 0x07; // Ensure channel is between 0 and 7
	ADMUX = (ADMUX & 0xF8) | channel; // Select ADC channel
	ADCSRA |= (1<<ADSC); // Start conversion
	while(ADCSRA & (1<<ADIF)); // Wait for conversion to complete
	ADCSRA |= (1<<ADIF); // Clear ADIF by writing one to it
	return ADC;
}

// Initialize stepper motor control pins
void Stepper_Init()
{
	DDRD |= (1<<IN1) | (1<<IN2) | (1<<IN3) | (1<<IN4); // Set control pins as output
}

// Stepper motor steps
void Stepper_Step(uint8_t step)
{
	switch(step)
	{
		case 0:
		PORTD = (1<<IN1);
		break;
		case 1:
		PORTD = (1<<IN1) | (1<<IN2);
		break;
		case 2:
		PORTD = (1<<IN2);
		break;
		case 3:
		PORTD = (1<<IN2) | (1<<IN3);
		break;
		case 4:
		PORTD = (1<<IN3);
		break;
		case 5:
		PORTD = (1<<IN3) | (1<<IN4);
		break;
		case 6:
		PORTD = (1<<IN4);
		break;
		case 7:
		PORTD = (1<<IN4) | (1<<IN1);
		break;
	}
}

// Main function
int main(void)
{
	uint16_t adc_value;
	uint8_t step = 0;
	double angle = 0.0;
	char buffer[10];
	
	ADC_Init(); // Initialize ADC
	Stepper_Init(); // Initialize stepper motor
	LCD_Init(); // Initialize LCD
	
	while(1)
	{
		adc_value = ADC_Read(0); // Read ADC value from channel 0
		step = (adc_value / 128) % 8; // Convert ADC value to step (0-7)
		Stepper_Step(step); // Perform stepper motor step
		angle = (adc_value / 1023.0) * 360; // Calculate angle in degrees
		
		// Display ADC value and angle on LCD
		LCD_Clear();
		LCD_String_xy(0, 0, "ADC Value: ");
		itoa(adc_value, buffer, 10);
		LCD_String_xy(0, 11, buffer);
		LCD_String_xy(1, 0, "Angle: ");
		dtostrf(angle, 5, 2, buffer);
		LCD_String_xy(1, 7, buffer);
		LCD_String_xy(1, 12, "deg");
		
		_delay_ms(1000); // Delay for smooth operation
	}
	
	return 0;
}
