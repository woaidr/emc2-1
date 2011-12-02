// ********************************************************************
// Copyright 2010, Institute of Computer and Network Engineering,
//                 TU-Braunschweig
// All rights reserved
// Any reproduction, use, distribution or disclosure of this program,
// without the express, prior written consent of the authors is
// strictly prohibited.
//
// University of Technology Braunschweig
// Institute of Computer and Network Engineering
// Hans-Sommer-Str. 66
// 38118 Braunschweig, Germany
//
// ESA SPECIAL LICENSE
//
// This program may be freely used, copied, modified, and redistributed
// by the European Space Agency for the Agency's own requirements.
//
// The program is provided "as is", there is no warranty that
// the program is correct or suitable for any purpose,
// neither implicit nor explicit. The program and the information in it
// contained do not necessarily reflect the policy of the
// European Space Agency or of TU-Braunschweig.
// ********************************************************************
// Title:      ahbctrl_test.cpp
//
// ScssId:
//
// Origin:     HW-SW SystemC Co-Simulation SoC Validation Platform
//
// Purpose:    This class contains functions common to all AHBCTRL
//             tests. It implements the blocking and non-blocking
//             TLM-AHB master interface and respective read/write 
//             routines.
//             
//
// Modified on $Date$
//          at $Revision$
//          by $Author$
//
// Principal:  European Space Agency
// Author:     VLSI working group @ IDA @ TUBS
// Maintainer: Thomas Schuster
// Reviewed:
// ********************************************************************

#include "ahbctrl_test.h"

// Constructor
ahbctrl_test::ahbctrl_test(sc_core::sc_module_name name,
			   unsigned int haddr, // haddr for random instr. generation
			   unsigned int hmask, // hmask for random instr. generation
			   unsigned int master_id, // id of the bus master
			   sc_core::sc_time inter, // interval of random instructions (waiting period)
			   amba::amba_layer_ids abstractionLayer) : sc_module(name),
  AHBDevice(
      master_id, // bus id (hindex)
      0x01,      // vendor: Gaisler Research (Fake the Leon)
      0x003,      // device
      0,
      0,
      0,
      0,
      0,
      0),
  ahb("ahb", amba::amba_AHB, abstractionLayer, false),
  snoop(&ahbctrl_test::snoopingCallBack,"SNOOP"),
  m_haddr(haddr),
  m_hmask(hmask),
  m_master_id(master_id),
  m_inter(inter),
  m_abstractionLayer(abstractionLayer),
  mResponsePEQ("ResponsePEQ"), 
  mDataPEQ("DataPEQ"), 
  mEndTransactionPEQ("EndTransactionPEQ") {

  // Calculate address bound for random instruction generation
  // from haddr/hmask
  m_addr_range_lower_bound = (m_haddr & m_hmask) << 20;
  m_addr_range_upper_bound = m_addr_range_lower_bound + (((~m_hmask & 0xfff) + 1) << 20);

  // Sanity check range
  assert(m_addr_range_upper_bound > m_addr_range_lower_bound);

  v::info << this->name() << "***************************************************       " << v::endl;
  v::info << this->name() << "* Testbench master " << m_master_id << " configured to    " << v::endl;
  v::info << this->name() << "* generate instructions with following parameters:        " << v::endl;
  v::info << this->name() << "* Lower address bound: " << hex << m_addr_range_lower_bound << v::endl;
  v::info << this->name() << "* Upper address bound: " << hex << m_addr_range_upper_bound << v::endl;
  v::info << this->name() << "* Interval: " << m_inter << v::endl;
  v::info << this->name() << "***************************************************" << v::endl;


  // For AT abstraction layer
  if (m_abstractionLayer == amba::amba_AT) {

    // Register non-blocking backward transport
    ahb.register_nb_transport_bw(this, &ahbctrl_test::nb_transport_bw);

    // Register threads for response processing
    SC_THREAD(ResponseThread);
    SC_THREAD(DataThread);
    SC_THREAD(cleanUP);

  }

  // Initialize offset to payload pointer
  tc = 0;
}

