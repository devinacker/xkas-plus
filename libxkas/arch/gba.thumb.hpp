struct xkasGBATHUMB : xkasArch {
	void init(unsigned pass);
	unsigned archaddr(unsigned fileaddr);
	unsigned fileaddr(unsigned archaddr);
	bool assemble_command(string&);
	xkasGBATHUMB(xkas&);

private:
	lstring part, subpart;

	bool reg8 (const string&, unsigned&);
	bool reg16(const string&, unsigned&);

	bool assemble_01();
	bool assemble_02();
	bool assemble_03();
	bool assemble_04();
	bool assemble_05();
	bool assemble_06();
	bool assemble_07();
	bool assemble_08();
	bool assemble_09();
	bool assemble_10();
	bool assemble_11();
	bool assemble_12();
	bool assemble_13();
	bool assemble_14();
	bool assemble_15();
	bool assemble_16();
	bool assemble_17();
	bool assemble_18();
	bool assemble_19();

	bool assemble_p1();
};
