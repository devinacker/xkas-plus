struct xkasNone : xkasArch {
	void init(unsigned pass);
	unsigned archaddr(unsigned fileaddr);
	unsigned fileaddr(unsigned archaddr);
	bool assemble_command(string&);
	xkasNone(xkas &self);
};
