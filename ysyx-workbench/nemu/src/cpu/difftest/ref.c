/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
    if (direction == DIFFTEST_TO_DUT)
  {
    buf = (void *)guest_to_host(addr);
  }
  else if (direction == DIFFTEST_TO_REF)
  {
    Log("%x,%lx", addr, n);
    char *buf_char = (char *)buf;
    for (int i = 0; i < n; i++)
    {
      paddr_write(addr + i, 1, buf_char[i]);
    }
  }
}

void difftest_regcpy(void *dut, bool direction)
{
  if (direction == DIFFTEST_TO_DUT)
  {
    for (int i = 0; i < 32; i++)
      ((CPU_state *)dut)->gpr[i] = cpu.gpr[i];
    ((CPU_state *)dut)->pc = cpu.pc;
  }
  else if (direction == DIFFTEST_TO_REF)
  {
    for (int i = 0; i < 32; i++)
      cpu.gpr[i] = ((CPU_state *)dut)->gpr[i];
    cpu.pc = ((CPU_state *)dut)->pc;
    // csr don't sync
    // cpu = *(CPU_state *)dut;
  }
}

void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

void difftest_raise_intr(word_t NO) {
  assert(0);
}

void difftest_init(int port) {
  /* Perform ISA dependent initialization. */
  void init_rand();
  void init_mem();
  void init_isa();
  init_rand();
  init_mem();
  init_isa();
}