// Delayed release of transactions
void ahbctrl_test::cleanUP() {

  tlm::tlm_generic_payload * trans;

  while(1) {

    wait(mEndTransactionPEQ.get_event());

    while((trans = mEndTransactionPEQ.get_next_transaction())) {

      v::debug << name() << "Release transaction: " << hex << trans << v::endl;

      ahb.release_transaction(trans);

    }
  }
}

// TLM non-blocking backward transport function
tlm::tlm_sync_enum ahbctrl_test::nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay) {

  v::debug << name() << "nb_transport_bw received transaction " << hex << &trans << " with phase " << phase << v::endl;

  // The slave has sent END_REQ
  if (phase == tlm::END_REQ) {

    // Usually the slave would send TLM_UPDATED/END_REQ
    // on the return path. In case END_REQ comes via backward path,
    // notify ahbwrite that request phase is over.
    mEndRequestEvent.notify();

    // For writes there will be no BEGIN_RESP.
    // Notify response thread to send BEGIN_DATA.
    if (trans.get_command() == tlm::TLM_WRITE_COMMAND) {

      mDataPEQ.notify(trans);

    }

    // Reset delay
    delay = SC_ZERO_TIME;

  // New response - goes into response PEQ
  } else if (phase == tlm::BEGIN_RESP) {

    // Put new response into ResponsePEQ
    mResponsePEQ.notify(trans, delay);

  // Data phase completed
  } else if (phase == amba::END_DATA) {

    // Release transaction
    mEndTransactionPEQ.notify(trans, delay);

    // Reset delay
    delay = SC_ZERO_TIME;

  } else {

    v::error << name() << "Invalid phase in call to nb_transport_bw!" << v::endl;
    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);

  }

  return tlm::TLM_ACCEPTED;
  
}

unsigned char * ahbctrl_test::get_datap() {

  return(tmp+tc);

}

unsigned char * ahbctrl_test::get_datap_word(unsigned int value) {

  memset(tmp+tc,0,4);
  
  // Assume LE host simulating BE master
  tmp[tc+3] = value & 0xff;
  tmp[tc+2] = (value >> 8) & 0xff;
  tmp[tc+1] = (value >> 16) & 0xff;
  tmp[tc+0] = (value >> 24) & 0xff;

  return(tmp+tc);

}

unsigned char * ahbctrl_test::get_datap_short(unsigned int value) {

  memset(tmp+tc,0,2);

  // Assume LE host simulating BE master
  tmp[tc+1] = value & 0xff; 
  tmp[tc+0] = (value >> 8) & 0xff;

  return(tmp+tc);

}

unsigned char * ahbctrl_test::get_datap_byte(unsigned int value) {
 
  memset(tmp+tc,0,1);

  tmp[tc] = value;

  return(tmp+tc);

}


// Increment test counter (tc)
// tc is offset for data, ref and debug arrays
void ahbctrl_test::inc_tptr() {

  tc = (tc + 4) % 1024;
  
}

void ahbctrl_test::inc_tptr(unsigned int value) {

  tc = (tc + value) % 1024;

}


