/***************************************************************************//**
*
*           _/       _/_/_/      _/         _/
*         _/_/      _/    _/    _/_/      __/
*       _/  _/     _/    _/    _/ _/    _/_/
*     _/_/_/_/    _/_/_/      _/  _/  _/ _/
*   _/      _/   _/    _/    _/   _/_/  _/
* _/        _/  _/      _/  _/    _/   _/
*
* @brief    This file is part of the SoCRocket ARM ISS generated by TRAP.
* @details  This SystemC model was automatically generated by running
*           $ python ARMArch.py
*           Please refer to the ARM TRAP source files for documentation.
* @author   Lillian Tadros (TUDO ETIT IRF)
* @date     2014-12-01 tadros: Initial ARM Cortex A9 (ARMv7-A) version
*
*
* (c) 2014-2016
* Technische Universitaet Dortmund
* Institut fuer Roboterforschung
* Project Embedded Multi-Core Systems for Mixed Criticality
* Applications in Dynamic and Changeable Real-time Environments (EMC2)
*
*
* This file is part of ARMCortexA9.
*
* ARMCortexA9 is free software; you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
* or see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#ifndef CORE_ARMCORTEXA9_FUNC_LT_REGISTERS_HPP
#define CORE_ARMCORTEXA9_FUNC_LT_REGISTERS_HPP


#include <iostream>
#include <modules/register.hpp>


#define FUNC_MODEL
#define LT_IF



/// Instruction Set States: ARM, Thumb, Jazelle, ThumbEE
enum class ISMODE: unsigned {
  ARM,        // 0x0
  THUMB,      // 0x1
  JAZELLE,    // 0x2
  THUMBEE     // 0x3
};




/// Execution Modes/Privilege Levels:
/// PL0: USR
/// PL1: SYS, IRQ, FIQ, UND, ABT, SVC, MON (only in Secure State)
/// PL2: HYP (only in Non-secure State and Virtualization Extensions)
enum class EXECMODE: unsigned {
  USR = 0x10,
  FIQ = 0x11,
  IRQ = 0x12,
  SVC = 0x13,
  MON = 0x16,
  ABT = 0x17,
  HYP = 0x1A,
  UND = 0x1B,
  SYS = 0x1F
};



#define CPSR R.cpsr

#define ID_PFR0 R.id_pfr0

#define ID_PFR1 R.id_pfr1

#define ID_DFR0 R.id_dfr0

#define ID_AFR0 R.id_afr0

#define ID_MMFR0 R.id_mmfr0

#define ID_MMFR1 R.id_mmfr1

#define ID_MMFR2 R.id_mmfr2

#define ID_MMFR3 R.id_mmfr3

#define ID_ISAR0 R.id_isar0

#define ID_ISAR1 R.id_isar1

#define ID_ISAR2 R.id_isar2

#define ID_ISAR3 R.id_isar3

#define ID_ISAR4 R.id_isar4

#define ID_ISAR5 R.id_isar5

#define MP_ID R.mp_id

#define SPSR R.spsr

#define RB R.rb

#define CPREGS R.cpregs

#define SPSR_IRQ R.spsr_irq

#define SPSR_FIQ R.spsr_fiq

#define SPSR_UND R.spsr_und

#define SPSR_ABT R.spsr_abt

#define SPSR_SVC R.spsr_svc

#define SPSR_HYP R.spsr_hyp

#define SPSR_MON R.spsr_mon

#define SP R.sp

#define LR R.lr

#define PC R.pc

#define SP_IRQ R.sp_irq

#define LR_IRQ R.lr_irq

#define R8_FIQ R.r8_fiq

#define R9_FIQ R.r9_fiq

#define R10_FIQ R.r10_fiq

#define R11_FIQ R.r11_fiq

#define R12_FIQ R.r12_fiq

#define SP_FIQ R.sp_fiq

#define LR_FIQ R.lr_fiq

#define SP_UND R.sp_und

#define LR_UND R.lr_und

#define SP_ABT R.sp_abt

#define LR_ABT R.lr_abt

#define SP_SVC R.sp_svc

#define LR_SVC R.lr_svc

#define SP_HYP R.sp_hyp

#define ELR_HYP R.elr_hyp

#define SP_MON R.sp_mon

#define LR_MON R.lr_mon

#define REGS R.regs

enum FIELDS_CPSR {
  CPSR_N,
  CPSR_Z,
  CPSR_C,
  CPSR_V,
  CPSR_Q,
  CPSR_IT10,
  CPSR_J,
  CPSR_GE,
  CPSR_IT72,
  CPSR_E,
  CPSR_A,
  CPSR_I,
  CPSR_F,
  CPSR_T,
  CPSR_M
}; // enum FIELDS_CPSR

enum FIELDS_ID_PFR0 {
  ID_PFR0_W7,
  ID_PFR0_W6,
  ID_PFR0_W5,
  ID_PFR0_W4,
  ID_PFR0_W3,
  ID_PFR0_W2,
  ID_PFR0_W1,
  ID_PFR0_W0
}; // enum FIELDS_ID_PFR0

enum FIELDS_ID_PFR1 {
  ID_PFR1_W7,
  ID_PFR1_W6,
  ID_PFR1_W5,
  ID_PFR1_W4,
  ID_PFR1_W3,
  ID_PFR1_W2,
  ID_PFR1_W1,
  ID_PFR1_W0
}; // enum FIELDS_ID_PFR1

enum FIELDS_ID_DFR0 {
  ID_DFR0_W7,
  ID_DFR0_W6,
  ID_DFR0_W5,
  ID_DFR0_W4,
  ID_DFR0_W3,
  ID_DFR0_W2,
  ID_DFR0_W1,
  ID_DFR0_W0
}; // enum FIELDS_ID_DFR0

enum FIELDS_ID_AFR0 {
  ID_AFR0_W7,
  ID_AFR0_W6,
  ID_AFR0_W5,
  ID_AFR0_W4,
  ID_AFR0_W3,
  ID_AFR0_W2,
  ID_AFR0_W1,
  ID_AFR0_W0
}; // enum FIELDS_ID_AFR0

enum FIELDS_ID_MMFR0 {
  ID_MMFR0_W7,
  ID_MMFR0_W6,
  ID_MMFR0_W5,
  ID_MMFR0_W4,
  ID_MMFR0_W3,
  ID_MMFR0_W2,
  ID_MMFR0_W1,
  ID_MMFR0_W0
}; // enum FIELDS_ID_MMFR0

enum FIELDS_ID_MMFR1 {
  ID_MMFR1_W7,
  ID_MMFR1_W6,
  ID_MMFR1_W5,
  ID_MMFR1_W4,
  ID_MMFR1_W3,
  ID_MMFR1_W2,
  ID_MMFR1_W1,
  ID_MMFR1_W0
}; // enum FIELDS_ID_MMFR1

enum FIELDS_ID_MMFR2 {
  ID_MMFR2_W7,
  ID_MMFR2_W6,
  ID_MMFR2_W5,
  ID_MMFR2_W4,
  ID_MMFR2_W3,
  ID_MMFR2_W2,
  ID_MMFR2_W1,
  ID_MMFR2_W0
}; // enum FIELDS_ID_MMFR2

enum FIELDS_ID_MMFR3 {
  ID_MMFR3_W7,
  ID_MMFR3_W6,
  ID_MMFR3_W5,
  ID_MMFR3_W4,
  ID_MMFR3_W3,
  ID_MMFR3_W2,
  ID_MMFR3_W1,
  ID_MMFR3_W0
}; // enum FIELDS_ID_MMFR3

enum FIELDS_ID_ISAR0 {
  ID_ISAR0_W7,
  ID_ISAR0_W6,
  ID_ISAR0_W5,
  ID_ISAR0_W4,
  ID_ISAR0_W3,
  ID_ISAR0_W2,
  ID_ISAR0_W1,
  ID_ISAR0_W0
}; // enum FIELDS_ID_ISAR0

enum FIELDS_ID_ISAR1 {
  ID_ISAR1_W7,
  ID_ISAR1_W6,
  ID_ISAR1_W5,
  ID_ISAR1_W4,
  ID_ISAR1_W3,
  ID_ISAR1_W2,
  ID_ISAR1_W1,
  ID_ISAR1_W0
}; // enum FIELDS_ID_ISAR1

enum FIELDS_ID_ISAR2 {
  ID_ISAR2_W7,
  ID_ISAR2_W6,
  ID_ISAR2_W5,
  ID_ISAR2_W4,
  ID_ISAR2_W3,
  ID_ISAR2_W2,
  ID_ISAR2_W1,
  ID_ISAR2_W0
}; // enum FIELDS_ID_ISAR2

enum FIELDS_ID_ISAR3 {
  ID_ISAR3_W7,
  ID_ISAR3_W6,
  ID_ISAR3_W5,
  ID_ISAR3_W4,
  ID_ISAR3_W3,
  ID_ISAR3_W2,
  ID_ISAR3_W1,
  ID_ISAR3_W0
}; // enum FIELDS_ID_ISAR3

enum FIELDS_ID_ISAR4 {
  ID_ISAR4_W7,
  ID_ISAR4_W6,
  ID_ISAR4_W5,
  ID_ISAR4_W4,
  ID_ISAR4_W3,
  ID_ISAR4_W2,
  ID_ISAR4_W1,
  ID_ISAR4_W0
}; // enum FIELDS_ID_ISAR4

enum FIELDS_ID_ISAR5 {
  ID_ISAR5_W7,
  ID_ISAR5_W6,
  ID_ISAR5_W5,
  ID_ISAR5_W4,
  ID_ISAR5_W3,
  ID_ISAR5_W2,
  ID_ISAR5_W1,
  ID_ISAR5_W0
}; // enum FIELDS_ID_ISAR5

enum FIELDS_SPSR {
  SPSR_N,
  SPSR_Z,
  SPSR_C,
  SPSR_V,
  SPSR_Q,
  SPSR_IT10,
  SPSR_J,
  SPSR_GE,
  SPSR_IT72,
  SPSR_E,
  SPSR_A,
  SPSR_I,
  SPSR_F,
  SPSR_T,
  SPSR_M
}; // enum FIELDS_SPSR

namespace core_armcortexa9_lt {

  /**
  * @brief Register Container Class
  *
  * Contains all registers and register banks of the processor as member variables.
  * It serves for encapsulating the instantiation details (defining fields, etc)
  * away from the processor. It also simplifies passing the registers to the
  * instructions, instead of passing each register individually.
  */
  class Registers {
    /// @name Constructors and Destructors
    /// @{

    public:
    Registers(
        unsigned MPROC_ID, unsigned ENTRY_POINT);

    /// @} Constructors and Destructors
    /// ------------------------------------------------------------------------
    /// @name Methods
    /// @{

    public:
    void reset();
    bool write(const unsigned& data);
    bool write_dbg(const unsigned& data);
    bool write_force(const unsigned& data);
    void execute_callbacks(
        const scireg_ns::scireg_callback_type& type, const uint32_t& offset = 0,
        const uint32_t& size = 0);
    std::ostream& operator <<(std::ostream& os) const;

    /// @} Methods
    /// ------------------------------------------------------------------------
    /// @name Data
    /// @{

    public:
    trap::Register<unsigned> cpsr;
    trap::Register<unsigned> id_pfr0;
    trap::Register<unsigned> id_pfr1;
    trap::Register<unsigned> id_dfr0;
    trap::Register<unsigned> id_afr0;
    trap::Register<unsigned> id_mmfr0;
    trap::Register<unsigned> id_mmfr1;
    trap::Register<unsigned> id_mmfr2;
    trap::Register<unsigned> id_mmfr3;
    trap::Register<unsigned> id_isar0;
    trap::Register<unsigned> id_isar1;
    trap::Register<unsigned> id_isar2;
    trap::Register<unsigned> id_isar3;
    trap::Register<unsigned> id_isar4;
    trap::Register<unsigned> id_isar5;
    trap::Register<unsigned> mp_id;
    trap::Register<unsigned> spsr[7];
    trap::Register<unsigned> rb[35];
    trap::Register<unsigned> cpregs[16];
    trap::RegisterAlias<unsigned> spsr_irq;
    trap::RegisterAlias<unsigned> spsr_fiq;
    trap::RegisterAlias<unsigned> spsr_und;
    trap::RegisterAlias<unsigned> spsr_abt;
    trap::RegisterAlias<unsigned> spsr_svc;
    trap::RegisterAlias<unsigned> spsr_hyp;
    trap::RegisterAlias<unsigned> spsr_mon;
    trap::RegisterAlias<unsigned> sp;
    trap::RegisterAlias<unsigned> lr;
    trap::RegisterAlias<unsigned> pc;
    trap::RegisterAlias<unsigned> sp_irq;
    trap::RegisterAlias<unsigned> lr_irq;
    trap::RegisterAlias<unsigned> r8_fiq;
    trap::RegisterAlias<unsigned> r9_fiq;
    trap::RegisterAlias<unsigned> r10_fiq;
    trap::RegisterAlias<unsigned> r11_fiq;
    trap::RegisterAlias<unsigned> r12_fiq;
    trap::RegisterAlias<unsigned> sp_fiq;
    trap::RegisterAlias<unsigned> lr_fiq;
    trap::RegisterAlias<unsigned> sp_und;
    trap::RegisterAlias<unsigned> lr_und;
    trap::RegisterAlias<unsigned> sp_abt;
    trap::RegisterAlias<unsigned> lr_abt;
    trap::RegisterAlias<unsigned> sp_svc;
    trap::RegisterAlias<unsigned> lr_svc;
    trap::RegisterAlias<unsigned> sp_hyp;
    trap::RegisterAlias<unsigned> elr_hyp;
    trap::RegisterAlias<unsigned> sp_mon;
    trap::RegisterAlias<unsigned> lr_mon;
    trap::RegisterAlias<unsigned> regs[16];

    /// @} Data
    /// ------------------------------------------------------------------------

  }; // class Registers

  /// **************************************************************************

} // namespace core_armcortexa9_lt

#endif // CORE_ARMCORTEXA9_FUNC_LT_REGISTERS_HPP
