#include "HeaderFiles.h"
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw() {
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN
            | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable GPIO port F peripherals
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOE
            | SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOF
            | SYSCTL_RCGC2_GPIOC;

    // Configure DIP Switches

    //Configure PORT A DIP switches
    GPIO_PORTA_DIR_R = 0x00;  // All bits are input
    GPIO_PORTA_DEN_R = 0xC0;  // enable DIP switch 3 and 2
    GPIO_PORTA_PUR_R = 0xC0;  // enable internal pull-up for DIP switch 3 and 2

    //Configure PORT B DIP switches
    GPIO_PORTB_DIR_R = 0x00;  // All bits are input
    GPIO_PORTB_DEN_R = 0x10;  // enable DIP switch 1
    GPIO_PORTB_PUR_R = 0x10;  // enable internal pull-up for DIP switch 1

    //Configure PORT D DIP switches
    GPIO_PORTD_DIR_R = 0x00;  // All bits are input
    GPIO_PORTD_DEN_R = 0x0F;  // enable DIP switch 7,8,9 and 10
    GPIO_PORTD_PUR_R = 0x0F; // enable internal pull-up for DIP switch 7,8,9 and 10

    //Configure PORT E DIP switches
    GPIO_PORTE_DIR_R = 0x00;  // All bits are input
    GPIO_PORTE_DEN_R = 0x0E;  // enable DIP switch 4,5 and 6
    GPIO_PORTE_PUR_R = 0x0E; // enable internal pull-up for DIP switch 4,5 and 6

    //Configure PORT F DIP switches
    GPIO_PORTF_DIR_R = 0x00;  // All bits are input
    GPIO_PORTF_DEN_R = 0x10;  // enable PUSH BUTTON
    GPIO_PORTF_PUR_R = 0x10;  // enable internal pull-up for PUSH BUTTON

    // Configure UART0 pins
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0; // turn-on UART0, leave other uarts in same status
    GPIO_PORTA_DEN_R |= 3;                         // default, added for clarity
    GPIO_PORTA_AFSEL_R |= 3;                       // default, added for clarity
    GPIO_PORTA_PCTL_R = GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;

    // Configure UART0 to 115200 baud, 8N1 format (must be 3 clocks from clock enable and config writes)
    UART0_CTL_R = 0;                 // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                 // use system clock (40 MHz)
    UART0_IBRD_R = 21; // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                               // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN; // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN; // enable TX, RX, and module

    // Configure UART1 pins
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1; // turn-on UART1, leave other uarts in same status
    GPIO_PORTC_DEN_R |= 0x30;                      // default, added for clarity
    GPIO_PORTC_AFSEL_R |= 0x30;                    // default, added for clarity
    GPIO_PORTC_PCTL_R |= GPIO_PCTL_PC5_U1TX | GPIO_PCTL_PC4_U1RX;

    // Configure UART1 to 250000 baud, 8N2 format
    UART1_CTL_R = 0;
    UART1_CC_R = UART_CC_CS_SYSCLK;
    UART1_IBRD_R = 10;
    UART1_FBRD_R = 0;
    UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2;
    UART1_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN | UART_CTL_EOT;
    UART1_IM_R = UART_IM_TXIM | UART_IM_RXIM;
    NVIC_EN0_R |= 1 << (INT_UART1 - 16);

    //Green LED Configuration
    GPIO_PORTF_DEN_R = 0x0E;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_DR2R_R = 0x0E;

    //Configure DATA ENABLE (PC6)
    GPIO_PORTC_DEN_R |= 0x40;
    GPIO_PORTC_DIR_R |= 0x40;
    GPIO_PORTC_DR2R_R |= 0x40;

    //PWM Mode for On-circuit LED
    GPIO_PORTB_DIR_R |= 0x20;   // make bit5 an output
    GPIO_PORTB_DR2R_R |= 0x20;  // set drive strength to 2mA
    GPIO_PORTB_DEN_R |= 0x20;   // enable bit5 for digital
    GPIO_PORTB_AFSEL_R |= 0x20; // select auxilary function for bit 5
    GPIO_PORTB_PCTL_R = GPIO_PCTL_PB5_M0PWM3; // enable PWM on bit 5

    //Configure PWM0 Module
    SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;             // turn-on PWM0 module
    __asm(" NOP");                                   // wait 3 clocks
    __asm(" NOP");
    __asm(" NOP");
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R0;                // reset PWM0 module
    SYSCTL_SRPWM_R = 0;                              // leave reset state
    PWM0_1_CTL_R = 0;                                // turn-off PWM0 generator 1
    PWM0_1_GENB_R = PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTLOAD_ONE;       // output 3 on PWM0, gen 1b, cmpb
    PWM0_1_LOAD_R = 1024;                            // set period to 40 MHz sys clock / 2 / 1024 = 19.53125 kHz
    PWM0_INVERT_R = PWM_INVERT_PWM3INV;              // invert outputs for duty cycle increases with increasing compare values
    PWM0_1_CMPB_R = 0;
    PWM0_1_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM0 generator 1
    PWM0_ENABLE_R = PWM_ENABLE_PWM3EN;               // enable outputs

    // Configure Timer 1 as the time base
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;       // turn-on timer
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER1_TAILR_R = 0x2625A;                        // set load value to 40e4 for 100 Hz interrupt rate
    TIMER1_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    NVIC_EN0_R |= 1 << (INT_TIMER1A-16);             // turn-on interrupt 37 (TIMER1A)
    TIMER1_CTL_R |= TIMER_CTL_TAEN;                  // turn-on timer

}

