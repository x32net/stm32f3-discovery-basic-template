#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "servo.h"
#include "beep.h"

#define ABS(x)         (x < 0) ? (-x) : x
#define PI                         (float)     3.14159265f
#define ACCEL_MULT  6

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

/* Private types -------------------------------------------------------------*/
typedef enum{ // direction is based on the LED that lights in that direction
  NORTH = 3,
  SOUTH = 10,
  WEST = 6,
  EAST = 7,
  UP = 5,
  DOWN = 4
} Direction_t;

/* Private variables ---------------------------------------------------------*/
  RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
__IO uint16_t ServoTimer = 0;
__IO uint8_t DataReady = 0;
__IO uint8_t LastPos = 0;
float AccBuffer[3] = {0.0f};
float AbsAccBuffer[3] = {0.0f};

/* Private function prototypes -----------------------------------------------*/
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
void Demo_CompassConfig(void);
void Demo_CompassReadAcc(float* pfData);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  *
  * this is called every 10ms
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  beep_tick();
  DataReady ++;
  ServoTimer ++;
  if(ServoTimer < 200) { //every 2 sec
	servo_set_pos(550);
  }
  else if(ServoTimer < 400) {
	servo_set_pos(1425);
  }
  else if(ServoTimer < 600) {
	servo_set_pos(2300);
  }
  else{
    ServoTimer = 0;
  }
}

/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */
int main(void)
{  
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  servo_init();
  beep_init();
  
  /* Initialize LEDs and User Button available on STM32F3-Discovery board */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);
    
  /* LEDs Off */
  STM_EVAL_LEDOff(LED4);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED6);
  STM_EVAL_LEDOff(LED7);
  STM_EVAL_LEDOff(LED10);
  STM_EVAL_LEDOff(LED8);
  STM_EVAL_LEDOff(LED9);
  STM_EVAL_LEDOff(LED5);
  
  /* Demo Compass & accelerometer */
  Demo_CompassConfig();
  
  DataReady = 0x00;
   
  /* Infinite loop */
  while (1)
  {   
        
    /* Wait for data ready */
    while(DataReady !=0x05)
    {}
    DataReady = 0x00;
    
    /* Read Accelerometer data */
    Demo_CompassReadAcc(AccBuffer);
      
    for(int i=0;i<3;i++) {
      AccBuffer[i] /= 100.0f;
    }
    
    for(int i=0; i<3; i++){
      AbsAccBuffer[i] = ABS(AccBuffer[i]);
    }
    if (  AbsAccBuffer[0] > ACCEL_MULT*AbsAccBuffer[1]
       && AbsAccBuffer[0] > ACCEL_MULT*AbsAccBuffer[2] ){
      if(AccBuffer[0] > 0){
        STM_EVAL_LEDOn(LED10);
        
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOff(LED5);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOff(LED9);
      }
      else{
        STM_EVAL_LEDOn(LED3);
        
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOff(LED5);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOff(LED9);
        STM_EVAL_LEDOff(LED10);
      }
    }
    else if (  AbsAccBuffer[1] > ACCEL_MULT*AbsAccBuffer[0]
            && AbsAccBuffer[1] > ACCEL_MULT*AbsAccBuffer[2] ){
      if(AccBuffer[1] > 0){
    	if(LastPos != 7){
    		LastPos = 7;
    		beep_on(1);
    	}
        STM_EVAL_LEDOn(LED7);
        
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOff(LED5);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOff(LED9);
        STM_EVAL_LEDOff(LED10);
      }
      else{
      	if(LastPos != 6){
      		LastPos = 6;
      		beep_on(0);
      	}
        STM_EVAL_LEDOn(LED6);
        
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOff(LED5);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOff(LED9);
        STM_EVAL_LEDOff(LED10);
      }
    }
    else if (  AbsAccBuffer[2] > ACCEL_MULT*AbsAccBuffer[1]
            && AbsAccBuffer[2] > ACCEL_MULT*AbsAccBuffer[0] ){
      if(AccBuffer[2] > 0){
        STM_EVAL_LEDOn(LED5);
        STM_EVAL_LEDOn(LED8);
        
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOff(LED9);
        STM_EVAL_LEDOff(LED10);
      }
      else{
        STM_EVAL_LEDOn(LED4);
        STM_EVAL_LEDOn(LED9);
        
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED5);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOff(LED10);
      }
    }
    else{
      /* LEDs Off */
      STM_EVAL_LEDOff(LED4);
      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED6);
      STM_EVAL_LEDOff(LED7);
      STM_EVAL_LEDOff(LED10);
      STM_EVAL_LEDOff(LED8);
      STM_EVAL_LEDOff(LED9);
      STM_EVAL_LEDOff(LED5);
    }
  }
}

