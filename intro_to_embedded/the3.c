// <editor-fold defaultstate="collapsed" desc="libraries">
#include "Includes.h"
#include "lcd.h"
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="pragma">
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF
#pragma config WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
// </editor-fold>


// <editor-fold defaultstate="collapsed" desc="variable declarations">
unsigned char deserts[4] = {3,1,9,2}; // stores the remaining deserts
unsigned char snackFood[4] = {3,1,2,3}; //stores the remaining snack Food
unsigned char fastFood[4] = {2,3,1,0};  //stores the remaining fast Food
unsigned char beverages[4] = {9,2,3,1}; //stores the remaining beverages



unsigned char S_RE1 = 0; //button state for RE1
int messageCounter = 0; //counter for "Welcome" message
unsigned char timer1Counter = 1; //counter for timer1 interrupt
unsigned char cursorPosition = 1; // 1-> first line 0-> second line
unsigned char cursorActive = 1; // 1 -> blinking ; 0 -> not blinking
unsigned char cursorType = 1; // 1 -> ">" ; 0 -> "X"
unsigned char varPORTB; //for reading PORTB to clear RBIF
unsigned char S_RB7 = 0; //button state for RB7
unsigned char timer0Counter = 1; //counter for timer0 interrupt
signed short oldADRESx = -1; //stores the old ADC of the first line
unsigned char selectedFirstLine; //1->bev. 2-> snack 3->fast 4->deserts
unsigned char selectedSecondLine;//saves the pid of the selected product
signed short oldADRESy = -1; //stores the old ADC of the second line
unsigned char S_RB6 = 0; //button state for RB6
unsigned toggledLine = 0; //check if we went to second line
unsigned timer1Counter2 = 1; //counter for special timer1 interrupts
unsigned selectionPhaseCounter = 1; //selection phase lasts 3 seconds
unsigned creditPhaseCounter = 1; //credit phase lasts 4 seconds
unsigned timer0Counter2 = 1; //counter for special timer0 interrupts
unsigned verifyPhaseCounter = 1; //verify phase lasts 3 seconds
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="function declarations">
void interrupt myISR();
void clearLine(int line); 
// </editor-fold>


