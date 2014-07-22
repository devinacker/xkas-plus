void xkasNone::init(unsigned pass) {}
unsigned xkasNone::archaddr(unsigned addr) { return addr; }
unsigned xkasNone::fileaddr(unsigned addr) { return addr; }
bool xkasNone::assemble_command(string&) { return false; }
xkasNone::xkasNone(xkas &self) : xkasArch(self) {}
