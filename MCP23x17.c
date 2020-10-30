/*
 * MCP23x17.c
 *
 *  Created on: 29 oct 2020
 *      Author: a.fabre
 */
#include "MCP23x17.h"

/**
 * @fun: uint8_t MCP23x17_init(MCP23x17_t *driver)
 * @brief  	MCP23x17 initialize driver
 * @param  	MCP23x17_t *driver
 * @note
 * @retval 	1 success | 0 error
 */
uint8_t MCP23x17_init(MCP23x17_t *driver)
{
	uint8_t iocon;

	if (driver->com.protocole == MCP23S17_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);
		wait(1);
		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	/* IOCON Config register */
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_IOCON, driver->config.portA.IOCON);
	iocon = MCP23x17_read(driver, MCP23x17_portA, MCP23x17_IOCON);

	if(iocon != driver->config.portA.IOCON)
	{
		iocon = 0; /* Error occured */
	}
	else
	{
		iocon = 1;
	}

	/* PORT A */
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_IODIR, driver->config.portA.IODIR);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_IPOL, driver->config.portA.IPOL);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_GPINTEN, driver->config.portA.GPINTEN);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_DEFVAL, driver->config.portA.DEFVAL);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_INTCON, driver->config.portA.INTCON);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_GPPU, driver->config.portA.GPPU);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_GPIO, driver->config.portA.default_GPIO);
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_OLAT, driver->config.portA.OLAT);

	/* PORT B */
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_IODIR, driver->config.portB.IODIR);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_IPOL, driver->config.portB.IPOL);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_GPINTEN, driver->config.portB.GPINTEN);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_DEFVAL, driver->config.portB.DEFVAL);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_INTCON, driver->config.portB.INTCON);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_GPPU, driver->config.portB.GPPU);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_GPIO, driver->config.portB.default_GPIO);
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_OLAT, driver->config.portB.OLAT);

	return iocon;
}

/**
 * @fun: 	uint8_t MCP23x17_read(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg)
 * @brief  	MCP23x17 read specific register
 * @param  	MCP23x17_t *driver
 * @param 	MCP23x17_port_t port A or B
 * @param	MCP23x17_register_t MCP23x17 register
 * @note
 * @retval 	value of register
 */
uint8_t MCP23x17_read(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg)
{
	uint8_t TxBuffer[4] = {0};
	uint8_t RxBuffer[4] = {0};
	uint8_t Opcode = driver->com.Opcode;

	if (driver->config.BANK == MCP23x17_BANK0)
	{
		reg<<=1;

		if (port == MCP23x17_portB)
		{
			reg+=1;
		}
	}
	else
	{
		if (port == MCP23x17_portB)
		{
			reg+=0x10;
		}
	}
	

	if (driver->com.protocole == MCP23S17_SPI)
	{
		Opcode &= 0x03u; /* With SPI only A0 and A1 are used for address decoding */
	}

	TxBuffer[0] = (MCP23x17_READ_CONTROLE_BYTE + (Opcode << 1u));
	TxBuffer[1] = (uint8_t) reg;


	if (driver->com.protocole == MCP23S17_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY))
		{
			;
		}

		if (HAL_SPI_TransmitReceive(driver->com.hspix, (uint8_t*) TxBuffer, (uint8_t *) RxBuffer, 3, 2000) != HAL_OK) 
		{
			Error_Handler();
		}

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY)) 
		{
			;
		}

		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	if (driver->com.protocole == MCP23017_I2C)
	{
		osMutexWait(driver->com.mutex, osWaitForever);

		if (HAL_I2C_Master_Transmit(driver->com.hi2c, (uint16_t) driver->com.Opcode, TxBuffer, 2, 1000) != HAL_OK) 
		{
			Error_Handler();
		}
		if (HAL_I2C_Master_Receive(driver->com.hi2c, (uint16_t) driver->com.Opcode, RxBuffer, 1, 1000) != HAL_OK) {
			Error_Handler();
		}

		osMutexRelease(driver->com.mutex);
	}

	return (int8_t) RxBuffer[2];
}

