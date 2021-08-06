/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hc05.h"
#include "math.h"
#include "sfud.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
TxPack txpack;
RxPack rxpack;
#define SFUD_DEMO_TEST_BUFFER_SIZE                     256
float  kP,kI,kD;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

int __io_putchar(int ch) {
    uint8_t temp[1] = {ch};
    HAL_UART_Transmit(&huart1, temp, 1, 0xff);
    return (ch);
}

void save(void);
void read(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    int i = 0;
    int motor,dir1;
    int dir2;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  while (sfud_init() != SFUD_SUCCESS){}
  read();

  HAL_TIM_Base_Start_IT(&htim14); //�?启定时器
  HAL_UART_Receive_IT(&huart2, (uint8_t *) &uartByte, 1);
  //  readValuePack(&rxpack);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
        if (readValuePack(&rxpack)) {
           if(rxpack.bools[0]){
               save();
           }
            motor=rxpack.integers[0];
            dir1 =rxpack.integers[1];
            dir2 =rxpack.integers[2];

            kP = (float)(rxpack.floats[0]);
            kI = (float)(rxpack.floats[1]);
            kD = (float)(rxpack.floats[2]);
        }

        txpack.floats[0] = 60 * sin(i / 127.0 * 3.1415926);
        txpack.floats[1] = 120 * sin(i / 127.0 * 3.1415926);
        txpack.floats[2] = 60;

        sendValuePack(&txpack);

        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        printf("%d,%d,%d,%d,%d,%d\r\n",motor,dir1,dir2,(int)(1000*kP),(int)(1000*kI),(int)(1000*kD));

        i++;
        if(i==255)
        {
            i=0;
        }

        HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
#pragma clang diagnostic pop
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void read(void)
{
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    uint16_t P,I,D;
    printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
    result = sfud_read(flash, 0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);  //读取flash数据存于PID
    if (result == SFUD_SUCCESS) {
        P = (sfud_demo_test_buf[1]<<8) + sfud_demo_test_buf[0];             //
        kP = (float)(P/1000.0);
        I = (sfud_demo_test_buf[4]<<8) + sfud_demo_test_buf[3];
        kI = (float)(I/1000.0);
        D = (sfud_demo_test_buf[6]<<8) + sfud_demo_test_buf[5];
        kD = (float)(D/1000.0);
    }
}

void save(void)
{
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    uint16_t Calculating;
    Calculating = (uint16_t) (rxpack.floats[0] * 1000);  //P
    sfud_demo_test_buf[0] =  (uint16_t)(Calculating) & 0xff;        //�??8�??
    sfud_demo_test_buf[1] =  (uint16_t)(Calculating >> 8) & 0xff;

    Calculating = (uint16_t) (rxpack.floats[1] * 1000);  //I
    sfud_demo_test_buf[3] = (Calculating) & 0xff;  //�??8�??
    sfud_demo_test_buf[4] = (Calculating >> 8) & 0xff;

    Calculating = (uint16_t) (rxpack.floats[2] * 1000);  //D
    sfud_demo_test_buf[5] = (Calculating) & 0xff;  //�??8�??
    sfud_demo_test_buf[6] = (Calculating >> 8) & 0xff;

    result = sfud_erase(flash, 0, sizeof(sfud_demo_test_buf));
    if (result == SFUD_SUCCESS) {
        printf("*********************************\r\n");}

    result = sfud_write(flash, 0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
    if (result == SFUD_SUCCESS) {
        printf("---------------------------------\r\n");}
}
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
