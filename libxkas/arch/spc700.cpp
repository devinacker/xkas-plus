void xkasSPC700::init(unsigned pass) {
}

unsigned xkasSPC700::archaddr(unsigned addr) {
	return addr & 0xFFFF;
}

unsigned xkasSPC700::fileaddr(unsigned addr) {
	return addr & 0xFFFF;
}

bool xkasSPC700::assemble_command(string &s) {
	part.qsplit<1>(" ", s);
	part[0].lower();
	part[0].trim(" ");
	
	if(part.size() > 1) { 
		part[1].trim(" ");
		subpart.split(",", part[1]);
	} else {
		return assemble_direct();
	}

	// assemble tcalls
	if (part.size() == 2 && part[0] == "tcall") {
		unsigned tcall = self.decode(part[1]);
		if (tcall > 15) {
			self.error = "invalid tcall";
			return false;
		}
		
		self.write(0x01 + (tcall << 4));
		return true;
	}

	if(assemble_reg()) return true;
	if(assemble_branch()) return true;
	
	#define match(_l, _r, _fn) \
		if (subpart[0].wildcard(_l "*" _r)) { \
			subpart[0].ltrim<1>(_l); \
			subpart[0].rtrim<1>(_r); \
			force_test(); \
			return _fn; \
		} else if (subpart[1].wildcard(_l "*" _r)) { \
			subpart[1].ltrim<1>(_l); \
			subpart[1].rtrim<1>(_r); \
			force_test(); \
			return _fn; \
		}
		
	match("#", "",    assemble_const())
	match("(", "+x)", assemble_indirect_x())
	match("(", ")+y", assemble_indirect_y())
	match("",  "+x",  assemble_addr_x() || assemble_dp_x())
	match("",  "+y",  assemble_addr_y() || assemble_dp_y())
	#undef match

	force_test();
	if(assemble_addr()) return true;
	if(assemble_dp()) return true;
	if(assemble_bit()) return true;
	
	return false;
}

void xkasSPC700::force_test() {
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

bool xkasSPC700::assemble_direct() {
	#define match(str_, hex_) \
		if(part[0] == str_ && part.size() == 1) { \
			self.write(hex_); \
			return true; \
		}

	match("brk",   0x0f)
	match("clrc",  0x60)
	match("clrp",  0x20)
	match("clrv",  0xe0)
	match("di",    0xc0)
	match("ei",    0xa0)
	match("nop",   0x00)
	match("notc",  0xed)
	match("ret",   0x6f)
	match("reti",  0x7f)
	match("setc",  0x80)
	match("setp",  0x40)
	match("sleep", 0xef)
	match("stop",  0xff)
	#undef match
	return false;
}

bool xkasSPC700::assemble_const() {
	if(force_word) return false;
	#define match(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(part[1]); \
			for(unsigned i = 0; i < n; i++) { \
				self.write(hex_); \
			} \
			return true; \
		}
	#define matchr(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[0] == reg_) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchm(str_, hex_) \
		if(part[0] == str_) { \
			unsigned a = self.decode(subpart[0]); \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(a); \
			return true; \
		}

	//repetition matches
	match("brk",   0x0f)
	match("clrc",  0x60)
	match("clrp",  0x20)
	match("clrv",  0xe0)
	match("di",    0xc0)
	match("ei",    0xa0)
	match("nop",   0x00)
	match("notc",  0xed)
	match("ret",   0x6f)
	match("reti",  0x7f)
	match("setc",  0x80)
	match("setp",  0x40)
	match("sleep", 0xef)
	match("stop",  0xff)
	
	//const matches
	// immediate to register
	matchr("adc", "a", 0x88)
	matchr("and", "a", 0x28)
	matchr("cmp", "a", 0x68)
	matchr("cmp", "x", 0xc8)
	matchr("cmp", "y", 0xad)
	matchr("eor", "a", 0x48)
	matchr("mov", "a", 0xe8)
	matchr("mov", "x", 0xcd)
	matchr("mov", "y", 0x8d)
	matchr("or",  "a", 0x08)
	matchr("sbc", "a", 0xa8)
	
	// immediate to memory
	matchm("adc", 0x98)
	matchm("and", 0x38)
	matchm("cmp", 0x78)
	matchm("eor", 0x58)
	matchm("mov", 0x8f)
	matchm("or",  0x18)
	matchm("sbc", 0xb8)
	
	#undef match
	#undef matchr
	#undef matchm
	return false;
}

