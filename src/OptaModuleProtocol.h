/* -------------------------------------------------------------------------- */
/* FILE NAME:   OptaModuleProtocol.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240508
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTAMODULEPROTOCOL
#define OPTAMODULEPROTOCOL

#include "OptaBlueProtocol.h"

/* msg reset controller (SET) */
#define ARG_CONTROLLER_RESET 0x01
#define LEN_CONTROLLER_RESET 0x01
#define CONTROLLER_RESET_CODE 0x56
#define CONTROLLER_RESET_LEN (BP_HEADER_DIM + LEN_CONTROLLER_RESET)
#define CONTROLLER_RESET_LEN_CRC (CONTROLLER_RESET_LEN + 1)
/* msg set address (SET) */
#define ARG_ADDRESS 0x02
#define LEN_ADDRESS 0x01
#define MSG_SET_ADDRESS_LEN (BP_HEADER_DIM + LEN_ADDRESS)
#define MSG_SET_ADDRESS_LEN_CRC (MSG_SET_ADDRESS_LEN + 1)
/* msg get address and type (GET) */
#define ARG_ADDRESS_AND_TYPE 0x03
#define LEN_ADDRESS_AND_TYPE 0x00
#define MSG_GET_ADDRESS_AND_TYPE_LEN (BP_HEADER_DIM + LEN_ADDRESS_AND_TYPE)
#define MSG_GET_ADDRESS_AND_TYPE_LEN_CRC (MSG_GET_ADDRESS_AND_TYPE_LEN + 1)
/* answer get address and type */
#define ANS_ARG_ADDRESS_AND_TYPE ARG_ADDRESS_AND_TYPE
#define ANS_LEN_ADDRESS_AND_TYPE 0x02
#define ANS_MSG_ADDRESS_AND_TYPE_LEN (BP_HEADER_DIM + ANS_LEN_ADDRESS_AND_TYPE)
#define ANS_MSG_ADDRESS_AND_TYPE_LEN_CRC (ANS_MSG_ADDRESS_AND_TYPE_LEN + 1)

#define ARG_GET_PRODUCT_TYPE 0x25
#define LEN_GET_PRODUCT_TYPE 0
#define GET_PRODUCT_TYPE_LEN (BP_HEADER_DIM + LEN_GET_PRODUCT_TYPE)

#define ANS_ARG_GET_PRODUCT_TYPE ARG_GET_PRODUCT_TYPE
#define ANS_LEN_GET_PRODUCT_TYPE 33
#define ANS_GET_PRODUCT_SIZE_POS BP_HEADER_DIM
#define LEN_ANS_GET_PRODUCT_TYPE (BP_HEADER_DIM + ANS_LEN_GET_PRODUCT_TYPE)
#define LEN_ANS_GET_PRODUCT_TYP_CRC (LEN_ANS_GET_PRODUCT_TYPE + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_GET_PRODUCT_LEN LEN_ANS_GET_PRODUCT_TYP_CRC
#else
#define CTRL_ANS_MSG_GET_PRODUCT_LEN LEN_ANS_GET_PRODUCT_TYPE
#endif


#ifdef USE_CONFIRM_RX_MESSAGE
#define ARG_CONFIRM_ADDRESS_RX 0x26
#define LEN_CONFIRM_ADDRESS_RX 2
#define CONFIRM_ADDRESS_FIRST_POS (BP_HEADER_DIM + 0)
#define CONFIRM_ADDRESS_SECOND_POS (BP_HEADER_DIM + 1)
#define CONFIRM_ADDRESS_FIRST_VALUE 0xC9
#define CONFIRM_ADDRESS_SECOND_VALUE 0xB1
#define CONFIRM_ADDRESS_RX_LEN  (BP_HEADER_DIM + LEN_CONFIRM_ADDRESS_RX)
#endif

/* ######################## */
/* VERSION related messages */
/* ######################## */

/* This message is common to all expansions */
#define ARG_GET_VERSION 0x16
#define LEN_GET_VERSION 0x00
#define GET_VERSION_LEN (BP_HEADER_DIM + LEN_GET_VERSION)
#define GET_VERSION_LEN_CRC (GET_VERSION_LEN + 1)