void putcUart0(char c) {
    while (UART0_FR_R & UART_FR_TXFF)
        {
yield();
}
    UART0_DR_R = c;
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++)
        putcUart0(str[i]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0() {
    while (UART0_FR_R & UART_FR_RXFE){
yield();
}
        ;
    return UART0_DR_R & 0xFF;
}

// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock
void waitMicrosecond(uint32_t us) {
    // Approx clocks per us
    __asm("WMS_LOOP0:   MOV  R1, #6");
    // 1
    __asm("WMS_AGAIN:   SUB  R1, #1");
    // 6
    __asm("             CBZ  R1, WMS_DONE1");
    // 5+1*3
    __asm("             NOP");
    // 5
    __asm("             B    WMS_AGAIN");
    // 5*3
    __asm("WMS_DONE1:   SUB  R0, #1");
    // 1
    __asm("             CBZ  R0, WMS_DONE0");
    // 1
    __asm("             B    WMS_LOOP0");
    // 1*3
    __asm("WMS_DONE0:");
    // ---
    // 40 clocks/us + error
}

void led_blink() {
    GREEN_LED = 1;
    waitMicrosecond(250000);
    GREEN_LED = 0;
}

void DIP() {
    OperationMode = !PIN_10;
    DIPvalue = ((!PIN_9) * 256) + ((!PIN_8) * 128) + ((!PIN_7) * 64)
            + ((!PIN_6) * 32) + ((!PIN_5) * 16) + ((!PIN_4) * 8)
            + ((!PIN_3) * 4) + ((!PIN_2) * 2) + ((!PIN_1) * 1);
    sprintf(string_buffer, "\r\nThe DIP address is : %d\r\n", DIPvalue);
    putsUart0(string_buffer);
    if (OperationMode == 1) {
        putsUart0("The controller is in Master Mode\r\n");
        DATA_ENABLE = 1;
    } else if (OperationMode == 0) {
        putsUart0("The controller is in Slave Mode\r\n");
        DATA_ENABLE = 0;
    }
}

void getString() {
    i = 0;
    for (j = 0; j <= 80; j++) {
        input[j] = 0;
    }
    putsUart0("\r\n");
    putsUart0("Enter the command\r\n");
    while (1) {
        ch = getcUart0();
        putcUart0(ch);   // Delay in printing on Tera Term as compared to typing
        if (ch == 13) {
            break;
        } else if (ch == 8) {
            i--;
            putcUart0(32);
            putcUart0(8);
            input[i] = 0;
        } else if (ch <= 44 || ch == 47 || (ch >= 58 && ch <= 64)
                || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 127)) {
            input[i] = 32;
            i++;
        } else if (ch >= 97 && ch <= 122) {
            ch = ch - 32;
            input[i] = ch;
            i++;
        } else {
            input[i] = ch;
            i++;
        }

    }
    putsUart0("\r\n");
    sprintf(string_buffer, "The string entered is %s", input);
    putsUart0(string_buffer);
    putsUart0("\r\n");
    ACTIVITY_LED = 1;
    ACTIVITY_LED_TIMEOUT = 25;
//  Removing the spaces in the input string
    for (j = 0; j <= 80; j++) {
        new_input[j] = 0;
    }
    j = 0;
    for (i = 0; i < strlen(input); i++) {
        if (input[i] == 32) {
            if (input[i - 1] == 32 || i == 0) {

            } else {
                new_input[j] = input[i];
                putcUart0(new_input[j]);
                j++;
            }
        } else {
            new_input[j] = input[i];
            putcUart0(new_input[j]);
            j++;

        }
    }
    for (k = 0; k <= 80; k++) {
        CommandName[k] = 0;
    }
    k = 0;
    i = 0;
    while (new_input[i] != 32) {
        if (i >= strlen(new_input)) {
            break;
        }
        CommandName[k] = new_input[i];
        k++;
        i++;

    }
    i++;
    for (k = 0; k <= 10; k++) {
        Address[k] = 0;
    }
    k = 0;
    while (new_input[i] != 32) {
        if (i > strlen(new_input)) {
            break;
        }
        Address[k] = new_input[i];
        k++;
        i++;
    }
    i++;
    for (k = 0; k <= 10; k++) {
        Value[k] = 0;
    }

    k = 0;
    while (new_input[i] != 0) {
        if (i > strlen(new_input)) {
            break;
        }
        Value[k] = new_input[i];
        k++;
        i++;
    }
}

