#include <rssi.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <time.h>
#include <Adapter.h>
#include <curl.h>

Adapter::Adapter(int hci_deviceid, const char* bt_addr) {
	this->hci_deviceid = hci_deviceid;
	this->bt_addr = bt_addr;
	init();
}

Adapter::Adapter(const char* bt_addr) {
	this->bt_addr = bt_addr;
	if (hci_get_route(NULL) >= 0)
		this->hci_deviceid = hci_get_route(NULL);
	init();
}

void Adapter::init(){
	int result;

	printf("INIT with: %s via hci%d\n", bt_addr, hci_deviceid);


	hciDevice = hci_open_dev(hci_deviceid);
	printf("HCISOCKET: %d\n", hciDevice);
	printf("l2capSock %s\n", (l2capSock == -1) ? strerror(errno) : "success");

	// create socket
	l2capSock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	printf("L2CAPSOCK: %d\n", l2capSock);
	printf("l2capSock %s\n", (l2capSock == -1) ? strerror(errno) : "success");

	const bdaddr_t BDADDR_ANY_TMP = { 0 };

	// bind
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.l2_family = AF_BLUETOOTH;
	bacpy(&sockAddr.l2_bdaddr, &BDADDR_ANY_TMP);
	sockAddr.l2_cid = htobs(ATT_CID);

	result = bind(l2capSock, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
	printf("BIND: %d\n", result);
	printf("bind %s\n", (result == -1) ? strerror(errno) : "success");
	if (result == -1) return;


	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.l2_family = AF_BLUETOOTH;
	str2ba(bt_addr, &sockAddr.l2_bdaddr);
	// BDADDR_LE_RANDOM is also possible
	sockAddr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
	sockAddr.l2_cid = htobs(ATT_CID);
	
	printf("connect\n");
	result = connect(l2capSock, (struct sockaddr *) &sockAddr,
			sizeof(sockAddr));
	printf("CONNECT L2CAPSOCK: %d\n", result);
	printf("connect %i %s\n", result,
			(result == -1) ? strerror(errno) : "success");
	if (result == -1) return;

	l2capConnInfoLen = sizeof(l2capConnInfo);
	getsockopt(l2capSock, SOL_L2CAP, L2CAP_CONNINFO, &l2capConnInfo,
			&l2capConnInfoLen);
	hciHandle = l2capConnInfo.hci_handle;

	printf("GETSOCKOPT L2CAPSOCK: %d\n", result);
	printf("getsockopt %i %s\n", result,
			(result == -1) ? strerror(errno) : "success");
}

int Adapter::getHciDeviceId(){
	return hci_deviceid;
}

int Adapter::read_rssi(int to, int8_t &rssi) {
	rssi = 0;
	int result = hci_read_rssi(hciDevice, hciHandle, &rssi, to);
	if (result < 0) {
		printf("rssi: %i %s\n", rssi,
				(result == -1) ? strerror(errno) : "success");
		printf("Probably a disconnect\n");
	}

	return result;
}

uint8_t Adapter::rssi_to_intensity(int8_t value) {
	float neg_intensity = value;
	const float rssi_min = -90, rssi_max = -50;
	const float intensity_min = 10, intensity_max = 100;
	const float rssi_range = (rssi_max - rssi_min), intensity_range = (intensity_max - intensity_min);

	// map rssi values to intensity commands
	// map -90 to 10 (0xA), map -30 to 100
	double scale = (rssi_range) / (intensity_range);
	uint8_t intensity = (uint8_t)(((neg_intensity - rssi_min) / scale) + intensity_min);

	// should be limited according to
	// https://metafetish.club/t/magic-motion-toys-maybe-universal-specifications/286
	if (intensity > 100) {
		intensity = 100;
	}
	return intensity;
}

/*
 * For fun try to write the given value directly over an HCI socket (not recommended!)
 */
void Adapter::write(uint8_t value) {
	// this is the payload we want to sent (see below) 
	uint8_t cmd[5] = { 0x04, 0x08, value, 0x64, 0x00 };

	// we want to send the following command:
	// 02 01 0e 0c 00 08 00 04 00 52 03 00 04 08 20 64 00
	// (this is easiest checked with wireshark)
	uint8_t raw_buffer[17];

	// 02: bluetooth hci h4 (hci_h4), with 0 the direction "Sent" and 2 the ACL data (0x02) type
	raw_buffer[0] = 0x02;
	// 01 0e: connection handle 0xe01 and PB flag and BC flag (reverse order)
	raw_buffer[1] = 0x01;
	raw_buffer[2] = 0x0e;
	// 0c 00: data total is length 12 (0x000c)
	raw_buffer[3] = 0x0c;
	raw_buffer[4] = 0x00;
	// 08 00: l2cap payload length: 8
	raw_buffer[5] = 0x08;
	raw_buffer[6] = 0x00;
	// 04 00: l2cap channel identifier: 0x0004
	raw_buffer[7] = 0x04;
	raw_buffer[8] = 0x00;

	// actually all the above we do not have to sent, this is all part of the l2cap protocol
	// and this header will be placed when writing to the l2cap socket
	// 52: btattr opcode (0x52 = write command)
	raw_buffer[9] = 0x52;

	// 03 00: handle (0x0003), we got the index of the handle through e.g. bluetoothctl (or wireshark)
	raw_buffer[10] = 0x03;
	raw_buffer[11] = 0x00;

	// 04 08 20 64 00: payload (see above)
	raw_buffer[12] = cmd[0];
	raw_buffer[13] = cmd[1];
	raw_buffer[14] = cmd[2];
	raw_buffer[15] = cmd[3];
	raw_buffer[16] = cmd[4];

	// finally, send the stuff!
	if (send(l2capSock, &raw_buffer[9], 17-9, 0)) {
		printf("%i\n", value);
	}
}

void sleep(int ms){
	clock_t end_time = clock() + ms * CLOCKS_PER_SEC/1000;
	while (clock() < end_time) {}
}


int main(int argc, const char* argv[]) {

	CurlClient cc;
	Adapter myAdapter(0, argv[1]);

	int counter = 0;

	while (true) {
		int8_t rssi = 0;
	
		std::string value = cc.Get("http://control-me.herokuapp.com/state");
		uint8_t intensity = atoi(value.c_str());
		//printf("Returns: %i\n", intensity);

		int result = myAdapter.read_rssi(100, rssi);
		if (result < 0) {
			// wait before we initialize
			sleep(1000);
			myAdapter.init();
			sleep(1000);
		} else {
			myAdapter.write(intensity);
			//printf("%i\n", rssi);
		}

		sleep(1000);
		counter++;
	}
}
