////////////////////////
/*					  //
 * CE865 Assignment 1 //
 * REACTION TIMER     //
 * Ogulcan Ozer.      //
 * oo18830            //
 *					  //
 */					  //
////////////////////////
//
//
//
//
//
////////
#include <config/AT91SAM7S256.h>// Hardware description header
#include <atmel/pit.h>// Periodic interval timer header
#include <atmel/aic.h>// Advanced interrupt controller header
#include <board/SevenSegmentDisplay.h>// OLED seven segment display header
#include <embed_assign_libraries/myLED.h>// Assignment LED libraries
#include <embed_assign_libraries/myBUTTON.h>// Assignment button libraries
//
////

#define PIT_PERIOD  1000 // Pit period value to get 1 millisecond intervals.

short Digit[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67 }; //Digit hex codes for OLED 7 segment display.

long second, millisecond; //Clock variables

//Function definitions, explanations are in functions themselves.
void timer_start(int ledNum);
void interrupt_timer(void);
void interrupt_flash(void);
void reset_clock(void);
void update_clock(void);
//

int main()

{	//Enable registers 24,25,26,27,28,29 as PIO pins.
	AT91C_BASE_PIOA->PIO_PER = (AT91C_PIO_PA24 | AT91C_PIO_PA25 | AT91C_PIO_PA26
			| AT91C_PIO_PA27 | AT91C_PIO_PA28 | AT91C_PIO_PA29);
	//Set pins 24,25 as outputs.
	AT91C_BASE_PIOA->PIO_OER = (AT91C_PIO_PA24 | AT91C_PIO_PA25);
	//Set pins 26,27,28,29  as inputs.
	AT91C_BASE_PIOA->PIO_ODR = (AT91C_PIO_PA26 | AT91C_PIO_PA27 | AT91C_PIO_PA28
			| AT91C_PIO_PA29);
	//Enable glitch filtering for input pins 26,27,28,29.
	AT91C_BASE_PIOA->PIO_IFER = (AT91C_PIO_PA26 | AT91C_PIO_PA27
			| AT91C_PIO_PA28 | AT91C_PIO_PA29);

	//Configure seven segment display, and initialise PIT with the defined period.
	Configure7SegmentDisplay();
	Set7SegmentDisplayValue(DISPLAY4, Digit[0] | 0x80);	// OR 0x80 for the "."
	Set7SegmentDisplayValue(DISPLAY3, Digit[0]);
	Set7SegmentDisplayValue(DISPLAY2, Digit[0]);
	Set7SegmentDisplayValue(DISPLAY1, Digit[0]);
	PIT_Init(PIT_PERIOD, BOARD_MCK / 1000000);
	//

	/*Reaction timer main loop.
	 *
	 * First configure AIC for led flashing.
	 * Enable PIT and wait for button1 press.
	 * After user starts the timer by pressing button1, disable AIC to configure it for the timer.
	 * Reset the clock.
	 * Start counting back.
	 * Finally turn a random led on, and pass the returned number to the timer function.
	 *
	 */
	do {
		AIC_DisableIT(AT91C_ID_SYS);
		AIC_ConfigureIT(AT91C_ID_SYS, 0, interrupt_flash);
		AIC_EnableIT(AT91C_ID_SYS);
		PIT_EnableIT();
		clearLEDS();	//Clear all the leds on the board.
		do {
		} while (!isReleased(1));
		do {
		} while (!isPressed(1));	//Wait for button press.
		PIT_DisableIT();
		AIC_DisableIT(AT91C_ID_SYS);
		AIC_ConfigureIT(AT91C_ID_SYS, 0, interrupt_timer);
		AIC_EnableIT(AT91C_ID_SYS);
		reset_clock();	//Reset the clock.
		countBack();//Count back. (Button press is prevented inside the function)
		int randNum = randomON();
		timer_start(randNum);	//Start the timer.

	} while (1);
	//
	//End of reaction timer main loop.

	return 0;
}

//Handler for led flashing every second, using the clock.
void interrupt_flash(void) {
	PIT_GetPIVR();

	update_clock();

	if (second % 2 == 0) {
		ledOFF(1, 2);
		ledON(1, 1);
	}
	if (second % 2 == 1) {
		ledOFF(1, 2);
		ledON(1, 0);
	}

}
//Interrupt handler function for time keeping.
void interrupt_timer(void) {
	PIT_GetPIVR();
	update_clock();

}

//Timer function to start/stop PIT, runs until 9.999 seconds or correct button press.
void timer_start(int ledNum) {

	PIT_EnableIT();	//Start the timer interrupts.

	do {
	} while (!isReleased(ledNum));
	while (!isPressed(ledNum))	//Check if the correct button is pressed.
	{
		//Update seven segment display.
		Set7SegmentDisplayValue(DISPLAY4, Digit[second] | 0x80);
		Set7SegmentDisplayValue(DISPLAY3, Digit[millisecond / 100]);
		Set7SegmentDisplayValue(DISPLAY2, Digit[((millisecond % 100) / 10)]);
		Set7SegmentDisplayValue(DISPLAY1, Digit[millisecond % 10]);
		if (second == 9 && millisecond == 999) {//Check if the timer reaches 9.999

			break;	// If so, get out.
		}

	}
	PIT_DisableIT();	//Disable the interrupt timer.

}
//End of timer function.

//Reset the clock and clear the seven segment display to 0000.
void reset_clock(void) {
	second = 0;
	millisecond = 0;
	Set7SegmentDisplayValue(DISPLAY4, Digit[second] | 0x80);
	Set7SegmentDisplayValue(DISPLAY3, Digit[millisecond / 100]);
	Set7SegmentDisplayValue(DISPLAY2, Digit[((millisecond % 100) / 10)]);
	Set7SegmentDisplayValue(DISPLAY1, Digit[millisecond % 10]);
}

//Update clock values.
void update_clock(void) {

	if (++millisecond == 1000) {
		millisecond = 0;
		second++;
	}
	if (second == 10)
		second = 0;

}
