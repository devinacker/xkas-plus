void xkasGBATHUMB::init(unsigned pass) {
}

unsigned xkasGBATHUMB::archaddr(unsigned addr) {
	return (addr & 0x07ffffff) + 0x08000000;
}

unsigned xkasGBATHUMB::fileaddr(unsigned addr) {
	return (addr - 0x08000000) & 0x07ffffff;
}

bool xkasGBATHUMB::assemble_command(string &s) {
	if(self.pc() & 1) {
		self.error = "PC must be aligned on 16-bit boundary";
		return false;
	}

	part.qsplit(" ", s);
	subpart.qsplit(",", part.size() == 2 ? part[1] : "");
	if(assemble_01()) return true;
	if(assemble_02()) return true;
	if(assemble_03()) return true;
	if(assemble_04()) return true;
	if(assemble_05()) return true;
	if(assemble_06()) return true;
	if(assemble_07()) return true;
	if(assemble_08()) return true;
	if(assemble_09()) return true;
	if(assemble_10()) return true;
	if(assemble_11()) return true;
	if(assemble_12()) return true;
	if(assemble_13()) return true;
	if(assemble_14()) return true;
	if(assemble_15()) return true;
	if(assemble_16()) return true;
	if(assemble_17()) return true;
	if(assemble_18()) return true;
	if(assemble_19()) return true;
	if(assemble_p1()) return true;
	return false;
}

bool xkasGBATHUMB::reg8(const string &s, unsigned &r) {
	if(s == "r0")  { r =  0; return true; }
	if(s == "r1")  { r =  1; return true; }
	if(s == "r2")  { r =  2; return true; }
	if(s == "r3")  { r =  3; return true; }
	if(s == "r4")  { r =  4; return true; }
	if(s == "r5")  { r =  5; return true; }
	if(s == "r6")  { r =  6; return true; }
	if(s == "r7")  { r =  7; return true; }
	return false;
}

bool xkasGBATHUMB::reg16(const string &s, unsigned &r) {
	if(s == "r0")  { r =  0; return true; }
	if(s == "r1")  { r =  1; return true; }
	if(s == "r2")  { r =  2; return true; }
	if(s == "r3")  { r =  3; return true; }
	if(s == "r4")  { r =  4; return true; }
	if(s == "r5")  { r =  5; return true; }
	if(s == "r6")  { r =  6; return true; }
	if(s == "r7")  { r =  7; return true; }
	if(s == "r8")  { r =  8; return true; }
	if(s == "r9")  { r =  9; return true; }
	if(s == "r10") { r = 10; return true; }
	if(s == "r11") { r = 11; return true; }
	if(s == "r12") { r = 12; return true; }
	if(s == "r13") { r = 13; return true; }
	if(s == "r14") { r = 14; return true; }
	if(s == "r15") { r = 15; return true; }

	//aliases
	if(s == "sp")  { r = 13; return true; }
	if(s == "lr")  { r = 14; return true; }
	if(s == "pc")  { r = 15; return true; }

	return false;
}

