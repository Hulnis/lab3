#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
using namespace std;
// Physical base address of GPIO
const unsigned gpio_address = 0x400d0000;
// Length of memory-mapped IO window
const unsigned gpio_size = 0xff;
const int gpio_led1_offset = 0x12C; // Offset for LED1
const int gpio_led2_offset = 0x130; // Offset for LED2
const int gpio_led3_offset = 0x134; // Offset for LED3
const int gpio_led4_offset = 0x138; // Offset for LED4
const int gpio_led5_offset = 0x13C; // Offset for LED5
const int gpio_led6_offset = 0x140; // Offset for LED6
const int gpio_led7_offset = 0x144; // Offset for LED7
const int gpio_led8_offset = 0x148; // Offset for LED8
const int gpio_sw1_offset = 0x14C; // Offset for Switch 1
const int gpio_sw2_offset = 0x150; // Offset for Switch 2
const int gpio_sw3_offset = 0x154; // Offset for Switch 3
const int gpio_sw4_offset = 0x158; // Offset for Switch 4
const int gpio_sw5_offset = 0x15C; // Offset for Switch 5
const int gpio_sw6_offset = 0x160; // Offset for Switch 6
const int gpio_sw7_offset = 0x164; // Offset for Switch 7
const int gpio_sw8_offset = 0x168; // Offset for Switch 8
const int gpio_pbtnl_offset = 0x16C; // Offset for left push button
const int gpio_pbtnr_offset = 0x170; // Offset for right push button
const int gpio_pbtnu_offset = 0x174; // Offset for up push button
const int gpio_pbtnd_offset = 0x178; // Offset for down push button
const int gpio_pbtnc_offset = 0x17C; // Offset for center push button

class ZedBoard
{
	char *pBase;
	int fd;
public:
ZedBoard()
{
	fd = open( "/dev/mem", O_RDWR);
	pBase = (char *) mmap(
	NULL,
	gpio_size,
	PROT_READ | PROT_WRITE,
	MAP_SHARED,
	*&fd,
	gpio_address);
	if (pBase == MAP_FAILED)
	{
		cerr << "Mapping I/O memory failed - Did you run with 'sudo'?\n";
		exit(1); // Returns 1 to the operating system;
	}
}
~ZedBoard()
{
	munmap(pBase, gpio_size);
	close(fd);
}
void RegisterWrite(int offset, int value)
{
	 * (int *) (pBase + offset) = value;
}
int RegisterRead(int offset)
{
	return * (int *) (pBase + offset);
}
void SetLedNumber(int value)
{
	RegisterWrite(gpio_led1_offset, value % 2);
	RegisterWrite(gpio_led2_offset, (value / 2) % 2);
	RegisterWrite(gpio_led3_offset, (value / 4) % 2);
	RegisterWrite(gpio_led4_offset, (value / 8) % 2);
	RegisterWrite(gpio_led5_offset, (value / 16) % 2);
	RegisterWrite(gpio_led6_offset, (value / 32) % 2);
	RegisterWrite(gpio_led7_offset, (value / 64) % 2);
	RegisterWrite(gpio_led8_offset, (value / 128) % 2);
}

int readSwitches() {
	int a = RegisterRead(gpio_sw1_offset);
	int b = RegisterRead(gpio_sw2_offset);
	int c = RegisterRead(gpio_sw3_offset);
	int d = RegisterRead(gpio_sw4_offset);
	int e = RegisterRead(gpio_sw5_offset);
	int f = RegisterRead(gpio_sw6_offset);
	int g = RegisterRead(gpio_sw7_offset);
	int h = RegisterRead(gpio_sw8_offset);
	return (a + (b * 2) + (c * 4) + (d * 8) + (e * 16) + (f * 32) + (g * 64) + (h * 128));
}

int PushButtonGet(bool one, bool two, bool three, bool four, bool five) {
	if (RegisterRead(gpio_pbtnl_offset) != one) {
		return 1;
	}
	if (RegisterRead(gpio_pbtnr_offset) != two) {
		return 2;
	}
	if (RegisterRead(gpio_pbtnu_offset) != three) {
		return 3;
	}
	if (RegisterRead(gpio_pbtnd_offset) != four) {
		return 4;
	}
	if (RegisterRead(gpio_pbtnc_offset) != five) {
		return 5;
	}
	return 0;
}
};


/**
* Write a 4-byte value at the specified general-purpose I/O location.
*
* @param pBase Base address returned by 'mmap'.
* @parem offset Offset where device is mapped.
* @param value Value to be written.
*/
void RegisterWrite(char *pBase, int offset, int value)
{
	* (int *) (pBase + offset) = value;
}
/**
* Read a 4-byte value from the specified general-purpose I/O location.
*
* @param pBase Base address returned by 'mmap'.
* @param offset Offset where device is mapped.
* @return Value read.
*/
int RegisterRead(char *pBase, int offset)
{
	return * (int *) (pBase + offset);
}
/**
* Show lower 8 bits of integer value on LEDs
*
* @param pBase Base address of I/O
* @param value Value to show on LEDs
*/


int main()
	{
	ZedBoard *zed = new ZedBoard();
	// Check error

	int value = zed->readSwitches();
	cout << value << endl;
	zed->SetLedNumber(value);
	bool oneOn = 0;
	bool twoOn = 0;
	bool threeOn = 0;
	bool fourOn = 0;
	bool fiveOn = 0;

	double time_counter = 0;
	clock_t currentTime = clock();
	clock_t prevTime = currentTime;
	int state;

	int speed = 0;
	int ticks = 1;
	bool waiting = true;
	while(1) {
		int state1 = 0;
		currentTime = clock();
		time_counter += (double)(currentTime - prevTime);
		prevTime = currentTime;
		ticks = speed;
		if(speed == 0) {
			ticks = 1;
			waiting = true;
		}
		int pushed = zed->PushButtonGet(oneOn, twoOn, threeOn, fourOn, fiveOn);
		if(pushed != 0) {
				state1 = pushed;
			}
		if(time_counter > (double)(1000000 / ticks)) {
			time_counter -= (double)(1000000 / ticks);
			cout << value << endl;
			if(state1 == 4) {
					speed -= 1;
					if(speed <= 0) {
						speed = 0;
					}
					fourOn = (fourOn + 1) % 2;
					continue;
			}
			if(state1 == 3) {
					speed += 1;
					threeOn = (threeOn + 1) % 2;
					waiting = false;
					continue;
			}
			if(state1 == 5) {
				if (zed->RegisterRead(gpio_pbtnc_offset) == 1) {
					value = zed->readSwitches();
					zed->SetLedNumber(value);
				}
				fiveOn = (fiveOn + 1) % 2;
				continue;
			}
			state = state1;
			if(waiting) {
				state = 0;
			}
			switch (state) {
				case 2:
					value = value + 1;
					zed->SetLedNumber(value);
					twoOn = (twoOn + 1) % 2;
					break;
				case 1:
					value = value - 1;
					zed->SetLedNumber(value);
					oneOn = (oneOn + 1) % 2;
					break;
				default:
					break;
			}
		}
	}
	delete &zed;
}

echo "# lab3" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/Hulnis/lab3.git
git push -u origin master
