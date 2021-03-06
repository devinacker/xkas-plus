# xkas-plus (v14+1)

This is a fork of xkas v14, because I like xkas v14 and wanted to touch it up a bit.

As with the original, this code is in the public domain.

## What's new
- Ability to assemble directly to an IPS patch instead of a binary (with the `-ips` switch)
- New architecture options:
  - `6502` / `nes.cpu` - MOS 6502
  - `65c02` - WDC 65C02 (untested)
  - `65ce02` - CSG 65CE02 (untested)
  - `huc6280` / `pce.cpu` / `tg16.cpu` - Hudson HuC6280 (untested)
  - `spc700` / `snes.smp` - Sony SPC700
    - See `test/test_spc.asm` for syntax examples
  - `65816` - identical to `snes.cpu`
- Ability to export defines and labels to a separate file (with the `-e filename` switch)  
  Supported output formats:
  - `*.asm` - assembly include
  - `*.ram.nl` - FCEUX debug symbols (labels only)
  - `*.vs` - VICE debug symbols (labels only)
- New directives (mostly for increased xkas v06 compatibility):
  - `fillbyte` / `padbyte` - specifies a default value for the `fill`, `fillto`, and `align` directives
    (these were originally two separate directives in v06, though I may make them distinct later)
  - `pad` - identical to `fillto`
  - `table` - allows you to specify a .tbl file to use for custom character encoding (up to four bytes per 8-bit character)
    - `cleartable` - restores the default encoding
  - `warnpc` - verifies that code is being assembled below a given address
  - `bank` - set current program bank (default `0`)
  - `banksize` - set size of program banks (default/maximum `$10000`)
    - These two directives control the mapping between physical and logical addresses. For example:  
    ```noheader
    banksize $2000
    bank 0
    org $8000 // assembles to file offset $0000
    org $9000 // assembles to file offset $1000
    bank 1
    org $8000 // assembles to file offset $2000
    org $9000 // assembles to file offset $3000
    org $a000 // assembles to file offset $2000 (not $4000!)
	```
	- Note that for `65816` / `snes.cpu`, the `bank` and `banksize` settings are only relevant when the standard `lorom`, `hirom`, etc. address mapping modes aren't in effect (so that you can work with less common addressing setups, like with the SA-1).
- Directives for `6502` / `nes.cpu`:
  - `header` / `noheader` - behave the same as their `snes.cpu` counterparts, but for a 16-byte iNES header instead of a 512-byte SNES copier header.
    - The default is `noheader`, so you can insert an iNES header manually before using the `header` directive for the actual ROM contents. The `header` directive does not generate an iNES header on its own.
    
- New directives for `65816` / `snes.cpu`:
  - `exlorom` / `exhirom` mapping modes

## Goals
- Support for more architectures (SuperFX, GB/Z80, etc)
  - Support for undocumented 65xx opcodes
- Some useful system-specific directives for the above
- Actual documentation of the above
- Some other useful(?) directives, including from older xkas versions
- Macros (another lost feature of xkas v06)
  - not quite the highest priority

## Why not bass?
bass was actually my original target for this project, but in using and modifying it I found a lot of bad flaws with the way bass parses code (think about the words "unintentionally significant whitespace" if you want an idea) that would have been a real pain to fix in an actually satisfactory way.

The table-based assembly in bass was an attractive feature that would have made it very simple to add support for most or all of the new architectures that I wanted, but the way it was implemented partially contributed to the above issue (and I don't know how well it would have worked for things like THUMB, which xkas v14 supports already, or pretty much any RISC architecture where opcodes and operands aren't byte-aligned).

## Why not xkas v06?
(a.k.a. the version almost everyone _actually_ uses)

v14's code is much cleaner and more easily extensible. That's more or less the only reason. Hopefully in re-adding some of v06's features I can bridge the gap a bit, but things like the new comment syntax will still keep it from being completely backwards compatible with v06.