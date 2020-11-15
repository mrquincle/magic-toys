#include <unistd.h>

class Adapter {
	const char* bt_addr;
	int hci_deviceid;

	int hciDevice;
	uint16_t hciHandle;

	int l2capSock;
	struct sockaddr_l2 sockAddr;
	struct l2cap_conninfo l2capConnInfo;
	socklen_t l2capConnInfoLen;


	public:
	Adapter(int hci_deviceid, const char* bt_addr);
	Adapter(const char * bt_addr);

	void init();

	int getHciDeviceId();
	int read_rssi(int to, int8_t & rssi);
	void write(int8_t value);
};