// Function for read access to AHB master socket
void ahbctrl_test::ahbread(unsigned int addr, unsigned char * data, unsigned int length, unsigned int burst_size) {

  tlm::tlm_phase phase;
  tlm::tlm_sync_enum status;
  sc_core::sc_time delay;

  // Allocate new transaction
  tlm::tlm_generic_payload *trans = ahb.get_transaction();

  v::debug << name() << "Allocate new transaction: " << hex << trans << v::endl;

  // Initialize transaction
  trans->set_command(tlm::TLM_READ_COMMAND);
  trans->set_address(addr);
  trans->set_data_length(length);
  trans->set_data_ptr(data);
  trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  // Set burst size extension
  amba::amba_burst_size* size_ext;
  ahb.validate_extension<amba::amba_burst_size> (*trans);
  ahb.get_extension<amba::amba_burst_size> (size_ext, *trans);
  size_ext->value = (length < 4) ? length : 4;  

  // Set master id extension
  amba::amba_id* m_id;
  ahb.validate_extension<amba::amba_id> (*trans);
  ahb.get_extension<amba::amba_id> (m_id, *trans);
  m_id->value = m_master_id;

  // Set transfer type extension
  amba::amba_trans_type * trans_ext;
  ahb.validate_extension<amba::amba_trans_type> (*trans);
  ahb.get_extension<amba::amba_trans_type> (trans_ext, *trans);
  trans_ext->value = amba::NON_SEQUENTIAL;

  // Init delay
  delay = SC_ZERO_TIME;

  if (m_abstractionLayer == amba::amba_LT) {

    // Blocking transport
    ahb->b_transport(*trans, delay);

  } else {

    // Initial phase for AT
    phase = tlm::BEGIN_REQ;

    v::debug << name() << "Transaction " << hex << trans << " call to nb_transport_fw with phase " << phase << v::endl;

    // Non-blocking transport
    status = ahb->nb_transport_fw(*trans, phase, delay);

    switch (status) {

      case tlm::TLM_ACCEPTED:
      case tlm::TLM_UPDATED:

	if (phase == tlm::BEGIN_REQ) {

	  // The slave returned TLM_ACCEPTED.
	  // Wait until END_REQ before giving control
	  // to the user (for sending next transaction).

	  wait(mEndRequestEvent);
	  
	} else if (phase == tlm::END_REQ) {

	  // The slave returned TLM_UPDATED with END_REQ.
	  // Wait until BEGIN_RESP comes in on the backward path
	  // and then return control to the user (for putting next
	  // transaction into pipeline).

	  wait(mEndRequestEvent);
	  
	} else if (phase == tlm::BEGIN_RESP) {

	  // Slave directly jumped to BEGIN_RESP.
	  // Notify the response thread and return control to user.
	  mResponsePEQ.notify(*trans, delay);

	} else {

	  // Forbidden phase
	  v::error << name() << "Invalid phase in return path (from call to nb_transport_fw)!" << v::endl;
	  trans->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);

	}

	break;

      case tlm::TLM_COMPLETED:

	// Slave directly jumps to TLM_COMPLETED

	break;
      
      default:

	v::error << name() << "Invalid return value from call to nb_transport_fw!" << v::endl;
	trans->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);

    }
  }
}


