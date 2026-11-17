#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <string.h>
#define true 1
#define false 0

//가로 100  세로 50 , x=0->i=49, y=0->j=24
//x: 49+i, y: 24-j

struct credit {
	char name[20];
};

//콘솔 창 커서 이동 
void gotoxy(int x, int y) {
	COORD Pos;
	Pos.X = x; 
	Pos.Y = y; 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Pos);
}

int axis() {
	int i,j=0;
	for(i=-49;i<=50;i+=1.0) {
		for(j=24;j>=-25;j-=1.0) {
			gotoxy(49+i,24-j);
			if(i==0 && j==0)
				printf("+");
			else if(i==0)
				printf("|");
			else if(j==0)
				printf("-");
				//빈칸일때 gotoxy로 한칸 밀 것(공백넣으면 goto했을때 밀림) 
		}
	}
	return 0;
}

int inst();

void credits() {
	int i=0;
	struct credit elses[5]={"Felix Brown","Felix Brown","Felix Brown","Felix Brown","Felix Brown"};
	struct credit special[5]={"Monster Energy","One of my friend","Youtube","Dev-C++"};
	gotoxy(101,1); printf("%s: %s\n","Assignment",elses[0].name);
	gotoxy(101,2); printf("%s: %s\n","Idea",elses[1].name);
	gotoxy(101,3); printf("%s: %s\n","Meal supply",elses[2].name);
	gotoxy(101,4); printf("%s: %s\n","The one who is sleepy",elses[3].name);
	gotoxy(101,5); printf("%s: %s\n","Assignment",elses[4].name);
	gotoxy(101,6); printf("Special Thanks\n");
	for(i=0;i<4;i++) {
		gotoxy(101,i+7);
		printf("%s\n",special[i].name);
	}
	while(getch()=='\n');
	system("cls");
	axis();
}

//다항함수 출력 
int polynomial(float a,float b,float c,float d, int e) { //int d, int e
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*pow(i,4)+b*pow(i,3)+c*pow(i,2)+d*i+e)==j)
				printf("*");
		}
	}
	return 0;
}

//삼각함수  예시: 10 0.25 0 0 
void trisin(float a,float b,float c,float d) {
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*sin(b*i+c)+d)==j)
				printf("*");
		}
	}
}

void tricos(float a,float b,float c,float d) { //
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*cos(b*i+c)+d)==j)
				printf("*");
		}
	}
}

void tritan(float a,float b,float c,float d) {
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*tan(b*i+c)+d)==j)
				printf("*");
		}
	}
}

void exponential(float a,float b,float c,float d) { //예시: 2 0.1 2.5 -25
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(pow(a,b*i+c)+d)==j)
				printf("*");
		}
	}
}

void naturalLog(float a,float b, float c,float d) { //예시: 5 1 1 1
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*log(b*i+c)+d)==j)
				printf("*");
		}
	}	
}

void logarithm(float a,float b,float c,float d,float e) { //예시: 10 2 1 25 -25 
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*(log(c*i+d)/log(b))+e)==j)
				printf("*");
		}
	}
}

void rational(float a,float b,float c,float d) { // 예시: 1 0 50 0
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(c/(a*i+b)+d)==j)
				printf("*");
		}
	}
}

void irrational(float a,float b,float c,float d) {
	float i=0;
	int j=0;
	for(i=-49;i<=50;i+=0.1) {
		for(j=24;j>=-25;j-=1) {
			gotoxy(49+i,24-j);
			if((int)(a*sqrt(b*i+c)+d)==j)
				printf("*");
		}
	}
}

int main(void){
	gotoxy(0,0);
	axis();
	int i=0;
	char input[100]={};
	float a1,a2,a3,a4,a5=0;
	double numInput[20]={};
	
	while(1) {
		gotoxy(101,0);
		for(i=0;i<100;i++) {
			printf(" ");
			input[i]=0;
		}
		gotoxy(101,0);
		printf("> ");
		
		scanf("%s",input);
		if(strcmp(input,"quit")==0) 
			break;
		else if(strcmp(input,"help")==0) 
			inst();
		else if(strcmp(input,"clear")==0) {
			system("cls");
			gotoxy(0,0);
			axis();
		}
		else if(strcmp(input,"polynomial")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f %f",&a1,&a2,&a3,&a4,&a5);
			polynomial(a1,a2,a3,a4,a5);
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"sin")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			trisin(a1,a2,a3,a4);
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"cos")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			tricos(a1,a2,a3,a4);
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"tan")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			tritan(a1,a2,a3,a4); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"exponential")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			exponential(a1,a2,a3,a4); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"ln")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			naturalLog(a1,a2,a3,a4); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"logarithm")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f %f",&a1,&a2,&a3,&a4,&a5);
			logarithm(a1,a2,a3,a4,a5); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"rational")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f %f",&a1,&a2,&a3,&a4);
			rational(a1,a2,a3,a4); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"irrational")==0) {
			gotoxy(101,1); printf("Enter a coefficient: ");
			scanf("%f %f %f",&a1,&a2,&a3,&a4);
			irrational(a1,a2,a3,a4); 
			gotoxy(101,1);
			for(i=0;i<100;i++)
				printf(" ");
		}
		else if(strcmp(input,"credits")==0)
			credits();
	}
	return 0;
}

int inst() {
	gotoxy(101,1); printf("help: Print information about commands\n");
	gotoxy(101,2); printf("polynomial(a,b,c,d,e): Draw a polynomial function (ax^4+bx^3+cx^2+dx+e)\n");
	gotoxy(101,3); printf("sin(a,b,c,d): Draw sin graph (asin(bx+c)+d)");
	gotoxy(101,4); printf("cos(a,b,c,d): Draw cos graph (acos(bx+c)+d)");
	gotoxy(101,5); printf("tan(a,b,c,d): Draw tan graph (atan(bx+c)+d)");
	gotoxy(101,6); printf("exponential(a,b,c,d): Draw exponential function (a^(bx+c)+d)");
	gotoxy(101,7); printf("ln(a,b,c,d): Draw nature logarithm function (a*ln(bx+c)+d)");
	gotoxy(101,8); printf("logarithm(a,b,c,d): Draw logarithm function (a*logb(cx+d)+e)");
	gotoxy(101,9); printf("rational(a,b,c,d): Draw rational function (c/(ax+b)+d)");
	gotoxy(101,10); printf("irrational(a,b,c,d): Draw irrational function (sqrt(ax+b)+c)");
	gotoxy(101,11); printf("quit: Exit Program\n");
	gotoxy(101,12); printf("Press Enter to continue");
	while(getch()=='\n');
	int i,j;
	for(i=1;i<=20;i++) {
		gotoxy(101,i);
		for(j=0;j<100;j++)
			printf(" ");
	}
	return 0;
}