int main() {
    
    // <editor-fold defaultstate="collapsed" desc="GENERAL CONFIG">
    
    // general interrupt config
    INTCON = 0b11100000; // enable (peripheral) interrupts + enable timer0 interrupt
    RCONbits.IPEN = 0; // all interrupts are high priority

    //timer1 configurations
    PIR1bits.TMR1IF = 0; // clear timer1 flag
    PIE1bits.TMR1IE = 1; // enable timer1 interrupt
    T1CON = 0b11110000;  // prescale 1:8
    TMR1 = 3040; // seed timer1
    
    //timer0 configurations
    T0CON = 0b01000111; //prescale 1:256
    TMR0 = 61;    //seed timer0
    
    //adc configurations
    PIR1bits.ADIF = 0; //clear adc flag
    PIE1bits.ADIE = 1; // enable adc interrupt
    ADCON0 = 0b00110000; // RH4 is selected for adc
    ADCON1 = 0b00000010; // RH4 -> analog
    ADCON2 = 0b10010010; // T_acq = 4*T_ad ; T_ad = 32 * T_osc    
    
    // </editor-fold>
    // <editor-fold defaultstate="collapsed" desc="PHASE - 1">
    // <editor-fold defaultstate="collapsed" desc="init LCD">
    //init LCD module
    InitLCD();
    ClearLCDScreen(); // </editor-fold>


    // <editor-fold defaultstate="collapsed" desc="poll for RE1">
    //polling for RE1
    while (1) {
        display7SEG();

        if (S_RE1 == 0) {
            if (PORTEbits.RE1 == 0)continue;
            else {
                S_RE1 = 1;
                continue;
            }
        } else {
            if (PORTEbits.RE1 == 1)continue;
            else break;
        }
    }
    S_RE1 = 0;
    // </editor-fold>


    //"Welcome" message to LCD
    WriteCommandToLCD(0x80); // cursor to first line
    WriteStringToLCD("  Ceng Vending  ");
    WriteCommandToLCD(0xC0); // cursor to second line
    WriteStringToLCD(" $$$Machine$$$  ");

    //the message shows up for 3 seconds
    while (messageCounter < 3739) {
        display7SEG();
        messageCounter++;
    }
    ClearLCDScreen();
    // </editor-fold>

start:    
    ADCON0bits.ADON  = 1; //adc module is active
    T1CONbits.TMR1ON = 1; //timer1 is active
    T0CONbits.TMR0ON = 1; //timer0 is active
    INTCONbits.RBIE  = 1; //rb interrupts are active
    
    
    while(!endingPhase)display7SEG();
    
    //RESET PROCEDURE
    // <editor-fold defaultstate="collapsed" desc="poll for RE1">
    //polling for RE1
    while (1) {
        display7SEG();

        if (S_RE1 == 0) {
            if (PORTEbits.RE1 == 0)continue;
            else {
                S_RE1 = 1;
                continue;
            }
        } else {
            if (PORTEbits.RE1 == 1)continue;
            else break;
        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="restore state">
    //restore state
    S_RE1 = 0;
    messageCounter = 0;
    timer1Counter = 1;
    cursorPosition = 1;
    cursorActive = 1;
    cursorType = 1;
    S_RB7 = 0;
    timer0Counter = 1;
    oldADRESx = -1;
    oldADRESy = -1;
    S_RB6 = 0;
    toggledLine = 0;
    timer1Counter2 = 1;
    selectionPhaseCounter = 1;
    creditPhaseCounter = 1;
    timer0Counter2 = 1;
    verifyPhaseCounter = 1;

    normalPhase = 1;
    selectionPhase = 0;
    creditPhase = 0;
    enterancePhase = 0;
    verifyPhase = 0;
    endingPhase = 0;
    D1on = 1;
    D2on = 1;
    D3on = 1;
    D0on = 1; // </editor-fold>
 
    // <editor-fold defaultstate="collapsed" desc="GENERAL CONFIG">
    
    // general interrupt config
    INTCON = 0b11100000; // enable (peripheral) interrupts + enable timer0 interrupt
    RCONbits.IPEN = 0; // all interrupts are high priority

    //timer1 configurations
    PIR1bits.TMR1IF = 0; // clear timer1 flag
    PIE1bits.TMR1IE = 1; // enable timer1 interrupt
    T1CON = 0b11110000;  // prescale 1:8
    TMR1 = 3040; // seed timer1
    
    //timer0 configurations
    T0CON = 0b01000111; //prescale 1:256
    TMR0 = 61;    //seed timer0
    
    //adc configurations
    PIR1bits.ADIF = 0; //clear adc flag
    PIE1bits.ADIE = 1; // enable adc interrupt
    ADCON0 = 0b00110000; // RH4 is selected for adc
    ADCON1 = 0b00000010; // RH4 -> analog
    ADCON2 = 0b10010010; // T_acq = 4*T_ad ; T_ad = 32 * T_osc    
    
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="init LCD">
    //init LCD module
    InitLCD();
    ClearLCDScreen(); // </editor-fold>

    
    goto start;
    return 0;
}

void interrupt myISR(){
    if(PIR1bits.TMR1IF && PIE1bits.TMR1IE){ //timer1 case
       PIR1bits.TMR1IF = 0; //clear timer1 flag
       TMR1 = 3040; // seed again
       
       if(timer1Counter == 5){ // 250 ms passed
           timer1Counter = 1;

            // <editor-fold defaultstate="collapsed" desc="cursor blinking">
            if (cursorPosition)WriteCommandToLCD(0x80); //first line
            else WriteCommandToLCD(0xC0); //second line

            if (cursorActive){
                if(cursorType)WriteStringToLCD(">");
                else WriteStringToLCD("X");
            }
            else WriteStringToLCD(" ");
            cursorActive = !cursorActive; // </editor-fold>

           // 1 second counter
           if(timer1Counter2 == 4){ //1 s passed
               timer1Counter2 = 1;
               
               // code is active in SELECTION PHASE ONLY
                // <editor-fold defaultstate="collapsed" desc="SELECTION PHASE ONLY">
                if (selectionPhase) { //1 s passed                
                    if (selectionPhaseCounter == 3) { //selection phase last 3 seconds
                        D1on = D0on = 1;
                        selectionPhase = 0;
                        creditPhase = 1;
                        
                        //change cursor to first line
                        if(!cursorPosition){
                            WriteCommandToLCD(0xC0);
                            WriteStringToLCD(" ");
                            WriteCommandToLCD(0x80);
                            cursorPosition = 1;
                        }
                        
                        //write "enter credit" message
                        clearLine(0x81);
                        WriteStringToLCD("Enter Credits");
                        
                        
                    } else {
                        D1on = !D1on;
                        D0on = !D0on;
                        selectionPhaseCounter++;
                    }
                }// </editor-fold>

               
               // code is active in CREDIT PHASE ONLY
               else if(creditPhase) {
                    // <editor-fold defaultstate="collapsed" desc="CREDIT PHASE ONLY">
                    if (creditPhaseCounter == 4) { //credit phase last 4 seconds
                        D3on = 1;
                        creditPhase = 0;
                        enterancePhase = 1;
                    } else {
                        D3on = !D3on;
                        creditPhaseCounter++;
                    }// </editor-fold>

               }
               
               
               
           }else timer1Counter2++; // 250 ms passed
           
           
       }else timer1Counter++; //50 ms passed
       
       
    }else if(INTCONbits.RBIF && INTCONbits.RBIE){ //rb interrupts case
        varPORTB = PORTB; //read PORTB to be able to clear RBIF
        INTCONbits.RBIF = 0; //clear the flag
        
        // <editor-fold defaultstate="collapsed" desc="RB7 case">
        if (S_RB7 == 0) {
            if (PORTBbits.RB7 == 0);
            else S_RB7 = 1;
        } else {
            if (PORTBbits.RB7 == 1);
            else {
                S_RB7 = 0; 
                
                
                if (cursorPosition) { //cursor at first line
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD(" ");
                    WriteCommandToLCD(0xC0);
                } else { // cursor at second line
                    WriteCommandToLCD(0xC0);
                    WriteStringToLCD(" ");
                    WriteCommandToLCD(0x80);
                }
                
                cursorPosition = !cursorPosition; //reverse cursor position
            }
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="RB6 case">
        if (S_RB6 == 0) {
            if (PORTBbits.RB6 == 0);
            else S_RB6 = 1;
        } else {
            if (PORTBbits.RB6 == 1);
            else {
                S_RB6 = 0;
                
                if(normalPhase){
                    selectionPhase = 1;
                    normalPhase = 0;
                    D1 = selectedSecondLine / 10;
                    D0 = selectedSecondLine % 10;

                    // <editor-fold defaultstate="collapsed" desc="zero product check">
                    if (selectedSecondLine <= 4) { //beverage chosen
                        if (beverages[selectedSecondLine - 1] == 0) {
                            cursorType = 0;
                            selectionPhase = 0;
                            normalPhase = 1;
                        }
                    } else if (selectedSecondLine <= 8) { //snack food chosen
                        if (snackFood[selectedSecondLine - 5] == 0) {
                            cursorType = 0;
                            selectionPhase = 0;
                            normalPhase = 1;
                        }

                    } else if (selectedSecondLine <= 12) { //fast food chosen
                        if (fastFood[selectedSecondLine - 9] == 0) {
                            cursorType = 0;
                            selectionPhase = 0;
                            normalPhase = 1;
                        }
                    } else { //desert chosen
                        if (deserts[selectedSecondLine - 13] == 0) {
                            cursorType = 0;
                            selectionPhase = 0;
                            normalPhase = 1;
                        }
                    }// </editor-fold>

                    
                }else if(enterancePhase){
                    enterancePhase = 0;
                    verifyPhase = 1;

                    // <editor-fold defaultstate="collapsed" desc="write first line">
                    //write appropriate first line
                    clearLine(0x81);
                    if (selectedFirstLine == 1)WriteStringToLCD("Beverages");
                    else if (selectedFirstLine == 2)WriteStringToLCD("Snack Food");
                    else if (selectedFirstLine == 3)WriteStringToLCD("Fast Food");
                    else WriteStringToLCD("Deserts"); // </editor-fold>

                    
                    
                    // <editor-fold defaultstate="collapsed" desc="make a purchase">
                    //make a purchase
                    if (selectedSecondLine == 1) {
                        D2 = D3 - 2;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --beverages[0]);
                    } else if (selectedSecondLine == 2) {
                        D2 = D3 - 3;
                        WriteCommandToLCD(0xCB);
                        WriteDataToLCD('0' + --beverages[1]);
                    } else if (selectedSecondLine == 3) {
                        D2 = D3 - 5;
                        WriteCommandToLCD(0xCB);
                        WriteDataToLCD('0' + --beverages[2]);                        
                    } else if (selectedSecondLine == 4) {
                        D2 = D3 - 4;
                        WriteCommandToLCD(0xCA);
                        WriteDataToLCD('0' + --beverages[3]);                        
                    } else if (selectedSecondLine == 5) {
                        D2 = D3 - 5;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --snackFood[0]);                        
                    } else if (selectedSecondLine == 6) {
                        D2 = D3 - 4;
                        WriteCommandToLCD(0xCB);
                        WriteDataToLCD('0' + --snackFood[1]);                          
                    } else if (selectedSecondLine == 7) {
                        D2 = D3 - 3;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --snackFood[2]);                          
                    } else if (selectedSecondLine == 8) {
                        D2 = D3 - 5;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --snackFood[3]);                         
                    } else if (selectedSecondLine == 9) {
                        D2 = D3 - 3;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --fastFood[0]);                          
                    } else if (selectedSecondLine == 10) {
                        D2 = D3 - 5;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --fastFood[1]);                        
                    } else if (selectedSecondLine == 11) {
                        D2 = D3 - 4;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --fastFood[2]);                        
                    } else if (selectedSecondLine == 12) {
                        D2 = D3 - 4;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --fastFood[3]);                        
                    } else if (selectedSecondLine == 13) {
                        D2 = D3 - 5;
                        WriteCommandToLCD(0xCB);
                        WriteDataToLCD('0' + --deserts[0]);                        
                    } else if (selectedSecondLine == 14) {
                        D2 = D3 - 4;
                        WriteCommandToLCD(0xCB);
                        WriteDataToLCD('0' + --deserts[1]);                         
                    } else if (selectedSecondLine == 15) {
                        D2 = D3 - 2;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --deserts[2]);                         
                    } else {
                        D2 = D3 - 3;
                        WriteCommandToLCD(0xCC);
                        WriteDataToLCD('0' + --deserts[3]);                         
                    }
                    // </editor-fold>

                }
            }
        }
        // </editor-fold>


        
        
    }else if(INTCONbits.TMR0IF && INTCONbits.TMR0IE){ //timer0 case
       INTCONbits.TMR0IF = 0; //clear timer0 flag
       TMR0 = 61; //seed again
       
       if(timer0Counter == 20){ //100 ms passed
           timer0Counter = 1;
           
           if(ADCON0bits.GO == 0)ADCON0bits.GO = 1; //start conversion
           
           //1 second counter
           if(timer0Counter2 == 10){//1 s passed
               timer0Counter2 = 1;
               
               //code active ONLY IN VERIFY PHASE
               if(verifyPhase){
                   
                   if(verifyPhaseCounter == 3){
                       //disable all interrupts
                       ADCON0bits.ADON  = 0;
                       T1CONbits.TMR1ON = 0; 
                       T0CONbits.TMR0ON = 0; 
                       INTCONbits.RBIE  = 0; 
                       
                       ClearLCDScreen();
                       //"Welcome" message to LCD
                       WriteCommandToLCD(0x80); // cursor to first line
                       WriteStringToLCD("  Ceng Vending  ");
                       WriteCommandToLCD(0xC0); // cursor to second line
                       WriteStringToLCD(" $$$Machine$$$  "); 
    
                       verifyPhase = 0;
                       endingPhase = 1;       
                       
                   }else{
                       D2on = !D2on;
                       verifyPhaseCounter++;
                   }
               }
               
           }else timer0Counter2++; //100ms passed
           
       }else timer0Counter++; //5ms passed
       
       
    }else if(PIR1bits.ADIF && PIE1bits.ADIE){ // adc interrupt case
        PIR1bits.ADIF = 0; //clear adc flag
        
        
        if(normalPhase || selectionPhase){
            if(cursorPosition){ //cursor at first line
                // <editor-fold defaultstate="collapsed" desc="cursor at first line">
            if (ADRES < 251) {
                if (oldADRESx == -1 || oldADRESx > 250 || toggledLine) {
                    clearLine(0x81);
                    WriteStringToLCD("Beverages");
                    clearLine(0xC1);
                    WriteStringToLCD("01-Water C:");
                    WriteDataToLCD('0' + beverages[0]);
                    WriteStringToLCD("$:2");

                    oldADRESx = ADRES;
                    oldADRESy = -1;
                    selectedFirstLine = 1;
                    selectedSecondLine = 1;
                    toggledLine = 0;
                    cursorType = 1;
                }
            } else if (ADRES < 501) {
                if (oldADRESx == -1 || !(oldADRESx >= 251 && oldADRESx <= 500) || toggledLine) {
                    clearLine(0x81);
                    WriteStringToLCD("Snack Food");
                    clearLine(0xC1);
                    WriteStringToLCD("05-Seeds C:");
                    WriteDataToLCD('0' + snackFood[0]);
                    WriteStringToLCD("$:5");

                    oldADRESx = ADRES;
                    oldADRESy = -1;
                    selectedFirstLine = 2;
                    selectedSecondLine = 5;
                    toggledLine = 0;
                    cursorType = 1;
                }
            } else if (ADRES < 751) {
                if (oldADRESx == -1 || !(oldADRESx >= 501 && oldADRESx <= 750) || toggledLine) {
                    clearLine(0x81);
                    WriteStringToLCD("Fast Food");
                    clearLine(0xC1);
                    WriteStringToLCD("09-Pizza C:");
                    WriteDataToLCD('0' + fastFood[0]);
                    WriteStringToLCD("$:3");

                    oldADRESx = ADRES;
                    oldADRESy = -1;
                    selectedFirstLine = 3;
                    selectedSecondLine = 9;
                    toggledLine = 0;
                    cursorType = 1;
                }
            } else {
                if (oldADRESx == -1 || oldADRESx < 751 || toggledLine) {
                    clearLine(0x81);
                    WriteStringToLCD("Deserts");
                    clearLine(0xC1);
                    WriteStringToLCD("13-Cake C:");
                    WriteDataToLCD('0' + deserts[0]);
                    WriteStringToLCD("$:5");

                    oldADRESx = ADRES;
                    oldADRESy = -1;
                    selectedFirstLine = 4;
                    selectedSecondLine = 13;
                    toggledLine = 0;
                    cursorType = 1;
                }
            }// </editor-fold>
            }else{ //cursor at second line
                // <editor-fold defaultstate="collapsed" desc="cursor at second line">
            if (ADRES < 251) {
                if (selectedFirstLine == 1) {
                    if (oldADRESy == -1 || oldADRESy > 250) {
                        clearLine(0xC1);
                        WriteStringToLCD("01-Water C:");
                        WriteDataToLCD('0' + beverages[0]);
                        WriteStringToLCD("$:2");
                        oldADRESy = ADRES;
                        selectedSecondLine = 1;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 2) {
                    if (oldADRESy == -1 || oldADRESy > 250) {
                        clearLine(0xC1);
                        WriteStringToLCD("05-Seeds C:");
                        WriteDataToLCD('0' + snackFood[0]);
                        WriteStringToLCD("$:5");
                        oldADRESy = ADRES;
                        selectedSecondLine = 5;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 3) {
                    if (oldADRESy == -1 || oldADRESy > 250) {
                        clearLine(0xC1);
                        WriteStringToLCD("09-Pizza C:");
                        WriteDataToLCD('0' + fastFood[0]);
                        WriteStringToLCD("$:3");
                        oldADRESy = ADRES;
                        selectedSecondLine = 9;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else {
                    if (oldADRESy == -1 || oldADRESy > 250) {
                        clearLine(0xC1);
                        WriteStringToLCD("13-Cake C:");
                        WriteDataToLCD('0' + deserts[0]);
                        WriteStringToLCD("$:5");
                        oldADRESy = ADRES;
                        selectedSecondLine = 13;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                }

            } else if (ADRES < 501) {
                if (selectedFirstLine == 1) {
                    if (oldADRESy == -1 || !(oldADRESy >= 251 && oldADRESy <= 500)) {
                        clearLine(0xC1);
                        WriteStringToLCD("02-Milk C:");
                        WriteDataToLCD('0' + beverages[1]);
                        WriteStringToLCD("$:3");
                        oldADRESy = ADRES;
                        selectedSecondLine = 2;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 2) {
                    if (oldADRESy == -1 || !(oldADRESy >= 251 && oldADRESy <= 500)) {
                        clearLine(0xC1);
                        WriteStringToLCD("06-Nuts C:");
                        WriteDataToLCD('0' + snackFood[1]);
                        WriteStringToLCD("$:4");
                        oldADRESy = ADRES;
                        selectedSecondLine = 6;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 3) {
                    if (oldADRESy == -1 || !(oldADRESy >= 251 && oldADRESy <= 500)) {
                        clearLine(0xC1);
                        WriteStringToLCD("10-Hambu C:");
                        WriteDataToLCD('0' + fastFood[1]);
                        WriteStringToLCD("$:5");
                        oldADRESy = ADRES;
                        selectedSecondLine = 10;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else {
                    if (oldADRESy == -1 || !(oldADRESy >= 251 && oldADRESy <= 500)) {
                        clearLine(0xC1);
                        WriteStringToLCD("14-Choc C:");
                        WriteDataToLCD('0' + deserts[1]);
                        WriteStringToLCD("$:4");
                        oldADRESy = ADRES;
                        selectedSecondLine = 14;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                }

            } else if (ADRES < 751) {
                if (selectedFirstLine == 1) {
                    if (oldADRESy == -1 || !(oldADRESy >= 501 && oldADRESy <= 750)) {
                        clearLine(0xC1);
                        WriteStringToLCD("03-Coke C:");
                        WriteDataToLCD('0' + beverages[2]);
                        WriteStringToLCD("$:5");
                        oldADRESy = ADRES;
                        selectedSecondLine = 3;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 2) {
                    if (oldADRESy == -1 || !(oldADRESy >= 501 && oldADRESy <= 750)) {
                        clearLine(0xC1);
                        WriteStringToLCD("07-Chips C:");
                        WriteDataToLCD('0' + snackFood[2]);
                        WriteStringToLCD("$:3");
                        oldADRESy = ADRES;
                        selectedSecondLine = 7;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 3) {
                    if (oldADRESy == -1 || !(oldADRESy >= 501 && oldADRESy <= 750)) {
                        clearLine(0xC1);
                        WriteStringToLCD("11-Sandw C:");
                        WriteDataToLCD('0' + fastFood[2]);
                        WriteStringToLCD("$:4");
                        oldADRESy = ADRES;
                        selectedSecondLine = 11;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else {
                    if (oldADRESy == -1 || !(oldADRESy >= 501 && oldADRESy <= 750)) {
                        clearLine(0xC1);
                        WriteStringToLCD("15-ICrea C:");
                        WriteDataToLCD('0' + deserts[2]);
                        WriteStringToLCD("$:2");
                        oldADRESy = ADRES;
                        selectedSecondLine = 15;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                }

            } else {
                if (selectedFirstLine == 1) {
                    if (oldADRESy == -1 || oldADRESy < 751) {
                        clearLine(0xC1);
                        WriteStringToLCD("04-Tea C:");
                        WriteDataToLCD('0' + beverages[3]);
                        WriteStringToLCD("$:4");
                        oldADRESy = ADRES;
                        selectedSecondLine = 4;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 2) {
                    if (oldADRESy == -1 || oldADRESy < 751) {
                        clearLine(0xC1);
                        WriteStringToLCD("08-PCorn C:");
                        WriteDataToLCD('0' + snackFood[3]);
                        WriteStringToLCD("$:5");
                        oldADRESy = ADRES;
                        selectedSecondLine = 8;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else if (selectedFirstLine == 3) {
                    if (oldADRESy == -1 || oldADRESy < 751) {
                        clearLine(0xC1);
                        WriteStringToLCD("12-Crack C:");
                        WriteDataToLCD('0' + fastFood[3]);
                        WriteStringToLCD("$:4");
                        oldADRESy = ADRES;
                        selectedSecondLine = 12;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                } else {
                    if (oldADRESy == -1 || oldADRESy < 751) {
                        clearLine(0xC1);
                        WriteStringToLCD("16-Biscu C:");
                        WriteDataToLCD('0' + deserts[3]);
                        WriteStringToLCD("$:3");
                        oldADRESy = ADRES;
                        selectedSecondLine = 16;
                        toggledLine = 1;
                        cursorType = 1;
                    }
                }
            }// </editor-fold>
            }            
        }else if(enterancePhase) {
            // <editor-fold defaultstate="collapsed" desc="enterance phase">
            if (ADRES < 103)D3 = 0;
            else if (ADRES < 205)D3 = 1;
            else if (ADRES < 307)D3 = 2;
            else if (ADRES < 409)D3 = 3;
            else if (ADRES < 511)D3 = 4;
            else if (ADRES < 613)D3 = 5;
            else if (ADRES < 715)D3 = 6;
            else if (ADRES < 817)D3 = 7;
            else if (ADRES < 919)D3 = 8;
            else D3 = 9; // </editor-fold>
        }
        
        
    }
}
void clearLine(int line){
    //Clear Procedure
    WriteCommandToLCD(line);
    WriteStringToLCD("               ");
    WriteCommandToLCD(line);
}