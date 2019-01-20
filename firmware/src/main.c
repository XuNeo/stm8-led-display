/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "ezled.h"
#include "parameter.h"
#include "commands.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

ezled_def ezled;  //ezled instance.
extern ezledif_def ezledif; //the led hardware defined in led

void command_set_blink(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_blink(&ezled, *ppara);
}
void command_set_blink_speed(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_blink_speed(&ezled, (led_speed_def)*ppara);
}
void command_set_scroll_speed(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_scroll_speed(&ezled, (led_speed_def)*ppara);
}

void command_set_contrast(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_contrast(&ezled, (ledcont_def)*ppara);
}

void command_print(uint8_t *ppara, uint8_t len){
  //case string to len
  char *pstr = (char*)ppara;
  if(ppara == 0) return;
  ppara[len] = '\0';
  ezled_print(&ezled, pstr);
}

/**
 * save current ezled settings as default.
*/
void command_save_settings(uint8_t *ppara, uint8_t len){
  ezled_para_def curr;
  curr.blink_speed = ezled.blink_speed;
  curr.scroll_speed = ezled.scroll_speed;
  curr.contrast = ezled.led_contrast;
  curr.signiture = PARA_SIGNITURE;
  parameter_set(&curr);
}

void command_add_font(uint8_t *ppara, uint8_t len){
  led_font_def font;
  if(len < 2) return;
  if(ppara == 0) return;
  if(*ppara == 0) return;
  font.c = *ppara++;
  font.font = *ppara;
  ezled_font_append(&ezled, &font);
}

void main(void)
{
  static uint8_t fontbuff[256];
  /* Initialization of the clock */
  /* Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  ezled_init(&ezled, &ezledif);
  ezled_set_fontbuf(&ezled, fontbuff, 256);
  parameter_load(&ezled);
  commands_init();
  /* Enable general interrupts */  
  enableInterrupts();
  while (1)
  {
    commands_poll();
    ezled_poll(&ezled);
  }
  
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  volatile static u8 *pfile;
  volatile static u32 line_;
  line_ = line;
  pfile = file;
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
