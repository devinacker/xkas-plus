# xkas

This is a fork of xkas v14, because I like xkas v14 and wanna touch it up a bit. I may rename it later.

## What's new
- Ability to assemble directly to an IPS patch instead of a binary (with the `-ips` switch)
- New architecture options:
  - `65816` - identical to `snes.cpu`
- New directives (mostly for increased xkas v06 compatibility):
  - `fillbyte` / `padbyte` - specifies a default value for the `fill`, `fillto`, and `align` directives
    (these were originally two separate directives in v06, though I may make them distinct later)
  - `pad` - identical to `fillto`
  - `table` - allows you to specify a .tbl file to use for custom character encoding (one byte per character only)
    - `cleartable` - restores the default encoding
  - `warnpc` - verifies that code is being assembled below a given address
- New directives for `65816` / `snes.cpu`:
  - `exlorom` / `exhirom` mapping modes

## Goals
- Support for more architectures (6502/65SC02/65CE02, SPC700, SuperFX, GB/Z80, etc)
- Some useful system-specific directives for the above (to help manage NES ROM banks, etc)
- Actual documentation of the above
- Some other useful(?) directives, including from older xkas versions
- Macros (another lost feature of xkas v06)
  - not quite the highest priority

## Why not bass?
bass was actually my original target for this project, but in using and modifying it I found a lot of bad flaws with the way bass parses code (think about the words "unintentionally significant whitespace" if you want an idea) that would have been a real pain to fix in an actually satisfactory way.

The table-based assembly in bass was an attractive feature that would have made it very simple to add support for most or all of the new architectures that I wanted, but the way it was implemented partially contributed to the above issue (and I don't think it would have worked for things like THUMB, which xkas v14 supports already, or pretty much any RISC architecture where opcodes and operands aren't byte-aligned).

## Why not xkas v06?
(a.k.a. the version almost everyone _actually_ uses)

v14's code is much cleaner and more easily extensible. That's more or less the only reason. Hopefully in re-adding some of v06's features I can bridge the gap a bit, but things like the new comment syntax will still keep it from being completely backwards compatible with v06.