// Function for write access to AHB master socket
void ahbctrl_test::ahbwrite(unsigned int addr, unsigned char * data, unsigned int length, unsigned int burst_size) {

  tlm::tlm_phase phase;
  tlm::tlm_sync_enum status;
  sc_core::sc_time delay;

  // Allocate new transaction
  tlm::tlm_generic_payload *trans = ahb.get_transaction();

  v::debug << name() << "Allocate new transaction " << hex << trans << v::endl;

  // Initialize transaction
  trans->set_command(tlm::TLM_WRITE_COMMAND);
  trans->set_address(addr);
  trans->set_data_length(length);
  trans->set_data_ptr(data);
  trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

  // Set burst size extension
  amba::amba_burst_size* size_ext;
  ahb.validate_extension<amba::amba_burst_size> (*trans);
  ahb.get_extension<amba::amba_burst_size> (size_ext, *trans);
  size_ext->value = (length < 4)? length : 4;

  // Set master id extension
  amba::amba_id* m_id;
  ahb.validate_extension<amba::amba_id> (*trans);
  ahb.get_extension<amba::amba_id> (m_id, *trans);
  m_id->value = m_master_id;

  // Set transfer type extension
  amba::amba_trans_type * trans_ext;
  ahb.validate_extension<amba::amba_trans_type> (*trans);
  ahb.get_extension<amba::amba_trans_type> (trans_ext, *trans);
  trans_ext->value = amba::NON_SEQUENTIAL;

  // Initialize delay
  delay = SC_ZERO_TIME;

  if (m_abstractionLayer == amba::amba_LT) {

    // Blocking transport
    ahb->b_transport(*trans, delay);

  } else {

    // Initial phase for AT
    phase = tlm::BEGIN_REQ;

    v::debug << name() << "Transaction " << hex << trans << " call to nb_transport_fw with phase " << phase << v::endl;

    // Non-blocking transport
    status = ahb->nb_transport_fw(*trans, phase, delay);

    switch (status) {

      case tlm::TLM_ACCEPTED:
      case tlm::TLM_UPDATED:

	if (phase == tlm::BEGIN_REQ) {

	  // The slave returned TLM_ACCEPTED
	  // Wait until END_REQ comes in on backward path
	  // before starting DATA phase.
	  wait(mEndRequestEvent);

	} else if (phase == tlm::END_REQ) {

	  // The slave returned TLM_UPDATED with END_REQ

	} else if (phase == amba::END_DATA) {

	  // Done - return control to user.

	} else {

	  // Forbidden phase
	  v::error << name() << "Invalid phase in return path (from call to nb_transport_fw)!" << v::endl;
	  trans->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);

	}

	break;

      case tlm::TLM_COMPLETED:

	// Slave directly jumps to TLM_COMPLETED (Pseudo AT).
	// Don't send END_RESP
	//wait(delay);

	break;
      
      default:

	v::error << name() << "Invalid return value from call to nb_transport_fw!" << v::endl;
	trans->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);

    }
  }
}

// Read operation / result will be checked against locally cached data
void ahbctrl_test::check_read(unsigned int addr, unsigned char* data, unsigned int length) {

  // Read from AHB
  ahbread(addr, data, length, 4);

}

// Generates random read operations within haddr/hmask region
void ahbctrl_test::random_read(unsigned int length) {

  // Random address
  unsigned int addr = (rand() % (m_addr_range_upper_bound-m_addr_range_lower_bound)) + m_addr_range_lower_bound;

  // Align address with respect to data length
  switch (length) {

  case 1:
    // byte address
    break;

  case 2:
    // half-word boundary
    addr = addr & 0xfffffffe;
    break;

  case 4:
    // word boundary
    addr = addr & 0xfffffffc;
    break;

  default:

    v::error << "Length not valid in random_write!" << v::endl;
  }

  // Read from AHB
  ahbread(addr, tmp + tc, length, 4);

  tc = (tc + 4) % 1000;

}

// Write operation / write data will be cached in local storage
void ahbctrl_test::check_write(unsigned int addr, unsigned char * data, unsigned int length) {

  unsigned int i;
  t_entry entry;

  // Write to AHB
  ahbwrite(addr, data, length, 4);  

  // Keep track in local cache
  for (i=0;i<length;i++) {

    entry.data = data[i];
    entry.valid = true;

    localcache[addr+i] = entry;

  }
}

// Generates random write operations within haddr/hmask region
void ahbctrl_test::random_write(unsigned int length) {

  unsigned int i;

  t_entry entry;

  // Random address
  unsigned int addr = (rand() % (m_addr_range_upper_bound-m_addr_range_lower_bound)) + m_addr_range_lower_bound;

  // Align address with respect to data length
  switch (length) {

  case 1:
    // byte address
    break;

  case 2:
    // half-word boundary
    addr = addr & 0xfffffffe;
    break;

  case 4:
    // word boundary
    addr = addr & 0xfffffffc;
    break;

  default:

    v::error << "Length not valid in random_write!" << v::endl;
  }
  
  v::debug << name() << "New random write!" << v::endl;

  // Random data
  for (i=0;i<4;i++) {

    if (i < length) {

      tmp[tc+i] = rand() % 256;

      v::debug << name() << "addr: " << hex << addr+i << " data: " << (unsigned int)tmp[tc+i] << v::endl;

    } else {

      tmp[tc+i] = 0;

    }
  
  }

  // Write to AHB
  ahbwrite(addr, tmp+tc, length, 4);

  // Keep track in local cache
  for (i=0;i<length;i++) {

    entry.data = tmp[tc+i];
    entry.valid = true;

    localcache[addr+i] = entry;

  }

  tc = (tc + 4) % 1000;

}