#define ANS_ARG_GET_VERSION ARG_GET_VERSION
#define ANS_LEN_GET_VERSION 3
#define GET_VERSION_MAJOR_POS (BP_HEADER_DIM)
#define GET_VERSION_MINOR_POS (BP_HEADER_DIM + 1)
#define GET_VERSION_RELEASE_POS (BP_HEADER_DIM + 2)
#define ANS_GET_VERSION_LEN (BP_HEADER_DIM + ANS_LEN_GET_VERSION)
#define ANS_GET_VERSION_LEN_CRC (ANS_GET_VERSION_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_GET_VERSION_LEN ANS_GET_VERSION_LEN_CRC
#else
#define CTRL_ANS_GET_VERSION_LEN ANS_GET_VERSION_LEN
#endif

/* ######################## */
/* REBOOT related messages  */
/* ######################## */

#define ARG_REBOOT 0xF3
#define LEN_REBOOT 0x02
#define REBOOT_1_POS (BP_HEADER_DIM)
#define REBOOT_2_POS (BP_HEADER_DIM + 1)
#define REBOOT_1_VALUE 0x58
#define REBOOT_2_VALUE 0x32
#define REBOOT_LEN (BP_HEADER_DIM + LEN_REBOOT)
#define REBOOT_LEN_CRC (REBOOT_LEN + 1)

#define ANS_ARG_REBOOT ARG_REBOOT
#define ANS_LEN_REBOOT 0x01
#define ANS_REBOOT_CODE_POS (BP_HEADER_DIM)
#define ANS_REBOOT_CODE 0x74
#define ANS_REBOOT_LEN (BP_HEADER_DIM + ANS_LEN_REBOOT)
#define ANS_REBOOT_LEN_CRC (ANS_REBOOT_LEN + 1)

/* ######################## */
/* FLASH related messages  */
/* ######################## */

#define ARG_SAVE_IN_DATA_FLASH 0x17
#define LEN_SAVE_IN_DATA_FLASH 35
#define SAVE_ADDRESS_1_POS (BP_HEADER_DIM)
#define SAVE_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define SAVE_DIMENSION_POS (BP_HEADER_DIM + 2)
#define SAVE_DATA_INIT_POS (BP_HEADER_DIM + 3)
#define SAVE_DATA_LEN (BP_HEADER_DIM + LEN_SAVE_IN_DATA_FLASH)
#define SAVE_DATA_LEN_CRC (SAVE_DATA_LEN + CRC)

#define ARG_GET_DATA_FROM_FLASH 0x18
#define LEN_GET_DATA_FROM_FLASH 3
#define READ_ADDRESS_1_POS (BP_HEADER_DIM)
#define READ_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define READ_DATA_DIM_POS (BP_HEADER_DIM + 2)
#define READ_DATA_LEN (BP_HEADER_DIM + LEN_GET_DATA_FROM_FLASH)
#define READ_DATA_LEN_CRC (READ_DATA_LEN + 1)

#define ANS_ARG_GET_DATA_FROM_FLASH 0x19
#define ANS_LEN_GET_DATA_FROM_FLASH 35
#define ANS_GET_DATA_ADDRESS_1_POS (BP_HEADER_DIM)
#define ANS_GET_DATA_ADDRESS_2_POS (BP_HEADER_DIM + 1)
#define ANS_GET_DATA_DIMENSION_POS (BP_HEADER_DIM + 2)
#define ANS_GET_DATA_DATA_INIT_POS (BP_HEADER_DIM + 3)
#define ANS_GET_DATA_LEN (BP_HEADER_DIM + ANS_LEN_GET_DATA_FROM_FLASH)
#define ANS_GET_DATA_LEN_CRC (ANS_GET_DATA_LEN + 1)
#ifdef BP_USE_CRC
#define CTRL_ANS_MSG_GET_FLASH_LEN ANS_GET_DATA_LEN_CRC
#else
#define CTRL_ANS_MSG_GET_FLASH_LEN ANS_GET_DATA_LEN
#endif
#endif
