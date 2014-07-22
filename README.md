# xkas

This is a fork of xkas v14, because I like xkas v14 and wanna touch it up a bit. I may rename it later.

## Goals
- Ability to assemble directly to a patch file (IPS, etc) instead of an actual binary
- Support for more architectures (6502/65SC02/65CE02, SPC700, SuperFX, GB/Z80, etc)
- Some useful system-specific directives for the above (to help manage NES ROM banks, etc)
- Actual documentation of the above
- Some useful(?) directives from older xkas versions (such as rep, fillbyte, pad, table, warnpc)
-- this means the return of support for character tables
- Macros (another lost feature of xkas v06)
-- not quite the highest priority

## Why not bass?
bass was actually my original target for this project, but in using and modifying it I found a lot of bad flaws with the way bass parses code (think about the words "unintentionally significant whitespace" if you want an idea) that would have been a real pain to fix in an actually satisfactory way.

The table-based assembly in bass was an attractive feature that would have made it very simple to add support for most or all of the new architectures that I wanted, but the way it was implemented partially contributed to the above issue (and I don't think it would have worked for things like THUMB, which xkas v14 supports already, or pretty much any RISC architecture where opcodes and operands aren't byte-aligned).

## Why not xkas v06?
(a.k.a. the version almost everyone _actually_ uses)

v14's code is much cleaner and more easily extensible. That's more or less the only reason. Hopefully in re-adding some of v06's features I can bridge the gap a bit, but things like the new comment syntax will still keep it from being completely backwards compatible with v06.