/************************************************************************************//**
* \file         Demo/ARMCM4_STM32L4_Nucleo_L496ZG_GCC/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM4_STM32L4_Nucleo_L496ZG_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2021  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "stm32l4xx.h"                           /* STM32 CPU and HAL header           */
#include "stm32l4xx_ll_rcc.h"                    /* STM32 LL RCC header                */
#include "stm32l4xx_ll_bus.h"                    /* STM32 LL BUS header                */
#include "stm32l4xx_ll_pwr.h"                    /* STM32 LL PWR header                */
#include "stm32l4xx_ll_system.h"                 /* STM32 LL SYSTEM header             */
#include "stm32l4xx_ll_utils.h"                  /* STM32 LL UTILS header              */
#include "stm32l4xx_ll_gpio.h"                   /* STM32 LL GPIO header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
void SystemClock_Config(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader */
  BootInit();

  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* HAL library initialization */
  HAL_Init();
  /* configure system clock */
  SystemClock_Config();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock to match the configuration in the bootloader's
**            configuration (blt_conf.h), specifically the macros:
**            BOOT_CPU_SYSTEM_SPEED_KHZ and BOOT_CPU_XTAL_SPEED_KHZ.
**            Note that the Lower Layer drivers were selected in CubeMX for the RCC
**            subsystem.
** \return    none.
**
****************************************************************************************/
void SystemClock_Config(void)
{
  /* Set flash latency. */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  /* Set flash latency. */
  if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {
    /* Error setting flash latency. */
    ASSERT_RT(BLT_FALSE);
  }

  /* Configure the main internal regulator output voltage. */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

  /* Enable the HSI clock. */
  LL_RCC_HSI_Enable();
  /* Wait till HSI is ready */
  while (LL_RCC_HSI_IsReady() != 1)
  {
    ;
  }
  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_HSI48_Enable();

   /* Wait till HSI48 is ready */
  while(LL_RCC_HSI48_IsReady() != 1)
  {
    ;
  }

  /* Configure and enable the PLL. */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_2, 20, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();
  /* Wait till PLL is ready */
  while (LL_RCC_PLL_IsReady() != 1)
  {
    ;
  }
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
    ;
  }
  /* Configure peripheral subsystem prescalers. */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  /* Update the system clock speed setting. */
  LL_SetSystemCoreClock(BOOT_CPU_SYSTEM_SPEED_KHZ * 1000u);
} /*** end of SystemClock_Config ***/


/************************************************************************************//**
** \brief     Initializes the Global MSP. This function is called from HAL_Init()
**            function to perform system level initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspInit(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;

  /* SYSCFG clock enable. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  /* PWR clock enable. */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* GPIO ports clock enable. */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART clock enable. */
  LL_PWR_EnableVddIO2();
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
#endif

  /* Configure GPIO pin for the LED. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

  /* Configure GPIO pin for (optional) backdoor entry input. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7 | LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOG, &GPIO_InitStruct);
#endif

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB pin configuration. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB clock enable. */
  LL_PWR_EnableVddUSB();
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_OTGFS);
#endif
} /*** end of HAL_MspInit ***/


/************************************************************************************//**
** \brief     DeInitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level de-initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
  /* Reset the RCC clock configuration to the default reset state. */
  LL_RCC_DeInit();
  
  /* Reset GPIO pin for the LED to turn it off. */
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

  /* Deinit used GPIOs. */
  LL_GPIO_DeInit(GPIOG);
  LL_GPIO_DeInit(GPIOC);
  LL_GPIO_DeInit(GPIOB);
  LL_GPIO_DeInit(GPIOA);

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB clock disable. */
  LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_OTGFS);
  LL_PWR_DisableVddUSB();
#endif

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART clock disable. */
  LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_LPUART1);
  LL_PWR_DisableVddIO2();
#endif

  /* GPIO ports clock disable. */
  LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
  LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /* PWR clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
  /* SYSCFG clock disable. */
  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
} /*** end of HAL_MspDeInit ***/


/*********************************** end of main.c *************************************/
