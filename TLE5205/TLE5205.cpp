/*
 * TLE5205.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: Mateusz
 */

#include "TLE5205.hpp"

typedef  enum {
	FREEWHELING = 0b11,
	MOTOR_CCW = 0b10,
	MOTOR_CW = 0b00,
	BRAKE = 0b01
}TLE5205_STATE_TypeDef;

TLE5205::TLE5205(MotorSideTypeDef side, TIM_HandleTypeDef *htim, uint32_t Channel)
{
	__htim = htim;
	__side = side;
	__Dir = MOTOR_DIR_FORWARD;
	__PowerPWM = 0;
	__IN1_STATE = (GPIO_PinState)(FREEWHELING & 0x1);
	__IN2_STATE = (GPIO_PinState)(FREEWHELING & 0x2);
	__Channel = Channel;
	if(side == MOTOR_LEFT)
	{
		__IN1_PIN = MOTOR_LEFT_IN1_Pin;
		__IN2_PIN = MOTOR_LEFT_IN2_Pin;
		__IN1_PORT = MOTOR_LEFT_IN1_Port;
		__IN2_PORT = MOTOR_LEFT_IN2_Port;
	}
	else if(side == MOTOR_RIGHT)
	{
		__IN1_PIN = MOTOR_RIGHT_IN1_Pin;
		__IN2_PIN = MOTOR_RIGHT_IN2_Pin;
		__IN1_PORT = MOTOR_RIGHT_IN1_Port;
		__IN2_PORT = MOTOR_RIGHT_IN2_Port;
	}
}


HAL_StatusTypeDef TLE5205::SetMotorPowerPWM(uint16_t PowerPWM)
{
	if(PowerPWM < 1000)
	{
		__PowerPWM = PowerPWM;
		__HAL_TIM_SET_COMPARE(this->__htim, this->__Channel, PowerPWM);
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef TLE5205::SetMotorDirection(MotorDirectionTypeDef Dir)
{
	__Dir = Dir;
	if(MOTOR_DIR_FORWARD)
	{
		__IN1_STATE = (GPIO_PinState)(MOTOR_CW & 0x1);
		__IN2_STATE = (GPIO_PinState)(MOTOR_CW & 0x2);
	}
	else
	{
		__IN1_STATE = (GPIO_PinState)(MOTOR_CCW & 0x1);
		__IN2_STATE = (GPIO_PinState)(MOTOR_CCW & 0x2);
	}
	return HAL_OK;
}

HAL_StatusTypeDef TLE5205::Disable(void)
{
	__PowerPWM = 0;
	__HAL_TIM_SET_COMPARE(this->__htim, this->__Channel, 0);
	return HAL_OK;
}

HAL_StatusTypeDef TLE5205::Brake(void)
{

	__IN1_STATE = (GPIO_PinState)(BRAKE & 0x1);
	__IN2_STATE = (GPIO_PinState)(BRAKE & 0x2);
	return HAL_OK;
}

HAL_StatusTypeDef TLE5205::Enable(void)
{
	return HAL_OK;
}

HAL_StatusTypeDef TLE5205::EmergencyStop(void)
{
	return this->Disable();
}

void TLE5205::TimCB(void)
{
	if(__PowerPWM == 0)
	{
		HAL_GPIO_WritePin(__IN1_PORT, __IN1_PIN, (GPIO_PinState)(FREEWHELING & 0x1));
		HAL_GPIO_WritePin(__IN2_PORT, __IN2_PIN, (GPIO_PinState)(FREEWHELING & 0x2));
	}
	else
	{
		HAL_GPIO_WritePin(__IN1_PORT, __IN1_PIN, __IN1_STATE);
		HAL_GPIO_WritePin(__IN2_PORT, __IN2_PIN, __IN2_STATE);
	}
}

void TLE5205::TimCBPulse(void)
{
	if(__htim->Channel == __Channel)
	{
		HAL_GPIO_WritePin(__IN1_PORT, __IN1_PIN, (GPIO_PinState)(FREEWHELING & 0x1));
		HAL_GPIO_WritePin(__IN2_PORT, __IN2_PIN, (GPIO_PinState)(FREEWHELING & 0x2));
	}
}