int CheckCommand() {
    for (i = 0; i < strlen(CommandName); i++) {
        if (65 <= CommandName[i] && CommandName[i] <= 90) {
            putcUart0(32);
        } else {
            putcUart0(94);
            return 1;
        }
    }
    return 0;
}
int CheckAddress() {
    putcUart0(32);
    for (i = 0; i < strlen(Address); i++) {
        if (!(48 <= Address[i] && Address[i] <= 57)) {
            putcUart0(94);
            return 1;
        } else {
            putcUart0(32);
        }
    }
    return 0;
}

int CheckValue() {
    putcUart0(32);
    for (i = 0; i < strlen(Value); i++) {
        if (!(48 <= Value[i] && Value[i] <= 57)) {
            putcUart0(94);
            return 1;
        } else {
            putcUart0(32);
        }
    }
    return 0;
}

void IsCommand() {
    putsUart0("\r\n");
    CheckCommand();
    int i =0;
    address = atoi(&Address);
    value = atoi(&Value);
    if (strcmp("SET", CommandName) == 0) {
        if(OperationMode == 1)
        {
            if (CheckAddress() == 1) {
                putsUart0("\r\nPlease enter the valid integer\r\n");
            } else if (CheckValue() == 1) {
                putsUart0("\r\nPlease enter the valid integer\r\n");
            }

            else if(!(48 <= Address[0] && Address[0] <= 57))
            {
                putsUart0("\r\nEnter an Address and Value to SET a value");
            }
            else if(!(48 <= Value[0] && Value[0] <= 57))
            {
                putsUart0("\r\nEnter the Value to be SET");
            }
            else if (address < 1|| address > MAX)
            {
                sprintf(string_buffer, "\r\nPlease enter an address between 1 and %d", MAX);
                putsUart0(string_buffer);
            }
            else if(value > MAX_VALUE)
            {
                putsUart0("\r\nEnter a value from 1 to 255");
            }
           else {
                DMXData[address] = value;
                /*for (i = 0; i < MAX; i++) {
                    sprintf(string_buffer, "data[%d] = %d\r\n", i, DMXData[i]);
                    putsUart0(string_buffer);
                }*/
                sprintf(string_buffer,
                        "\r\nThe DMX DATA address %d is SET to value %d", address,
                        value);
                putsUart0(string_buffer);
            }

        }
        else if(OperationMode == 0)
        {
            putsUart0("\r\nYou cannot use SET command in Slave mode");
        }

    } else if (strcmp("GET", CommandName) == 0) {
        address = atoi(&Address);
        if (CheckAddress() == 1) {
            putsUart0("\r\nPlease enter the valid integer\r\n");
        }
        else if(!(48 <= Address[0] && Address[0] <= 57))
        {
            putsUart0("\r\nEnter an Address to GET a value");
        }
        else if (address < 1 || address > MAX)
        {
            sprintf(string_buffer, "\r\nPlease enter an address between 1 and %d", MAX);
            putsUart0(string_buffer);
        }
        else if(Value[0]!=0)
        {
            putsUart0("\r\nPlease enter one argument. GET command is an unary command");
        }
        else {
            sprintf(string_buffer,
                    "\r\nWe GET the value %d from the DMX Address %d\r\n",
                    DMXData[address], address);
            putsUart0(string_buffer);
        }
    }
    else if (strcmp("GETDATA", CommandName) == 0) {
           address = atoi(&Address);
           if (CheckAddress() == 1) {
               putsUart0("\r\nPlease enter the valid integer\r\n");
           }
           else if(!(48 <= Address[0] && Address[0] <= 57))
           {
               putsUart0("\r\nEnter an Address to GETDATA a value");
           }
           else if (address > 512)
           {
               sprintf(string_buffer, "You reached end of the array %d", MAX);
               putsUart0(string_buffer);
           }
           else if(Value[0]!=0)
           {
               putsUart0("\r\nPlease enter one argument. GETDATA command is an unary command");
           }
           else if (address == 0)
           {
               sprintf(string_buffer, "Slot 0 contains the START CODE, which has the value %d", DMXReceiveData[0]);
               putsUart0(string_buffer);
           }
           else {
               sprintf(string_buffer,
                       "\r\nWe GET the value %d from the Received Array Address %d\r\n",
                       DMXReceiveData[address], address);
               putsUart0(string_buffer);
           }
       }
    else if (strcmp("CLEAR", CommandName) == 0) {
        if (OperationMode == 1)
        {
            if(Address[0] != 0)
            {
                putsUart0("\r\nCLEAR command doesn't accept any arguments");
            }
            else
            {
                for (i = 0; i < MAX; i++) {
                    DMXData[i] = '\0';
                }
                sprintf(string_buffer, "\r\nDMX Data has been cleared\r\n");
                putsUart0(string_buffer);

            }

        }
        else if(OperationMode == 0)
        {
            putsUart0("\r\nYou cannot use CLEAR command in Slave Mode");
        }
    } else if (strcmp("MAX", CommandName) == 0) {
        if(OperationMode == 1)
        {
            if (CheckAddress() == 1) {
                putsUart0("\r\nPlease enter the valid integer\r\n");
            }
            else if(Value[0] != 0)
            {
                putsUart0("\r\nValue not required in MAX command");
            }
            else if(!(48 <= Address[0] && Address[0] <= 57))
            {
                putsUart0("\r\nPlease enter the new MAX Address with MAX command");
            }
            else if(0 > address || address > 512)
            {
                putsUart0("Enter the MAX address between 0 to 512");
            }
            else {
                address = atoi(&Address);
                MAX = address;
                sprintf(string_buffer,
                        "\r\nThe Maximum address value is changed to %d\r\n", MAX);
                putsUart0(string_buffer);

            }

        }
        else if (OperationMode == 0)
        {
            putsUart0("\r\nYou cannot use MAX command in Slave mode");
        }


    } else if (strcmp("ON", CommandName) == 0) {
        if(OperationMode == 1)
        {
            if(Address[0] != 0)
            {
                putsUart0("\r\nON command doesn't accept any arguments");
            }
            else if (UnderTransmission == 1) {
                putsUart0("\r\nTransmission in progress...\r\n");
            } else {
                sprintf(string_buffer, "\r\nThe DMX Data send flag is High\r\n");
                putsUart0(string_buffer);
                TransmitEnable = 1;
            }

        }
        else if(OperationMode == 0)
        {
            putsUart0("\r\nYou cannot use ON command in Slave mode");
        }

    } else if (strcmp("OFF", CommandName) == 0) {
        if(OperationMode == 1)
        {
            if(Address[0] != 0)
            {
                putsUart0("\r\nOFF command doesn't accept any arguments");
            }
            else
            {
                sprintf(string_buffer, "\r\nThe DMX Data send flag is Low\r\n");
                putsUart0(string_buffer);
                TransmitEnable = 0;
                UnderTransmission = 0;
            }

        }
        else if(OperationMode == 0)
        {
            putsUart0("\r\nYou cannot use OFF command in Slave mode");
        }
    }
    else if (strcmp("POLL", CommandName) == 0){
        if(Address[0] != 0 || Value[0] != 0)
        {
            putsUart0("\r\nPOLL command doesn't accept any arguments");

        }
        else
        {
            putsUart0("\r\nPolling Started\r\n");
            POLL_FLAG = 1;
        }

    }
    else if (strcmp("MODE", CommandName) == 0){
        address = atoi(&Address);
        if(Value[0] != 0 || CheckAddress() == 1)
        {
            if(Value[0] != 0)
            {
                putsUart0("\r\nPlease enter one argument. MODE command is an unary command");
            }
            else
            {
                putsUart0("\r\nPlease enter 0 for Slave and 1 for Master Configuration");
            }

        }
        else if(!(48 <= Address[0] && Address[0] <= 57))
        {
            putsUart0("\r\nEnter a Value too..");
        }
        else
        {
            OperationMode = address;
            setMode();
        }
    }
    else {
        putsUart0("\r\nEnter a valid command\r\n");
    }
}

