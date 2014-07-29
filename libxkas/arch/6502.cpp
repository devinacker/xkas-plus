void xkas6502::init(unsigned pass) {
  header = false;
  bank = 0;
  bank_size = 0x10000;
}

unsigned xkas6502::archaddr(unsigned addr) {
  if(header) addr -= 16;

  // assume the address to be found is within the current bank
  unsigned base = (self.state.org / bank_size) * bank_size;
  addr = base + (addr % bank_size);
  return addr;
}

unsigned xkas6502::fileaddr(unsigned addr) {
  // assume the address is within the current bank
  addr = (bank * bank_size) + (addr % bank_size);

  if(header) addr += 16;
  return addr;
}

bool xkas6502::assemble_command(string &s) {
  part.qsplit<1>(" ", s);
  part[0].lower();
  part[0].trim(" ");
  if(part.size() > 1) part[1].trim(" ");

  if(part[0] == "header") {
    header = true;
    return true;
  }

  if(part[0] == "noheader") {
    header = false;
    return true;
  }
  
  if (part[0] == "banksize" && part.size() == 2) {
    unsigned n = self.decode(part[1]);
    if (n > 0x10000) {
      self.error = "bank size cannot exceed 0x10000";
      return false;
    }
    bank_size = n;
    return true;
  }
  
  if (part[0] == "bank" && part.size() == 2) {
    bank = self.decode(part[1]);
    return true;
  }

  if(part.size() == 1) {
    if(assemble_direct()) return true;
    return false;
  }

  if(part[1].wildcard("#*")) {
    part[1].ltrim<1>("#");
    force_test();
    if(assemble_const()) return true;
    return false;
  }

  if(part[1].wildcard("(*,x)")) {
    part[1].ltrim<1>("(");
    part[1].rtrim<1>(",x)");
    force_test();
    if(assemble_indirect_x()) return true;
    return false;
  }

  if(part[1].wildcard("(*),y")) {
    part[1].ltrim<1>("(");
    part[1].rtrim<1>("),y");
    force_test();
    if(assemble_indirect_y()) return true;
    return false;
  }

  if(part[1].wildcard("(*)")) {
    part[1].ltrim<1>("(");
    part[1].rtrim<1>(")");
    force_test();
    if(assemble_indirect()) return true;
    return false;
  }

  if(part[1].wildcard("*,x")) {
    part[1].rtrim<1>(",x");
    force_test();
    if(assemble_addr_x()) return true;
    if(assemble_dp_x()) return true;
    return false;
  }

  if(part[1].wildcard("*,y")) {
    part[1].rtrim<1>(",y");
    force_test();
    if(assemble_addr_y()) return true;
    if(assemble_dp_y()) return true;
    return false;
  }

  if(assemble_branch()) return true;

  force_test();
  if(assemble_addr()) return true;
  if(assemble_dp()) return true;

  return false;
}

void xkas6502::force_test() {
  force_byte = force_word = false;

  if(part[0].endswith(".b")) {
    force_byte = true;
    part[0].rtrim<1>(".b");
  } else if(part[0].endswith(".w")) {
    force_word = true;
    part[0].rtrim<1>(".w");
  } else {
    force_byte = part[1].wildcard("$??");
    force_word = part[1].wildcard("$????");
  }
}

bool xkas6502::assemble_direct() {
  #define match(str_, hex_) \
    if(part[0] == str_ && part.size() == 1) { \
      self.write(hex_); \
      return true; \
    }

  match("asl", 0x0a)
  match("clc", 0x18)
  match("cld", 0xd8)
  match("cli", 0x58)
  match("clv", 0xb8)
  match("dex", 0xca)
  match("dey", 0x88)
  match("inx", 0xe8)
  match("iny", 0xc8)
  match("lsr", 0x4a)
  match("nop", 0xea)
  match("pha", 0x48)
  match("php", 0x08)
  match("pla", 0x68)
  match("plp", 0x28)
  match("rol", 0x2a)
  match("ror", 0x6a)
  match("rti", 0x40)
  match("rts", 0x60)
  match("sec", 0x38)
  match("sed", 0xf8)
  match("sei", 0x78)
  match("tax", 0xaa)
  match("tay", 0xa8)
  match("tsx", 0xba)
  match("txa", 0x8a)
  match("txs", 0x9a)
  match("tya", 0x98)
  #undef match
  return false;
}

bool xkas6502::assemble_const() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      for(unsigned i = 0; i < n; i++) { \
        self.write(hex_); \
      } \
      return true; \
    }
  #define matchb(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }

  //repetition matches
  match("asl", 0x0a)
  match("clc", 0x18)
  match("cld", 0xd8)
  match("cli", 0x58)
  match("clv", 0xb8)
  match("dex", 0xca)
  match("dey", 0x88)
  match("inx", 0xe8)
  match("iny", 0xc8)
  match("lsr", 0x4a)
  match("nop", 0xea)
  match("pha", 0x48)
  match("php", 0x08)
  match("pla", 0x68)
  match("plp", 0x28)
  match("rol", 0x2a)
  match("ror", 0x6a)
  match("rti", 0x40)
  match("rts", 0x60)
  match("sec", 0x38)
  match("sed", 0xf8)
  match("sei", 0x78)
  match("tax", 0xaa)
  match("tay", 0xa8)
  match("tsx", 0xba)
  match("txa", 0x8a)
  match("txs", 0x9a)
  match("tya", 0x98)
  
  //const matches
  matchb("adc", 0x69)
  matchb("and", 0x29)
  matchb("brk", 0x00)
  matchb("cmp", 0xc9)
  matchb("cpx", 0xe0)
  matchb("cpy", 0xc0)
  matchb("eor", 0x49)
  matchb("lda", 0xa9)
  matchb("ldx", 0xa2)
  matchb("ldy", 0xa0)
  matchb("ora", 0x09)
  matchb("sbc", 0xe9)
  #undef match
  #undef matchb
  return false;
}

