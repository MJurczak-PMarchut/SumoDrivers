/*
 * CommManager.cpp
 *
 *  Created on: Nov 13, 2021
 *      Author: Mateusz
 */
#include "CommManager.hpp"
#include "vector"

#ifndef MAX_MESSAGE_NO_IN_QUEUE
#define MAX_MESSAGE_NO_IN_QUEUE 5
#endif

#define VECTOR_NOT_FOUND 0xFF

CommManager::CommManager()
{

}
#ifdef I2C_USES_DMA
HAL_StatusTypeDef CommManager::AttachCommInt(I2C_HandleTypeDef *hi2c, DMA_HandleTypeDef *hdma)
#else
HAL_StatusTypeDef CommManager::AttachCommInt(I2C_HandleTypeDef *hi2c)
#endif
{
	CommQueue<I2C_HandleTypeDef*> Init;
	for(CommQueue<I2C_HandleTypeDef*> handleQueueVect : this->__hi2cQueueVect)
	{
		if(hi2c == handleQueueVect.handle)
		{
			return HAL_ERROR;
		}
	}
	Init.handle = hi2c;
#ifdef I2C_USES_DMA
	Init.hdma = hdma;
#endif
	this->__hi2cQueueVect.push_back(Init);
	return HAL_OK;
}
#ifdef SPI_USES_DMA
HAL_StatusTypeDef CommManager::AttachCommInt(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma)
#else
HAL_StatusTypeDef CommManager::AttachCommInt(SPI_HandleTypeDef *hspi)
#endif
{
	CommQueue<SPI_HandleTypeDef*> Init;
	for(CommQueue<SPI_HandleTypeDef*> handleQueueVect : this->__hspiQueueVect)
	{
		if(hspi == handleQueueVect.handle)
		{
			return HAL_ERROR;
		}
	}
	Init.handle = hspi;
#ifdef SPI_USES_DMA
	Init.hdma = hdma;
#endif
	this->__hspiQueueVect.push_back(Init);
	return HAL_OK;
}
#ifdef UART_USES_DMA
HAL_StatusTypeDef CommManager::AttachCommInt(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma)
#else
HAL_StatusTypeDef CommManager::AttachCommInt(UART_HandleTypeDef *huart)
#endif
{
	CommQueue<UART_HandleTypeDef*> Init;
	for(CommQueue<UART_HandleTypeDef*> handleQueueVect : this->__huartQueueVect)
	{
		if(huart == handleQueueVect.handle)
		{
			return HAL_ERROR;
		}
	}
	Init.handle = huart;
#ifdef UART_USES_DMA
	Init.hdma = hdma;
#endif
	this->__huartQueueVect.push_back(Init);
	return HAL_OK;
}

HAL_StatusTypeDef CommManager::PushCommRequestIntoQueue(MessageInfoTypeDef *MsgInfo)
{
	uint8_t VectorIndex;
#if defined(SPI_USES_DMA) or defined(I2C_USES_DMA) or defined(UART_USES_DMA)
	HAL_StatusTypeDef ret;
#endif
	VectorIndex = this->__CheckIfCommIntIsAttachedAndHasFreeSpace(&MsgInfo->uCommInt, MsgInfo->eCommType);
	if(VectorIndex != VECTOR_NOT_FOUND)
	{
		return HAL_ERROR;
	}
	else
	{
		switch(MsgInfo->eCommType)
		{
			case COMM_INT_SPI_TXRX:
			{
#ifdef SPI_USES_DMA
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					ret = HAL_SPI_TransmitReceive_DMA(MsgInfo->uCommInt.hspi, MsgInfo->pTxData, MsgInfo->pRxData, MsgInfo->len);
					__HAL_DMA_DISABLE_IT(this->__hspiQueueVect[VectorIndex].hdma, DMA_IT_HT);
					return ret;
				}
				return HAL_OK;
#elif defined SPI_USES_IT
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					return HAL_SPI_TransmitReceive_IT(MsgInfo->uCommInt.hspi, MsgInfo->pTxData, MsgInfo->pRxData, MsgInfo->len);
				}
				return HAL_OK;
#elif defined SPI_USES_WAIT
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					return HAL_SPI_TransmitReceive(MsgInfo->uCommInt.hspi, MsgInfo->pTxData, MsgInfo->pRxData, MsgInfo->len, COMMUNICATION_TIMEOUT);
				}
				return HAL_OK;
#endif
			}
				break;
			case COMM_INT_SPI_RX:
			{
#ifdef SPI_USES_DMA
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					ret = HAL_SPI_Receive_DMA(MsgInfo->uCommInt.hspi, MsgInfo->pRxData, MsgInfo->len);
					__HAL_DMA_DISABLE_IT(this->__hspiQueueVect[VectorIndex].hdma, DMA_IT_HT);
					return ret;
				}
				return HAL_OK;
