struct xkasHuC6280 : xkasArch {
	void init(unsigned pass);
	unsigned archaddr(unsigned fileaddr);
	unsigned fileaddr(unsigned archaddr);
	bool assemble_command(string&);
	xkasHuC6280(xkas &self);

private:
	bool force_byte, force_word;
	unsigned bank_size, bank;
	lstring part, subpart;

	void force_test();

	bool assemble_direct();
	bool assemble_const();
	bool assemble_indirect_x();
	bool assemble_indirect_y();
	bool assemble_indirect();
	bool assemble_move();
	bool assemble_test();
	bool assemble_addr_x();
	bool assemble_dp_x();
	bool assemble_addr_y();
	bool assemble_dp_y();
	bool assemble_addr();
	bool assemble_dp();
	bool assemble_branch();
};
