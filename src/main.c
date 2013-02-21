#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "servo.h"
#include "beep.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "gyro.h"
#include <stdlib.h>

#define ABS(x)         (x < 0) ? (-x) : x
#define PI                         (float)     3.14159265f
#define ACCEL_MULT  6

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

typedef enum{
  DIFF_EASY = 0, //sounds on, LEDs always lit
  DIFF_MED = 1, //sounds on, LEDs lit for already known paths
  DIFF_HARD = 2,  //sounds on, no LEDs
  DIFF_IMPOSSIBLE = 3 //no sound, no LEDs
} Difficulty_t;

/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t     TimingDelay = 0;
__IO uint16_t     ServoTimer  = 0;
__IO uint8_t      DataReady   = 0;
__IO Direction_t  LastDir     = UP;
__IO uint8_t      DirPos      = 0;
__IO uint8_t      BestDirPos  = 0;
__IO State_t      State       = STATE_INIT;
__IO unsigned int Seed = 0;
     Difficulty_t MyDifficulty = DIFF_EASY;
float AccBuffer[3] = {0.0f};
float AbsAccBuffer[3] = {0.0f};
const Indication_t IndicationMap[6][6] =
{
    {IND_INV, IND_INV, IND_CCW, IND_CW,  IND_SN,  IND_NS},
    {IND_INV, IND_INV, IND_CW,  IND_CCW, IND_NS,  IND_SN},
    {IND_CW,  IND_CCW, IND_INV, IND_INV, IND_WE,  IND_EW},
    {IND_CCW, IND_CW,  IND_INV, IND_INV, IND_EW,  IND_WE},
    {IND_NS,  IND_SN,  IND_EW,  IND_WE,  IND_INV, IND_INV},
    {IND_SN,  IND_NS,  IND_WE,  IND_EW,  IND_INV, IND_INV}
};
const int NumDir = 13;
Direction_t DirList[13] =
{ UP, NORTH, EAST, DOWN, SOUTH, WEST, NORTH, UP, WEST, SOUTH, EAST, SOUTH, UP };
//const int NumDir = 3;
//const Direction_t DirList[3] =
//{ UP, NORTH, UP };

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
  DataReady ++;
  beep_tick();
  led_tick();
  GyroTick();
}

void GenerateDirList(){
  srand(Seed);
  int myDirPos = 1; //the first direction must always be "UP"
  while(myDirPos < (NumDir-1)){
    DirList[myDirPos] = (Direction_t)(rand() % 6);
    if(IndicationMap[DirList[myDirPos-1]][DirList[myDirPos]] != IND_INV ){ //legitimate move
      if(myDirPos != (NumDir-2)
          || IndicationMap[DirList[myDirPos]][UP] != IND_INV){
        myDirPos++;
      }
    }
  }
}

void HandleNewDir(Direction_t dir){
  if(dir == DirList[DirPos] ){
    if(MyDifficulty != DIFF_IMPOSSIBLE){
      beep_on(BEEP_HI);
    }
    LastDir = dir;
    if(++DirPos == NumDir){
      State = STATE_WON;
      BestDirPos = 0;
      return;
    }
    if(DirPos > BestDirPos){
      BestDirPos = DirPos;
    }
    if(MyDifficulty == DIFF_EASY
      || (MyDifficulty == DIFF_MED && DirPos < BestDirPos)
      ) {
      led_set_indication(IndicationMap[dir][DirList[DirPos]]);
    }
    else{
      led_set_indication(IND_INV);
    }
  }
  else if(dir == LastDir){
    //do nothing
  }
  else{
    if(MyDifficulty != DIFF_IMPOSSIBLE){
      beep_on(BEEP_LO);
    }
    LastDir = UP;
    DirPos = 0;
    led_set_indication(IND_INV);
  }
}

void DoPuzzle(){
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
      HandleNewDir(SOUTH);
    }
    else{
      HandleNewDir(NORTH);
    }
  }
  else if (  AbsAccBuffer[1] > ACCEL_MULT*AbsAccBuffer[0]
          && AbsAccBuffer[1] > ACCEL_MULT*AbsAccBuffer[2] ){
    if(AccBuffer[1] > 0){
      HandleNewDir(EAST);
    }
    else{
      HandleNewDir(WEST);
    }
  }
  else if (  AbsAccBuffer[2] > ACCEL_MULT*AbsAccBuffer[1]
          && AbsAccBuffer[2] > ACCEL_MULT*AbsAccBuffer[0] ){
    if(AccBuffer[2] > 0){
      HandleNewDir(UP);
    }
    else{
      HandleNewDir(DOWN);
    }
  }
}