void Uart1Interrupt() {
    if (OperationMode == 1 && TransmitEnable == 1)
    {
        if (index > MAX) {
            phase = BREAKMAB;
            DMXData[0] = 0;
            index = 1;
        }
        switch (phase) {
        case BREAKMAB:
            UART1_CTL_R = 0;
            UART1_CC_R = UART_CC_CS_SYSCLK;
            UART1_IBRD_R = 25;
            UART1_FBRD_R = 0;
            UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2;
            UART1_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN
                    | UART_CTL_EOT;
            UART1_IM_R = UART_IM_TXIM | UART_IM_RXIM;
            NVIC_EN0_R |= 1 << (INT_UART1 - 16);
            phase = STARTCODE;
            UART1_DR_R = 0x00;
            break;
        case STARTCODE:
            UART1_CTL_R = 0;
            UART1_CC_R = UART_CC_CS_SYSCLK;
            UART1_IBRD_R = 10;
            UART1_FBRD_R = 0;
            UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2;
            UART1_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN
                    | UART_CTL_EOT;
            UART1_IM_R = UART_IM_TXIM | UART_IM_RXIM;
            NVIC_EN0_R |= 1 << (INT_UART1 - 16);
            phase = PACKETS;
            UART1_DR_R = 0x00;
            break;
        case PACKETS:
            UART1_DR_R = DMXData[index++];
            break;
        default:
            break;
        }
        TRANSMIT_LED = 1;
        TRANSMIT_LED_TIMEOUT = 25;
    }
    else if (OperationMode == 0) {
        TransmitEnable = 0;
        RxData = UART1_DR_R;
        if(RxData & (1<<8))
        {
            RxIndex = 0;
            UpdateDevice();
        }
        else
        {
            DMXReceiveData[RxIndex++] = (0xFF & RxData);
            //DMXReceiveData[RxIndex--] = DMXReceiveData[RxIndex];
            GREEN_LED=1;
            GREEN_LED_TIMEOUT = 25;
        }
    }
    UART1_ICR_R |= UART_ICR_TXIC | UART_ICR_RXIC;
}

