struct xkasSNESCPU : xkasArch {
  void init(unsigned pass);
  unsigned archaddr(unsigned fileaddr);
  unsigned fileaddr(unsigned archaddr);
  bool assemble_command(string&);
  xkasSNESCPU(xkas &self);

private:
  enum {
    NoROM,
    LoROM,
    HiROM,
  } mapper;
  bool header;
  bool force_byte, force_word, force_long;
  lstring part, subpart;

  void force_test();

  bool assemble_direct();
  bool assemble_const();
  bool assemble_indirect_x();
  bool assemble_indirect_y();
  bool assemble_indirect();
  bool assemble_indirect_long_y();
  bool assemble_indirect_long();
  bool assemble_sr();
  bool assemble_sry();
  bool assemble_move();
  bool assemble_long_x();
  bool assemble_addr_x();
  bool assemble_dp_x();
  bool assemble_addr_y();
  bool assemble_dp_y();
  bool assemble_long();
  bool assemble_addr();
  bool assemble_dp();
  bool assemble_branch();
};
