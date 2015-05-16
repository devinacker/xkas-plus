// CSG 65ce02 assembly
// based on 65c02.cpp

bool xkas65ce02::assemble_command(string &s) {
	part.qsplit<1>(" ", s);
	part[0].lower();
	part[0].trim(" ");
	if(part.size() > 1) part[1].trim(" ");

	#define match(_l, _r, _fn) \
		if (part[1].wildcard(_l "*" _r)) { \
			part[1].ltrim<1>(_l); \
			part[1].rtrim<1>(_r); \
			force_test(); \
			return _fn; \
		}
	match("#", "",      assemble_const())
	match("(", ",s),y", assemble_sry())
	match("(", ",x)",   assemble_indirect_x())
	match("(", "),y",   assemble_indirect_y())
	match("(", "),z",   assemble_indirect_z())
	match("(", ")",     assemble_indirect())
	match("",  ",x",    assemble_addr_x() || assemble_dp_x())
	match("",  ",y",    assemble_addr_y() || assemble_dp_y())
	#undef match

	force_test();
	if(assemble_branch()) return true;
	if(assemble_addr()) return true;
	if(assemble_dp()) return true;

	return false;
}

void xkas65ce02::force_test() {
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

bool xkas65ce02::assemble_direct() {
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
	match("stp", 0xcb)
	match("wai", 0xdb)
	
	// new 65ce02 instructions
	match("asr", 0x43)
	match("cle", 0x02)
	match("dez", 0x3b)
	match("inz", 0x1b)
	match("map", 0x5c)
	match("neg", 0x42)
	match("phz", 0xdb)
	match("plz", 0xfb)
	match("see", 0x03)
	match("tab", 0x5b)
	match("tba", 0x7b)
	match("taz", 0x4b)
	match("tsy", 0x0b)
	match("tys", 0x2b)
	match("tza", 0x6b)
	#undef match
	return false;
}

bool xkas65ce02::assemble_const() {
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			for(unsigned i = 0; i < n; i++) { \
				self.write(hex_); \
			} \
			return true; \
		}
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
	match("stp", 0xcb)
	match("wai", 0xdb)
	// new 65ce02 instructions
	match("asr", 0x43)
	match("cle", 0x02)
	match("dez", 0x3b)
	match("inz", 0x1b)
	match("map", 0x5c)
	match("neg", 0x42)
	match("phz", 0xdb)
	match("plz", 0xfb)
	match("see", 0x03)
	match("tab", 0x5b)
	match("tba", 0x7b)
	match("taz", 0x4b)
	match("tsy", 0x0b)
	match("tys", 0x2b)
	match("tza", 0x6b)
	
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
	// new 65ce02 instructions
	matchb("cpz", 0xc2)
	matchb("ldz", 0xa3)
	matchw("phw", 0xf4)
	matchb("rts", 0x62)
	#undef match
	#undef matchb
	#undef matchw
	return false;
}


bool xkas65ce02::assemble_sry() {
	if(force_word) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	// new 65ce02 instructions
	match("lda", 0xe2)
	match("sta", 0x82)
	#undef match
	return false;
}

bool xkas65ce02::assemble_indirect_x() {
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
	matchb("adc", 0x61)
	matchb("and", 0x21)
	matchb("cmp", 0xc1)
	matchb("eor", 0x41)
	matchb("lda", 0xa1)
	matchb("ora", 0x01)
	matchb("sbc", 0xe1)
	matchb("sta", 0x81)
	// new 65ce02 instructions
	matchw("jmp", 0x7c)
	matchw("jsr", 0x23)
	#undef matchb
	#undef matchw
	return false;
}

