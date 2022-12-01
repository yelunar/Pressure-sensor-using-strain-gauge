#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float offset = 0.0, input = 0.0, output = 0.0, out = 0.0;

byte CH_H[1] = { 0x06 }; // MCP3204 CH0�� IN // MCP3204 ä�� ����
byte CH_L[1] = { 0x40 }; // MCP3204 CH0�� OUT // MCP3204 datasheet ����

void setup() {
	SPI.begin(); //SPI ���
	lcd.begin();
	lcd.clear();

	SPCR = 0x50; //spi ��� ��������
	SPSR = 0x01; //spi ��� ��������

	DDRB |= _BV(2); //��Ž� cs ��ȣ ����
	PORTB |= _BV(2); //��Ž� cs ��ȣ ����

	for (int i = 0; i < 100; i++)
	{
		offset += (float)MCP_read(0x06, 0x00) / 4095.0; //MCP3204 ���ش� 4095
	}
	offset /= 4095.0;
}

void loop() {
	float in = (float)MCP_read(0x06, 0x00) - 1633.0; // ������ �� ��
	input = in * in * (-0.0002) + 0.5657 * in; //MCP3024�� CH0�� ������ ���� ���Է� ��ȯ 
	output = input * 0.6 + output * 0.4; //LPF ����

	if (output < 1) {
		output = 0;
	}

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("weight: ");
	lcd.print(output, 0);
	lcd.print("g");

	delay(100);
}

unsigned short MCP_read(byte CHH, byte CHL) {
	byte H, L;

	PORTB &= ~_BV(2);
	SPI.transfer(CHH); // IN��ȣ SPI
	H = SPI.transfer(CHL);
	L = SPI.transfer(0x00);
	PORTB |= _BV(2);

	return ((H & 0x0F) << 8) | L; //SPIƮ������ / ����Ʈ ���� 8���� ��� / H,L ����Ʈ ���� / �ѹ���Ʈ ��ŭ �о ��ġ�� ������
}