## 스트레인게이지를 이용한 압력센서
### (Pressure sensor using strain gauge)
###

> **<개발 이유>**

 압력센서는 입력된 압력을 감지하여서 전지신호로 변환시키기 위해 사용되는 센서이다. 최근의 압력센서는 가전제품을 비롯하여 자동차, 의료기기, 환경관련 설비와 산업체등 다양한 분야의 제어까지 응용되는 분야가 광범위하다. 고온이나 고압, 습도 등의 여러 환경에서도 사용할 수 있는 가볍고 작으면서 저렴한 압력센서가 요구된다. 
 스트레인 게이지는 크기가 매우 작고 가벼우며 비용이 저렴하기에 압력센서 개발에 사용하기 적합하였다. 스트레인 게이지는 금속에 외력을 가하게 되면 변형하는 원리인 잡아당겨 늘어날 때 가늘어지면 저항이 증가하고, 압축하게 되면 줄어들고 저항이 감소하는 것을 이용했다. 
 스트레인 게이지를 이용한 압력센서를 제작하기 위해 적합한 스트레인 게이지 소자를 데이터 시트를 참고하여 선정하였다. 회로에 필요한 소자를 선정하여 압력 센서의 회로를 설계하였다. 



``` C

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

```
