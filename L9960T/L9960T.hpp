/*
 * L9960T.hpp
 *
 *  Created on: Nov 13, 2021
 *      Author: Mateusz
 */

#ifndef SUMODRIVERS_L9960T_L9960T_HPP_
#define SUMODRIVERS_L9960T_L9960T_HPP_
#include "MotorControl.hpp"
#include "RobotSpecificDefines.hpp"

typedef enum {CURRENT_RANGE_0 = 0, CURRENT_RANGE_1 = 1, CURRENT_RANGE_2 = 2, CURRENT_RANGE_3 = 3} L9960T_CurrentRange;

class L9960T : protected MCInterface{
	public:
		L9960T(MotorSideTypeDef side, SPI_HandleTypeDef *hspi, CommManager *CommunicationManager);
		L9960T(MotorSideTypeDef side);
		HAL_StatusTypeDef AttachPWMTimerAndChannel(TIM_HandleTypeDef *htim, uint32_t Channel);
		HAL_StatusTypeDef SetMotorPowerPWM(uint16_t PowerPWM);
		HAL_StatusTypeDef SetMotorDirection(MotorDirectionTypeDef Dir);
		HAL_StatusTypeDef SetMaxCurrent(L9960T_CurrentRange CurrentRange);
		HAL_StatusTypeDef Disable(void);
		HAL_StatusTypeDef Enable(void);
		HAL_StatusTypeDef EmergencyStop(void);
		HAL_StatusTypeDef CheckIfControllerInitializedOk(void);
	private:
		CommManager *__CommunicationManager;
		MotorSideTypeDef __side;
		SPI_HandleTypeDef *__hspi;
		uint16_t __CS_Pin;
		uint16_t __IN1_PWM_PIN;
		uint16_t __IN2_DIR_PIN;
		uint16_t __DIS_PIN;
		GPIO_TypeDef *__CS_Port;
		GPIO_TypeDef *__IN1_PWM_PORT;
		GPIO_TypeDef *__IN2_DIR_PORT;
		GPIO_TypeDef *__DIS_PORT;
		GPIO_PinState __Direction;
		TIM_HandleTypeDef *__htim;
		uint32_t __Channel;

};


#endif /* SUMODRIVERS_L9960T_L9960T_HPP_ */
