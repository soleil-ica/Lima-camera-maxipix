#include <string>
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "INIReader.h"
#include <assert.h>

using namespace std;

int main() {
	string line;
	ifstream file("config/mydet_chip_1.edf");
	if (file.is_open()) {
		char *hdr = new char[1024];
		file.seekg(0, ios::beg);
		file.read(hdr, 1024);
		if (hdr[1022] == '}')
			cout << "good" << endl;
		long size = 65536;
		char *memblock = new char[size];
		file.seekg(1024, ios::beg);
		file.read(memblock, size);

		file.close();
	} else {
		cout << "Unable to open file";
	}
	std::string fname  = "config/tpxatl25.cfg";
	INIReader reader(fname);
    if (reader.ParseError() < 0) {
		std::cout << "I/O error while reading file." << std::endl;
    }
    std::string section = "config";
    assert(reader.Get(section, "version", "Unknown") == "1.0");
    assert(reader.Get(section, "type", "Unknown") == "MAXIPIX");

    section = "dacs";
	assert(reader.GetInteger(section, "biaslvds",-1) == 128);
	assert(reader.GetInteger(section, "buffanaloga", -1) == 127);
	assert(reader.GetInteger(section, "buffanalogb", -1) == 127);
	assert(reader.GetInteger(section, "ctpr", -1) == 1);
	assert(reader.GetInteger(section, "disc", -1) == 127);
	assert(reader.GetInteger(section, "fbk", -1) == 128);
	assert(reader.GetInteger(section, "gnd", -1) == 80);
	assert(reader.GetInteger(section, "hist", -1) == 0);
	assert(reader.GetInteger(section, "ikrum", -1) == 5);
	assert(reader.GetInteger(section, "preamp",-1) == 255);
	assert(reader.GetInteger(section, "reflvds",-1) == 128);
	assert(reader.GetInteger(section, "thl",-1) == 7572);
	assert(reader.GetInteger(section, "ths",-1) == 60);
	assert(reader.GetInteger(section, "vcas",-1) == 130);


	section = "detmodule";
	assert(reader.Get(section, "connection","Unknown") == "priam");
	assert(reader.Get(section, "asic","Unknown") == "TPX1");
	assert(reader.Get(section, "sensor","Unknown") == "Si");
	assert(reader.GetInteger(section, "frequency",-1) == 100);
	assert(reader.Get(section, "polarity","Unknown") == "POSITIVE");
	assert(reader.GetInteger(section, "bias",-1) == 150);
	assert(reader.GetInteger(section, "nchips",-1) == 4);
	assert(reader.GetInteger(section, "xpixels",-1) == 256);
	assert(reader.GetInteger(section, "ypixels",-1) == 256);
	assert(reader.GetInteger(section, "chip_1",-1) == 1);
	assert(reader.GetInteger(section, "chip_2",-1) == 2);
	assert(reader.GetInteger(section, "chip_3",-1) == 3);
	assert(reader.GetInteger(section, "chip_4",-1) == 4);
	assert(reader.GetInteger(section, "pitch",-1) == 1);
	assert(reader.GetInteger(section, "xfirst",-1) == 0);
	assert(reader.GetInteger(section, "yfirst",-1) == 0);
	assert(reader.Get(section, "name_1","Unknown") == "W15C12");
	assert(reader.Get(section, "name_2","Unknown") == "W15A6");
	assert(reader.Get(section, "name_3","Unknown") == "W15G7");
	assert(reader.Get(section, "name_4","Unknown") == "W15L8");
	assert(reader.GetInteger(section, "energy",-1) == 10);

	section = "calibration";
	assert(reader.Get(section, "mode","Unknown") == "SPM");
	assert(reader.GetInteger(section, "energy",-1) == 5);
	assert(reader.GetInteger(section, "thlnoise_1",-1) == 7575);
	assert(reader.GetInteger(section, "thlnoise_2",-1) == 7626);
	assert(reader.GetInteger(section, "thlnoise_3",-1) == 7586);
	assert(reader.GetInteger(section, "thlnoise_4",-1) == 7585);
	assert(reader.GetInteger(section, "thlxray_1",-1) == 7425);
	assert(reader.GetInteger(section, "thlxray_2",-1) == 7526);
	assert(reader.GetInteger(section, "thlxray_3",-1) == 7486);
	assert(reader.GetInteger(section, "thlxray_4",-1) == 7485);

	section = "layout_standard";
	assert(reader.Get(section, "layout","Unknown") == "L_2x2");
	assert(reader.GetInteger(section, "xchips",-1) == 2);
	assert(reader.GetInteger(section, "ychips",-1) == 2);
	assert(reader.GetInteger(section, "xgap",-1) == 4);
	assert(reader.GetInteger(section, "ygap",-1) == 4);
	assert(reader.GetInteger(section, "pos_1",-1) == 00);
	assert(reader.GetInteger(section, "pos_2",-1) == 01);
	assert(reader.GetInteger(section, "pos_3",-1) == 10);
	assert(reader.GetInteger(section, "pos_4",-1) == 11);
	assert(reader.GetInteger(section, "rot_1",-1) == 1);
	assert(reader.GetInteger(section, "rot_2",-1) == -1);
	assert(reader.GetInteger(section, "rot_3",-1) == 1);
	assert(reader.GetInteger(section, "rot_4",-1) == -1);

	section = "layout_general";
	assert(reader.GetInteger(section, "xc_1",-1) == 84);
	assert(reader.GetInteger(section, "yc_1",-1) == 0);
	assert(reader.GetInteger(section, "rot_1",-1) == 0);
	assert(reader.GetInteger(section, "xc_4",-1) == 344);
	assert(reader.GetInteger(section, "yc_4",-1) == 84);
	assert(reader.GetInteger(section, "rot_4",-1) == 0);
	assert(reader.GetInteger(section, "xc_3",-1) == 260);
	assert(reader.GetInteger(section, "yc_3",-1) == 344);
	assert(reader.GetInteger(section, "rot_3",-1) == 2);
	assert(reader.GetInteger(section, "xc_2",-1) == 0);
	assert(reader.GetInteger(section, "yc_2",-1) == 260);
	assert(reader.GetInteger(section, "rot_2",-1) == 0);


	return 0;
}

