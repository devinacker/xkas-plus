// Hudson Soft HuC6280 (TurboGrafx-16 / PC Engine)
// based on 65c02.cpp

bool xkasHuC6280::assemble_command(string &s) {
	part.qsplit<1>(" ", s);
	part[0].lower();
	part[0].trim(" ");
	if(part.size() > 1) part[1].trim(" ");

	if(part.size() == 1) {
		if(assemble_direct()) return true;
		return false;
	}

	#define match(_l, _r, _fn) \
		if (part[1].wildcard(_l "*" _r)) { \
			part[1].ltrim<1>(_l); \
			part[1].rtrim<1>(_r); \
			force_test(); \
			return _fn; \
		}
	match("#", "",    assemble_const())
	match("(", ",x)", assemble_indirect_x())
	match("(", "),y", assemble_indirect_y())
	match("(", ")",   assemble_indirect())
	match("",  ",x",  assemble_addr_x() || assemble_dp_x())
	match("",  ",y",  assemble_addr_y() || assemble_dp_y())
	#undef match

	force_test();
	
	if(part[1].wildcard("*,*")) {
		subpart.split(",", part[1]);
		if (subpart.size() > 3) return false;
		
		return assemble_move() || assemble_test();
	}

	if(assemble_branch()) return true;
	if(assemble_addr()) return true;
	if(assemble_dp()) return true;

	return false;
}

void xkasHuC6280::force_test() {
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

bool xkasHuC6280::assemble_direct() {
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
	
	// new 65c02 instructions
	match("dec", 0x3a)
	match("inc", 0x1a)
	match("phx", 0xda)
	match("phy", 0x5a)
	match("plx", 0xfa)
	match("ply", 0x7a)
	
	// new HuC6280 instructions
	match("cla", 0x62)
	match("clx", 0x82)
	match("cly", 0xc2)
	match("csh", 0xd4)
	match("csl", 0x54)
	match("sax", 0x22)
	match("say", 0x42)
	match("set", 0xf4)
	match("sxy", 0x02)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_const() {
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
	// new 65c02 instructions
	match("dec", 0x3a)
	match("inc", 0x1a)
	match("phx", 0xda)
	match("phy", 0x5a)
	match("plx", 0xfa)
	match("ply", 0x7a)
	// new HuC6280 instructions
	match("cla", 0x62)
	match("clx", 0x82)
	match("cly", 0xc2)
	match("csh", 0xd4)
	match("csl", 0x54)
	match("sax", 0x22)
	match("say", 0x42)
	match("set", 0xf4)
	match("sxy", 0x02)
	
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
	// new 65c02 instructions
	matchb("bit", 0x89)
	// new HuC6280 instructions
	matchb("st0", 0x03)
	matchb("st1", 0x13)
	matchb("st2", 0x23)
	matchb("tam", 0x53)
	matchb("tma", 0x43)
	#undef match
	#undef matchb
	return false;
}

bool xkasHuC6280::assemble_indirect_x() {
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

bool xkasHuC6280::assemble_indirect_y() {
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

bool xkasHuC6280::assemble_indirect() {
	#define matchb(str_, hex_) \
		if(part[0] == str_ && !force_word) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchw(str_, hex_) \
		if(part[0] == str_ && !force_byte) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	matchw("jmp", 0x6c)
	// new 65c02 instructions
	matchb("adc", 0x72)
	matchb("and", 0x32)
	matchb("cmp", 0xd2)
	matchb("eor", 0x52)
	matchb("lda", 0xb2)
	matchb("ora", 0x12)
	matchb("sbc", 0xf2)
	matchb("sta", 0x92)
	#undef matchb
	#undef matchw
	return false;
}

bool xkasHuC6280::assemble_move() {
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			self.write(hex_); \
			unsigned n = self.decode(subpart[0]); \
			self.write(n); \
			self.write(n >> 8); \
			n = self.decode(subpart[1]); \
			self.write(n); \
			self.write(n >> 8); \
			n = self.decode(subpart[2]); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	
	match("tai", 0xf3)
	match("tdd", 0xc3)
	match("tia", 0xe3)
	match("tii", 0x73)
	match("tin", 0xd3)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_test() {
	if (part[0] != "tst") return false;
	
	if (subpart.size() == 2) {
		self.write(force_byte ? 0x83 : 0x93);
	} else if (subpart[2] == "x") {
		self.write(force_byte ? 0xa3 : 0xb3);
	} else return false;
	
	unsigned n = self.decode(subpart[1]);
	unsigned a = self.decode(subpart[2]);
	self.write(n);
	self.write(a);
	if (!force_byte) self.write(a >> 8);
	return true;
}

bool xkasHuC6280::assemble_addr_x() {
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
	// new 65c02 instructions
	match("bit", 0x3c)
	match("stz", 0x9e)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_dp_x() {
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
	// new 65c02 instructions
	match("bit", 0x34)
	match("stz", 0x74)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_addr_y() {
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

bool xkasHuC6280::assemble_dp_y() {
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

bool xkasHuC6280::assemble_addr() {
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
	// new 65c02 instructions
	match("stz", 0x9c)
	match("trb", 0x1c)
	match("tsb", 0x0c)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_dp() {
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
	// new 65c02 instructions
	match("rmb0", 0x07)
	match("rmb1", 0x17)
	match("rmb2", 0x27)
	match("rmb3", 0x37)
	match("rmb4", 0x47)
	match("rmb5", 0x57)
	match("rmb6", 0x67)
	match("rmb7", 0x77)
	match("smb0", 0x87)
	match("smb1", 0x97)
	match("smb2", 0xa7)
	match("smb3", 0xb7)
	match("smb4", 0xc7)
	match("smb5", 0xd7)
	match("smb6", 0xe7)
	match("smb7", 0xf7)
	match("stz", 0x64)
	match("tsb", 0x04)
	match("trb", 0x14)
	#undef match
	return false;
}

bool xkasHuC6280::assemble_branch() {
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
	// new 65c02 instructions
	match("bbr0", 0x0f)
	match("bbr1", 0x1f)
	match("bbr2", 0x2f)
	match("bbr3", 0x3f)
	match("bbr4", 0x4f)
	match("bbr5", 0x5f)
	match("bbr6", 0x6f)
	match("bbr7", 0x7f)
	match("bbs0", 0x8f)
	match("bbs1", 0x9f)
	match("bbs2", 0xaf)
	match("bbs3", 0xbf)
	match("bbs4", 0xcf)
	match("bbs5", 0xdf)
	match("bbs6", 0xef)
	match("bbs7", 0xff)
	match("bra", 0x80)
	// new HuC6280 instructions
	match("bsr", 0x44)
	#undef match
	return false;
}

xkasHuC6280::xkasHuC6280(xkas &self) : xkasArch(self) {
}
