#include "libxkas/libxkas.hpp"

int main(int argc, char **argv) {
  string outputFilename;
  lstring inputFilename;

  for(unsigned i = 1; i < argc; i++) {
    if(!strcmp(argv[i], "-o") && argc >= i + 1) {
      outputFilename = argv[++i];
    } else if(!strbegin(argv[i], "-")) {
      inputFilename.append(argv[i]);
    } else {
      print("unrecognized option: ", argv[i], "\n");
      return 0;
    }
  }

  if(outputFilename == "" || inputFilename.size() == 0) {
    print("xkas v14\n");
    print("usage: xkas -o output.bin input.asm\n");
    return 0;
  }

  xkas as;
  as.open(outputFilename);
  foreach(filename, inputFilename) {
    if(as.assemble(filename) == false) break;
  }
  as.close();
  return 0;
}
