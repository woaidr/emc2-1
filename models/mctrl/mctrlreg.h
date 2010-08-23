/***********************************************************************/
/* Project:    HW-SW SystemC Co-Simulation SoC Validation Platform     */
/*                                                                     */
/* File:       mctrlreg.h                                              */
/*             defines addresses, write masks, and default values of   */
/*             all registers of the MCTRL memory controller            */
/*                                                                     */
/* Modified on $Date$   */
/*          at $Revision$                                          */
/*                                                                     */
/* Principal:  European Space Agency                                   */
/* Author:     VLSI working group @ IDA @ TUBS                         */
/* Maintainer: Dennis Bode                                             */
/***********************************************************************/

#ifndef MCTRL_REGISTER_H
#define MCTRL_REGISTER_H


/***AMBA bus addresses (32 bit)***/

#define MCTRL_AMBA_BASE             (0xC0000000)

#define MCTRL_MCFG1                 (0x00)
#define MCTRL_MCFG2                 (0x04)
#define MCTRL_MCFG3                 (0x08)
#define MCTRL_MCFG4                 (0x0C)


/***register contents (config bit masks)***/

//memory configuration register 1
#define MCTRL_MCFG1_WRITE_MASK      (0x1EF80BFF)
#define MCTRL_MCFG1_IOBUSW          (0x18000000)
#define MCTRL_MCFG1_IBRDY           (0x04000000)
#define MCTRL_MCFG1_BEXCN           (0x02000000)
#define MCTRL_MCFG1_IO_WAITSTATES   (0x00F00000)
#define MCTRL_MCFG1_IOEN            (0x00080000)
#define MCTRL_MCFG1_PWEN            (0x00000800)
#define MCTRL_MCFG1_PROM_WIDTH      (0x00000300)
#define MCTRL_MCFG1_PROM_WRITE_WS   (0x000000F0)
#define MCTRL_MCFG1_PROM_READ_WS    (0x0000000F)

//memory configuration register 2
#define MCTRL_MCFG2_WRITE_MASK      (0xFFFD7EFF)
#define MCTRL_MCFG2_SDRF            (0x80000000)
#define MCTRL_MCFG2_TRP             (0x40000000)
#define MCTRL_MCFG2_SDRAM_TRFC      (0x38000000)
#define MCTRL_MCFG2_TCAS            (0x04000000)
#define MCTRL_MCFG2_SDRAM_BANKSZ    (0x03800000)
#define MCTRL_MCFG2_SDRAM_COSZ      (0x00600000)
#define MCTRL_MCFG2_SDRAM_CMD       (0x00180000)
#define MCTRL_MCFG2_D64             (0x00040000)
#define MCTRL_MCFG2_MS              (0x00010000)
#define MCTRL_MCFG2_SE              (0x00004000)
#define MCTRL_MCFG2_SI              (0x00002000)
#define MCTRL_MCFG2_RAM_BANK_SIZE   (0x00001E00)
#define MCTRL_MCFG2_RBRDY           (0x00000080)
#define MCTRL_MCFG2_RMW             (0x00000040)
#define MCTRL_MCFG2_RAM_WIDTH       (0x00000030)
#define MCTRL_MCFG2_RAM_WRITE_WS    (0x0000000C)
#define MCTRL_MCFG2_RAM_READ_WS     (0x00000003)

//memory configuration register 3
#define MCTRL_MCFG3_WRITE_MASK      (0x07FFF000)
#define MCTRL_MCFG3_SDRAM_RLD_VAL   (0x07FFF000)

//memory configuration register 4
#define MCTRL_MCFG4_WRITE_MASK      (0xE0FE007F)
#define MCTRL_MCFG4_ME              (0x80000000)
#define MCTRL_MCFG4_CE              (0x40000000)
#define MCTRL_MCFG4_EM              (0x20000000)
#define MCTRL_MCFG4_TXSR            (0x00F00000)
#define MCTRL_MCFG4_PMODE           (0x000E0000)
#define MCTRL_MCFG4_DS              (0x00000060)
#define MCTRL_MCFG4_TCSR            (0x00000018)
#define MCTRL_MCFG4_PASR            (0x00000007)


