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

#include "arm/intunit/main.hpp"
#include "arm/intunit/processor.hpp"
#include "arm/intunit/instructions.hpp"

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <signal.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem.hpp>
#include <modules/memory_lt.hpp>
#include <common/report.hpp>
#include <systemc.h>
#include <elfloader/elf_frontend.hpp>
#include <elfloader/exec_loader.hpp>
#include <stdexcept>
#include <debugger/gdb_stub.hpp>
#include <profiler/profiler.hpp>
#include <osemu/osemu.hpp>

#ifdef _WIN32
#pragma warning(disable : 4101)
#endif
#define WIN32_LEAN_AND_MEAN

std::string banner = std::string("\n\
\t\n\
\t          _/       _/_/_/      _/         _/\n\
\t        _/_/      _/    _/    _/_/      __/\n\
\t      _/  _/     _/    _/    _/ _/    _/_/\n\
\t    _/_/_/_/    _/_/_/      _/  _/  _/ _/\n\
\t  _/      _/   _/    _/    _/   _/_/  _/\n\
\t_/        _/  _/      _/  _/    _/   _/\n\
\t\n\
\t@brief    This file is part of the SoCRocket ARM ISS generated by TRAP.\n\
\t@details  This SystemC model was automatically generated by running\n\
\t          $ python ARMArch.py\n\
\t          Please refer to the ARM TRAP source files for documentation.\n\
\t@author   Lillian Tadros (TUDO ETIT IRF)\n\
\t@date     2014-12-01 tadros: Initial ARM Cortex A9 (ARMv7-A) version\n\
\t\n\
\n\n\t\t2014-2016\n\
\tTechnische Universitaet Dortmund\n\
\tInstitut fuer Roboterforschung\n\
\tProject Embedded Multi-Core Systems for Mixed Criticality\n\
\tApplications in Dynamic and Changeable Real-time Environments (EMC2)\n\
\n\n");

GDBStub<unsigned>* gdb_stub_ref = NULL;

void stop_simulation(int sig) {
  if (gdb_stub_ref != NULL && gdb_stub_ref->simulation_paused) {
    std::cerr << std::endl << "Cannot pause simulation, simulation already paused. Use the connected GDB debugger to control it." << std::endl << std::endl;
  } else {
    std::cerr << std::endl << "Interrupted the simulation." << std::endl <<
    std::endl;
    sc_stop();
    wait(SC_ZERO_TIME);
  }
} // stop_simulation()

/// ****************************************************************************

std::pair<unsigned, unsigned> get_cycle_range(
    const std::string& cycles_range,
    const std::string& application) {
  std::pair<unsigned, unsigned> decoded_range;
  std::size_t foundSep = cycles_range.find('-');
  if (foundSep == std::string::npos) {
    THROW_EXCEPTION("Invalid address range " << cycles_range << ", expected address range in the format start-end.");
  }
  std::string start = cycles_range.substr(0, foundSep);
  std::string end = cycles_range.substr(foundSep + 1);
  // First try decimal numbers, then hex, then check if a corresponding symbol
  // exists. If neither works return an error.
  try {
    decoded_range.first = boost::lexical_cast<unsigned>(start);
  }
  catch(...) {
    try {
      decoded_range.first = (unsigned)std::strtoul(start.c_str(), NULL, 0);
    }
    catch(...) {
      trap::ELFFrontend& elf_frontend = trap::ELFFrontend::get_instance(application);
      bool valid = true;
      decoded_range.first = elf_frontend.get_sym_addr(start, valid);
      if (!valid) {
        THROW_EXCEPTION("Start address range " << start << " does not specify a valid address or a valid symbol.");
      }
    }
  }
  try {
    decoded_range.second = boost::lexical_cast<unsigned>(end);
  }
  catch(...) {
    try {
      decoded_range.second = (unsigned)std::strtoul(end.c_str(), NULL, 0);
    }
    catch(...) {
      trap::ELFFrontend& elf_frontend = trap::ELFFrontend::get_instance(application);
      bool valid = true;
      decoded_range.second = elf_frontend.get_sym_addr(end, valid);
      if (!valid) {
        THROW_EXCEPTION("End address range " << end << " does not specify a valid address or a valid symbol.");
      }
    }
  }

  return decoded_range;
} // get_cycle_range()

