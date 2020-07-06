/****************************************************************************
**	init.cpp
*****************************************************************************
**	Board initializations
**	>GPIO
**	>EXTI
**	>ECLIC
****************************************************************************/

/****************************************************************************
**	INCLUDE
****************************************************************************/

#include <gd32vf103.h>
//Onboard RBG LEDs of the Longan Nano
#include "longan_nano_led.hpp"

/****************************************************************************
**	NAMESPACES
****************************************************************************/

//using namespace std;

/****************************************************************************
**	GLOBAL VARIABILE
****************************************************************************/

/****************************************************************************
**	FUNCTION PROTOTYPES
****************************************************************************/

//Initialize the board
extern bool init( void );

//Initialize RTC Timer to generate a system tick
extern bool init_rtc( void );
//Initialize the EXTI edge detection interrupt from the GPIO
extern bool init_exti(void);
//Initialize the ECLIC interrupt controller
extern bool init_eclic(void);

/****************************************************************************
**	FUNCTION
****************************************************************************/

/***************************************************************************/
//!	@brief init
//!	init |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	initialize the longan nano pheriperals
/***************************************************************************/

bool init( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Return
	bool f_ret = false;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//	GPIO
	//	ECLIC
	//	EXTI

	//Initialize the Longan Nano builtin RBG LEDs
	f_ret |= Longan_nano::Leds::init();
	//Initialize LEDs to OFF
	Longan_nano::Leds::set_color( Longan_nano::Led_color::BLACK );
	//EXTI GPIO interrupt: PA8 R/F -> EXTI_5_9
	f_ret |= init_exti();
	//RTC system tick
	f_ret |= init_rtc();
	//Initialize ECLIC Interrupt controller. Interrupts can now be generated
	f_ret |= init_eclic();


	//if: initialization failed
	if (f_ret == true)
	{
		
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return f_ret;
}	//end init: init |

/***************************************************************************/
//!	@brief init
//!	init_exti |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	initialize the EXTI edge detection interrupt from the GPIO
//!	PA8 R/F -> EXTI_5_9
/***************************************************************************/

bool init_exti(void)
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	
	//Setup the boot button
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_8);

	//Clock the alternate functions
	rcu_periph_clock_enable(RCU_AF);

	//Initialize the EXTI. IRQ can be generated from GPIO edge detectors
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_8);
	exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_BOTH);

	//Clear interrupt flag. Ensure no spurious execution at start
	exti_interrupt_flag_clear(EXTI_8);
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return false;	//OK
}	//end init: init_exti |

/***************************************************************************/
//!	@brief init
//!	init_rtc |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	initialize the RTC Real Time Counter
//!	setup to emit a 4Hz SECOND interrupt
//!
//!		RTC Interrupts:
//!	RTC_INT_SECOND: 	Generated every time RTC_CNT changes (after the prescaler RTC_PSC)
//!	RTC_INT_ALARM:		Generated when RTC_CNT equals RTC_ALRM
//! RTC_INT_OV:			Generated when the 32b RTC_CNT counter overflows to zero
/***************************************************************************/

bool init_rtc(void)
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------	
	
	//Backup Clock
	rcu_periph_clock_enable(RCU_BKPI);
	//
	rcu_periph_clock_enable(RCU_PMU);
	//Enter configuration mode
	pmu_backup_write_enable();
	//
	bkp_deinit();

	//Enable LX crystal 32768 [Hz]
	rcu_osci_on(RCU_LXTAL);
	//Wait for crystal to stabilize
	rcu_osci_stab_wait(RCU_LXTAL);
	//Clock the RTC with the LX Crystal
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
	rcu_periph_clock_enable(RCU_RTC);

	//wait for RTC registers synchronization
	rtc_register_sync_wait();
	//wait until last write operation on RTC registers has finished
	rtc_lwoff_wait();

	//Set the RTC Prescaler
	rtc_prescaler_set(8192);
	//wait until last write operation on RTC registers has finished
	rtc_lwoff_wait();

	//Emit an interrupt each time RTC_CNT counts
	rtc_interrupt_enable(RTC_INT_SECOND);
	//wait until last write operation on RTC registers has finished
	rtc_lwoff_wait();
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return false;	//OK
}	//end init: init_rtc |

/***************************************************************************/
//!	@brief init
//!	init_eclic |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	Initialize the ECLIC interrupt controller
/***************************************************************************/

bool init_eclic(void)
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	
	//Initialize the ECLIC IRQ lines
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(EXTI5_9_IRQn, 1, 1);
	
	//Enable the RTC Interrupt
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL1_PRIO3);
    eclic_irq_enable(RTC_IRQn, 1, 0);
	
	//Enable the interrupts. From now on interrupt handlers can be executed
	eclic_global_interrupt_enable();
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return false;	//OK
}	//end init: init_eclic |