bool xkasSPC700::assemble_reg() {
	#define match(str_, reg_, hex_) \
		if (part[0] == str_ && part[1] == reg_) { \
			self.write(hex_); \
			return true; \
		}
	#define match2(str_, rl_, rr_, hex_) \
		if (part[0] == str_ && subpart[0] == rl_ && subpart[1] == rr_) { \
			self.write(hex_); \
			return true; \
		}
	
	// single register
	match("asl",  "a",   0x1c)
	match("daa",  "a",   0xdf)
	match("das",  "a",   0xbe)
	match("dec",  "a",   0x9c)
	match("dec",  "x",   0x1d)
	match("dec",  "y",   0xdc)
	match("inc",  "a",   0xbc)
	match("inc",  "x",   0x3d)
	match("inc",  "y",   0xfc)
	match("lsr",  "a",   0x5c)
	match("mul",  "ya",  0xcf)
	match("pop",  "a",   0xae)
	match("pop",  "x",   0xce)
	match("pop",  "y",   0xee)
	match("pop",  "psw", 0x83)
	match("push", "a",   0x2d)
	match("push", "psw", 0x0d)
	match("push", "x",   0x4d)
	match("push", "y",   0x6d)
	match("rol",  "a",   0x3c)
	match("ror",  "a",   0x7c)
	match("xcn",  "a",   0x9f)
	
	// register to register
	match2("adc", "a",    "(x)",  0x86)
	match2("adc", "(x)",  "(y)",  0x99)
	match2("and", "a",    "(x)",  0x26)
	match2("and", "(x)",  "(y)",  0x39)
	match2("cmp", "a",    "(x)",  0x66)
	match2("cmp", "(x)",  "(y)",  0x79)
	match2("div", "ya",   "x",    0x9e)
	match2("eor", "a",    "(x)",  0x46)
	match2("eor", "(x)",  "(y)",  0x59)
	match2("mov", "a",    "x",    0x7d)
	match2("mov", "a",    "(x)",  0xe6)
	match2("mov", "a",    "(x)+", 0xbf)
	match2("mov", "a",    "y",    0xdd)
	match2("mov", "sp",   "x",    0xbd)
	match2("mov", "x",    "a",    0x5d)
	match2("mov", "x",    "sp",   0x9d)
	match2("mov", "(x)",  "a",    0xc6)
	match2("mov", "(x)+", "a",    0xaf)
	match2("mov", "y",    "a",    0xfd)
	match2("or",  "(x)",  "(y)",  0x19)
	match2("or",  "a",    "(x)",  0x06)
	match2("sbc", "a",    "(x)",  0xa6)
	match2("sbc", "(x)",  "(y)",  0xb9)
	
	#undef match
	#undef match2
	return false;
}

bool xkasSPC700::assemble_indirect_x() {
	#define matchl(str_, hex_) \
		if(part[0] == str_ && subpart[0] == "a" && !force_word) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchs(str_, hex_) \
		if(part[0] == str_ && subpart[1] == "a" && !force_word) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchw(str_, hex_) \
		if(part[0] == str_ && !force_byte) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	
	// load
	matchl("adc", 0x87)
	matchl("and", 0x27)
	matchl("cmp", 0x67)
	matchl("eor", 0x47)
	matchl("mov", 0xe7)
	matchl("or",  0x07)
	matchl("sbc", 0xa7)
	
	// store
	matchs("mov", 0xc7)
	
	// jump
	matchw("jmp", 0x1f)
	
	#undef matchl
	#undef matchs
	#undef matchw
	return false;
}

