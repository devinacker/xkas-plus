void xkasSNESCPU::init(unsigned pass) {
	mapper = NoROM;
	header = false;
}

unsigned xkasSNESCPU::archaddr(unsigned addr) {
	if(header) addr -= 512;

	switch(mapper) {
		case NoROM: {
			addr = xkasArch::archaddr(addr);
			break;
		}

		case LoROM: {
			addr = ((addr & 0x7f8000) << 1) + 0x8000 + (addr & 0x7fff);
			break;
		}

		case HiROM: {
			addr = 0xc00000 + (addr & 0x3fffff);
			break;
		}
		
		case ExLoROM: {
			addr = ((addr & 0x7f8000) << 1) + 0x8000 + (addr & 0x7fff) ^ 0x800000;
			break;
		}
		
		case ExHiROM: {
			addr = 0xc00000 + (addr & 0x7fffff);
			if (addr >= 0x1000000)
				addr ^= 0x1400000;
			break;
		}
	}

	return addr;
}

unsigned xkasSNESCPU::fileaddr(unsigned addr) {
	switch(mapper) {
		case NoROM: {
			addr = xkasArch::fileaddr(addr);
			break;
		}

		case LoROM: {
			addr = ((addr & 0x7f0000) >> 1) + (addr & 0x7fff);
			break;
		}

		case HiROM: {
			addr = addr & 0x3fffff;
			break;
		}
		
		case ExLoROM: {
			addr = (((addr ^ 0x800000) & 0xff0000) >> 1) + (addr & 0x7fff);
			break;
		}

		case ExHiROM: {
			if (addr >= 0x800000)
				addr &= 0x3fffff;
			else
				addr = (addr & 0x3fffff) | 0x400000;
			break;
		}
	}

	if(header) addr += 512;
	return addr;
}

bool xkasSNESCPU::assemble_command(string &s) {
	part.qsplit<1>(" ", s);
	part[0].lower();
	part[0].trim(" ");
	if(part.size() > 1) part[1].trim(" ");

	if(part[0] == "norom") {
		mapper = NoROM;
		return true;
	}

	if(part[0] == "lorom") {
		mapper = LoROM;
		return true;
	}

	if(part[0] == "hirom") {
		mapper = HiROM;
		return true;
	}

	if(part[0] == "exlorom") {
		mapper = ExLoROM;
		return true;
	}

	if(part[0] == "exhirom") {
		mapper = ExHiROM;
		return true;
	}

	if(part[0] == "header") {
		header = true;
		return true;
	}

	if(part[0] == "noheader") {
		header = false;
		return true;
	}

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
	match("#", "",      assemble_const())
	match("",  ",s",    assemble_sr())
	match("(", ",s),y", assemble_sry())
	match("(", ",x)",   assemble_indirect_x())
	match("(", "),y",   assemble_indirect_y())
	match("(", ")",     assemble_indirect())
	match("[", "],y",   assemble_indirect_long_y())
	match("[", "]",     assemble_indirect_long())
	match("",  ",x",    assemble_long_x() || assemble_addr_x() || assemble_dp_x())
	match("",  ",y",    assemble_addr_y() || assemble_dp_y())
	#undef match

	if(part[1].wildcard("*,*")) {
		if(assemble_move()) return true;
		return false;
	}

	if(assemble_branch()) return true;

	force_test();
	if(assemble_long()) return true;
	if(assemble_addr()) return true;
	if(assemble_dp()) return true;

	return false;
}

void xkasSNESCPU::force_test() {
	force_byte = force_word = force_long = false;

	if(part[0].endswith(".b")) {
		force_byte = true;
		part[0].rtrim<1>(".b");
	} else if(part[0].endswith(".w")) {
		force_word = true;
		part[0].rtrim<1>(".w");
	} else if(part[0].endswith(".l")) {
		force_long = true;
		part[0].rtrim<1>(".l");
	} else {
		force_byte = part[1].wildcard("$??");
		force_word = part[1].wildcard("$????");
		force_long = part[1].wildcard("$??????");
	}
}

