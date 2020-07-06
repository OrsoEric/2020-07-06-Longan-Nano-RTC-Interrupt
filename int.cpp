/****************************************************************************
**	int.cpp
*****************************************************************************
**	Interrupt Service Routines
**		NOTE:
**	extern "C" keyword tells the C++ compiler not to mangle the ISR nfunction name
**	the Interrupt Vector Table is declared in Start.S and requires special names
**	for the ISRs without mangling to link correctly
****************************************************************************/

/****************************************************************************
**	Interrupt Vectors
*****************************************************************************
**	EXTI5_9_IRQHandler			GPIO edge detectors 5 to 9	 
**
****************************************************************************/

/****************************************************************************
**	INCLUDE
****************************************************************************/

#include <gd32vf103.h>
//Onboard RBG LEDs of the Longan Nano
#include "longan_nano_led.hpp"

/***************************************************************************/
//!	@brief ISR
//!	EXTI5:9 LINES
/***************************************************************************/
//! @return void |
//! @details \n
//!	Handles the ISR lines generated by the GPIO lines 5 to 9
/***************************************************************************/

extern "C"
void EXTI5_9_IRQHandler()
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	
	//If: interrupt from PA8 boot button
	if (exti_interrupt_flag_get(EXTI_8) != RESET)
	{
		//Clear the interrupt from PA8 boot button
		exti_interrupt_flag_clear(EXTI_8);
		//Toggle the green led
		Longan_nano::Leds::toggle( Longan_nano::Led_color::GREEN );
	}
	//Default: interrupt from an unhandled GPIO
	else
	{
		//Do nothing (should clear the interrupt flags)
	}
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
}	//End ISR: EXTI5_9_IRQHandler

/***************************************************************************/
//!	@brief ISR
//!	RTC 
/***************************************************************************/
//! @return void |
//! @details \n
//!	Handles the ISR lines generated by the RTC
//!		Includes three vectors:
//!	RTC_INT_SECOND: 	Generated every time RTC_CNT changes (after the prescaler RTC_PSC)
//!	RTC_INT_ALARM:		Generated when RTC_CNT equals RTC_ALRM
//! RTC_INT_OV:			Generated when the 32b RTC_CNT counter overflows to zero
/***************************************************************************/

extern "C"
void RTC_IRQHandler(void)
{
	//SECOND interrupt, generated by a change in RTC:CNT
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET)
	{
		//Clear flag
        rtc_flag_clear(RTC_FLAG_SECOND);

		Longan_nano::Leds::toggle( Longan_nano::Led_color::RED ); 
    }
	
	//ALARM interrupt, generated when RTC_CNT equals RTC_ALRM
	if (rtc_flag_get(RTC_FLAG_ALARM) != RESET)
	{
		//Clear flag
        rtc_flag_clear(RTC_FLAG_ALARM);
		
	}
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
}	//End ISR: RTC_IRQHandler