int GyroDir = 0;

void DoGyro(){
  float pos = GyroReadAngPos();
  if(GyroDir == 0){
    if(pos > 22.5){
      GyroDir = 1;
      STM_EVAL_LEDOn(ORIG_LED3);
      beep_on(BEEP_HI);
      MyDifficulty = DIFF_EASY;
      led_set_indication(IND_EASY);
    }
    else if(pos < -22.5){
      GyroDir = -1;
      STM_EVAL_LEDOn(ORIG_LED3);
      beep_on(BEEP_HI);
      MyDifficulty = DIFF_EASY;
      led_set_indication(IND_EASY);
    }
  }
  else{
    pos = pos * GyroDir;
    if(pos < 0){
      STM_EVAL_LEDOn(ORIG_LED10);
      beep_on(BEEP_HI);
      led_set_indication(IND_INV);
      GenerateDirList();
      State = STATE_PUZZLE;
    }
    else if(pos > 45 && MyDifficulty ==DIFF_EASY){
      beep_on(BEEP_HI);
      STM_EVAL_LEDOn(ORIG_LED5);
      STM_EVAL_LEDOn(ORIG_LED4);
      MyDifficulty = DIFF_MED;
      led_set_indication(IND_MED);
    }
    else if(pos > 67.5 && MyDifficulty ==DIFF_MED){
      beep_on(BEEP_HI);
      STM_EVAL_LEDOn(ORIG_LED7);
      STM_EVAL_LEDOn(ORIG_LED6);
      MyDifficulty = DIFF_HARD;
      led_set_indication(IND_HARD);
    }
    else if(pos > 90 && MyDifficulty ==DIFF_HARD){
      beep_on(BEEP_HI);
      STM_EVAL_LEDOn(ORIG_LED9);
      STM_EVAL_LEDOn(ORIG_LED8);
      MyDifficulty = DIFF_IMPOSSIBLE;
      led_set_indication(IND_IMPOSSIBLE);
    }
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
  led_init();
  button_init();
  GyroInit();
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  /* Demo Compass & accelerometer */
  Demo_CompassConfig();
  
  DataReady = 0x00;

//  led_set_indication(IndicationMap[UP][DirList[0]]);

  /* Infinite loop */
  while (1)
  {   
    switch(State){
    case STATE_INIT:
      if(button_state()){
        servo_close();
        GyroZeroAndEnable();
        led_set_indication(IND_INV);
        State = STATE_GYRO;
      }
      else{
        servo_open();
        State = STATE_OPEN;
      }
      break;
    case STATE_OPEN:
      led_set_indication(IND_INV);
      if(button_state()){
        Delay(50); //wait half a second
        servo_close();
        GyroZeroAndEnable();
        led_set_indication(IND_INV);
        State = STATE_GYRO;
      }
      else{
        //box is still open, do nothing
      }
      break;
    case STATE_GYRO:
      //if the box is open, we can't continue the puzzle
      if(!button_state()){
        servo_open();
        LastDir = UP;
        DirPos = 0;
        GyroDir = 0;
        MyDifficulty = DIFF_EASY;
        State = STATE_OPEN;
        break;
      }
      DoGyro();
      break;
    case STATE_PUZZLE:
      //if the box is open, we can't continue the puzzle
      if(!button_state()){
        servo_open();
        LastDir = UP;
        DirPos = 0;
        GyroDir = 0;
        MyDifficulty = DIFF_EASY;
        State = STATE_OPEN;
        break;
      }
      //else...
      DoPuzzle();
      break;
    case STATE_WON:
      led_set_indication(IND_INV);
      servo_open();
      LastDir = UP;
      DirPos = 0;
      GyroDir = 0;
      MyDifficulty = DIFF_EASY;
      if(!button_state()){
        State = STATE_OPEN;
      }
      break;
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

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t L3GD20_TIMEOUT_UserCallback(void)
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