void UpdateDevice()
{
    //CircuitLED = DMXReceiveData[DIPvalue+0];
    PWM0_1_CMPB_R = DMXReceiveData[DIPvalue+0];
    /*if(DMXReceiveData[DIPvalue+0]!=0)
    {
        CircuitLED=1;
    }*/
}
void Transmit() {
    if (TransmitEnable == 1 && UnderTransmission == 0) {
        phase = STARTCODE;
        UnderTransmission = 1;
        UART1_DR_R = 0x00;
    }
}

void Timer1Isr()
{
    if(ACTIVITY_LED_TIMEOUT != 0)
    {
        ACTIVITY_LED_TIMEOUT--;
        if(ACTIVITY_LED_TIMEOUT == 0)
        {
            ACTIVITY_LED = 0;
        }
    }
    else if(TRANSMIT_LED_TIMEOUT != 0)
    {
        TRANSMIT_LED_TIMEOUT--;
        if(TRANSMIT_LED_TIMEOUT == 0)
        {
            TRANSMIT_LED = 0;
        }
    }
    else if(GREEN_LED_TIMEOUT != 0)
    {
        GREEN_LED_TIMEOUT--;
        if(GREEN_LED_TIMEOUT == 0)
        {
            GREEN_LED = 0;
        }
    }               // clear interrupt flag
    TIMER1_ICR_R = TIMER_ICR_TATOCINT;
}

void setMode()
{
    if(OperationMode == 0)
    {
        putsUart0("\r\nThe Mode is in Slave Mode");
        initHw();
        DATA_ENABLE = 0;
    }
    else if(OperationMode == 1)
    {
        putsUart0("\r\nThe Mode is in Master Mode");
        initHw();
        TransmitEnable = 1;
        UnderTransmission = 0;
        DATA_ENABLE = 1;
        Transmit();
    }
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
int main(void) {

    initHw();
    led_blink();
    DIP();
    Transmit();
    while (1) {
        getString();
        IsCommand();
        Transmit();
    }

}