bool xkasSPC700::assemble_indirect_y() {
	#define matchl(str_, hex_) \
		if(part[0] == str_ && subpart[0] == "a" && !force_word) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchs(str_, hex_) \
		if(part[0] == str_ && subpart[1] == "a" && !force_word) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	
	// load
	matchl("adc", 0x97)
	matchl("and", 0x37)
	matchl("cmp", 0x77)
	matchl("eor", 0x57)
	matchl("mov", 0xf7)
	matchl("or",  0x17)
	matchl("sbc", 0xb7)
	
	// store
	matchs("mov", 0xd7)
	
	#undef matchl
	#undef matchs
	return false;
}

bool xkasSPC700::assemble_addr_x() {
	if(force_byte) return false;
	#define matchl(str_, hex_) \
		if(part[0] == str_ && subpart[0] == "a") { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	#define matchs(str_, hex_) \
		if(part[0] == str_ && subpart[1] == "a") { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	
	// load
	matchl("adc", 0x95)
	matchl("and", 0x35)
	matchl("cmp", 0x75)
	matchl("eor", 0x55)
	matchl("mov", 0xf5)
	matchl("or",  0x15)
	matchl("sbc", 0xb5)
	
	// store
	matchs("mov", 0xd5)
	
	#undef matchl
	#undef matchs
	return false;
}

bool xkasSPC700::assemble_dp_x() {
	if(force_word) return false;
	#define matchl(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[0] == reg_) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchs(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[1] == reg_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchb(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	
	// load
	matchl("adc", "a", 0x94)
	matchl("and", "a", 0x34)
	matchl("cmp", "a", 0x74)
	matchl("eor", "a", 0x54)
	matchl("mov", "a", 0xf4)
	matchl("mov", "y", 0xfb)
	matchl("or",  "a", 0x14)
	matchl("sbc", "a", 0xb4)
	
	// store
	matchs("mov", "a", 0xd4)
	matchs("mov", "y", 0xdb)
	
	// direct page
	matchb("asl", 0x1b)
	matchb("dec", 0x9b)
	matchb("inc", 0xbb)
	matchb("lsr", 0x5b)
	matchb("rol", 0x3b)
	matchb("ror", 0x7b)
	
	#undef matchl
	#undef matchs
	#undef matchb
	return false;
}

bool xkasSPC700::assemble_addr_y() {
	if(force_byte) return false;
	#define matchl(str_, hex_) \
		if(part[0] == str_ && subpart[0] == "a") { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	#define matchs(str_, hex_) \
		if(part[0] == str_ && subpart[1] == "a") { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	
	// load
	matchl("adc", 0x96)
	matchl("and", 0x36)
	matchl("cmp", 0x76)
	matchl("eor", 0x56)
	matchl("mov", 0xf6)
	matchl("or",  0x16)
	matchl("sbc", 0xb6)
	
	// store
	matchs("mov", 0xd6)
	
	#undef matchl
	#undef matchs
	return false;
}

bool xkasSPC700::assemble_dp_y() {
	if(force_word) return false;
	
	if (part[0] == "mov" && subpart[0] == "x") {
		self.write(0xd9);
		self.write(self.decode(subpart[1]));
		return true;
	} else if (part[0] == "mov" && subpart[1] == "x") {
		self.write(0xf9);
		self.write(self.decode(subpart[0]));
		return true;
	}
	
	return false;
}

bool xkasSPC700::assemble_addr() {
	if(force_byte) return false;
	#define matchl(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[0] == reg_) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	#define matchs(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[1] == reg_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	#define matchw(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			self.write(n >> 8); \
			return true; \
		}
	// load
	matchl("adc", "a", 0x85)
	matchl("and", "a", 0x25)
	matchl("cmp", "a", 0x65)
	matchl("cmp", "x", 0x1e)
	matchl("cmp", "y", 0x5e)
	matchl("eor", "a", 0x45)
	matchl("mov", "a", 0xe5)
	matchl("mov", "x", 0xe9)
	matchl("mov", "y", 0xec)
	matchl("or",  "a", 0x05)
	matchl("sbc", "a", 0xa5)
	
	// store
	matchs("mov", "a", 0xc5)
	matchs("mov", "x", 0xc9)
	matchs("mov", "y", 0xcc)
	
	// address only
	matchw("asl",  0x0c)
	matchw("call", 0x3f)
	matchw("dec",  0x8c)
	matchw("inc",  0xac)
	matchw("jmp",  0x5f)
	matchw("lsr",  0x4c)
	matchw("rol",  0x2c)
	matchw("ror",  0x6c)
	matchw("tclr", 0x4e) matchw("tclr1", 0x4e)
	matchw("tset", 0x0e) matchw("tset1", 0x0e)	
	#undef matchl
	#undef matchs
	#undef matchw
	return false;
}

bool xkasSPC700::assemble_dp() {
	if(force_word) return false;
	#define matchl(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[0] == reg_) { \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchs(str_, reg_, hex_) \
		if(part[0] == str_ && subpart[1] == reg_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define matchb(str_, hex_) \
		if(part[0] == str_) { \
			unsigned n = self.decode(subpart[0]); \
			self.write(hex_); \
			self.write(n); \
			return true; \
		}
	#define match2(str_, hex_) \
		if(part[0] == str_) { \
			unsigned m = self.decode(subpart[0]); \
			unsigned n = self.decode(subpart[1]); \
			self.write(hex_); \
			self.write(n); \
			self.write(m); \
			return true; \
		}
	
	// load
	matchl("adc",  "a",  0x84)
	matchl("addw", "ya", 0x7a)
	matchl("and",  "a",  0x24)
	matchl("cmp",  "a",  0x64)
	matchl("cmp",  "x",  0x3e)
	matchl("cmp",  "y",  0x7e)
	matchl("cmpw", "ya", 0x5a)
	matchl("eor",  "a",  0x44)
	matchl("or",   "a",  0x04)
	matchl("mov",  "a",  0xe4)
	matchl("mov",  "x",  0xeb)
	matchl("mov",  "y",  0xf8)
	matchl("movw", "ya", 0xba)
	matchl("sbc",  "a",  0xa4)
	matchl("subw", "ya", 0x9a)
	
	// store
	matchs("mov",  "a",  0xc4)
	matchs("mov",  "x",  0xd8)
	matchs("mov",  "y",  0xcb)
	matchs("movw", "ya", 0xda)
	
	// single byte
	matchb("asl",  0x0b)
	matchb("clr0", 0x12)
	matchb("clr1", 0x32)
	matchb("clr2", 0x52)
	matchb("clr3", 0x72)
	matchb("clr4", 0x92)
	matchb("clr5", 0xb2)
	matchb("clr6", 0xd2)
	matchb("clr7", 0xf2)
	matchb("dec",  0x8b)
	matchb("decw", 0x1a)
	matchb("inc",  0xab)
	matchb("incw", 0x3a)
	matchb("lsr",  0x4b)
	matchb("pcall", 0x4f)
	matchb("rol",  0x2b)
	matchb("ror",  0x6b)
	matchb("set0", 0x02)
	matchb("set1", 0x22)
	matchb("set2", 0x42)
	matchb("set3", 0x62)
	matchb("set4", 0x82)
	matchb("set5", 0xa2)
	matchb("set6", 0xc2)
	matchb("set7", 0xe2)
	
	// mem to mem
	match2("adc", 0x89)
	match2("and", 0x29)
	match2("cmp", 0x69)
	match2("eor", 0x49)
	match2("mov", 0xfa)
	match2("or",  0x09)
	match2("sbc", 0xa9)
	#undef matchl
	#undef matchs
	#undef matchb
	#undef match2
	return false;
}

bool xkasSPC700::assemble_branch() {
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
	#define matchb(str_, hex_) \
		if (part[0] == str_ && subpart.size() == 2) { \
			int m = self.decode(subpart[0]); \
			int n = self.decode(subpart[1]); \
			if(!subpart[1].wildcard("$??")) { \
				n = n - (self.pc() + 2); \
			} \
			if(self.pass == 2 && (n < -128 || n > 127)) { \
				self.error = "branch target out of bounds"; \
				return false; \
			} \
			self.write(hex_); \
			self.write(m); \
			self.write(n); \
			return true; \
		}
	
	// simple branches
	match("bpl", 0x10)
	match("bra", 0x2f)
	match("bmi", 0x30)
	match("bvc", 0x50)
	match("bvs", 0x70)
	match("bcc", 0x90)
	match("bcs", 0xb0)
	match("bne", 0xd0)
	match("beq", 0xf0)
	
	// direct page branches
	matchb("bbc0", 0x13)
	matchb("bbc1", 0x33)
	matchb("bbc2", 0x53)
	matchb("bbc3", 0x73)
	matchb("bbc4", 0x93)
	matchb("bbc5", 0xb3)
	matchb("bbc6", 0xd3)
	matchb("bbc7", 0xf3)
	matchb("bbs0", 0x03)
	matchb("bbs1", 0x23)
	matchb("bbs2", 0x43)
	matchb("bbs3", 0x63)
	matchb("bbs4", 0x83)
	matchb("bbs5", 0xa3)
	matchb("bbs6", 0xc3)
	matchb("bbs7", 0xe3)
	
	// two stupid special cases
	// cbne $dp+x, label
	if (subpart[0].wildcard("*+x")) {
		subpart[0].rtrim<1>("+x");
		matchb("cbne", 0xde)
	}
	// dbnz y, label
	else if (subpart[0] == "y") {
		part[1] = subpart[1];
		match("dbnz", 0xfe)
	}
	
	matchb("cbne", 0x2e)
	matchb("dbnz", 0x6e)
	#undef match
	#undef matchb
	return false;
}

bool xkasSPC700::assemble_bit() {
	// assemble the bitwise instructions with funky syntax
	// (SET1 $dp, bit and CLR1 $dp, bit are currently handled TASM-style
	//  by assemble_dp())
	#define match(str_, part_, hex_) \
		if (part[0] == str_) { \
			unsigned mem = self.decode(subpart[part_]); \
			unsigned bit = self.decode(subpart[part_ + 1]); \
			if (mem & (7 << 13)) { \
				self.error = "address out of bounds ($0000 - $DFFF)"; \
				return false; \
			} else if (bit > 7) { \
				self.error = "bit out of bounds"; \
				return false; \
			} \
			mem |= bit << 13; \
			self.write(hex_); \
			self.write(mem); \
			self.write(mem >> 8); \
			return true; \
		}
	#define matchn(str_, part_, hex_) \
		if (subpart[part_].wildcard("/*")) { \
			subpart[part_].ltrim<1>("/"); \
			match(str_, part_, hex_)\
		}
	
	if (subpart.size() == 2) {
		match("not1", 0, 0xea)
		
	} else if (subpart[0] == "C" || subpart[0] == "c") {
		matchn("and1", 1, 0x6a)
		matchn("or1",  1, 0x2a)
		
		match("and1", 1, 0x4a)
		match("or1",  1, 0x2a)
		match("eor1", 1, 0x8a)
		match("mov1", 1, 0xaa)
		
	} else if (subpart[2] == "C" || subpart[2] == "c") {
		match("mov1", 0, 0xca)
	} 
	#undef match
	#undef matchn
	return false;
}

xkasSPC700::xkasSPC700(xkas &self) : xkasArch(self) {
}