/**
 * @fun: 	uint8_t MCP23x17_readportA(MCP23x17_t *driver)
 * @brief  	MCP23x17 read GPIO port A
 * @param  	MCP23x17_t *driver
 * @note
 * @retval 	value of GPIO's port A
 */
uint8_t MCP23x17_readportA(MCP23x17_t *driver) 
{
	return MCP23x17_read(driver, MCP23x17_portA, MCP23x17_GPIO);
}

/**
 * @fun: 	uint8_t MCP23x17_readportB(MCP23x17_t *driver)
 * @brief  	MCP23x17 read GPIO port B
 * @param  	MCP23x17_t *driver
 * @note
 * @retval 	value of GPIO's port B
 */
uint8_t MCP23x17_readportB(MCP23x17_t *driver) 
{
	return MCP23x17_read(driver, MCP23x17_portB, MCP23x17_GPIO);
}

/**
 * @fun: 	void MCP23x17_write(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg, uint8_t value)
 * @brief  	MCP23x17 read specific register
 * @param  	MCP23x17_t *driver
 * @param 	MCP23x17_port_t port A or B
 * @param  	MCP23x17_register_t MCP23x17 register
 * @param  	int8_t value of register
 * @note
 * @retval  none
 */
void MCP23x17_write(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg, uint8_t value)
{
	uint8_t TxBuffer[4] = {0};
	uint8_t RxBuffer[4] = {0};
	uint8_t Opcode = driver->com.Opcode;

	if (driver->config.BANK == MCP23x17_BANK0)
	{
		reg<<=1;

		if (port == MCP23x17_portB)
		{
			reg+=1;
		}
	}
	else
	{
		if (port == MCP23x17_portB)
		{
			reg+=0x10;
		}
	}

	if (driver->com.protocole == MCP23S17_SPI)
	{
		Opcode &= 0x03u; /* With SPI only A0 and A1 are used for address decoding */
	}

	TxBuffer[0] = (MCP23x17_WRITE_CONTROLE_BYTE + (Opcode << 1u));
	TxBuffer[1] = (uint8_t) reg;
	TxBuffer[2] = value;


	if (driver->com.protocole == MCP23S17_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY))
		{
			;
		}

		if (HAL_SPI_TransmitReceive(driver->com.hspix, (uint8_t*) TxBuffer, (uint8_t *) RxBuffer, 3, 2000) != HAL_OK) 
		{
			Error_Handler();
		}

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY)) 
		{
			;
		}

		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	if (driver->com.protocole == MCP23017_I2C)
	{
		osMutexWait(driver->com.mutex, osWaitForever);

		if (HAL_I2C_Master_Transmit(driver->com.hi2c, (uint16_t) driver->com.Opcode, TxBuffer, 3, 1000) != HAL_OK) 
		{
			Error_Handler();
		}

		osMutexRelease(driver->com.mutex);
	}
}
/**
 * @fun: 	void MCP23x17_writeportA(MCP23x17_t *driver, uint8_t value)
 * @brief  	MCP23x17 write port A
 * @param  	MCP23x17_t *device
 * @param	uint8_t value of port A
 * @note
 * @retval 	none
 */
void MCP23x17_writeportA(MCP23x17_t *driver, uint8_t value)
{
	MCP23x17_write(driver, MCP23x17_portA, MCP23x17_GPIO, value);
}

/**
 * @fun: 	void MCP23x17_writeportB(MCP23x17_t *driver, uint8_t value)
 * @brief  	MCP23x17 write port B
 * @param  	MCP23x17_t *device
 * @param	uint8_t value of port B
 * @note
 * @retval 	none
 */
void MCP23x17_writeportB(MCP23x17_t *driver, uint8_t value)
{
	MCP23x17_write(driver, MCP23x17_portB, MCP23x17_GPIO, value);
}