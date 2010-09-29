// ***********************************************************************
// * Project:    HW-SW SystemC Co-Simulation SoC Validation Platform     *
// *                                                                     *
// * File:       localram.h - Class definition of a local RAM that       *
// *             can be attached to the icache and dcache controllers.   *
// *             The LocalRAM enables fast 0-waitstate access            *
// *             to instructions or data.                                *
// *                                                                     *
// * Modified on $Date$   *
// *          at $Revision$                                           *
// *                                                                     *
// * Principal:  European Space Agency                                   *
// * Author:     VLSI working group @ IDA @ TUBS                         *
// * Maintainer: Thomas Schuster                                         *
// ***********************************************************************

#ifndef __LOCALRAM_H__
#define __LOCALRAM_H__

#include "defines.h"

// Local scratchpad ram can optionally be attached to both instruction and data cache controllers.
// The scratch pad ram provides fast 0-waitstates ram memories for instructions and data.
// The ram can be between 1 - 512 kbyte, and mapped to any 16 Mbyte block in the address space.
// Accesses the the scratchpad ram are not cached, and will not appear on the AHB bus. 
// The scratch pads do not appear on the AHB bus and can only be read or written by the
// processor. The instruction scratchpad must be initialized by software (through store instr.)
// before it can be used. The default address for the instruction ram is 0x83000000,
// and for the data ram 0x8f000000.
// ! Local scratch pad ram can only be enabled when the MMU is disabled !
// ! Address decoding and checking is done in class mmu_cache !

/// @brief Local Scratchpad RAM
class localram : public sc_core::sc_module {

 public:

  // external interface functions:
  // -----------------------------
  /// read from scratchpad
  void read(unsigned int address, unsigned char *data, unsigned int len, sc_core::sc_time *t, unsigned int *debug);
  /// write to scratchpad
  void write(unsigned int address, unsigned char *data, unsigned int len, sc_core::sc_time *t, unsigned int *debug);

  // constructor
  /// @brief Constructor of scratchpad RAM implementation (localram)
  /// @param name    SystemC module name
  /// @param lrsize  Local ram size. Size in kbyte = 2^lrsize (like top-level template)
  /// @param lrstart Local ram start address. The 8 most significant bits of the address.
  localram(sc_core::sc_module_name name, unsigned int lrsize, unsigned int lrstart);

  /// destructor
  ~localram();

  // the actual local ram
  t_cache_data * scratchpad;

  // helpers
  // -------
  t_cache_data m_default_entry;

  // local ram parameters
  // --------------------
  /// size of the local ram in words
  unsigned int m_lrsize;
  /// start address of the local ram
  unsigned int m_lrstart;

};

#endif // __LOCALRAM_H__
