////////////////////////
/*					  //
 * CE865 Assignment 1 //
 * Button Headers for //
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
#ifndef _LEDLIB_H_
#define _LEDLIB_H_

//Checks if button "bNum" is pressed or not. 
int isPressed(int bNum);

//Checks if button "bNum" is released or not. 
int isReleased(int bNum);

//Procedure to check if any of the buttons is pressed.
int isAnyPressed(void);

#endif
