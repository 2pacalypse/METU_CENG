LIST    P=18F8722

#INCLUDE <p18f8722.inc>
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF	
ra4button_state udata 0x20
ra4button_state
 
ra4toggle_flag udata 0x21
ra4toggle_flag
 
rc4button_state udata 0x22
rc4button_state
 
rc4toggle_flag udata 0x23
rc4toggle_flag
 
a udata 0x24
a
 
b udata 0x25
b
 
c udata 0x26
c 
 
org	h'00' 
    

 
goto main

init:
    clrf PORTA
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    clrf PORTF
    CLRF PORTG
    
    
    clrf ra4button_state
    clrf ra4toggle_flag
    
    clrf rc4button_state
    clrf rc4toggle_flag
    
    
    
    movlw 0x0F ; SET I/O TO BE DIGITAL
    

    
    movwf ADCON1
    
    movwf PORTA ; TURN ON RA[03]
    movwf PORTD ; TURN ON RD[03]
    
    movlw 0x09
    movwf PORTB ; TURN ON RB0 RB03
    movwf PORTC ; TURN ON RC0 RC03
  
    movlw 0x10 
    movwf TRISA ; RA4 INPUT, RA[03] OUTPUT 
    movwf TRISC  ; RC4 INPUT, RC[03] OUTPUT 
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
    movlw 0x00 
    movwf TRISB
    movwf TRISD
    movwf TRISE
    movwf TRISF
    movwf TRISG
    
    return
   
ra_listen:
    btfsc ra4button_state, 0
    goto bs1
    goto bs0
    
bs1:    
    btfsc PORTA, 4
    return
    bsf ra4toggle_flag, 0
    bcf ra4button_state, 0
    return
bs0:
    btfss PORTA, 4
    return
    bsf ra4button_state, 0
    return
    
rc4listen:
    btfsc rc4button_state, 0
    goto rc4bs1
    goto rc4bs0

rc4bs1:
    nop
    nop
    nop
    btfsc  PORTC, 4
    return
    bsf rc4toggle_flag, 0
    bcf rc4button_state, 0
    return
rc4bs0:
    btfsc PORTC, 4
    bsf rc4button_state, 0
    return
    
delay:
    movlw 0x0b
    movwf c
    movlw 0xd0
    movwf b
    movlw 0x2a
    movwf a
d1:
    decfsz a
    goto judge
    decfsz b
    goto d1
    decfsz c
    goto d1
    return

judge:
    call rc4listen
    btfss rc4toggle_flag,0
    goto d1
    return
    
main:
    call init
    loop1:
	btfsc ra4toggle_flag, 0
	goto bpath
	call ra_listen
	goto loop1
    bpath:	
	clrf rc4toggle_flag
	    
	clrf PORTB
	clrf PORTC
	clrf PORTA
	movlw 0x01
	movwf PORTD
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	movlw 0x02
	movwf PORTD
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath	
	
	movlw 0x04
	movwf PORTD
	
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	movlw 0x08
	movwf PORTD
	
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTD
	MOVLW 0X08
	MOVWF PORTC
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTC
	MOVLW 0X08
	MOVWF PORTB
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTB
	MOVLW 0X08
	MOVWF PORTA
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTB
	MOVLW 0X04
	MOVWF PORTA
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTB
	MOVLW 0X02
	MOVWF PORTA
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTB
	MOVLW 0X01
	MOVWF PORTA
	
        call delay
	btfsc rc4toggle_flag,0
	goto spath
	

	
	CLRF PORTA
	MOVLW 0X01
	MOVWF PORTB
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTB
	MOVLW 0X01
	MOVWF PORTC
	
	call delay
	btfsc rc4toggle_flag,0
	goto spath
	
	CLRF PORTC
	
	
	
	
l1:	call rc4listen
	btfsc rc4toggle_flag,0
	goto spath
	goto l1

	
	
	
    spath:
	clrf rc4toggle_flag   
	
	clrf PORTB
	clrf PORTC
	clrf PORTA
	movlw 0x01
	movwf PORTD
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	movlw 0x02
	movwf PORTD
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	movlw 0x04
	movwf PORTD
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	clrf PORTD
	movlw 0x04
	movwf PORTC
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	clrf PORTC
	movlw 0x04
	movwf PORTB
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	movlw 0x02
	movwf PORTB
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	movlw 0x01
	movwf PORTB
	
	call delay
	btfsc rc4toggle_flag,0
	goto bpath
	
	clrf PORTB
	movlw 0x01
	movwf PORTC
	
    	call delay
	CLRF PORTC
	
l2:	call rc4listen
	btfsc rc4toggle_flag,0
	goto bpath
	goto l2
	
	return
end
	
	
	
	