// Snooping function - invalidates dirty cache entries
void ahbctrl_test::snoopingCallBack(const t_snoop & snoop, const sc_core::sc_time & delay) {

  v::debug << name() << "Snooping write operation on AHB interface (MASTER: " << hex << snoop.master_id << " ADDR: " \
	   << hex << snoop.address << " LENGTH: " << snoop.length << ")" << v::endl;

  // Make sure we are not snooping ourself ;)
  // Check if address was used for a random instruction from this master.
  if (snoop.master_id != m_master_id) {

    for (unsigned int i = 0; i < snoop.length; i++) {

      // Look up local cache
      it = localcache.find(snoop.address + i);

      if (it != localcache.end()) {

	v::debug << name() << "Invalidate data at address: " << hex << snoop.address + i << v::endl;

	// Delete the valid bit
	(it->second).valid = false;

      }
    }
  }
}

// For mctrl tests - if testbench is connected through bus model
// start: start addr bytes, end: end addr bytes, length in bytes
bool ahbctrl_test::writeCheck(uint32_t start, uint32_t end, uint32_t width, uint32_t length, bool fail) {

    // Array with number of entries equal to transfer size in bytes 
    uint8_t data[end-start];

    // Pointers to data array with different resolution (8, 16, 32, 64 bits)
    uint8_t  *data8 = reinterpret_cast<uint8_t *>(data);
    uint16_t *data16 = reinterpret_cast<uint16_t *>(data);
    uint32_t *data32 = reinterpret_cast<uint32_t *>(data);
    uint64_t *data64 = reinterpret_cast<uint64_t *>(data);

    bool error = false;

    // Write to memory
    v::debug << name() << "Write Check " << v::endl;

    // Step through data array with transfer width (8, 16, 32 or 64 bits)
    for(uint32_t i = 0; i < end - start; i+=width) {
        
      // Index is word offset in data array
      uint32_t index = i >> ((int)log2(width));

      // Byte address of current transfer
      uint32_t address = start + i;
      //
      uint32_t rdata = std::rand();

      v::debug << name() << "loop counter=" << i << ", index=" << index << ", length=" << length << ", address=" << address << v::endl;

      switch(width) {
      
         case 1:
           data8[index] = static_cast<uint8_t>((rdata) & 0xFF);
           break;
         case 2:
           data16[index] = static_cast<uint16_t>((rdata) & 0xFFFF);
           break;
         case 4:
           data32[index] = static_cast<uint32_t>((rdata) & 0xFFFFFFFF);
           break;
         case 8:
           data64[index] = static_cast<uint64_t>(((rdata) & 0xFFFFFFFFll) | (((rdata) & 0xFFFFFFFFll) << 32));
         break;

      }
 
      v::debug << name() << "Write at counter=" << i << "(" << i-length+width << ") and from address=" << v::uint32 << address << "(" << v::uint32 << address-length+width << ") with length=" << std::dec << length << "(" << i-length+width+length-1 << ")" << v::endl;

      if((i+width)%length==0) {

        // Send data word to write function
	ahbwrite(address-length+width, &data[i-length+width], length, 4);
               
      }
    }

    wait(5000, SC_NS);

    // Read from memory (using debug interface)
    for(uint32_t i = 0; i < end - start; i++) {

        uint32_t address = start + i;
        uint8_t c = 0;

        ahbread_dbg(address, &c, 1);

        if(c!=data[i]&&!fail) {

            v::info << name() << "Expecting " << v::uint8 << (uint32_t)data[i]
                << " at address " << v::uint32 << address << " but got " 
                << v::uint8 << (uint32_t)c << v::endl;
            v::warn << name() << "Test failed" << v::endl << v::endl;
            error |= true;

        } else if(c==data[i]&&fail) {

            v::warn << name() << "Test failed: Result is matching, but this test should not match!" << v::endl << v::endl; 
            error |= true;

        }
    }

    return error;
}