bool xkasSNESCPU::assemble_direct() {
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
	match("dec", 0x3a) match("dea", 0x3a)
	match("dex", 0xca)
	match("dey", 0x88)
	match("inc", 0x1a) match("ina", 0x1a)
	match("inx", 0xe8)
	match("iny", 0xc8)
	match("lsr", 0x4a)
	match("nop", 0xea)
	match("pha", 0x48)
	match("phb", 0x8b)
	match("phd", 0x0b)
	match("phk", 0x4b)
	match("php", 0x08)
	match("phx", 0xda)
	match("phy", 0x5a)
	match("pla", 0x68)
	match("plb", 0xab)
	match("pld", 0x2b)
	match("plp", 0x28)
	match("plx", 0xfa)
	match("ply", 0x7a)
	match("rol", 0x2a)
	match("ror", 0x6a)
	match("rti", 0x40)
	match("rtl", 0x6b)
	match("rts", 0x60)
	match("sec", 0x38)
	match("sed", 0xf8)
	match("sei", 0x78)
	match("stp", 0xdb)
	match("tax", 0xaa)
	match("tay", 0xa8)
	match("tcd", 0x5b) match("tad", 0x5b)
	match("tcs", 0x1b) match("tas", 0x1b)
	match("tdc", 0x7b) match("tda", 0x7b)
	match("tsc", 0x3b) match("tsa", 0x3b)
	match("tsx", 0xba)
	match("txa", 0x8a)
	match("txs", 0x9a)
	match("txy", 0x9b)
	match("tya", 0x98)
	match("tyx", 0xbb)
	match("wai", 0xcb)
	match("xba", 0xeb) match("swa", 0xeb)
	match("xce", 0xfb)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_const() {
	if(force_long) return false;
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
	#define matchw(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	#define matchv(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			if(!force_byte) self.write(n >> 8); \
			return true; \
		}

	//repetition matches
	match("asl", 0x0a)
	match("clc", 0x18)
	match("cld", 0xd8)
	match("cli", 0x58)
	match("clv", 0xb8)
	match("dec", 0x3a) match("dea", 0x3a)
	match("dex", 0xca)
	match("dey", 0x88)
	match("inc", 0x1a) match("ina", 0x1a)
	match("inx", 0xe8)
	match("iny", 0xc8)
	match("lsr", 0x4a)
	match("nop", 0xea)
	match("pha", 0x48)
	match("phb", 0x8b)
	match("phd", 0x0b)
	match("phk", 0x4b)
	match("php", 0x08)
	match("phx", 0xda)
	match("phy", 0x5a)
	match("pla", 0x68)
	match("plb", 0xab)
	match("pld", 0x2b)
	match("plp", 0x28)
	match("plx", 0xfa)
	match("ply", 0x7a)
	match("rol", 0x2a)
	match("ror", 0x6a)
	match("rti", 0x40)
	match("rtl", 0x6b)
	match("rts", 0x60)
	match("sec", 0x38)
	match("sed", 0xf8)
	match("sei", 0x78)
	match("stp", 0xdb)
	match("tax", 0xaa)
	match("tay", 0xa8)
	match("tcd", 0x5b) match("tad", 0x5b)
	match("tcs", 0x1b) match("tas", 0x1b)
	match("tdc", 0x7b) match("tda", 0x7b)
	match("tsc", 0x3b) match("tsa", 0x3b)
	match("tsx", 0xba)
	match("txa", 0x8a)
	match("txs", 0x9a)
	match("txy", 0x9b)
	match("tya", 0x98)
	match("tyx", 0xbb)
	match("wai", 0xcb)
	match("xba", 0xeb) match("swa", 0xeb)
	match("xce", 0xfb)

	//const matches
	matchv("adc", 0x69)
	matchv("and", 0x29)
	matchv("bit", 0x89)
	matchb("brk", 0x00)
	matchv("cmp", 0xc9)
	matchb("cop", 0x02)
	matchv("cpx", 0xe0)
	matchv("cpy", 0xc0)
	matchv("eor", 0x49)
	matchv("lda", 0xa9)
	matchv("ldx", 0xa2)
	matchv("ldy", 0xa0)
	matchv("ora", 0x09)
	matchb("rep", 0xc2)
	matchv("sbc", 0xe9)
	matchb("sep", 0xe2)
	matchb("wdm", 0x42)
	#undef match
	#undef matchb
	#undef matchw
	#undef matchv
	return false;
}

bool xkasSNESCPU::assemble_indirect_x() {
	if(force_long) return false;
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
	matchw("jmp", 0x7c)
	matchw("jsr", 0xfc)
	matchb("lda", 0xa1)
	matchb("ora", 0x01)
	matchb("sbc", 0xe1)
	matchb("sta", 0x81)
	#undef matchb
	#undef matchw
	return false;
}

