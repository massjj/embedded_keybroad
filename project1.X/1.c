#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void USART_Init(unsigned int ubrr) {
    // Set baud rate 
    UBRR0 = ubrr;
    // Double Transmission Speed
    UCSR0A |= (1 << U2X0);
    // Enable receiver and transmitter 
    UCSR0B |= (1 << RXEN0)|(1 << TXEN0);
    // Set frame format: 8data 
    UCSR0C |= (1 << UCSZ01)|(1 << UCSZ00);
}
void USART_Transmit( unsigned char data ) {
     //LED-state high
    PORTD |= (1 << PORTD7);
    _delay_ms(50);      
    //ascii Put data into buffer, sends the data 
    //USART_Transmit
    while ( !( UCSR0A & (1 << UDRE0)) ) ;
    UDR0 = data;
    _delay_ms(300);
    //LED-state low
    PORTD &= ~(1 << PORTD7);
    _delay_ms(10);
}

void TIMER1_Init() {
    //clear OC0B and fast PWM mode 3
    TCCR0A |= (1 << COM0B1)| (1 << WGM01) | (1 << WGM00);
    //waveform and clk/64
    TCCR0B |=(1 << CS01) | (1 << CS00);

}

void keyScan() {
    
    //button
        //taking value from the column
        uint8_t pressC = PINB ;
        //toggle row
        DDRB ^= (1 <<  DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);
        PORTB ^= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3);
        //toggle col
        DDRC ^= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3);
        PORTC ^= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
        _delay_ms(5);
        
        // taking value from the row
        int temp = PINC ;
        uint8_t pressR = temp << 4 ; // shift to front
        
        //assembly value from col and row
        uint8_t press = pressC | pressR;
        
        //Compare
        //Column one
        if(press == 0b11101110){
            USART_Transmit(65); //A
           
        }
        else if (press == 0b11011110){
            USART_Transmit(66); //B
           
        }
        else if (press == 0b10111110){
            USART_Transmit(67); //C
            _delay_ms(300);
        }
        else if(press == 0b01111110){
            USART_Transmit(68); //D
           
        }
        //Column Two
        else if(press == 0b11101101){
            USART_Transmit(69); //E
         
        }
        else if(press == 0b11011101){
            USART_Transmit(70); //F
          
        }
        else if(press == 0b10111101){
            USART_Transmit(71); //G
        
        }
        else if(press == 0b01111101){
            USART_Transmit(72); //H
         
        }
        //Column Three
        else if(press == 0b11101011){
            USART_Transmit(73); //I
     
        }
        else if(press == 0b11011011){
            USART_Transmit(74); //J
         
        }
        else if(press == 0b10111011){
            USART_Transmit(75); //K
      
        }
        else if(press == 0b01111011){
            USART_Transmit(76); //L
        
        }
         //Column Four
        else if(press == 0b11100111){
            USART_Transmit(77); //M
  
        }
        else if(press == 0b11010111){
            USART_Transmit(78); //N

        }
        else if(press == 0b10110111){
            USART_Transmit(79); //O

        }
        else if(press == 0b01110111){
            USART_Transmit(80); //P
        }
}
 
unsigned char n_led = 1;

int main(void) {
    USART_Init(103);
    TIMER1_Init();
    
    //set trig port to output,LED-Bright
    DDRD |= (1 << DDD5);
    
    //set trig port to output, LED-state
    DDRD |= (1 << DDD7);
    
    //button input light
    DDRD &= ~(1 << DDD2);
    PORTD |= (1 << PORTD2);
    
    //output row button
    DDRB |= (1 <<  DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);
    PORTB &= ~(1 << PORTB0) & ~(1 << PORTB1) & ~(1 << PORTB2) & ~(1 << PORTB3);
    
    //input col button
    DDRC &= ~(1 <<  DDC0) & ~(1 << DDC1) & ~(1 << DDC2) & ~(1 << DDC3);
    PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
    
    while (1) {
        //keyScan
        keyScan();
        //light
        if(!(PIND & (1 << PORTD2))){
            //level light
            switch(n_led){
                case 1:
                    OCR0B = 0;
                break;
                case 2:
                    OCR0B = 50;
                break;
                case 3:
                    OCR0B = 100;
                    break;
               case 4:
                   OCR0B = 255;
                   n_led = 0;
                   break;
                }
            n_led++;
            _delay_ms(300);
            }
        if (PINC & ((1 << PORTC0)| (1 << PORTC1)| (1 << PORTC2)| (1 <<PORTC3)))
        {
            PORTD |= (1 << PORTD6);
        }
        else{
            PORTD &= ~(1 << PORTD6);
        }
        
    }
}