/***register default values***/

//memory configuration register 1
#define MCTRL_MCFG1_IOBUSW_DEFAULT          (0x10000000)
#define MCTRL_MCFG1_IBRDY_DEFAULT           (0x00000000)
#define MCTRL_MCFG1_BEXCN_DEFAULT           (0x00000000)
#define MCTRL_MCFG1_IO_WAITSTATES_DEFAULT   (0x00F00000)
#define MCTRL_MCFG1_IOEN_DEFAULT            (0x00080000)
#define MCTRL_MCFG1_PWEN_DEFAULT            (0x00000800)
#define MCTRL_MCFG1_PROM_WIDTH_DEFAULT      (0x00000200)
#define MCTRL_MCFG1_PROM_WRITE_WS_DEFAULT   (0x000000F0)
#define MCTRL_MCFG1_PROM_READ_WS_DEFAULT    (0x0000000F)
//                                        +
#define MCTRL_MCFG1_DEFAULT                 (0x10F80AFF)

//memory configuration register 2
#define MCTRL_MCFG2_SDRF_DEFAULT            (0x80000000)
#define MCTRL_MCFG2_TRP_DEFAULT             (0x40000000)
#define MCTRL_MCFG2_SDRAM_TRFC_DEFAULT      (0x38000000)
#define MCTRL_MCFG2_TCAS_DEFAULT            (0x04000000)
#define MCTRL_MCFG2_SDRAM_BANKSZ_DEFAULT    (0x03800000)
#define MCTRL_MCFG2_SDRAM_COSZ_DEFAULT      (0x00600000)
#define MCTRL_MCFG2_SDRAM_CMD_DEFAULT       (0x00000000)
#define MCTRL_MCFG2_D64_DEFAULT             (0x00000000)
#define MCTRL_MCFG2_MS_DEFAULT              (0x00000000)
#define MCTRL_MCFG2_SE_DEFAULT              (0x00004000)
#define MCTRL_MCFG2_SI_DEFAULT              (0x00000000)
#define MCTRL_MCFG2_RAM_BANK_SIZE_DEFAULT   (0x00001E00)
#define MCTRL_MCFG2_RBRDY_DEFAULT           (0x00000000)
#define MCTRL_MCFG2_RMW_DEFAULT             (0x00000000)
#define MCTRL_MCFG2_RAM_WIDTH_DEFAULT       (0x00000030)
#define MCTRL_MCFG2_RAM_WRITE_WS_DEFAULT    (0x0000000C)
#define MCTRL_MCFG2_RAM_READ_WS_DEFAULT     (0x00000003)
//                                        +
#define MCTRL_MCFG2_DEFAULT                 (0xFFE05E3F)

//memory configuration register 3
#define MCTRL_MCFG3_DEFAULT                 (0x07FFF000)

//memory configuration register 4
#define MCTRL_MCFG4_ME_DEFAULT              (0x00000000)
#define MCTRL_MCFG4_CE_DEFAULT              (0x00000000)
#define MCTRL_MCFG4_EM_DEFAULT              (0x00000000)
#define MCTRL_MCFG4_TXSR_DEFAULT            (0x00F00000)
#define MCTRL_MCFG4_PMODE_DEFAULT           (0x00000000)
#define MCTRL_MCFG4_DS_DEFAULT              (0x00000000)
#define MCTRL_MCFG4_TCSR_DEFAULT            (0x00000018)
#define MCTRL_MCFG4_PASR_DEFAULT            (0x00000007)
//                                        +
#define MCTRL_MCFG4_DEFAULT                 (0x00F0001F)

#endif