//move shifted register
//15-13 = 000b
//12-11 = opcode
//10-06 = offset
//05-03 = rs
//02-00 = rd
bool xkasGBATHUMB::assemble_01() {
	unsigned opcode, offset, rs, rd;

	if(subpart.size() != 3) return false;
	if(reg8(subpart[0], rd) == false) return false;
	if(reg8(subpart[1], rs) == false) return false;
	if(strbegin(subpart[2], "#") == false) return false;
	offset = self.decode(substr(subpart[2], 1));
			 if(part[0] == "lsl") opcode = 0;
	else if(part[0] == "lsr") opcode = 1;
	else if(part[0] == "asr") opcode = 2;
	else return false;

	uint16_t data = 0;
	data |= (0) << 13;
	data |= (opcode & 3) << 11;
	data |= (offset & 31) << 6;
	data |= (rs & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//add / subtract
//15-11 = 00011b
//10-09 = opcode
//08-06 = rn
//05-03 = rs
//02-00 = rd
bool xkasGBATHUMB::assemble_02() {
	unsigned opcode, rn, rs, rd;

	if(part[0] == "add" || part[0] == "sub") {
		if(subpart.size() != 3) return false;
		if(reg8(subpart[0], rd) == false) return false;
		if(reg8(subpart[1], rs) == false) return false;
		bool immediate = strbegin(subpart[2], "#");
		if(immediate == true) {
			rn = self.decode(substr(subpart[2], 1));
		} else {
			if(reg8(subpart[2], rn) == false) return false;
		}

				 if(part[0] == "add") opcode = 0 + (immediate ? 2 : 0);
		else if(part[0] == "sub") opcode = 1 + (immediate ? 2 : 0);
	} else if(part[0] == "mov") {
		//equivalent to add rd,rs,#0
		if(subpart.size() != 2) return false;
		opcode = 2;
		rn = 0;
		if(reg8(subpart[0], rd) == false) return false;
		if(reg8(subpart[1], rs) == false) return false;
	} else return false;

	uint16_t data = 0;
	data |= (3) << 11;
	data |= (opcode & 3) << 9;
	data |= (rn & 7) << 6;
	data |= (rs & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//move / compare / add subtract immediate
//15-13 = 001b
//12-11 = opcode
//10-08 = rd
//07-00 = imm
bool xkasGBATHUMB::assemble_03() {
	unsigned opcode, rd, imm;

	if(subpart.size() != 2) return false;
	if(reg8(subpart[0], rd) == false) return false;
	if(strbegin(subpart[1], "#") == false) return false;
	imm = self.decode(substr(subpart[1], 1));
			 if(part[0] == "mov") opcode = 0;
	else if(part[0] == "cmp") opcode = 1;
	else if(part[0] == "add") opcode = 2;
	else if(part[0] == "sub") opcode = 3;
	else return false;

	uint16_t data = 0;
	data |= (1) << 13;
	data |= (opcode & 3) << 11;
	data |= (rd & 7) << 8;
	data |= (imm & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//ALU operations
//15-10 = 010000b
//09-06 = opcode
//05-03 = rs
//02-00 = rd
bool xkasGBATHUMB::assemble_04() {
	unsigned opcode, rs, rd;

	if(subpart.size() != 2) return false;
	if(reg8(subpart[0], rd) == false) return false;
	if(reg8(subpart[1], rs) == false) return false;
			 if(part[0] == "and") opcode =  0;
	else if(part[0] == "eor") opcode =  1;
	else if(part[0] == "lsl") opcode =  2;
	else if(part[0] == "lsr") opcode =  3;
	else if(part[0] == "asr") opcode =  4;
	else if(part[0] == "adc") opcode =  5;
	else if(part[0] == "sbc") opcode =  6;
	else if(part[0] == "ror") opcode =  7;
	else if(part[0] == "tst") opcode =  8;
	else if(part[0] == "neg") opcode =  9;
	else if(part[0] == "cmp") opcode = 10;
	else if(part[0] == "cmn") opcode = 11;
	else if(part[0] == "orr") opcode = 12;
	else if(part[0] == "mul") opcode = 13;
	else if(part[0] == "bic") opcode = 14;
	else if(part[0] == "mvn") opcode = 15;
	else return false;

	uint16_t data = 0;
	data |= (16) << 10;
	data |= (opcode & 15) << 6;
	data |= (rs & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//high register operations / branch exchange
//15-10 = 010001b
//09-08 = opcode
//07    = msbd
//06    = msbs
//05-03 = rs
//02-00 = rd
bool xkasGBATHUMB::assemble_05() {
	unsigned opcode, rs, rd;

	if(part[0] == "add" || part[0] == "cmp" || part[0] == "mov") {
		if(subpart.size() != 2) return false;
		if(reg16(subpart[0], rd) == false) return false;
		if(reg16(subpart[1], rs) == false) return false;
		if(rd < 8 && rs < 8) return false; //either rd or rs must be >= r8
				 if(part[0] == "add") opcode = 0;
		else if(part[0] == "cmp") opcode = 1;
		else if(part[0] == "mov") opcode = 2;
	} else if(part[0] == "nop") {
		//equivalent to mov r8,r8
		if(subpart.size() != 1) return false;
		if(subpart[0] != "") return false;
		opcode = 2;
		rs = 8;
		rd = 8;
	} else if(part[0] == "bx" || part[0] == "blx") {
		opcode = 3;
		if(subpart.size() != 1) return false;
		if(reg16(subpart[0], rs) == false) return false;
		if(part[0] == "bx") {
			rd = 0;
		} else { //part[0] == "blx"
			rd = 8;
			if(rs == 15) return false; //blx cannot use r15
		}
	} else return false;

	uint16_t data = 0;
	data |= (17) << 10;
	data |= (opcode & 3) << 8;
	data |= ((rd >> 3) & 1) << 7;
	data |= ((rs >> 3) & 1) << 6;
	data |= (rs & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load PC-relative
//15-11 = 01001b
//10-08 = rd
//07-00 = imm (scale 4)
bool xkasGBATHUMB::assemble_06() {
	unsigned rd, imm;

	if(subpart.size() != 3) return false;
	if(reg8(subpart[0], rd) == false) return false;
	if(subpart[1] != "[pc") return false;
	if(!strbegin(subpart[2], "#")) return false;
	if(!strend(subpart[2], "]")) return false;
	if(part[0] != "ldr") return false;

	string temp = substr(subpart[2], 1);
	temp.rtrim<1>("]");
	imm = self.decode(temp);

	if(imm & 3) {
		self.error = "immediate must be 32-bit aligned";
		return false;
	}
	imm >>= 2;

	uint16_t data = 0;
	data |= (9) << 11;
	data |= (rd & 7) << 8;
	data |= (imm & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load / store with register offset
//15-12 = 0101b
//11-10 = opcode
//09    = 0b
//08-06 = ro
//05-03 = rb
//02-00 = rd
bool xkasGBATHUMB::assemble_07() {
	unsigned opcode, ro, rb, rd;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(!strbegin(subpart[1], "[")) return false;
	if(!strend(subpart[2], "]")) return false;

	string trb = subpart[1];
	trb.ltrim<1>("[");
	if(!reg8(trb, rb)) return false;
	string tro = subpart[2];
	tro.rtrim<1>("]");
	if(!reg8(tro, ro)) return false;

			 if(part[0] == "str")  opcode = 0;
	else if(part[0] == "strb") opcode = 1;
	else if(part[0] == "ldr")  opcode = 2;
	else if(part[0] == "ldrb") opcode = 3;
	else return false;

	uint16_t data = 0;
	data |= (5) << 12;
	data |= (opcode & 3) << 10;
	data |= (0) << 9;
	data |= (ro & 7) << 6;
	data |= (rb & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load / store sign-extended byte / halfword
//15-12 = 0101b
//11-10 = opcode
//09    = 1b
//08-06 = ro
//05-03 = rb
//02-00 = rd
bool xkasGBATHUMB::assemble_08() {
	unsigned opcode, ro, rb, rd;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(!strbegin(subpart[1], "[")) return false;
	if(!strend(subpart[2], "]")) return false;

	string trb = subpart[1];
	trb.ltrim<1>("[");
	if(!reg8(trb, rb)) return false;
	string tro = subpart[2];
	tro.rtrim<1>("]");
	if(!reg8(tro, ro)) return false;

			 if(part[0] == "strh") opcode = 0;
	else if(part[0] == "ldsb") opcode = 1;
	else if(part[0] == "ldrh") opcode = 2;
	else if(part[0] == "ldsh") opcode = 3;
	else return false;

	uint16_t data = 0;
	data |= (5) << 12;
	data |= (opcode & 3) << 10;
	data |= (1) << 9;
	data |= (ro & 7) << 6;
	data |= (rb & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load / store with immediate offset
//15-13 = 011b
//12-11 = opcode
//10-06 = imm (8-bit step 1, 32-bit step 4)
//05-03 = rb
//02-00 = rd
bool xkasGBATHUMB::assemble_09() {
	unsigned opcode, imm, rb, rd;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(!strbegin(subpart[1], "[")) return false;
	if(!strbegin(subpart[2], "#")) return false;
	if(!strend(subpart[2], "]")) return false;

	string trb = subpart[1];
	trb.ltrim<1>("[");
	if(!reg8(trb, rb)) return false;
	string timm = subpart[2];
	timm.ltrim<1>("#");
	timm.rtrim<1>("]");
	imm = self.decode(timm);

			 if(part[0] == "str")  opcode = 0;
	else if(part[0] == "ldr")  opcode = 1;
	else if(part[0] == "strb") opcode = 2;
	else if(part[0] == "ldrb") opcode = 3;
	else return false;

	if(opcode <= 1) {
		if(imm & 3) {
			self.error = "immediate must be 32-bit aligned";
			return false;
		}
		imm >>= 2;
	}

	uint16_t data = 0;
	data |= (3) << 13;
	data |= (opcode & 3) << 11;
	data |= (imm & 31) << 6;
	data |= (rb & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load / store halfword
//15-12 = 1000b
//11    = opcode
//10-06 = imm (step 2)
//05-03 = rb
//02-00 = rd
bool xkasGBATHUMB::assemble_10() {
	unsigned opcode, imm, rb, rd;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(!strbegin(subpart[1], "[")) return false;
	if(!strbegin(subpart[2], "#")) return false;
	if(!strend(subpart[2], "]")) return false;

	string trb = subpart[1];
	trb.ltrim<1>("[");
	if(!reg8(trb, rb)) return false;
	string timm = subpart[2];
	timm.ltrim<1>("#");
	timm.rtrim<1>("]");
	imm = self.decode(timm);

			 if(part[0] == "strh") opcode = 0;
	else if(part[0] == "ldrh") opcode = 1;
	else return false;

	if(imm & 1) {
		self.error = "immediate must be 16-bit aligned";
		return false;
	}
	imm >>= 1;

	uint16_t data = 0;
	data |= (8) << 12;
	data |= (opcode & 1) << 11;
	data |= (imm & 31) << 6;
	data |= (rb & 7) << 3;
	data |= (rd & 7) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//load / store SP-relative
//15-12 = 1001b
//11    = opcode
//10-08 = rd
//07-00 = imm (step 4)
bool xkasGBATHUMB::assemble_11() {
	unsigned opcode, rd, imm;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(subpart[1] != "[sp") return false;
	if(!strbegin(subpart[2], "#")) return false;
	if(!strend(subpart[2], "]")) return false;

	string timm = subpart[2];
	timm.ltrim<1>("#");
	timm.rtrim<1>("]");
	imm = self.decode(timm);

			 if(part[0] == "str") opcode = 0;
	else if(part[0] == "ldr") opcode = 1;
	else return false;

	if(imm & 3) {
		self.error = "immediate must be 32-bit aligned";
		return false;
	}
	imm >>= 2;

	uint16_t data = 0;
	data |= (9) << 12;
	data |= (opcode & 1) << 11;
	data |= (rd & 7) << 8;
	data |= (imm & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//get relative address
//15-12 = 1010b
//11    = opcode
//10-08 = rd
//07-00 = imm (step 4)
bool xkasGBATHUMB::assemble_12() {
	unsigned opcode, rd, imm;

	if(subpart.size() != 3) return false;
	if(!reg8(subpart[0], rd)) return false;
	if(subpart[1] != "pc" && subpart[1] != "sp") return false;
	if(!strbegin(subpart[2], "#")) return false;
	imm = self.decode(substr(subpart[2], 1));

	if(part[0] != "add") return false;
	opcode = (subpart[1] == "sp"); //0 = "pc", 1 = "sp"

	if(imm & 3) {
		self.error = "immediate must be 32-bit aligned";
		return false;
	}
	imm >>= 2;

	uint16_t data = 0;
	data |= (10) << 12;
	data |= (opcode & 1) << 11;
	data |= (rd & 7) << 8;
	data |= (imm & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//add offset to stack pointer
//15-08 = 10110000b
//07    = sign
//06-00 = imm (step 4)
bool xkasGBATHUMB::assemble_13() {
	unsigned sign, imm;

	if(subpart.size() != 2) return false;
	if(subpart[0] != "sp") return false;
	if(!strbegin(subpart[1], "#")) return false;
	int value = self.decode(substr(subpart[1], 1));
	sign = (value < 0);
	imm = abs(value);
	if(part[0] != "add") return false;

	if(imm & 3) {
		self.error = "immediate must be 32-bit aligned";
		return false;
	}
	imm >>= 2;

	uint16_t data = 0;
	data |= (0xb0) << 8;
	data |= (sign) << 7;
	data |= (imm & 127) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//push / pop registers
//15-12 = 1011b
//11    = opcode
//10-09 = 10b
//08    = pc / lr bit
//07-00 = rlist
bool xkasGBATHUMB::assemble_14() {
	unsigned opcode, pclr = 0, rlist = 0;

	if(!strbegin(part[1], "{")) return false;
	if(!strend(part[1], "}")) return false;
	if(part[0] != "push" && part[0] != "pop") return false;

	string temp = part[1];
	temp.ltrim<1>("{");
	temp.rtrim<1>("}");
	lstring list;
	list.split(",", temp);

	for(unsigned i = 0; i < list.size(); i++) {
		if(strpos(list[i], "-")) {
			lstring range;
			range.split("-", list[i]);
			if(range.size() != 2) return false;
			unsigned rfrom, rto;
			if(!reg8(range[0], rfrom)) return false;
			if(!reg8(range[1], rto)) return false;
			if(rfrom >= rto) return false;

			for(unsigned l = rfrom; l <= rto; l++) rlist |= (1 << l);
		} else if(part[0] == "push" && list[i] == "lr") {
			pclr = 1;
		} else if(part[0] == "pop" && list[i] == "pc") {
			pclr = 1;
		} else {
			unsigned rn;
			if(!reg8(list[i], rn)) return false;
			rlist |= (1 << rn);
		}
	}

	opcode = (part[0] == "pop"); //0 = "push", 1 = "pop"

	uint16_t data = 0;
	data |= (11) << 12;
	data |= (opcode & 1) << 11;
	data |= (2) << 9;
	data |= (pclr & 1) << 8;
	data |= (rlist & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//multiple load / store
//15-12 = 1100b
//11    = opcode
//10-08 = rb
//07-00 = rlist
bool xkasGBATHUMB::assemble_15() {
	unsigned opcode, rb, rlist;

	if(subpart.size() < 2) return false;
	if(!strend(subpart[0], "!")) return false;
	string rtemp = subpart[0];
	rtemp.rtrim<1>("!");
	if(!reg8(rtemp, rb)) return false;
	if(part[0] != "ldmia" && part[0] != "stmia") return false;

	string temp = part[1];
	temp.ltrim<1>(subpart[0]);
	temp.ltrim<1>(",");
	if(!strbegin(temp, "{")) return false;
	if(!strend(temp, "}")) return false;
	temp.ltrim<1>("{");
	temp.rtrim<1>("}");
	lstring list;
	list.split(",", temp);

	for(unsigned i = 0; i < list.size(); i++) {
		if(strpos(list[i], "-")) {
			lstring range;
			range.split("-", list[i]);
			if(range.size() != 2) return false;
			unsigned rfrom, rto;
			if(!reg8(range[0], rfrom)) return false;
			if(!reg8(range[1], rto)) return false;
			if(rfrom >= rto) return false;

			for(unsigned l = rfrom; l <= rto; l++) rlist |= (1 << l);
		} else {
			unsigned rn;
			if(!reg8(list[i], rn)) return false;
			rlist |= (1 << rn);
		}
	}

	opcode = (part[0] == "ldmia"); //0 = "stmia", 1 = "ldmia"

	uint16_t data = 0;
	data |= (12) << 12;
	data |= (opcode & 1) << 11;
	data |= (rb & 7) << 8;
	data |= (rlist & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//conditional branch
//15-12 = 1101b
//11-08 = opcode
//07-00 = signed offset (step 2)
bool xkasGBATHUMB::assemble_16() {
	unsigned opcode;
	signed offset;

	if(subpart.size() != 1) return false;

			 if(part[0] == "beq") opcode =  0;
	else if(part[0] == "bne") opcode =  1;
	else if(part[0] == "bcs") opcode =  2;
	else if(part[0] == "bcc") opcode =  3;
	else if(part[0] == "bmi") opcode =  4;
	else if(part[0] == "bpl") opcode =  5;
	else if(part[0] == "bvs") opcode =  6;
	else if(part[0] == "bvc") opcode =  7;
	else if(part[0] == "bhi") opcode =  8;
	else if(part[0] == "bls") opcode =  9;
	else if(part[0] == "bge") opcode = 10;
	else if(part[0] == "blt") opcode = 11;
	else if(part[0] == "bgt") opcode = 12;
	else if(part[0] == "ble") opcode = 13;
	else return false;

	offset = self.decode(subpart[0]);
	offset -= self.pc() + 4;
	offset >>= 1;
	if(self.pass == 2) {
		if(offset < -256 || offset > 255) {
			self.error = "branch target out of bounds";
			return false;
		}
	}

	uint16_t data = 0;
	data |= (13) << 12;
	data |= (opcode & 15) << 8;
	data |= (offset & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//software interrupt and breakpoint
//15-08 = 11011111b (swi), 10111110 (bkpt)
//07-00 = imm
bool xkasGBATHUMB::assemble_17() {
	unsigned opcode, imm;

	if(subpart.size() != 1) return false;
	if(!strbegin(subpart[0], "#")) return false;
	imm = self.decode(substr(subpart[0], 1));

			 if(part[0] == "swi")  opcode = 0xdf;
	else if(part[0] == "bkpt") opcode = 0xbe;
	else return false;

	uint16_t data = 0;
	data |= (opcode & 255) << 8;
	data |= (imm & 255) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//unconditional branch
//15-11 = 11100b
//10-00 = signed offset (step 2)
bool xkasGBATHUMB::assemble_18() {
	signed offset;

	if(subpart.size() != 1) return false;
	if(part[0] != "b") return false;

	offset = self.decode(subpart[0]);
	offset -= self.pc() + 4;
	offset >>= 1;
	if(self.pass == 2) {
		if(offset < -2048 || offset > 2047) {
			self.error = "branch target out of bounds";
			return false;
		}
	}

	uint16_t data = 0;
	data |= (28) << 11;
	data |= (offset & 2047) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

//long branch with link
//15-11 = 11110b
//10-00 = upper 11 offset bits
//15-11 = 11111b (bl), 11101b (blx)
//10-00 = lower 11 offset bits (step 2)
bool xkasGBATHUMB::assemble_19() {
	unsigned opcode;
	signed offset;

	if(subpart.size() != 1) return false;
	if(part[0] != "bl" && part[0] != "blx") return false;

	offset = self.decode(subpart[0]);
	offset -= self.pc() + 4;
	offset >>= 1;
	if(self.pass == 2) {
		if(offset < -0x400000 || offset > 0x3fffff) {
			self.error = "branch target out of bounds";
			return false;
		}
	}

	opcode = (part[0] == "bl" ? 31 : 29); //"bl" = 31, "blx" = 29

	uint16_t data = 0;
	data |= (30) << 11;
	data |= ((offset >> 11) & 2047) << 0;
	self.write(data);
	self.write(data >> 8);

	data = 0;
	data |= (opcode & 31) << 11;
	data |= (offset & 2047) << 0;
	self.write(data);
	self.write(data >> 8);
	return true;
}

/* pseudo-opcodes */

//ldr rN,=#imm -> ldr rN,[pc,#0]; b +; dd 0ximm; +
bool xkasGBATHUMB::assemble_p1() {
	unsigned rd, imm;

	if(subpart.size() != 2) return false;
	if(!reg8(subpart[0], rd)) return false;  
	if(!strbegin(subpart[1], "=#")) return false;

	string temp = subpart[1];
	temp.ltrim<1>("=#");
	imm = self.decode(temp);

	if(part[0] != "ldr") return false;

	//align PC so that immediate data is 32-bit aligned
	if(!(self.pc() & 2)) {
		//ldr rN,[pc,#0]
		self.write(0x00);
		self.write(0x48 | rd);
		//b +4
		self.write(0x01);
		self.write(0xe0);
		//self.pc() & 3 == 0
	} else {
		//ldr rN,[pc,#4]
		self.write(0x01);
		self.write(0x48 | rd);
		//b +6
		self.write(0x02);
		self.write(0xe0);
		//nop
		self.write(0xc0);
		self.write(0x46);
		//self.pc() & 3 == 0
	}

	self.write(imm);
	self.write(imm >> 8);
	self.write(imm >> 16);
	self.write(imm >> 24);

	return true;
}

xkasGBATHUMB::xkasGBATHUMB(xkas &self) : xkasArch(self) {
}
