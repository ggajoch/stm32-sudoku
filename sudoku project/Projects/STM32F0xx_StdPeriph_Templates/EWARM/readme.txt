/**
  @page ewarm EWARM Project Template for STM32F0xx devices

  @verbatim
  ******************* (C) COPYRIGHT 2013 STMicroelectronics ********************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    22-November-2013
  * @brief   This sub-directory contains all the user-modifiable files needed
  *          to create a new project linked with the STM32F0xx Standard Peripheral 
  *          Library and working with IAR Embedded Workbench for ARM (EWARM)
  *          software toolchain.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  @endverbatim

@par Directory contents

 - project .ewd/.eww/.ewp : A pre-configured project file with the provided library 
                            structure that produces an executable image with EWARM.

 - stm32f0xx_flash.icf    : STM32F0xx Medium-density devices Linker configuration file 
                            used to place program code (readonly) in internal FLASH 
                            and data (readwrite, Stack and Heap)in internal SRAM.
                            You can customize this file to your need.

 - stm32f0xx_ram.icf      : STM32F0xx Medium-density devices  Linker configuration file 
                            used to place program code (readonly) and data (readwrite,
                            Stack and Heap)in internal SRAM.
                            You can customize this file to your need.

 - stm32f0xx_ld_flash.icf : STM32F0xx Low-density devices Linker configuration file 
                            used to place program code (readonly) in internal FLASH 
                            and data (readwrite, Stack and Heap)in internal SRAM.
                            You can customize this file to your need.

 - stm32f030_flash.icf    : STM32F030 devices Linker configuration file 
                            used to place program code (readonly) in internal FLASH 
                            and data (readwrite, Stack and Heap)in internal SRAM.
                            You can customize this file to your need.

 - stm32f030_ram.icf      : STM32F030 devices  Linker configuration file 
                            used to place program code (readonly) and data (readwrite,
                            Stack and Heap)in internal SRAM.
                            You can customize this file to your need.
                            
@par How to use it ?

 - Open the Project.eww workspace.
 - In the workspace toolbar select the project config:
     - STM32F0XX_MD(STM32F0x1xx): to configure the project for STM32F0xx Medium-density devices.
                                  You can use STMicroelectronics STM320518-EVAL or STM32F0-Discovery
                                  board to run this project.
     - STM32F0XX_LD(STM32F0x0xx): to configure the project for STM32F0xx Low-density devices.
                                  You need to use custom HW board to run this project.

     - STM32F030: to configure the project for STM32F030 devices.
                  You can use STM32F0308-Discovery
                  board to run this project.
                                  
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Debug
 - Run program: Debug->Go(F5)


@note The needed define symbols for this config are already declared in the
      preprocessor section: USE_STDPERIPH_DRIVER, STM32F0XX, USE_STM320518_EVAL

@note - STM32F0xx Low-density devices are STM32F050xx and STM32F060xx microcontrollers
        where the Flash memory ranges between 16 and 32 Kbytes.
      - STM32F0xx Medium-density devices are STM32F051xx and STM32F061xx microcontrollers
        where the Flash memory ranges between 16 and 64 Kbytes  

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
