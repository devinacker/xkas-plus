#include "libxkas/libxkas.hpp"

int main(int argc, char **argv) {
	xkas::Format format = xkas::format_bin;
	string outputFilename, exportFilename;
	lstring inputFilename;

	for(unsigned i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-o") && argc >= i + 1) {
			outputFilename = argv[++i];
		
		} else if(!strcmp(argv[i], "-e") && argc >= i + 1) {
			exportFilename = argv[++i];
		
		} else if (!strcmp(argv[i], "-ips")) {
			format = xkas::format_IPS;
		
		} else if(!strbegin(argv[i], "-")) {
			inputFilename.append(argv[i]);
		
		} else {
			print("unrecognized option: ", argv[i], "\n");
			return 1;
		}
	}

	if(outputFilename == "" || inputFilename.size() == 0) {
		print("xkas-plus v14+1\n");
		print("usage: xkas -o output.bin input.asm\n");
		return 0;
	}

	xkas as;
	unsigned err = 0;
	as.open(outputFilename, format);
	foreach(filename, inputFilename) {
		if(as.assemble(filename) == false) {
		  err = 1;
		  break;
		}
	}
	if (!err && exportFilename != "") {
		as.exportFile(exportFilename);
	}
	as.close();
	return err;
}