#elif defined SPI_USES_IT
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					return HAL_SPI_Receive_IT(MsgInfo->uCommInt.hspi, MsgInfo->pRxData, MsgInfo->len);
				}
				return HAL_OK;
#elif defined SPI_USES_WAIT
				if(this->__hspiQueueVect[VectorIndex].MsgInfo.size() > 0)
				{
					this->__hspiQueueVect[VectorIndex].MsgInfo.push(*MsgInfo); //Queue not empty, push message back
				}
				if((MsgInfo->uCommInt.hspi->State == HAL_SPI_STATE_READY))
				{
					//Queue empty and peripheral ready, send message
					return HAL_SPI_Receive_IT(MsgInfo->uCommInt.hspi, MsgInfo->pRxData, MsgInfo->len, COMMUNICATION_TIMEOUT);
				}
				return HAL_OK;
#endif
			}
				break;
			case COMM_INT_I2C_TXRX:

				break;
			case COMM_INT_I2C_RX:

				break;
			case COMM_INT_UART_TXRX:

				break;
			case COMM_INT_UART_RX:

				break;
			default:
			{
				return HAL_ERROR;
			}
		}
	}
	return HAL_ERROR;
}

uint8_t CommManager::__CheckIfCommIntIsAttachedAndHasFreeSpace(CommIntUnionTypeDef *uCommInt, CommIntTypeDef eCommIntType)
{
	uint8_t u8Iter;
	switch(eCommIntType)
	{
		case COMM_INT_SPI_TXRX:
		case COMM_INT_SPI_RX:
		{
			for(u8Iter = 0; u8Iter < this->__hspiQueueVect.size(); u8Iter++)
			{
				if((uCommInt->hspi->Instance == __hspiQueueVect[u8Iter].handle->Instance) && (__hspiQueueVect[u8Iter].MsgInfo.size() < MAX_MESSAGE_NO_IN_QUEUE))
				{
					return u8Iter;
				}
			}
			return VECTOR_NOT_FOUND;
		}
			break;
		case COMM_INT_I2C_TXRX:
		case COMM_INT_I2C_RX:
		{
			for(u8Iter = 0; u8Iter < this->__hi2cQueueVect.size(); u8Iter++)
			{
				if((uCommInt->hi2c->Instance == __hi2cQueueVect[u8Iter].handle->Instance) && (__hi2cQueueVect[u8Iter].MsgInfo.size() < MAX_MESSAGE_NO_IN_QUEUE))
				{
					return u8Iter;
				}
			}
			return VECTOR_NOT_FOUND;
		}
			break;
		case COMM_INT_UART_TXRX:
		case COMM_INT_UART_RX:
		{
			for(u8Iter = 0; u8Iter < this->__huartQueueVect.size(); u8Iter++)
			{
				if((uCommInt->huart->Instance == __huartQueueVect[u8Iter].handle->Instance) && (__huartQueueVect[u8Iter].MsgInfo.size() < MAX_MESSAGE_NO_IN_QUEUE))
				{
					return u8Iter;
				}
			}
			return VECTOR_NOT_FOUND;
		}
			break;
		default:
		{
			return VECTOR_NOT_FOUND;
		}
	}
	return VECTOR_NOT_FOUND;
}

