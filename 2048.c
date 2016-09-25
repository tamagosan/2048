//2048 GAME   by TAMAGOSAN
//PIC16F877A   XC8 compiler

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#define _XTAL_FREQ 20000000

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

unsigned int block[4][4]={{8888,8888,8888,8888},{8888,8888,8888,8888},{8888,8888,8888,8888},{8888,8888,8888,8888}};
char addd[4][4]={0};
unsigned long score=88888888;
unsigned long highscore;
unsigned char seed;

char blocknumrnd(void);
void seg7hyouji(void);
void blocksetrnd(void);
void gameover(void);
void up(void);
void down(void);
void left(void);
void right(void);

void main(void){
	TRISD=0b0001111;
	TRISA=0;
	TRISB=0;
	TRISC=0;
	TRISE=0;
	ADCON1=0;
	PORTA=0;
	PORTB=0;
	PORTC=0;
	PORTD=0;
	PORTE=0;

	char btf,i,j;
	
	for(i=0;i<30;i++) seg7hyouji();
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			block[j][i]=0;
		}
	}
	highscore=(eeprom_read(0x00)*65536)+(eeprom_read(0x01)*256)+eeprom_read(0x02);
	seed=eeprom_read(0x03);
	score=highscore;
	for(i=0;i<80;i++) seg7hyouji();
	score=0;

	blocksetrnd();
	blocksetrnd();
	seg7hyouji();
	while(1){
		seed++;
		if(seed==255)seed=0;
		if(RD0==0 && btf==0){
			up();
			btf=1;
		}
		if(RD1==0 && btf==0){
			down();
			btf=1;
		}
		if(RD2==0 && btf==0){
			left();
			btf=1;
		}
		if(RD3==0 && btf==0){
			right();
			btf=1;
		}
		if(RD0*RD1*RD2*RD3==1) btf=0;
		if(RD0+RD1+RD2+RD3==0){
			eeprom_write(0x00,0);
			eeprom_write(0x01,0);
			eeprom_write(0x02,0);
			highscore=0;
		}
		seg7hyouji();
	}
}

char blocknumrnd(void){
	char num=2;
	seed++;
	seed=rand()%256;
	srand(seed);
	if(rand()<6000) num=4;
	return num;
}

void blocksetrnd(void){
	char p=1;
	char num;
	while(p){
		seed++;
		if(seed==255)seed=0;
		srand(seed);
		num=rand()/2048;
		if(block[num%4][num/4]==0) p=0;
	}
	block[num%4][num/4]=blocknumrnd();
	eeprom_write(0x03,seed);
}


void seg7hyouji(void){
	char xi,yi,p,f;
	char a[9];
	if(9999999<score)sprintf(a,"%lu",score);
	if(999999<score && score<10000000)sprintf(a,"0%lu",score);
	if(99999<score && score<1000000)sprintf(a,"00%lu",score);
	if(9999<score && score<100000)sprintf(a,"000%lu",score);
	if(999<score && score<10000)sprintf(a,"0000%lu",score);
	if(99<score && score<1000)sprintf(a,"00000%lu",score);
	if(9<score && score<100)sprintf(a,"000000%lu",score);
	if(score<10)sprintf(a,"0000000%lu",score);
	RD4=1;
	RD5=1;
	PORTC=a[5]+a[4]*16-816;
	PORTB=a[7]+a[6]*16-816;
	RD6=0;
	RD7=1;
	__delay_us(500);
	PORTC=a[1]+a[0]*16-816;
	PORTB=a[3]+a[2]*16-816;
	RD6=1;
	RD7=0;
	__delay_us(500);
	RD6=1;
	RD7=1;
	for(yi=0;yi<4;yi++){
		PORTB=0xff;
		PORTC=0xff;
		if(yi==2){
			RD4=1;
			RD5=0;
		}
		if(yi==0){
			RD4=0;
			RD5=1;
		}
		for(xi=0;xi<4;xi++){
			PORTB=0xff;
			PORTC=0xff;
			sprintf(a,"%d000",block[xi][yi]);
			if(block[xi][yi]<1000){
				for(f=0;f<4;f++) if(a[f]<49 || a[f]>57) a[f]=58;
			}
			p=yi*4+xi;
			if (p>7){
				PORTA=(p-8)*8;
			}else{
				PORTA=p;
			}
			PORTB=a[3]+a[2]*16-816;
			PORTC=a[1]+a[0]*16-816;
			__delay_us(500);
		}
	}
	RD4=1;
	RD5=1;
}
			
