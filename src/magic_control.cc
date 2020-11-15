#include <iostream>
#include <iomanip>
#include <blepp/blestatemachine.h>
#include <blepp/pretty_printers.h>
#include <unistd.h>

using namespace std;
using namespace chrono;
using namespace BLEPP;

int main(int argc, char **argv)
{
	if(argc != 3) {
		cerr << "Please supply address and intensity.\n";
		cerr << "Usage:\n";
		cerr << "prog <addres> <intensity>\n";
		exit(1);
	}

	char *address = argv[1];
	uint8_t intensity = atoi(argv[2]);
	log_level = Error;

	BLEGATTStateMachine gatt;

	std::function<void()> found_services_and_characteristics_cb = [&gatt, &intensity](){
		for(auto& service: gatt.primary_services)
			for(auto& characteristic: service.characteristics) {
				//cout << "Found " << to_str(characteristic.uuid) << endl;
				if(characteristic.uuid == UUID("78667579-a914-49a4-8333-aa3c0cd8fedc")) {
					cout << "Send intensity level " << (int)intensity << endl;
					uint8_t cmd[5] = { 0x04, 0x08, intensity, 0x64, 0x00 };
					characteristic.write_command(cmd, 5);
				}
			}
	};

	gatt.setup_standard_scan(found_services_and_characteristics_cb);

	gatt.cb_disconnected = [](BLEGATTStateMachine::Disconnect d) {
		cerr << "Disconnect for reason " << BLEGATTStateMachine::get_disconnect_string(d) << endl;
		exit(1);
	};

	cout << "Connect to device " << address << endl;
	gatt.connect_blocking(address);

	try {
		for(;;) {
			gatt.read_and_process_next();
		}
	} catch(std::runtime_error e) {
		cerr << "Something's stopping bluetooth working: " << e.what() << endl;
	} catch(std::logic_error e) {
		cerr << "Oops, someone fouled up: " << e.what() << endl;
	}

}
