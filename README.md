# Pressure-sensor-using-strain-gauge
스트레인게이지를 이용한 압력센서🦾

'''

#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float offset = 0.0, input = 0.0, output = 0.0, out = 0.0;

byte CH_H[1] = { 0x06 }; // MCP3204 CH0번 IN // MCP3204 채널 설정
byte CH_L[1] = { 0x40 }; // MCP3204 CH0번 OUT // MCP3204 datasheet 참조

void setup() {
	SPI.begin(); //SPI 통신
	lcd.begin();
	lcd.clear();

	SPCR = 0x50; //spi 통신 레지스터
	SPSR = 0x01; //spi 통신 레지스터

	DDRB |= _BV(2); //통신시 cs 신호 제어
	PORTB |= _BV(2); //통신시 cs 신호 제어

	for (int i = 0; i < 100; i++)
	{
		offset += (float)MCP_read(0x06, 0x00) / 4095.0; //MCP3204 분해능 4095
	}
	offset /= 4095.0;
}

void loop() {
	float in = (float)MCP_read(0x06, 0x00) - 1633.0; // 디지털 값 차
	input = in * in * (-0.0002) + 0.5657 * in; //MCP3024의 CH0의 디지털 값을 무게로 변환 
	output = input * 0.6 + output * 0.4; //LPF 설정

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
	SPI.transfer(CHH); // IN신호 SPI
	H = SPI.transfer(CHL);
	L = SPI.transfer(0x00);
	PORTB |= _BV(2);

	return ((H & 0x0F) << 8) | L; //SPI트랜스퍼 / 바이트 단위 8개씩 통신 / H,L 바이트 단위 / 한바이트 만큼 밀어서 합치기 데이터
}

'''