bool xkas65ce02::assemble_indirect_y() {
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

bool xkas65ce02::assemble_indirect_z() {
	if(force_word) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	match("adc", 0x72)
	match("and", 0x32)
	match("cmp", 0xd2)
	match("eor", 0x52)
	match("lda", 0xb2)
	match("ora", 0x12)
	match("sbc", 0xf2)
	match("sta", 0x92)
	#undef match
	return false;
}

bool xkas65ce02::assemble_indirect() {
	#define match(str_, hex_) \
		if(part[0] == str_ && !force_byte) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	match("jmp", 0x6c)
	// new 65ce02 instructions
	match("jsr", 0x22)
	#undef match
	return false;
}

bool xkas65ce02::assemble_addr_x() {
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
	// new 65ce02 instructions
	match("ldz", 0xbb)
	match("sty", 0x8b)
	#undef match
	return false;
}

bool xkas65ce02::assemble_dp_x() {
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
	// new 65ce02 instructions
	match("asr", 0x54)
	#undef match
	return false;
}

bool xkas65ce02::assemble_addr_y() {
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
	// new 65ce02 instructions
	match("stx", 0x9b)
	#undef match
	return false;
}

bool xkas65ce02::assemble_dp_y() {
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

bool xkas65ce02::assemble_addr() {
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
	// new 65ce02 instructions
	match("asw", 0xcb)
	match("cpz", 0xdc)
	match("ldz", 0xab)
	match("phw", 0xfc)
	match("row", 0xeb)
	#undef match
	return false;
}

bool xkas65ce02::assemble_dp() {
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
	// new 65ce02 instructions
	match("asr", 0x44)
	match("cpz", 0xd4)
	match("dew", 0xc3)
	match("inw", 0xe3)
	#undef match
	return false;
}

bool xkas65ce02::assemble_branch() {
	#define match(str_, hex_, word_) \
		if(part[0] == str_) { \
			int n = self.decode(part[1]); \
			if(!word_) { \
				if(!part[1].wildcard("$??")) { \
					n = n - (self.pc() + 2); \
				} \
				if(self.pass == 2 && (n < -128 || n > 127)) { \
					self.error = "short branch target out of bounds"; \
					return false; \
				} \
				self.write(hex_); \
				self.write(n); \
			} else { \
				if(!part[1].wildcard("$????")) { \
					n = n - (self.pc() + 3); \
				} \
				if(self.pass == 2 && (n < -32768 || n > 32767)) { \
					self.error = "long branch target out of bounds"; \
					return false; \
				} \
				self.write(hex_); \
				self.write(n); \
				self.write(n >> 8); \
			} \
			return true; \
		}
	match("bcc", 0x90, !force_word)
	match("bcs", 0xb0, !force_word)
	match("beq", 0xf0, !force_word)
	match("bne", 0xd0, !force_word)
	match("bmi", 0x30, !force_word)
	match("bpl", 0x10, !force_word)
	match("bvc", 0x50, !force_word)
	match("bvs", 0x70, !force_word)
	// new 65c02 instructions
	match("bbr0", 0x0f, !force_word)
	match("bbr1", 0x1f, !force_word)
	match("bbr2", 0x2f, !force_word)
	match("bbr3", 0x3f, !force_word)
	match("bbr4", 0x4f, !force_word)
	match("bbr5", 0x5f, !force_word)
	match("bbr6", 0x6f, !force_word)
	match("bbr7", 0x7f, !force_word)
	match("bbs0", 0x8f, !force_word)
	match("bbs1", 0x9f, !force_word)
	match("bbs2", 0xaf, !force_word)
	match("bbs3", 0xbf, !force_word)
	match("bbs4", 0xcf, !force_word)
	match("bbs5", 0xdf, !force_word)
	match("bbs6", 0xef, !force_word)
	match("bbs7", 0xff, !force_word)
	match("bra", 0x80, !force_word)
	// new 65ce02 instructions
	match("bcc", 0x93, !force_byte)
	match("bcs", 0xb3, !force_byte)
	match("beq", 0xf3, !force_byte)
	match("bne", 0xd3, !force_byte)
	match("bmi", 0x33, !force_byte)
	match("bpl", 0x13, !force_byte)
	match("bra", 0x83, !force_byte) match("brl", 0x83, true)
	match("bsr", 0x63, !force_byte)
	match("bvc", 0x53, !force_byte)
	match("bvs", 0x73, !force_byte)
	#undef match
	return false;
}

xkas65ce02::xkas65ce02(xkas &self) : xkasArch(self) {
}