bool xkasSNESCPU::assemble_indirect_y() {
	if(force_word || force_long) return false;
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

bool xkasSNESCPU::assemble_indirect() {
	if(force_long) return false;
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
	matchb("adc", 0x72)
	matchb("and", 0x32)
	matchb("cmp", 0xd2)
	matchb("eor", 0x52)
	matchw("jmp", 0x6c)
	matchb("lda", 0xb2)
	matchb("ora", 0x12)
	matchb("pei", 0xd4)
	matchb("sbc", 0xf2)
	matchb("sta", 0x92)
	#undef matchb
	#undef matchw
	return false;
}

bool xkasSNESCPU::assemble_indirect_long_y() {
	if(force_word || force_long) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	match("adc", 0x77)
	match("and", 0x37)
	match("cmp", 0xd7)
	match("eor", 0x57)
	match("lda", 0xb7)
	match("ora", 0x17)
	match("sbc", 0xf7)
	match("sta", 0x97)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_indirect_long() {
	if(force_long) return false;
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
	matchb("adc", 0x67)
	matchb("and", 0x27)
	matchb("cmp", 0xc7)
	matchb("eor", 0x47)
	matchw("jmp", 0xdc) matchw("jml", 0xdc)
	matchb("lda", 0xa7)
	matchb("ora", 0x07)
	matchb("sbc", 0xe7)
	matchb("sta", 0x87)
	#undef matchb
	#undef matchw
	return false;
}

bool xkasSNESCPU::assemble_sr() {
	if(force_word || force_long) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	match("adc", 0x63)
	match("and", 0x23)
	match("cmp", 0xc3)
	match("eor", 0x43)
	match("lda", 0xa3)
	match("ora", 0x03)
	match("sbc", 0xe3)
	match("sta", 0x83)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_sry() {
	if(force_word || force_long) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	match("adc", 0x73)
	match("and", 0x33)
	match("cmp", 0xd3)
	match("eor", 0x53)
	match("lda", 0xb3)
	match("ora", 0x13)
	match("sbc", 0xf3)
	match("sta", 0x93)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_move() {
	if(part[0] == "mvn" || part[0] == "mvp") {
		//(mvn,mvp) source,dest -> (0x54,0x44) dest source
		subpart.split(",", part[1]);
		if(subpart.size() > 2) return false;
		self.write(part[0] == "mvn" ? 0x54 : 0x44);
		self.write(self.decode(subpart[1]));
		self.write(self.decode(subpart[0]));
		return true;
	}
	return false;
}

bool xkasSNESCPU::assemble_long_x() {
	if(force_byte || force_word) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			self.write(n >> 16); \
			return true; \
		}
	match("adc", 0x7f)
	match("and", 0x3f)
	match("cmp", 0xdf)
	match("eor", 0x5f)
	match("lda", 0xbf)
	match("ora", 0x1f)
	match("sbc", 0xff)
	match("sta", 0x9f)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_addr_x() {
	if(force_byte || force_long) return false;
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
	match("bit", 0x3c)
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
	match("stz", 0x9e)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_dp_x() {
	if(force_word || force_long) return false;
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
	match("bit", 0x34)
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
	match("stz", 0x74)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_addr_y() {
	if(force_byte || force_long) return false;
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

bool xkasSNESCPU::assemble_dp_y() {
	if(force_word || force_long) return false;
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

bool xkasSNESCPU::assemble_long() {
	if(force_byte || force_word) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			self.write(n >> 16); \
			return true; \
		}
	match("adc", 0x6f)
	match("and", 0x2f)
	match("cmp", 0xcf)
	match("eor", 0x4f)
	match("jml", 0x5c)
	match("jsl", 0x22)
	match("lda", 0xaf)
	match("ora", 0x0f)
	match("sbc", 0xef)
	match("sta", 0x8f)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_addr() {
	if(force_byte || force_long) return false;
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
	match("pea", 0xf4)
	match("per", 0x62)
	match("rol", 0x2e)
	match("ror", 0x6e)
	match("sbc", 0xed)
	match("sta", 0x8d)
	match("stx", 0x8e)
	match("sty", 0x8c)
	match("stz", 0x9c)
	match("trb", 0x1c)
	match("tsb", 0x0c)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_dp() {
	if(force_word || force_long) return false;
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
	match("stz", 0x64)
	match("trb", 0x14)
	match("tsb", 0x04)
	#undef match
	return false;
}

bool xkasSNESCPU::assemble_branch() {
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
	match("brl", 0x82, true)
	match("bcc", 0x90, false)
	match("bcs", 0xb0, false)
	match("beq", 0xf0, false)
	match("bne", 0xd0, false)
	match("bmi", 0x30, false)
	match("bpl", 0x10, false)
	match("bvc", 0x50, false)
	match("bvs", 0x70, false)
	match("bra", 0x80, false)
	#undef match
	return false;
}

xkasSNESCPU::xkasSNESCPU(xkas &self) : xkasArch(self) {
}