bool xkas6502::assemble_indirect_x() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_ && !force_word) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("adc", 0x61)
  match("and", 0x21)
  match("cmp", 0xc1)
  match("eor", 0x41)
  match("lda", 0xa1)
  match("ora", 0x01)
  match("sbc", 0xe1)
  match("sta", 0x81)
  #undef match
  return false;
}

bool xkas6502::assemble_indirect_y() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("adc", 0x71)
  match("and", 0x31)
  match("cmp", 0xd1)
  match("eor", 0x51)
  match("lda", 0xb1)
  match("ora", 0x11)
  match("sbc", 0xf1)
  match("sta", 0x91)
  #undef match
  return false;
}

bool xkas6502::assemble_indirect() {
  if(force_byte) return false;
  #define match(str_, hex_) \
    if(part[0] == str_ && !force_byte) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      self.write(n >> 8); \
      return true; \
    }
  match("jmp", 0x6c)
  #undef match
  return false;
}

bool xkas6502::assemble_addr_x() {
  if(force_byte) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      self.write(n >> 8); \
      return true; \
    }
  match("adc", 0x7d)
  match("and", 0x3d)
  match("asl", 0x1e)
  match("cmp", 0xdd)
  match("dec", 0xde)
  match("eor", 0x5d)
  match("inc", 0xfe)
  match("lda", 0xbd)
  match("ldy", 0xbc)
  match("lsr", 0x5e)
  match("ora", 0x1d)
  match("rol", 0x3e)
  match("ror", 0x7e)
  match("sbc", 0xfd)
  match("sta", 0x9d)
  #undef match
  return false;
}

bool xkas6502::assemble_dp_x() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("adc", 0x75)
  match("and", 0x35)
  match("asl", 0x16)
  match("cmp", 0xd5)
  match("dec", 0xd6)
  match("eor", 0x55)
  match("inc", 0xf6)
  match("lda", 0xb5)
  match("ldy", 0xb4)
  match("lsr", 0x56)
  match("ora", 0x15)
  match("rol", 0x36)
  match("ror", 0x76)
  match("sbc", 0xf5)
  match("sta", 0x95)
  match("sty", 0x94)
  #undef match
  return false;
}

bool xkas6502::assemble_addr_y() {
  if(force_byte) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      self.write(n >> 8); \
      return true; \
    }
  match("adc", 0x79)
  match("and", 0x39)
  match("cmp", 0xd9)
  match("eor", 0x59)
  match("lda", 0xb9)
  match("ldx", 0xbe)
  match("ora", 0x19)
  match("sbc", 0xf9)
  match("sta", 0x99)
  #undef match
  return false;
}

bool xkas6502::assemble_dp_y() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("ldx", 0xb6);
  match("stx", 0x96);
  #undef match
  return false;
}

bool xkas6502::assemble_addr() {
  if(force_byte) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      self.write(n >> 8); \
      return true; \
    }
  match("adc", 0x6d)
  match("and", 0x2d)
  match("asl", 0x0e)
  match("bit", 0x2c)
  match("cmp", 0xcd)
  match("cpx", 0xec)
  match("cpy", 0xcc)
  match("dec", 0xce)
  match("eor", 0x4d)
  match("inc", 0xee)
  match("jmp", 0x4c)
  match("jsr", 0x20)
  match("lda", 0xad)
  match("ldx", 0xae)
  match("ldy", 0xac)
  match("lsr", 0x4e)
  match("ora", 0x0d)
  match("rol", 0x2e)
  match("ror", 0x6e)
  match("sbc", 0xed)
  match("sta", 0x8d)
  match("stx", 0x8e)
  match("sty", 0x8c)
  #undef match
  return false;
}

bool xkas6502::assemble_dp() {
  if(force_word) return false;
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      unsigned n = self.decode(part[1]); \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("adc", 0x65)
  match("and", 0x25)
  match("asl", 0x06)
  match("bit", 0x24)
  match("cmp", 0xc5)
  match("cpx", 0xe4)
  match("cpy", 0xc4)
  match("dec", 0xc6)
  match("eor", 0x45)
  match("inc", 0xe6)
  match("lda", 0xa5)
  match("ldx", 0xa6)
  match("ldy", 0xa4)
  match("lsr", 0x46)
  match("ora", 0x05)
  match("rol", 0x26)
  match("ror", 0x66)
  match("sbc", 0xe5)
  match("sta", 0x85)
  match("stx", 0x86)
  match("sty", 0x84)
  #undef match
  return false;
}

bool xkas6502::assemble_branch() {
  #define match(str_, hex_) \
    if(part[0] == str_) { \
      int n = self.decode(part[1]); \
      if(!part[1].wildcard("$??")) { \
        n = n - (self.pc() + 2); \
      } \
      if(self.pass == 2 && (n < -128 || n > 127)) { \
        self.error = "branch target out of bounds"; \
        return false; \
      } \
      self.write(hex_); \
      self.write(n); \
      return true; \
    }
  match("bcc", 0x90)
  match("bcs", 0xb0)
  match("beq", 0xf0)
  match("bne", 0xd0)
  match("bmi", 0x30)
  match("bpl", 0x10)
  match("bvc", 0x50)
  match("bvs", 0x70)
  #undef match
  return false;
}

xkas6502::xkas6502(xkas &self) : xkasArch(self) {
}