// For tests of mctrl - if testbench is connected through bus model
bool ahbctrl_test::readCheck(const uint32_t start, uint32_t end, uint32_t width, const uint32_t length, bool fail) {

    end += end % width;
    uint8_t data[end-start];
    uint8_t *data8 = reinterpret_cast<uint8_t *>(data);
    uint16_t *data16 = reinterpret_cast<uint16_t *>(data);
    uint32_t *data32 = reinterpret_cast<uint32_t *>(data);
    uint64_t *data64 = reinterpret_cast<uint64_t *>(data);
    bool error = false;

    // Write to memory (using debug interface)
    v::debug << name() << "Read Check " << v::endl;

    for(uint32_t i = 0; i < end - start; i+=width) {

        uint32_t index = i / width;
        // Prepare data word
        uint32_t address = start + i;
        uint32_t rdata = std::rand();
 
	v::debug << name() << "loop counter=" << i << ", index=" << index << ", length=" << length << ", address=" << address << v::endl;

        switch(width) {
            case 1:
                  data8[index] = static_cast<uint8_t>((rdata) & 0xFF);
                  break;
            case 2:
                  data16[index] = static_cast<uint16_t>((rdata) & 0xFFFF);
                  break;
            case 4:
                  data32[index] = static_cast<uint32_t>((rdata) & 0xFFFFFFFF);
                  break;
            case 8:
                  data64[index] = static_cast<uint64_t>(((rdata) & 0xFFFFFFFFll) | (((rdata) & 0xFFFFFFFFll) << 32));
                  break;
        }
        if((i+width)%length==0) {

            //Send data word to write function
            v::debug << name() << "Write at counter=" << i << "(" << i-length+width << ") and from address=" << v::uint32 << address << "(" << v::uint32 << address-length+width << ") with length=" << std::dec << length << "(" << i-length+width+length-1 << ")" << v::endl;
            ahbwrite_dbg(address-length+width, &data[i-length+width], length);
        }
    }

    // Read from memory
    for(uint32_t i = 0, j = 0; i < end - start; i+=width, j++) {

        uint32_t address = start + i;
        uint8_t d[width];
        uint8_t *d8 = reinterpret_cast<uint8_t *>(d);
        uint16_t *d16 = reinterpret_cast<uint16_t *>(d);
        uint32_t *d32 = reinterpret_cast<uint32_t *>(d);
        uint64_t *d64 = reinterpret_cast<uint64_t *>(d);
        
        ahbread(address, d, width, 4);

        switch(width) {
            case 1:
                if(*d8!=data8[j]&&!fail) {
                    v::info << name() << "Expecting " << v::uint8 << (uint32_t)data8[j]
                            << " at address " << v::uint32 << address << " but got " 
                            << v::uint8 << (uint32_t)*d8 << v::endl;
                    v::warn << name() << "Test failed" << v::endl << v::endl;
                    error |= true;
                } else if(*d16==data8[j]&&fail) {
                    v::warn << name() << "Test failed: Result is matching, but this test should not match!" << v::endl << v::endl; 
                    error |= true;
                }
                break;
            case 2:
                if(*d16!=data16[j]&&!fail) {
                    v::info << name() << "Expecting " << v::uint16 << data16[j]
                            << " at address " << v::uint32 << address << " but got " 
                            << v::uint16 << *d16 << v::endl;
                    v::warn << name() << "Test failed" << v::endl << v::endl;
                    error |= true;
                } else if(*d16==data16[j]&&fail) {
                    v::warn << name() << "Test failed: Result is matching, but this test should not match!" << v::endl << v::endl; 
                    error |= true;
                }
                break;
            case 4:
                if(*d32!=data32[j]&&!fail) {
                    v::info << name() << "Expecting " << v::uint32 << data32[j]
                            << " at address " << v::uint32 << address << " but got " 
                            << v::uint32 << *d32 << v::endl;
                    v::warn << name() << "Test failed" << v::endl << v::endl;
                    error |= true;
                } else if(*d32==data32[j]&&fail) {
                    v::warn << name() << "Test failed: Result is matching, but this test should not match!" << v::endl << v::endl; 
                    error |= true;
                }
                break;
            case 8:
                if(*d64!=data64[j]&&!fail) {
                    v::info << name() << "Expecting " << v::uint64 << data64[j]
                            << " at address " << v::uint32 << address << " but got " 
                            << v::uint64 << *d64 << v::endl;
                    v::warn << name() << "Test failed" << v::endl << v::endl;
                    error |= true;
                } else if(*d64==data64[i]&&fail) {
                    v::warn << name() << "Test failed: Result is matching, but this test should not match!" << v::endl << v::endl; 
                    error |= true;
                }
                break;
        }
    }
    return error;
}