/**
  * @brief  Configure the Mems to compass application.
  * @param  None
  * @retval None
  */
void Demo_CompassConfig(void)
{
  LSM303DLHCMag_InitTypeDef LSM303DLHC_InitStructure;
  LSM303DLHCAcc_InitTypeDef LSM303DLHCAcc_InitStructure;
  LSM303DLHCAcc_FilterConfigTypeDef LSM303DLHCFilter_InitStructure;
  
  /* Configure MEMS magnetometer main parameters: temp, working mode, full Scale and Data rate */
  LSM303DLHC_InitStructure.Temperature_Sensor = LSM303DLHC_TEMPSENSOR_DISABLE;
  LSM303DLHC_InitStructure.MagOutput_DataRate =LSM303DLHC_ODR_30_HZ ;
  LSM303DLHC_InitStructure.MagFull_Scale = LSM303DLHC_FS_8_1_GA;
  LSM303DLHC_InitStructure.Working_Mode = LSM303DLHC_CONTINUOS_CONVERSION;
  LSM303DLHC_MagInit(&LSM303DLHC_InitStructure);
  
   /* Fill the accelerometer structure */
  LSM303DLHCAcc_InitStructure.Power_Mode = LSM303DLHC_NORMAL_MODE;
  LSM303DLHCAcc_InitStructure.AccOutput_DataRate = LSM303DLHC_ODR_50_HZ;
  LSM303DLHCAcc_InitStructure.Axes_Enable= LSM303DLHC_AXES_ENABLE;
  LSM303DLHCAcc_InitStructure.AccFull_Scale = LSM303DLHC_FULLSCALE_2G;
  LSM303DLHCAcc_InitStructure.BlockData_Update = LSM303DLHC_BlockUpdate_Continous;
  LSM303DLHCAcc_InitStructure.Endianness=LSM303DLHC_BLE_LSB;
  LSM303DLHCAcc_InitStructure.High_Resolution=LSM303DLHC_HR_ENABLE;
  /* Configure the accelerometer main parameters */
  LSM303DLHC_AccInit(&LSM303DLHCAcc_InitStructure);
  
  /* Fill the accelerometer LPF structure */
  LSM303DLHCFilter_InitStructure.HighPassFilter_Mode_Selection =LSM303DLHC_HPM_NORMAL_MODE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_CutOff_Frequency = LSM303DLHC_HPFCF_16;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI1 = LSM303DLHC_HPF_AOI1_DISABLE;
  LSM303DLHCFilter_InitStructure.HighPassFilter_AOI2 = LSM303DLHC_HPF_AOI2_DISABLE;

  /* Configure the accelerometer LPF main parameters */
  LSM303DLHC_AccFilterConfig(&LSM303DLHCFilter_InitStructure);
}

/**
* @brief Read LSM303DLHC output register, and calculate the acceleration ACC=(1/SENSITIVITY)* (out_h*256+out_l)/16 (12 bit rappresentation)
* @param pnData: pointer to float buffer where to store data
* @retval None
*/
void Demo_CompassReadAcc(float* pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2];
  uint8_t buffer[6], cDivider;
  uint8_t i = 0;
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
  
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, buffer, 6);
   
  if(ctrlx[1]&0x40)
    cDivider=64;
  else
    cDivider=16;

  /* check in the control register4 the data alignment*/
  if(!(ctrlx[0] & 0x40) || (ctrlx[1] & 0x40)) /* Little Endian Mode or FIFO mode */
  {
    for(i=0; i<3; i++)
    {
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) + buffer[2*i])/cDivider;
    }
  }
  else /* Big Endian Mode */
  {
    for(i=0; i<3; i++)
      pnRawData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) + buffer[2*i+1])/cDivider;
  }
  /* Read the register content */
  LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_CTRL_REG4_A, ctrlx,2);


  if(ctrlx[1]&0x40)
  {
    /* FIFO mode */
    LSM_Acc_Sensitivity = 0.25;
  }
  else
  {
    /* normal mode */
    /* switch the sensitivity value set in the CRTL4*/
    switch(ctrlx[0] & 0x30)
    {
    case LSM303DLHC_FULLSCALE_2G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;
      break;
    case LSM303DLHC_FULLSCALE_4G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_4g;
      break;
    case LSM303DLHC_FULLSCALE_8G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_8g;
      break;
    case LSM303DLHC_FULLSCALE_16G:
      LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_16g;
      break;
    }
  }

  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)pnRawData[i]/LSM_Acc_Sensitivity;
  }

}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
