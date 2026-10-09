/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 2 - Exercise 10
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// Clock variables
int hour = 15;
int minute = 8;
int second = 50;

int index_led = 0;
int led_buffer[4] = {1, 5, 0, 8};

// Matrix LED parameters
const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
uint8_t matrix_buffer[8] = {0};

// 8 columns of 'A' followed by 8 blank columns for smooth scrolling
const uint8_t font_A[16] = {
  0x00, 0xFC, 0x12, 0x11, 0x12, 0xFC, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
int shift_index = 0;

// Software timer variables (TIM2 tick = 10ms)
int timer0_counter = 0, timer0_flag = 0;  // clock + DOT
int timer1_counter = 0, timer1_flag = 0;  // 7-segment scan
int timer2_counter = 0, timer2_flag = 0;  // matrix column scan
int timer3_counter = 0, timer3_flag = 0;  // matrix animation shift
int TIMER_CYCLE = 10; // TIM2 interrupt period is 10ms
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);

void setTimer0(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void timer_run(void);

void displayROW(uint8_t value);
void clearAllColumns(void);
void updateLEDMatrix(int index);
void updateAnimation(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Software Timer functions
void setTimer0(int duration) {
  timer0_counter = duration / TIMER_CYCLE;
  timer0_flag = 0;
}

void setTimer1(int duration) {
  timer1_counter = duration / TIMER_CYCLE;
  timer1_flag = 0;
}

void setTimer2(int duration) {
  timer2_counter = duration / TIMER_CYCLE;
  timer2_flag = 0;
}

void setTimer3(int duration) {
  timer3_counter = duration / TIMER_CYCLE;
  timer3_flag = 0;
}

// Called every 10ms from the timer interrupt: only counts down, nothing heavy
void timer_run(void) {
  if (timer0_counter > 0) {
    timer0_counter--;
    if (timer0_counter == 0) {
      timer0_flag = 1;
    }
  }
  if (timer1_counter > 0) {
    timer1_counter--;
    if (timer1_counter == 0) {
      timer1_flag = 1;
    }
  }
  if (timer2_counter > 0) {
    timer2_counter--;
    if (timer2_counter == 0) {
      timer2_flag = 1;
    }
  }
  if (timer3_counter > 0) {
    timer3_counter--;
    if (timer3_counter == 0) {
      timer3_flag = 1;
    }
  }
}

// 7-segment display decoder (Common Anode, 0 = segment ON)
void display7SEG(int num) {
  uint8_t seg_code[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

  if (num >= 0 && num <= 9) {
    uint8_t code = seg_code[num];
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin, (code & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin, (code & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG2_Pin, (code & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG3_Pin, (code & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG4_Pin, (code & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG5_Pin, (code & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, (code & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

// Update 7-segment LED digit selection
void update7SEG(int index) {
  // Disable all digits (PNP transistor: HIGH = OFF)
  HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);

  display7SEG(led_buffer[index]);

  switch (index) {
    case 0:
      HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET);
      break;
    case 1:
      HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET);
      break;
    case 2:
      HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET);
      break;
    case 3:
      HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET);
      break;
    default:
      break;
  }
}

// Update digital clock values in led_buffer
void updateClockBuffer(void) {
  led_buffer[0] = hour / 10;
  led_buffer[1] = hour % 10;
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute % 10;
}

// Put one byte on ROW0..ROW7 (bit = 1 -> LED lit, rows are active HIGH)
void displayROW(uint8_t value) {
  HAL_GPIO_WritePin(GPIOB, ROW0_Pin, (value & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW1_Pin, (value & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW2_Pin, (value & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW3_Pin, (value & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW4_Pin, (value & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW5_Pin, (value & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW6_Pin, (value & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW7_Pin, (value & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Disable all matrix columns (ULN2803 is NPN: LOW = column OFF)
void clearAllColumns(void) {
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin | ENM1_Pin | ENM2_Pin | ENM3_Pin |
                           ENM4_Pin | ENM5_Pin | ENM6_Pin | ENM7_Pin, GPIO_PIN_RESET);
}

// Show one column: clear columns, output row data, then enable that column
void updateLEDMatrix(int index) {
  clearAllColumns();
  displayROW(matrix_buffer[index]);

  switch (index) {
    case 0: HAL_GPIO_WritePin(GPIOA, ENM0_Pin, GPIO_PIN_SET); break;
    case 1: HAL_GPIO_WritePin(GPIOA, ENM1_Pin, GPIO_PIN_SET); break;
    case 2: HAL_GPIO_WritePin(GPIOA, ENM2_Pin, GPIO_PIN_SET); break;
    case 3: HAL_GPIO_WritePin(GPIOA, ENM3_Pin, GPIO_PIN_SET); break;
    case 4: HAL_GPIO_WritePin(GPIOA, ENM4_Pin, GPIO_PIN_SET); break;
    case 5: HAL_GPIO_WritePin(GPIOA, ENM5_Pin, GPIO_PIN_SET); break;
    case 6: HAL_GPIO_WritePin(GPIOA, ENM6_Pin, GPIO_PIN_SET); break;
    case 7: HAL_GPIO_WritePin(GPIOA, ENM7_Pin, GPIO_PIN_SET); break;
    default: break;
  }
}

// Copy an 8-column window of font_A into matrix_buffer, then slide the window by 1
void updateAnimation(void) {
  for (int i = 0; i < 8; i++) {
    matrix_buffer[i] = font_A[(shift_index + i) % 16];
  }
  shift_index = (shift_index + 1) % 16;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // Start hardware timer 2 interrupt (10ms tick)
  updateClockBuffer();           // Fill led_buffer from hour/minute

  setTimer0(1000); // Software timer 0: clock + DOT blink every 1s
  setTimer1(250);  // Software timer 1: scan 4 seven-segment LEDs every 250ms
  setTimer2(10);   // Software timer 2: scan LED matrix columns every 10ms
  setTimer3(150);  // Software timer 3: shift letter A every 150ms
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // clock calculation and DOT blinking (Timer 0)
    if (timer0_flag == 1) {
      setTimer0(1000);
      second++;
      if (second >= 60) {
        second = 0;
        minute++;
        if (minute >= 60) {
          minute = 0;
          hour++;
          if (hour >= 24) {
            hour = 0;
          }
        }
      }
      updateClockBuffer();
      HAL_GPIO_TogglePin(GPIOA, DOT_Pin);
    }

    // scan 7-segment LEDs (Timer 1)
    if (timer1_flag == 1) {
      setTimer1(250);
      update7SEG(index_led);
      index_led++;
      if (index_led >= 4) {
        index_led = 0;
      }
    }

    // scan LED matrix columns (Timer 2)
    if (timer2_flag == 1) {
      setTimer2(10);
      updateLEDMatrix(index_led_matrix);
      index_led_matrix++;
      if (index_led_matrix >= MAX_LED_MATRIX) {
        index_led_matrix = 0;
      }
    }

    // shift letter A one column (Timer 3, from lab10)
    if (timer3_flag == 1) {
      setTimer3(150);
      updateAnimation();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function (10ms period: 8MHz / 8000 / 10)
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  *        NOTE: generated by CubeIDE from the .ioc file (do not edit by hand).
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin|ENM1_Pin|ENM2_Pin|LED_RED_Pin
                          |ENM3_Pin|ENM4_Pin|ENM5_Pin|ENM6_Pin
                          |ENM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DOT_Pin|EN0_Pin|EN1_Pin|EN2_Pin
                          |EN3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ROW2_Pin|ROW3_Pin|ROW4_Pin|ROW5_Pin
                          |ROW6_Pin|ROW7_Pin|ROW0_Pin|ROW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins on port A */
  GPIO_InitStruct.Pin = ENM0_Pin|ENM1_Pin|ENM2_Pin|DOT_Pin
                          |LED_RED_Pin|EN0_Pin|EN1_Pin|EN2_Pin
                          |EN3_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins on port B */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin
                          |SEG6_Pin|ROW0_Pin|ROW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
// Timer interrupt callback function (invoked every 10ms)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
    timer_run(); // Decrement software timer counters every 10ms
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
/* USE_FULL_ASSERT */
