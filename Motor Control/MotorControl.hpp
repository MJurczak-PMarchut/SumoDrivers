/*
 * MotorControl.hpp
 *
 *  Created on: 12 lis 2021
 *      Author: Mateusz
 */
#ifndef SUMODRIVERS_MOTOR_CONTROL_MOTORCONTROL_HPP_
#define SUMODRIVERS_MOTOR_CONTROL_MOTORCONTROL_HPP_
#include "../Configuration.h"
#if (defined(UART_USES_DMA) or defined(UART_USES_IT) or defined(UART_USES_WAIT)) or (defined(SPI_USES_DMA) or defined(SPI_USES_IT) or defined(SPI_USES_WAIT))

#include <queue>

typedef enum {MOTOR_LEFT, MOTOR_RIGHT} MotorSideTypeDef;
typedef enum {VESC_CONTROLLER, l9960T_CONTROLLER} ControllerTypeTypeDef;
typedef enum {MOTOR_DIR_FORWARD, MOTOR_DIR_BACKWARD} MotorDirectionTypeDef;

typedef struct {
	uint8_t used_flag;
	uint8_t used_comm_interface;
	MotorSideTypeDef used_motor_side;
	uint8_t array_index;
}ActiveMotorControllerTypeDef;

class MCInterface{
	public:
#if defined(UART_USES_DMA) or defined(UART_USES_IT) or defined(UART_USES_WAIT)
		MCInterface(MotorSideTypeDef side, UART_HandleTypeDef *huart );
#endif
#if defined(SPI_USES_DMA) or defined(SPI_USES_IT) or defined(SPI_USES_WAIT)
		MCInterface(MotorSideTypeDef side, SPI_HandleTypeDef *hspi, uint16_t CS_Pin, GPIO_TypeDef *GPIOx);
#endif
		virtual HAL_StatusTypeDef SetMotorPowerPWM(uint16_t PowerPWM) = 0;
		virtual HAL_StatusTypeDef SetMotorDirection(MotorDirectionTypeDef Dir) = 0;
//		virtual HAL_StatusTypeDef SetMaxCurrent(uint32_t MaxCurrent_mA) = 0;
		virtual HAL_StatusTypeDef Disable() = 0;
		HAL_StatusTypeDef CheckIfControllerAttachedOk(void);
		HAL_StatusTypeDef EmergencyStop(void);
	private:
		HAL_StatusTypeDef __CheckIfControllerAvailable(MotorSideTypeDef side, uint8_t used_comm_interface);
		uint8_t index;
};

#endif

#endif /* SUMODRIVERS_MOTOR_CONTROL_MOTORCONTROL_HPP_ */
