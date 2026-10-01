#include <xc.h>

#define _XTAL_FREQ 20000000UL

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF


/* ======================================================
   NODE 2 PIN MAP
   ======================================================

   LDR
   RA0 / AN0

   ULTRASONIC 1
   RB0 = TRIG
   RB1 = ECHO

   ULTRASONIC 2
   RB2 = TRIG
   RB3 = ECHO

   RAIN GAUGE
   RB4

   STATUS
   RC0 = GREEN LED
   RC1 = YELLOW LED
   RC2 = RED LED
   RC5 = BUZZER

   UART
   RC6 = TX
   RC7 = RX

   LCD
   RD0 = RS
   RD1 = EN
   RD4 = D4
   RD5 = D5
   RD6 = D6
   RD7 = D7
*/


/* ======================================================
   LCD
   ====================================================== */

#define LCD_RS RD0
#define LCD_EN RD1


void LCD_Command(unsigned char cmd)
{
    LCD_RS = 0;

    RD4 = (cmd >> 4) & 1;
    RD5 = (cmd >> 5) & 1;
    RD6 = (cmd >> 6) & 1;
    RD7 = (cmd >> 7) & 1;

    LCD_EN = 1;
    __delay_us(2);
    LCD_EN = 0;

    RD4 = cmd & 1;
    RD5 = (cmd >> 1) & 1;
    RD6 = (cmd >> 2) & 1;
    RD7 = (cmd >> 3) & 1;

    LCD_EN = 1;
    __delay_us(2);
    LCD_EN = 0;

    __delay_ms(2);
}


void LCD_Char(unsigned char data)
{
    LCD_RS = 1;

    RD4 = (data >> 4) & 1;
    RD5 = (data >> 5) & 1;
    RD6 = (data >> 6) & 1;
    RD7 = (data >> 7) & 1;

    LCD_EN = 1;
    __delay_us(2);
    LCD_EN = 0;

    RD4 = data & 1;
    RD5 = (data >> 1) & 1;
    RD6 = (data >> 2) & 1;
    RD7 = (data >> 3) & 1;

    LCD_EN = 1;
    __delay_us(2);
    LCD_EN = 0;

    __delay_us(100);
}


void LCD_String(const char *str)
{
    while(*str)
    {
        LCD_Char(*str);
        str++;
    }
}


void LCD_Init(void)
{
    __delay_ms(20);

    LCD_Command(0x02);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);

    __delay_ms(5);
}


void LCD_Number(unsigned int num)
{
    LCD_Char((num / 1000) + '0');
    LCD_Char(((num / 100) % 10) + '0');
    LCD_Char(((num / 10) % 10) + '0');
    LCD_Char((num % 10) + '0');
}


/* ======================================================
   ADC - LDR
   RA0 / AN0
   ====================================================== */

void ADC_Init(void)
{
    ADCON0 = 0x41;
    ADCON1 = 0x8E;
}


unsigned int ADC_Read(void)
{
    __delay_us(20);

    GO_nDONE = 1;

    while(GO_nDONE);

    return ((unsigned int)ADRESH << 8) | ADRESL;
}


/* ======================================================
   TIMER1
   ====================================================== */

void Timer1_Init(void)
{
    T1CON = 0x10;

    TMR1H = 0;
    TMR1L = 0;

    TMR1IF = 0;
}


/* ======================================================
   ULTRASONIC 1
   RB0 = TRIG
   RB1 = ECHO
   ====================================================== */

unsigned int Ultrasonic1(void)
{
    unsigned int count;

    RB0 = 0;
    __delay_us(5);

    RB0 = 1;
    __delay_us(10);
    RB0 = 0;


    /* Wait for echo HIGH */

    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1ON = 1;

    while(RB1 == 0)
    {
        if(TMR1IF)
        {
            TMR1ON = 0;
            TMR1IF = 0;

            return 999;
        }
    }


    /* Echo started */

    TMR1ON = 0;

    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1ON = 1;


    /* Measure echo HIGH duration */

    while(RB1 == 1)
    {
        if(TMR1IF)
        {
            TMR1ON = 0;
            TMR1IF = 0;

            return 999;
        }
    }


    TMR1ON = 0;

    count = ((unsigned int)TMR1H << 8) | TMR1L;

    return count / 145;
}