void gameover(void){
	char p=0;
	char i,j;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if (block[j][i]==0 || block[j+1][i]==0 || block[j][i+1]==0 || block[j+1][i+1]==0 || block[j][i]==block[j+1][i] || block[j][i]==block[j][i+1] || block[j+1][i+1]==block[j+1][i] || block[j+1][i+1]==block[j][i+1]) p=1;
		}
	}
	if(p==0){
		RE2=1;
		if(highscore<score){
			eeprom_write(0x00,(char)(score/65536));
			eeprom_write(0x01,(char)((score%65536)/256));
			eeprom_write(0x02,score%65536%256);
		}
		while(1) seg7hyouji();
	}
}

void up(void){
	char md=0;
	char t,i,j;
	for(t=0;t<3;t++){
		for(i=0;i<4;i++){
			for(j=1;j<4;j++){
				if (block[i][j-1]==0 && block[i][j]!=0){
					block[i][j-1]=block[i][j];
					addd[i][j-1]=addd[i][j];
					block[i][j]=0;
					addd[i][j]=0;
					md=1;
				}
				if (block[i][j-1]==block[i][j] && addd[i][j-1]==0 && addd[i][j]==0 && block[i][j]!=0){
					block[i][j-1]=block[i][j]*2;
					score=score+block[i][j]*2;
					block[i][j]=0;
					addd[i][j-1]=1;
					md=1;
				}
			}
		}
		seg7hyouji();
	}
 	for(i=0;i<4;i++){
		 	for(j=0;j<4;j++){
				addd[j][i]=0;
			}
	}
 	if (md==1) blocksetrnd();
	seg7hyouji();
 	gameover();
}

void down(void){
	char md=0;
	char t,i,j;
	for(t=0;t<3;t++){
		for(i=0;i<4;i++){
			for(j=1;j<4;j++){
				if (block[i][4-j]==0 && block[i][3-j]!=0){
					block[i][4-j]=block[i][3-j];
					addd[i][4-j]=addd[i][3-j];
					block[i][3-j]=0;
					addd[i][3-j]=0;
					md=1;
				}
				if (block[i][4-j]==block[i][3-j] && addd[i][4-j]==0 && addd[i][3-j]==0 && block[i][3-j]!=0){
					block[i][4-j]=block[i][3-j]*2;
					score=score+block[i][3-j]*2;
					block[i][3-j]=0;
					addd[i][4-j]=1;
					md=1;
				}
			}
		}
		seg7hyouji();
	}
 	for(i=0;i<4;i++){
		 	for(j=0;j<4;j++){
				addd[j][i]=0;
			}
	}
 	if (md==1) blocksetrnd();
	seg7hyouji();
 	gameover();
}

void left(void){
	char md=0;
	char t,i,j;
	for(t=0;t<3;t++){
		for(i=0;i<4;i++){
			for(j=1;j<4;j++){
				if (block[j-1][i]==0 && block[j][i]!=0){
					block[j-1][i]=block[j][i];
					addd[j-1][i]=addd[j][i];
					block[j][i]=0;
					addd[j][i]=0;
					md=1;
				}
				if (block[j-1][i]==block[j][i] && addd[j-1][i]==0 && addd[j][i]==0 && block[j][i]!=0){
					block[j-1][i]=block[j][i]*2;
					score=score+block[j][i]*2;
					block[j][i]=0;
					addd[j-1][i]=1;
					md=1;
				}
			}
		}
		seg7hyouji();
	}	
 	for(i=0;i<4;i++){
		 	for(j=0;j<4;j++){
				addd[j][i]=0;
			}
	}
	if (md==1) blocksetrnd();
	seg7hyouji();
	gameover();
}

void right(void){
	char md=0;
	char t,i,j;
	for(t=0;t<3;t++){
		for(i=0;i<4;i++){
			for(j=1;j<4;j++){
				if (block[4-j][i]==0 && block[3-j][i]!=0){
					block[4-j][i]=block[3-j][i];
					addd[4-j][i]=addd[3-j][i];
					block[3-j][i]=0;
					addd[3-j][i]=0;
					md=1;
				}
				if (block[4-j][i]==block[3-j][i] && addd[4-j][i]==0 && addd[3-j][i]==0 && block[3-j][i]!=0){
					block[4-j][i]=block[3-j][i]*2;
					score=score+block[3-j][i]*2;
					block[3-j][i]=0;
					addd[4-j][i]=1;
					md=1;
				}
			}
		}
		seg7hyouji();
	}
 	for(i=0;i<4;i++){
		 	for(j=0;j<4;j++){
				addd[j][i]=0;
			}
	}
	if (md==1) blocksetrnd();
	seg7hyouji();
	gameover();
}
