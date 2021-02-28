#include <Compiler.h>


#include <U_LEDMatrix.h>
#include <U_GPIOConfig.h>
#include <U_DrvSPI.h>

int volatile timer2Flag;
int count = 0;
int previous;

void AnTOnAt(uint8_t index);
void AnTOffAt(uint8_t index);
void sendToBuffer(ImageHandle *hImage);
void sentToBufferOnPhase(ImageHandle *hImage, Phase phase);
uint8_t getThreshold(Phase phase);

static const AnTType AnT = {
//static const _GPIOConfig AnT[] = {
															{&GPIOA, 12}, //antr1
															{&GPIOA, 11}, //antr2
															{&GPIOA, 10}, //antr3
															{&GPIOA, 9 }, //antr4
															{&GPIOA, 8 }, //antr5
															{&GPIOC, 9 }, //antr6
															{&GPIOC, 8 }, //antr7
															{&GPIOC, 7 }, //antr8
};


uint8_t GnImage[][] = {
      {255, 255, 170, 170, 85, 85, 0, 0},
			{255, 255, 170, 170, 85, 85, 0, 0},
      {255, 255, 170, 170, 85, 85, 0, 0},
			{255, 255, 170, 170, 85, 85, 0, 0},
      {0, 0, 85, 85, 170, 170, 255, 255}, 
			{0, 0, 85, 85, 170, 170, 255, 255},
      {0, 0, 85, 85, 170, 170, 255, 255},
			{0, 0, 85, 85, 170, 170, 255, 255}
};


uint8_t RdImage[][] = {
			{250, 250, 250, 250, 250, 250, 250, 250},
      {80, 80, 80, 80, 80, 80, 80, 80},
      {5, 5, 5, 5, 5, 5, 5, 5},
      {175, 175, 175, 175, 175, 175, 175, 175},
      {250, 250, 250, 250, 250, 250, 250, 250},
      {80, 80, 80, 80, 80, 80, 80, 80},
      {5, 5, 5, 5, 5, 5, 5, 5},
      {175, 175, 175, 175, 175, 175, 175, 175}
};





void screenOn(ScreenHandle* hScreen, SPIHandle* hSPIGn,SPIHandle* hSPIRd){
	
	while(timer2Flag == 1){
		timer2Flag =0;
		SPIOutEnOff(hSPIGn);
		SPIOutEnOff(hSPIRd);
		AnTOnAt(count);
		previous = count-1;
		if(count == 0){
			previous = 7;
		}
		AnTOffAt(previous);
		count++;
		if (count == 8){
			count = 0;
		}
		SPIEmit(hSPIGn,pop(&hSreen->hImageGn.buffer));
		SPIEmit(hSPIRd,pop(&hSreen->hImageRd.buffer));
		SPILatch(hSPIGn);
		SPILatch(hSPIRd);
		SPIOutEn(hSPIGn);
		SPIOutEn(hSPIRd);

	
	}
	
}

void imageHandleInit(ScreenHandle* hScreen){
	hScreen->hImageGn->image= &GnImage;
	hScreen->hImageRd->image= &RdImage;
	sendToBuffer(hScreen->hImageGn);
	sendToBuffer(hScreen->hImageGn);
	
}

void sendToBuffer(ImageHandle *hImage) {
  sentToBufferOnPhase(hImage, phase0);
  sentToBufferOnPhase(hImage, phase1);
  sentToBufferOnPhase(hImage, phase2);
  sentToBufferOnPhase(hImage, phase3);
}

void sentToBufferOnPhase(ImageHandle *hImage, Phase phase) {
	uint32_t data = 0;
  uint32_t flag = 0;
	size_t col;
	size_t row;
	size_t j;
	
  uint8_t threshold = getThreshold(phase);
	

  for (col = 0; col < 8; col++) {
		for (row = 0; row < 8; row++) {
      for (j = 0; j < 4; j++) {
        flag = ((hImage->image[row][col] & ((1 << (8 - j * 2)) - 1)) >> (6 - j * 2)) >= threshold;
        data = (data << 1) + flag;
      }
      push(&hImage->buffer, data);
      data = 0;
     }
   }
 }

uint8_t getThreshold(Phase phase) {
  uint8_t threshold;
  switch (phase) {
  case phase0:
    threshold = 1;
    break;
  case phase1:
    threshold = 2;
    break;
  case phase2:
    threshold = 3;
    break;
  case phase3:
    threshold = 3;
    break;
  }
  return threshold;
}



















void AnTInit(void){
	int i=0;
	for(i=0;i<ANT_LEN;i++){
		GPIOConfig(AnT[i],GPIO_O_STD_PP_02MHZ);
		setGPIOPin(AnT[i]);
	}
}


void AnTOnAt(uint8_t index){
	resetGPIOPin(AnT[index]);
}

void AnTOffAt(uint8_t index){
	setGPIOPin(AnT[index]);
}