/// ****************************************************************************

TLMIntrInitiator_1::TLMIntrInitiator_1(sc_core::sc_module_name _name) :
  sc_module(_name),
  init_socket("init_socket") {
  init_socket.register_nb_transport_bw(this, &TLMIntrInitiator_1::nb_transport_bw);
  init_socket.register_invalidate_direct_mem_ptr(this, &TLMIntrInitiator_1::invalidate_direct_mem_ptr);
} // TLMIntrInitiator_1()

/// ----------------------------------------------------------------------------

tlm::tlm_sync_enum TLMIntrInitiator_1::nb_transport_bw(
    int tag,
    tlm::tlm_generic_payload& payload,
    tlm::tlm_phase& phase,
    sc_core::sc_time& delay) {
  return tlm::TLM_COMPLETED;
} // nb_transport_bw()

/// ----------------------------------------------------------------------------

void TLMIntrInitiator_1::invalidate_direct_mem_ptr(
    int tag,
    sc_dt::uint64 start_range,
    sc_dt::uint64 end_range) {

} // invalidate_direct_mem_ptr()

/// ----------------------------------------------------------------------------


/// ****************************************************************************

/**
* @brief Main Processor Testbench
*
* Instantiates a processor and performs basic connections where required.
* TRAP-Gen debugging and profiling tools as instantiated as chosen by the
* command-line options.
*/
int sc_main(
    int argc,
    char** argv) {
  using namespace core_armcortexa9_lt;
  using namespace trap;

  std::cerr << banner << std::endl;

  boost::program_options::options_description desc("Processor simulator for ARMCortexA9", 120);
  desc.add_options()
  ("help,h", "produces the help message")
  ("debugger,d", "activates the use of the software debugger")
  ("profiler,p", boost::program_options::value<std::string>(),
  "activates the use of the software profiler, specifying the name of the output file")
  ("prof_range,g", boost::program_options::value<std::string>(),
  "specifies the range of addresses restricting the profiler instruction statistics")
  ("disable_fun_prof,n", "disables profiling statistics for the application routines")
  ("application,a", boost::program_options::value<std::string>(),
  "application to be executed on the simulator")
  ("disassembler,i", "prints the disassembly of the application")
  ("history,y", boost::program_options::value<std::string>(),
  "prints on the specified file the instruction history")
  ("arguments,r", boost::program_options::value<std::string>(),
  "command line arguments (if any) of the application being simulated - comma separated")
  ("environment,e", boost::program_options::value<std::string>(),
  "environmental variables (if any) visible to the application being simulated - comma separated")
  ("sysconf,s", boost::program_options::value<std::string>(),
  "configuration information (if any) visible to the application being simulated - comma separated")
  ;

  std::cerr << std::endl;

  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc,
    argv, desc), vm);
  }
  catch(boost::program_options::invalid_command_line_syntax& e) {
    std::cerr << "Cannot parse command line parameters." << std::endl << std::endl;
    std::cerr << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return -1;
  }
  catch(boost::program_options::validation_error& e) {
    std::cerr << "Cannot parse command line parameters." << std::endl << std::endl;
    std::cerr << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return -1;
  }
  catch(boost::program_options::error& e) {
    std::cerr << "Cannot parse command line parameters." << std::endl << std::endl;
    std::cerr << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return -1;
  }
  boost::program_options::notify(vm);

  // Check that the parameters are specified correctly.
  if (vm.count("help") != 0) {
    std::cout << desc << std::endl;
    return 0;
  }
  if (vm.count("application") == 0) {
    std::cerr << "Use the --application command line option to specify the application to be simulated." << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return -1;
  }

  // Proceed with the actual instantiation of the processor.
  CoreARMCortexA9LT processor("ARMCortexA9");

  std::cout << std::endl << "Loading the application and initializing the tools ..." << std::endl;
  // Load the executable code.
  boost::filesystem::path application_path = boost::filesystem::system_complete(boost::filesystem::path(vm["application"].as<std::string>()));
  if (!boost::filesystem::exists(application_path)) {
    std::cerr << "Application " << vm["application"].as<std::string>() << " does not exist." << std::endl;
    return -1;
  }
  ExecLoader loader(vm["application"].as<std::string>());
  // Copy the binary code into memory.
  unsigned char* program_data = loader.get_program_data();
  unsigned program_dim = loader.get_program_dim();
  unsigned program_data_start = loader.get_data_start();
  for (unsigned i = 0; i < program_dim; i++) {
    processor.data_memory.write_byte_dbg(program_data_start + i, program_data[i]);
  }
  if (vm.count("disassembler") != 0) {
    std::cout << "Entry Point: " << std::hex << std::showbase << loader.get_program_start()
    << std::endl << std::endl;
    for (unsigned i = 0; i < program_dim; i+= 4) {
      Instruction* cur_instr = processor.decode(processor.data_memory.read_word_dbg(loader.get_data_start()
      + i));
      std::cout << std::hex << std::noshowbase << std::setw(8) << std::setfill(' ')
      << program_data_start + i << ":   " << std::setw(8) << std::setfill('0')
      << processor.data_memory.read_word_dbg(program_data_start + i);
      if (cur_instr != NULL) {
        std::cout << "    " << cur_instr->get_mnemonic();
      }
      std::cout << std::endl;
    }
    return 0;
  }

  // Set the processor variables.
  processor.ENTRY_POINT = loader.get_program_start();
  processor.PROGRAM_LIMIT = program_dim + program_data_start;
  processor.PROGRAM_START = program_data_start;
  TLMIntrInitiator_1 IRQ_initiator("IRQ_initiator");
  IRQ_initiator.init_socket.bind(processor.IRQ_port.target_socket);
  TLMIntrInitiator_1 FIQ_initiator("FIQ_initiator");
  FIQ_initiator.init_socket.bind(processor.FIQ_port.target_socket);

  // Initialize the instruction history management. Note that both need to be
  // enabled if the debugger is being used and/or if history needs to be dumped
  // to an output file.
  if (vm.count("debugger") > 0) {
    processor.enable_history();
  }
  if (vm.count("history") > 0) {
#ifndef ENABLE_HISTORY
    std::cout << std::endl << "Unable to initialize instruction history as it has " << "been disabled at compilation time." << std::endl << std::endl;
#endif
    processor.enable_history(vm["history"].as<std::string>());
  }

  // Initialize the tools (e.g. debugger, os emulator, etc).
  OSEmulator<unsigned> os_emulator(processor.ABIIf);
  GDBStub<unsigned> gdb_stub(processor.ABIIf);
  Profiler<unsigned> profiler(processor.ABIIf, vm["application"].as<std::string>(),
  vm.count("disable_fun_prof") > 0);

  os_emulator.init_sys_calls(vm["application"].as<std::string>());
  std::vector<std::string> options;
  options.push_back(vm["application"].as<std::string>());
  if (vm.count("arguments") > 0) {
    // Parse the environment. The options are in the form 'option,option,...'
    std::string packedOpts = vm["arguments"].as<std::string>();
    while(packedOpts.size() > 0) {
      std::size_t foundComma = packedOpts.find(',');
      if (foundComma != std::string::npos) {
        options.push_back(packedOpts.substr(0, foundComma));
        packedOpts = packedOpts.substr(foundComma + 1);
      } else {
        options.push_back(packedOpts);
        break;
      }
    }
  }
  os_emulator.set_program_args(options);
  if (vm.count("environment") > 0) {
    // Parse the environment. The options are in the form 'option=value,option=value,...'
    std::string packedEnv = vm["environment"].as<std::string>();
    while(packedEnv.size() > 0) {
      std::size_t foundComma = packedEnv.find(',');
      std::string curEnv;
      if (foundComma != std::string::npos) {
        curEnv = packedEnv.substr(0, foundComma);
        packedEnv = packedEnv.substr(foundComma + 1);
      } else {
        curEnv = packedEnv;
        packedEnv = "";
      }
      // Split the current environment.
      std::size_t equalPos = curEnv.find('=');
      if (equalPos == std::string::npos) {
        std::cerr << "Invalid command line environment option: Expected 'var=value', got " << curEnv << '.' << std::endl;
        return -1;
      }
      os_emulator.set_env(curEnv.substr(0, equalPos), curEnv.substr(equalPos +
      1));
    }
  }
  if (vm.count("sysconf") > 0) {
    // Parse the environment. The options are in the form 'option=value,option=value,...'
    std::string packedEnv = vm["sysconf"].as<std::string>();
    while(packedEnv.size() > 0) {
      std::size_t foundComma = packedEnv.find(',');
      std::string curEnv;
      if (foundComma != std::string::npos) {
        curEnv = packedEnv.substr(0, foundComma);
        packedEnv = packedEnv.substr(foundComma + 1);
      } else {
        curEnv = packedEnv;
        packedEnv = "";
      }
      // Split the current environment.
      std::size_t equalPos = curEnv.find('=');
      if (equalPos == std::string::npos) {
        std::cerr << "Invalid command line sysconf option: Expected 'var=value', got " << curEnv << '.' << std::endl;
        return -1;
      }
      try {
        os_emulator.set_sysconf(curEnv.substr(0, equalPos), boost::lexical_cast<int>(curEnv.substr(equalPos
        + 1)));
      }
      catch(...) {
        std::cerr << "Invalid command line sysconf option: Expected 'var=value', got " << curEnv << '.' << std::endl;
        return -1;
      }
    }
  }
  processor.tool_manager.add_tool(os_emulator);
  if (vm.count("debugger") != 0) {
    processor.tool_manager.add_tool(gdb_stub);
    gdb_stub.initialize();
    processor.data_memory.set_debugger(&gdb_stub);
    gdb_stub_ref = &gdb_stub;
  }
  if (vm.count("profiler") != 0) {
    std::set<std::string> toIgnoreFuns = os_emulator.get_registered_functions();
    toIgnoreFuns.erase("main");
    profiler.add_ignored_functions(toIgnoreFuns);
    // Check if the profiler needs to be restricted to a specific cycle range.
    if (vm.count("prof_range") != 0) {
      std::pair<unsigned, unsigned> decodedProfRange = get_cycle_range(vm["prof_range"].as<std::string>(),
      vm["application"].as<std::string>());
      // The range is in the form start-end, where start and end can be both
      // integers (both normal and hex) or symbols of the binary file.
      profiler.set_profiling_range(decodedProfRange.first, decodedProfRange.second);
    }
    processor.tool_manager.add_tool(profiler);
  }

  // Register the signal handlers for the CTRL^C key combination.
  (void) signal(SIGINT, stop_simulation);
  (void) signal(SIGTERM, stop_simulation);
  (void) signal(10, stop_simulation);

  std::cout << "Initialized tools." << std::endl;

  // Start execution.
  boost::timer t;
  sc_start();
  double elapsed_sec = t.elapsed();
  if (vm.count("profiler") != 0) {
    profiler.print_csv_stats(vm["profiler"].as<std::string>());
  }
  std::cout << std::endl << "Elapsed real time:" << elapsed_sec << 's' <<
  std::endl;
  std::cout << "Executed Instructions: " << processor.num_instructions <<
  std::endl;
  std::cout << "Execution speed: " << (double)processor.num_instructions/(elapsed_sec*1e6)
  << "MIPS" << std::endl;
  std::cout << "Elapsed time: " << std::dec << processor.total_cycles << " cycles" << std::endl;
  std::cout << std::endl;

  return 0;
} // sc_main()

/// ****************************************************************************

