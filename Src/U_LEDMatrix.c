#include <Compiler.h>


#include <U_LEDMatrix.h>
#include <U_GPIOConfig.h>
#include <U_DrvSPI.h>
#include <U_Queue.h>

#include <string.h>


int volatile timer2Flag;
int count = 0;
int previous;





void LEDOnAtCol(ImageHandle *hImage,uint8_t index);
void LEDOffAtCol(ImageHandle *hImage,uint8_t index);


void ImageOutEn		(ImageHandle *hImage);
void ImageOutEnOff(ImageHandle *hImage);
void ColDataSend	(ImageHandle *hImage);
void ImageLatch		(ImageHandle *hImage);

void ChannelInit(ChannelHandle *hChannel, Color color);
void fillBuffer(ChannelHandle* hChannel);
void sentToBufferOnPhase(ChannelHandle* hChannel, Phase phase);
uint8_t getThreshold(Phase phase);







static const ImageData dataGn= {
	255, 255, 170, 170, 85, 85, 0, 0,
	255, 255, 170, 170, 85, 85, 0, 0,
	255, 255, 170, 170, 85, 85, 0, 0,
	255, 255, 170, 170, 85, 85, 0, 0,
	0, 0, 85, 85, 170, 170, 255, 255, 
	0, 0, 85, 85, 170, 170, 255, 255,
	0, 0, 85, 85, 170, 170, 255, 255,
	0, 0, 85, 85, 170, 170, 255, 255
};

static const ImageData dataRd= {
	250, 250, 250, 250, 250, 250, 250, 250,
	80, 80, 80, 80, 80, 80, 80, 80,
	5, 5, 5, 5, 5, 5, 5, 5,
	175, 175, 175, 175, 175, 175, 175, 175,
	250, 250, 250, 250, 250, 250, 250, 250,
	80, 80, 80, 80, 80, 80, 80, 80,
	5, 5, 5, 5, 5, 5, 5, 5,
	175, 175, 175, 175, 175, 175, 175, 175
};	

static const Column columnConfig = {
	{&GPIOA, 12}, //antr1
	{&GPIOA, 11}, //antr2
	{&GPIOA, 10}, //antr3
	{&GPIOA, 9 }, //antr4
	{&GPIOA, 8 }, //antr5
	{&GPIOC, 9 }, //antr6
	{&GPIOC, 8 }, //antr7
	{&GPIOC, 7 }, //antr8
};






void ImageInit(ImageHandle *hImage){
	int i=0;
	ChannelInit(&hImage->hChannelGn,Gn);
	ChannelInit(&hImage->hChannelRd,Rd);
	memcpy(hImage->hColumn,columnConfig,sizeof(columnConfig));

	for(i=0;i<COLUMN_LEN;i++){
		GPIOConfig(hImage->hColumn[i],GPIO_O_STD_PP_02MHZ);
		LEDOffAtCol(hImage,i);
	}
}




void ScreenOn(ImageHandle *hImage){
	while(timer2Flag == 1){
		
		timer2Flag =0;

		ImageOutEnOff(hImage);

		LEDOnAtCol(hImage,count);
		previous = count-1;
		if(count == 0){
			previous = 7;
		}

		LEDOffAtCol(hImage,previous);
		count++;
		if (count == 8){
			count = 0;
		}
		
		if(isEmpty(&hImage->hChannelGn.buffer)){
			fillBuffer(&hImage->hChannelGn);
		}
		
		if(isEmpty(&hImage->hChannelRd.buffer)){
			fillBuffer(&hImage->hChannelRd);
		}

		ColDataSend(hImage);
		ImageLatch(hImage);
		ImageOutEn(hImage);
	}
}




void LEDOnAtCol(ImageHandle *hImage,uint8_t index){
	resetGPIOPin(hImage->hColumn[index]);
}

void LEDOffAtCol(ImageHandle *hImage,uint8_t index){
	setGPIOPin(hImage->hColumn[index]);
}

void ImageOutEn(ImageHandle *hImage){
	SPIOutEn(&hImage->hChannelGn.hSPI);
	SPIOutEn(&hImage->hChannelRd.hSPI);
}

void ImageOutEnOff(ImageHandle *hImage){
	SPIOutEnOff(&hImage->hChannelGn.hSPI);
	SPIOutEnOff(&hImage->hChannelRd.hSPI);
}

void ColDataSend(ImageHandle *hImage){
	SPIEmit(&hImage->hChannelGn.hSPI,pop(&hImage->hChannelGn.buffer));
	SPIEmit(&hImage->hChannelRd.hSPI,pop(&hImage->hChannelRd.buffer));
}

void ImageLatch(ImageHandle *hImage){
	SPILatch(&hImage->hChannelGn.hSPI);
	SPILatch(&hImage->hChannelRd.hSPI);
}



void ChannelInit(ChannelHandle *hChannel, Color color){

	SPIInit(&hChannel->hSPI, color);
	QueueInit(&hChannel->buffer);
	switch (color) {
		case Gn:
			memcpy(hChannel->data, dataGn,sizeof(dataGn));
			break;
		case Rd:
			memcpy(hChannel->data, dataRd,sizeof(dataRd));
	}
}


void fillBuffer(ChannelHandle* hChannel) {
  sentToBufferOnPhase(hChannel, phase0);
  sentToBufferOnPhase(hChannel, phase1);
  sentToBufferOnPhase(hChannel, phase2);
  sentToBufferOnPhase(hChannel, phase3);
}


void sentToBufferOnPhase(ChannelHandle* hChannel, Phase phase) {
	uint32_t data = 0;
  uint32_t flag = 0;
	size_t col;
	size_t row;
	size_t j;
	
  uint8_t threshold = getThreshold(phase);
	
  for (col = 0; col < COLUMN_LEN; col++) {
		for (row = 0; row < ROW_LEN; row++) {
      for (j = 0; j < 4; j++) {
        flag = ((hChannel->data[row][col] & ((1 << (8 - j * 2)) - 1)) >> (6 - j * 2)) >= threshold;
        data = (data << 1) + flag;
      }
		}
		push(&hChannel->buffer, data);
		data = 0;
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
