// AHB debug read
void ahbctrl_test::ahbread_dbg(uint32_t addr, uint8_t *data, uint32_t length) {

    tlm::tlm_generic_payload gp;

    gp.set_command(tlm::TLM_READ_COMMAND);
    gp.set_address(addr);
    gp.set_data_length(length);
    gp.set_streaming_width(4);
    gp.set_byte_enable_ptr(NULL);
    gp.set_data_ptr(data);
    gp.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    this->ahb->transport_dbg(gp);

    if(gp.get_response_status()!=tlm::TLM_OK_RESPONSE) {
      v::warn << name() << "Read from Address " << v::uint32 << addr 
              << " was " << gp.get_response_string() << v::endl;
    }
}

// AHB debug write
void ahbctrl_test::ahbwrite_dbg(uint32_t addr, uint8_t *data, uint32_t length) {
    tlm::tlm_generic_payload gp;

    // For sub-word access over the AHB, the burst-size extension must be set
    amba::amba_burst_size *amba_burst_size;
    ahb.validate_extension<amba::amba_burst_size> (gp);
    ahb.get_extension<amba::amba_burst_size>(amba_burst_size, gp);
    amba_burst_size->value = (length < 4)? length : 4;

    gp.set_command(tlm::TLM_WRITE_COMMAND);
    gp.set_address(addr);
    gp.set_data_length(length);
    gp.set_streaming_width(4);
    gp.set_byte_enable_ptr(NULL);
    gp.set_data_ptr(data);
    this->ahb->transport_dbg(gp);
    
    if(gp.get_response_status()!=tlm::TLM_OK_RESPONSE) {
      v::warn << name() << "Write to Address " << v::uint32 << addr
              << " was " << gp.get_response_string() << v::endl;
    }
}

