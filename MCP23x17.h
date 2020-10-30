/*
 * MCP23x17.h
 *
 *  Created on: 29 oct 2020
 *      Author: a.fabre
 */

/*
------------------------------------------------------------
Exemple of initialisation
------------------------------------------------------------
MCP23x17_t MCP23S17_CIP0059_MOTOR = {
	.com.protocole = MCP23S17_SPI,
	.com.hspix = hspi2,
	.com.mutex = hspi2_mutex,
	.com.Opcode = 0b101,
	
	.config.BANK = MCP23x17_BANK0,
	.config.portA.INTCON = 0xFF,
	.config.portA.IODIR = 0xFF,
};

MCP23x17_init(&MCP23S17_CIP0059_MOTOR);

------------------------------------------------------------
Exemple of structure to hold read function result
------------------------------------------------------------
typedef union MCP23x17_NAME_portA_s {
	uint8_t GPIO;
	struct {
		uint8_t GP0:1;
		uint8_t GP1:1;
		uint8_t GP2:1;
		uint8_t GP3:1;
		uint8_t GP4:1;
		uint8_t GP5:1;
		uint8_t GP6:1;
		uint8_t GP7:1;
	};
} MCP23x17_NAME_portA_t;

MCP23x17_NAME_portA_t driver_portA;
driver_portA.GPIO = MCP23x17_readportA(&MCP23S17_CIP0059_MOTOR);

if (driver_portA.GP0 == 1)
{
	...
}

*/


#ifndef _MCP23x17_H_
#define _MCP23x17_H_

#include <stdint.h>

#define MCP23x17_READ_CONTROLE_BYTE 	0x41u
#define MCP23x17_WRITE_CONTROLE_BYTE 	0x40u

/*
------------------------------------------------------------
 BANK 0 Registers Addresses
------------------------------------------------------------
BANK0_IODIRA  	= 0
BANK0_IODIRB	= 1
BANK0_IPOLA	    = 2
BANK0_IPOLB	    = 3
BANK0_GPINTENA  = 4
BANK0_GPINTENB  = 5
BANK0_DEFVALA	= 6
BANK0_DEFVALB	= 7
BANK0_INTCONA	= 8
BANK0_INTCONB	= 9
BANK0_IOCONA	= 10
BANK0_IOCONB	= 11
BANK0_GPPUA	    = 12
BANK0_GPPUB	    = 13
BANK0_INTFA	    = 14
BANK0_INTFB	    = 15
BANK0_INTCAPA	= 16
BANK0_INTCAPB	= 17
BANK0_GPIOA	    = 18
BANK0_GPIOB 	= 19
BANK0_OLATA	    = 20
BANK0_OLATB	    = 21

------------------------------------------------------------ 
BANK 1 Registers Addresses
------------------------------------------------------------
BANK1_IODIRA 	= 0
BANK1_IPOLA  	= 1
BANK1_GPINTENA  = 2
BANK1_DEFVALA 	= 3
BANK1_INTCONA   = 4
BANK1_IOCONA	= 5
BANK1_GPPUA	    = 6
BANK1_INTFA	    = 7
BANK1_INTCAPA	= 8
BANK1_GPIOA	    = 9
BANK1_OLATA	    = 10

BANK1_IODIRB 	= 16
BANK1_IPOLB	    = 17
BANK1_GPINTENB	= 18
BANK1_DEFVALB	= 19
BANK1_INTCONB	= 20
BANK1_IOCONB	= 21
BANK1_GPPUB	    = 22
BANK1_INTFB	    = 23
BANK1_INTCAPB	= 24
BANK1_GPIOB	    = 25
BANK1_OLATB	    = 26
*/

typedef enum MCP23x17_register_e {
	MCP23x17_IODIR,				/* IODIR: I/O DIRECTION REGISTER */
	MCP23x17_IPOL,				/* IPOL: INPUT POLARITY PORT REGISTER If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin */
	MCP23x17_GPINTEN,			/* INTERRUPT-ON-CHANGE CONTROL REGISTER */
	MCP23x17_DEFVAL,			/* DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE */
	MCP23x17_INTCON,			/* INTERRUPT CONTROL REGISTER */
	MCP23x17_IOCON,				/* I/OEXPANDER CONFIGURATION REGISTER */
	MCP23x17_GPPU,				/* GPPU: GPIO PULL-UP RESISTOR REGISTER */
	MCP23x17_INTF,				/* READ ONLY : INTF: INTERRUPT FLAG REGISTER (ADDR 0x07) */
	MCP23x17_INTCAP,			/* READ ONLY : INTCAP: INTERRUPT CAPTURED VALUE FOR PORT REGISTER */
	MCP23x17_GPIO,				/* Port adress */
	MCP23x17_OLAT,				/* OUTPUT LATCH REGISTER */
} MCP23x17_register_t;

typedef enum MCP23x17_version_e {
	MCP23x17_UNDEFINED,
	MCP23S17_SPI,
	MCP23017_I2C,
} MCP23x17_version_t;

typedef enum MCP23x17_BANK_e {
	MCP23x17_BANK0,
	MCP23x17_BANK1,
} MCP23x17_BANK_t;

typedef enum MCP23x17_port_e {
	MCP23x17_portA,
	MCP23x17_portB,
} MCP23x17_port_t;

typedef struct MCP23x17_port_config_s {
	uint8_t IODIR;
	uint8_t IPOL;
	uint8_t GPINTEN;
	uint8_t DEFVAL;
	uint8_t INTCON;
	uint8_t IOCON;
	uint8_t GPPU;
	uint8_t default_GPIO;
	uint8_t OLAT;
} MCP23x17_port_config_t;

typedef struct MCP23x17_config_s {
	MCP23x17_BANK_t 		BANK;
	MCP23x17_port_config_t 	portA;
	MCP23x17_port_config_t 	portB;
} MCP23x17_config_t;

typedef struct MCP23x17_com_s{
	MCP23x17_version_t 		protocole;
	I2C_HandleTypeDef* 		hi2c;
	SPI_HandleTypeDef*		hspix;
	osMutexId* 				mutex;
	uint8_t 				Opcode:3;
} MCP23x17_com_t;

typedef struct MCP23x17_s {
	MCP23x17_com_t			com;
	MCP23x17_config_t 		config;
} MCP23x17_t;

uint8_t MCP23x17_init(MCP23x17_t *driver);
uint8_t MCP23x17_read(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg);
uint8_t MCP23x17_readportA(MCP23x17_t *driver);
uint8_t MCP23x17_readportB(MCP23x17_t *driver);
void MCP23x17_write(MCP23x17_t *driver, MCP23x17_port_t port, MCP23x17_register_t reg, uint8_t value);
void MCP23x17_writeportA(MCP23x17_t *driver, uint8_t value);
void MCP23x17_writeportB(MCP23x17_t *driver, uint8_t value);

#endif /* _MCP23x17_H_ */