/* ======================================================
   ULTRASONIC 2
   RB2 = TRIG
   RB3 = ECHO
   ====================================================== */

unsigned int Ultrasonic2(void)
{
    unsigned int count;

    RB2 = 0;
    __delay_us(5);

    RB2 = 1;
    __delay_us(10);
    RB2 = 0;


    /* Wait for echo HIGH */

    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1ON = 1;

    while(RB3 == 0)
    {
        if(TMR1IF)
        {
            TMR1ON = 0;
            TMR1IF = 0;

            return 999;
        }
    }


    /* Echo started */

    TMR1ON = 0;

    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1ON = 1;


    /* Measure echo HIGH duration */

    while(RB3 == 1)
    {
        if(TMR1IF)
        {
            TMR1ON = 0;
            TMR1IF = 0;

            return 999;
        }
    }


    TMR1ON = 0;

    count = ((unsigned int)TMR1H << 8) | TMR1L;

    return count / 145;
}


/* ======================================================
   UART
   RC6 = TX
   RC7 = RX
   ====================================================== */

void UART_Init(void)
{
    TRISC6 = 0;
    TRISC7 = 1;

    /* 9600 baud @ 20 MHz */

    SPBRG = 129;

    BRGH = 1;
    SYNC = 0;
    SPEN = 1;

    TXEN = 1;
    CREN = 1;
}


void UART_SendChar(char data)
{
    while(!TXIF);

    TXREG = data;
}


/*
   FINAL PROTOCOL

   S = SAFE
   W = WARNING
   C = CRITICAL
*/

void UART_SendStatus(unsigned char status)
{
    if(status == 0)
    {
        UART_SendChar('S');
    }

    else if(status == 1)
    {
        UART_SendChar('W');
    }

    else
    {
        UART_SendChar('C');
    }
}


/* ======================================================
   STATUS OUTPUT
   ====================================================== */

void SetStatus(unsigned char status)
{
    /* Turn everything OFF first */

    RC0 = 0;
    RC1 = 0;
    RC2 = 0;
    RC5 = 0;


    /* SAFE */

    if(status == 0)
    {
        RC0 = 1;
    }


    /* WARNING */

    else if(status == 1)
    {
        RC1 = 1;
    }


    /* CRITICAL */

    else
    {
        RC2 = 1;

        RC5 = 1;
        __delay_ms(150);
        RC5 = 0;
    }
}


/* ======================================================
   MAIN
   ====================================================== */

