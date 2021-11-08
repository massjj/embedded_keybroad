#define F_CPU 8000000UL

#define MAX 17
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


char buffer[10];
char receive[10];
char string[] = {114,101,97,100,32};
char true[] = {32,58,32,84,82,85,69,32,'\n\r'};
char false[] = {32,58,32,70,65,76,83,69,32,'\n\r'};
int i; 
int j;
unsigned char n_led = 1;
unsigned char value = 0;
unsigned char received_char = 0;
char history[25];

int arr[MAX];
int front = 0;
int rear = 0;

void USART_Init(unsigned int ubrr) {
    // Set baud rate 
    UBRR0 = ubrr;
    // Double Transmission Speed
    UCSR0A |= (1 << U2X0);
    // Enable receiver and transmitter and receive complete interrupt 
    UCSR0B |= (1 << RXEN0)|(1 << TXEN0) | (1<<RXCIE0);
    // Set frame format: 8data 
    UCSR0C |= (1 << UCSZ01)|(1 << UCSZ00);
}

void TIMER1_Init() {
    //clear OC0B and fast PWM mode 3 and waveform
    TCCR0A |= (1 << COM0B1)| (1 << WGM01) | (1 << WGM00);
    //clk/64 = 125kHz
    TCCR0B |=(1 << CS01) | (1 << CS00);

}
unsigned char USART_Receive(){ //input
    // Wait for data to be received
    while ( !(UCSR0A & (1 << RXC0)) );
    //Get and return received data from buffer
    return UDR0;
}

void USART_Transmit( uint8_t data ){ //output
    //Wait for empty transmit buffer
    while ( !( UCSR0A & (1 << UDRE0)) );
    //Put data into buffer, sends the data 
    UDR0 = data; 
}

void LED(){
    //clear OC0A and fast PWM mode 3 and waveform
    TCCR0A |= (1 << COM0A1)| (1 << WGM01) | (1 << WGM00);
    //clk/64
    TCCR0B |=(1 << CS01) | (1 << CS00);
    //LED
    OCR0A = 255;
    _delay_ms(200);
    OCR0A = 0;
    _delay_ms(30);
}

void LED_bright(){
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
}


void deletion(){
    for(int i = 0; i< rear - 1; i++ ){
        arr[i] = arr[i + 1];      
    }
    rear = rear -1 ;
}

void insert(unsigned char received ){
    if((front == 0 && rear == MAX-1) || (front == rear+1)){
        deletion();  
    }
    if(front == -1){
        front = -1;
        rear = 0;
    }
    else {
        arr[rear] = received ;
        rear = rear+1;
        
    }
}

void button(unsigned char received){
    USART_Transmit(received); 
    LED();
    insert(received);
    _delay_ms(60);
}

//check status
ISR(USART_RX_vect){
    unsigned char data = UDR0;    
    //interrupt by =
    if(data == 61){
        //choose buffer for proof
        for(i = 0;i<sizeof(buffer);i++){
        unsigned char received_char = USART_Receive();
        buffer[i] = received_char;
        //buffer correct
        if (buffer[i] == value){            
            USART_Transmit(32);
            
            for(i = 0;i<sizeof(string);i++){
                USART_Transmit(string[i]);}
                    
            USART_Transmit(received_char);
              
            for(i = 0;i<sizeof(true);i++){
                USART_Transmit(true[i]);}
            
        }
        //buffer incorrect
        else{
            USART_Transmit(32);
            
            for(i = 0;i<sizeof(string);i++){
                USART_Transmit(string[i]);}
                    
            USART_Transmit(received_char);
              
            for(i = 0;i<sizeof(false);i++){
                USART_Transmit(false[i]);}
            }
        }
    }    
    // press ? to check the history
    if(data == 63){
        for(i = 0;i<sizeof(arr);++i){
            USART_Transmit(arr[i]);} 
        }
    
    if(data == 48){
        OCR0B = 0; //0
    }
    if(data == 49){
        OCR0B = 50; //1
    }
    if(data == 50){
        OCR0B = 100; //2
    }
    if(data == 51){
        OCR0B = 255; //3
    }
}

// check history

int main(void) {
    USART_Init(103);
    TIMER1_Init();
    sei();

    //set trig port to output,LED-Bright
    DDRD |= (1 << DDD5);
 
    //set trig port to output,LED-status
    DDRD |= (1 << DDD6);
    
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
        LED_bright();
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
        uint8_t pressR = temp << 4 ; // chip to front
        
        //assembly value from col and row
        uint8_t press = pressC | pressR;
        
        //Compare
        //Row one..

        if(press == 0b11101110){ //A
            value = 65;
            received_char = 65;
            button(65);
        }

        else if (press == 0b11011110){ //B
            value = 66;
            received_char = 66;
            button(66);
        }
        else if (press == 0b10111110){ //C
            value = 67;
            received_char = 67;
            button(67);
        }
        else if(press == 0b01111110){ //D
            value = 68;
            received_char = 68;
            button(68);
        }
        //Column Two
        else if(press == 0b11101101){ //E
            value = 69;
            received_char = 69;
            button(69);
        }
        else if(press == 0b11011101){ //F
            value = 70;
            received_char = 70;
            button(70);
        }
        else if(press == 0b10111101){ //G
            value = 71;
            received_char = 71;
            button(71);
        }
        else if(press == 0b01111101){ //H
            value = 72;
            received_char = 72;
            button(72);
        }
        //Column Three
        else if(press == 0b11101011){ //I
            value = 73;
            received_char = 73;
            button(73);
        }
        else if(press == 0b11011011){ //J
            value = 74;
            received_char = 74;
            button(74);
        }
        else if(press == 0b10111011){ //K
            value = 75;
            received_char = 75;
            button(75);
        }
        else if(press == 0b01111011){ //L
            value = 76;
            received_char = 76;
            button(76);
        }
         //Column Four
        else if(press == 0b11100111){ //M
            value = 77;
            received_char = 77;
            button(77);
        }
        else if(press == 0b11010111){ //N
            value = 78;
            received_char = 78;
            button(78);
        }
        else if(press == 0b10110111){ //O
            value = 79;
            received_char = 79;
            button(79);
        }
        else if(press == 0b01110111){ //P
            value = 80;
            received_char = 80;
            button(80);
        }

        
    }
}