// Thread for data phase processing in write operations (sends BEGIN_DATA)
void ahbctrl_test::DataThread() {

  tlm::tlm_generic_payload* trans;
  tlm::tlm_phase phase;
  sc_core::sc_time delay;
  tlm::tlm_sync_enum status;

  while(1) {

    //v::debug << name() << "Data thread waiting for new data phase." << v::endl;

    // Wait for new data phase
    wait(mDataPEQ.get_event());

    //v::debug << name() << "DataPEQ Event" << v::endl;

    // Get transaction from PEQ
    trans = mDataPEQ.get_next_transaction();

    // Prepare BEGIN_DATA
    phase = amba::BEGIN_DATA;
    delay = SC_ZERO_TIME;

    v::debug << name() << "Transaction " << hex << trans << " call to nb_transport_fw with phase " << phase << v::endl;

    // Call nb_transport_fw with BEGIN_DATA
    status = ahb->nb_transport_fw(*trans, phase, delay);

    switch (status) {

      case tlm::TLM_ACCEPTED:
      case tlm::TLM_UPDATED:

	if (phase == amba::BEGIN_DATA) {

	  // The slave returned TLM_ACCEPTED.
	  // Wait for END_DATA to come in on backward path.

	  // v::debug << name() << "Waiting mEndDataEvent" << v::endl;
	  // wait(mEndDataEvent);
	  // v::debug << name() << "mEndDataEvent" << v::endl;

	} else if (phase == amba::END_DATA) {

	  // Slave sent TLM_UPDATED/END_DATA.
	  // Data phase completed.
	  // wait(delay);

	} else {

	  // Forbidden phase
	  v::error << name() << "Invalid phase in return path (from call to nb_transport_fw)!" << v::endl;

	}

	break;

      case tlm::TLM_COMPLETED:

	// Slave directly jumps to TLM_COMPLETED (Pseudo AT).
	//wait(delay);
	
	break;

    }
  }
}

// Thread for response synchronization (sync and send END_RESP)
void ahbctrl_test::ResponseThread() {

  tlm::tlm_generic_payload* trans;
  tlm::tlm_phase phase;
  sc_core::sc_time delay;
  tlm::tlm_sync_enum status;

  while(1) {

    // Wait for response from slave
    wait(mResponsePEQ.get_event());

    // Get transaction from PEQ
    trans = mResponsePEQ.get_next_transaction();

    // Check result
    checkTXN(trans);
  
    // Prepare END_RESP
    phase = tlm::END_RESP;
    delay = sc_core::SC_ZERO_TIME;

    v::debug << name() << "Transaction " << hex << trans << " call to nb_transport_fw with phase " << phase << v::endl;

    // Call nb_transport_fw with END_RESP
    status = ahb->nb_transport_fw(*trans, phase, delay);

    // Return value must be TLM_COMPLETED or TLM_ACCEPTED
    assert((status==tlm::TLM_COMPLETED)||(status==tlm::TLM_ACCEPTED));

    // Cleanup
    mEndTransactionPEQ.notify(*trans, delay);
  }
}

// Check transaction
void ahbctrl_test::checkTXN(tlm::tlm_generic_payload * trans) {

  // Cache data
  t_entry tmp;
  tmp.data = 0;
  tmp.valid = 0;

  // Unpack transaction
  tlm::tlm_command cmd = trans->get_command();
  unsigned int addr    = trans->get_address();
  unsigned char * data = trans->get_data_ptr();
  unsigned int length  = trans->get_data_length();

  // Verify reads against localcache
  if (cmd == tlm::TLM_READ_COMMAND) {

    // Check result
    for (unsigned int i=0; i<length; i++) {

      // Look up local cache
      it = localcache.find(addr+i);

      // Assume 0, if location was never written before
      if (it != localcache.end()) {

	// We (this master) wrote to this address before.
	tmp = it->second;

      } else {

	// Address was not written by this master before.
	tmp.data = 0;
	tmp.valid = false;

      }

      v::debug << name() << "ADDR: " << hex << addr+i << " DATA: " << (unsigned int)data[i] << " EXPECTED: " << tmp.data << " VALID: " << tmp.valid << v::endl;
   
      if (tmp.valid == true) {

	if (data[i] != tmp.data) {
	  
	  v::error << name() << "Result of read operation not correct!!" << v::endl;
	  assert(0);
	}

      } else {

	v::debug << name() << "No local reference for checking or data invalidated by snooping!" << v::endl;

      }
    }
  }
}