void main(void)
{
    unsigned int distance1;
    unsigned int distance2;
    unsigned int nearest;

    unsigned int light;

    unsigned int rain_tips = 0;

    unsigned char previous_rain;

    unsigned char risk_score;

    unsigned char status;


    /* ==================================================
       PORT CONFIGURATION
       ================================================== */

    /* LDR */

    TRISA0 = 1;


    /* Ultrasonic 1 */

    TRISB0 = 0;
    TRISB1 = 1;


    /* Ultrasonic 2 */

    TRISB2 = 0;
    TRISB3 = 1;


    /* Rain gauge */

    TRISB4 = 1;


    /* LEDs + buzzer */

    TRISC0 = 0;
    TRISC1 = 0;
    TRISC2 = 0;
    TRISC5 = 0;


    /* UART */

    TRISC6 = 0;
    TRISC7 = 1;


    /* LCD */

    TRISD = 0x00;


    /* Clear outputs */

    PORTD = 0x00;

    RB0 = 0;
    RB2 = 0;

    RC0 = 0;
    RC1 = 0;
    RC2 = 0;
    RC5 = 0;


    previous_rain = RB4;


    /* ==================================================
       INITIALIZATION
       ================================================== */

    LCD_Init();

    ADC_Init();

    Timer1_Init();

    UART_Init();


    /* ==================================================
       START SCREEN
       ================================================== */

    LCD_Command(0x01);

    LCD_String("NODE 2");

    LCD_Command(0xC0);

    LCD_String("SAFETY SYSTEM");

    __delay_ms(2000);


    /* ==================================================
       MAIN LOOP
       ================================================== */

    while(1)
    {
        /* ----------------------------------------------
           READ ULTRASONIC
           ---------------------------------------------- */

        distance1 = Ultrasonic1();

        __delay_ms(50);

        distance2 = Ultrasonic2();

        __delay_ms(50);


        /* ----------------------------------------------
           FIND NEAREST OBJECT
           ---------------------------------------------- */

        if(distance1 < distance2)
            nearest = distance1;
        else
            nearest = distance2;


        /* ----------------------------------------------
           READ LDR
           ---------------------------------------------- */

        light = ADC_Read();


        /* ----------------------------------------------
           RAIN GAUGE
           ---------------------------------------------- */

        if(RB4 == 0 && previous_rain == 1)
        {
            __delay_ms(20);

            if(RB4 == 0)
            {
                rain_tips++;
            }
        }

        previous_rain = RB4;


        /* ----------------------------------------------
           RESET RISK
           ---------------------------------------------- */

        risk_score = 0;


        /* ----------------------------------------------
           DISTANCE SCORE
           ---------------------------------------------- */

        if(nearest < 20)
        {
            risk_score += 3;
        }

        else if(nearest <= 50)
        {
            risk_score += 2;
        }


        /* ----------------------------------------------
           LDR SCORE
           ---------------------------------------------- */

        if(light < 300)
        {
            risk_score += 2;
        }

        else if(light < 600)
        {
            risk_score += 1;
        }


        /* ----------------------------------------------
           RAIN SCORE
           ---------------------------------------------- */

        if(rain_tips >= 5)
        {
            risk_score += 2;
        }

        else if(rain_tips >= 1)
        {
            risk_score += 1;
        }


        /* ----------------------------------------------
           FINAL STATUS
           ---------------------------------------------- */

        if(risk_score >= 5)
        {
            status = 2;
        }

        else if(risk_score >= 3)
        {
            status = 1;
        }

        else
        {
            status = 0;
        }


        /* ----------------------------------------------
           LOCAL LED + BUZZER
           ---------------------------------------------- */

        SetStatus(status);


        /* ----------------------------------------------
           LCD - SENSOR DATA
           ---------------------------------------------- */

        LCD_Command(0x01);

        LCD_String("D1:");

        LCD_Number(distance1);

        LCD_String(" D2:");

        LCD_Number(distance2);


        LCD_Command(0xC0);

        LCD_String("RISK:");

        LCD_Char(risk_score + '0');

        __delay_ms(1000);


        /* ----------------------------------------------
           LCD - STATUS
           ---------------------------------------------- */

        LCD_Command(0x01);


        if(status == 0)
        {
            LCD_String("STATUS: SAFE");
        }

        else if(status == 1)
        {
            LCD_String("STATUS: WARNING");
        }

        else
        {
            LCD_String("STATUS: CRITICAL");
        }


        LCD_Command(0xC0);

        LCD_String("LDR:");

        LCD_Number(light);

        __delay_ms(1000);


        /* ----------------------------------------------
           LCD - RAIN
           ---------------------------------------------- */

        LCD_Command(0x01);

        LCD_String("RAIN TIPS:");

        LCD_Command(0xC0);

        LCD_Number(rain_tips);

        __delay_ms(1000);


        /* ----------------------------------------------
           SEND STATUS TO NODE 1
           ---------------------------------------------- */

        UART_SendStatus(status);


        /*
           Give Node 1 time to process the status.
           Node 1 only needs S/W/C.
        */

        __delay_ms(500);
    }
}
