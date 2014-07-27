/*
  xkas
  version: 0.14 (2010-10-19)
  author: byuu
  license: public domain
*/

#ifndef LIBXKAS_HPP
#define LIBXKAS_HPP

#include <nall/array.hpp>
#include <nall/file.hpp>
#include <nall/foreach.hpp>
#include <nall/function.hpp>
#include <nall/stdint.hpp>
#include <nall/string.hpp>
#include <nall/vector.hpp>
using namespace nall;

struct xkas;

struct xkasArch {
  virtual void init(unsigned pass) = 0;
  virtual unsigned archaddr(unsigned fileaddr) = 0;
  virtual unsigned fileaddr(unsigned archaddr) = 0;
  virtual bool assemble_command(string&) = 0;
  xkasArch(xkas &self) : self(self) {}
  xkas &self;
};

#include "arch/none.hpp"
#include "arch/gba.thumb.hpp"
#include "arch/snes.cpu.hpp"
#include "arch/6502.hpp"

struct xkas {
  enum Format { format_bin, format_IPS };
  bool open(const char *filename, unsigned fmt = format_bin);
  bool assemble(const char *filename);
  void close();
  xkas();

private:
  enum Pass { pass_read = 1, pass_assemble = 2 };
  unsigned pass;
  string error;
  string warning;

  //file manipulation
  unsigned format;
  file binary;
  void write(uint8_t data);
  
  //format-specific file things (for patches, etc)
  unsigned patch_addr;
  linear_vector<uint8_t> patch_data;
  void write_IPS();

  bool assemble_file(const char*);
  void assemble_defines(string&);
  bool assemble_command(string&);

  //arch
  enum Endian : unsigned { endian_lsb, endian_msb };
  unsigned endian;
  xkasArch *arch;
  xkasNone arch_none;
  xkasGBATHUMB arch_gba_thumb;
  xkasSNESCPU arch_snes_cpu;
  xkas6502 arch_6502;
  friend class xkasNone;
  friend class xkasGBATHUMB;
  friend class xkasSNESCPU;
  friend class xkas6502;

  struct Define {
    string name;
    string value;
  };

  struct Label {
    string name;
    unsigned offset;
  };

  //state
  unsigned pc() const;  //return program counter (base address)

  struct State {
    unsigned org;
    unsigned base;
    linear_vector<Define> define;
    linear_vector<Label> label;
    string active_namespace;
    Label active_label;
    unsigned plus_label_counter;
    unsigned minus_label_counter;
    uint64_t table[256];
    uint8_t fill_byte;
  } state;

  //tool
  int eval_integer(const char *&s);
  int eval(const char *&s, int depth = 0);
  int decode(const char *s);
  bool decode_label(string &s);
  bool find_label(string &name, unsigned &offset);
};

#endif
