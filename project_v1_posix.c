#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>     // read, STDIN_FILENO
#include <termios.h>    // 키 입력(getch) 구현용
#include <sys/ioctl.h>  // 터미널 크기 확인용

// 가로 100  세로 50 , x=0->i=49, y=0->j=24
// x: 49+i, y: 24-j
//
// [POSIX 포팅 메모]
// - windows.h (COORD, SetConsoleCursorPosition) -> ANSI 이스케이프 시퀀스
// - system("cls")                              -> 화면 지우기 이스케이프 시퀀스
// - conio.h의 getch()                           -> termios로 직접 구현
// - 터미널 크기: 가로 181칸 이상, 세로 51줄 이상 필요

#define PANEL_X 101   // 오른쪽 입력/도움말 패널 시작 열
#define PANEL_W 80    // 패널 지울 때 공백 개수 (원래 100 -> 터미널 폭 때문에 축소)

struct credit {
	char name[32];
};

//콘솔 창 커서 이동 (좌표는 0부터 시작, ANSI는 1부터 시작이라 +1)
void gotoxy(int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	printf("\033[%d;%dH", y + 1, x + 1);
}

// system("cls") 대체
void cls(void) {
	printf("\033[2J\033[H");
	fflush(stdout);
}

// conio.h의 getch() 대체: 엔터 없이 키 하나를 읽음 (화면에 에코 안 함)
int getch(void) {
	struct termios oldt, rawt;
	unsigned char ch = 0;
	ssize_t n;

	fflush(stdout);
	if (tcgetattr(STDIN_FILENO, &oldt) == -1) {   // 터미널이 아니면(파이프 등) 그냥 읽기
		n = read(STDIN_FILENO, &ch, 1);
		return n == 1 ? ch : EOF;
	}
	rawt = oldt;
	rawt.c_lflag &= ~(ICANON | ECHO);
	rawt.c_cc[VMIN] = 1;
	rawt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &rawt);
	n = read(STDIN_FILENO, &ch, 1);
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return n == 1 ? ch : EOF;
}

// 값이 NaN/무한대/너무 크면 (int) 변환이 정의되지 않은 동작이라 막아줌
static int row_of(double v) {
	if (!isfinite(v) || v > 1000.0 || v < -1000.0)
		return 9999;   // 화면 밖 값 -> 절대 그려지지 않음
	return (int)v;
}

int axis() {
	int i, j = 0;
	for (i = -49; i <= 50; i += 1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (i == 0 && j == 0)
				printf("+");
			else if (i == 0)
				printf("|");
			else if (j == 0)
				printf("-");
		}
	}
	fflush(stdout);
	return 0;
}

int inst();

void credits() {
	int i = 0;
	struct credit elses[5] = {{"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}};
	struct credit special[4] = {{"Monster Energy"}, {"One of my friend"}, {"Youtube"}, {"Dev-C++"}};
	gotoxy(PANEL_X, 1); printf("%s: %s\n", "Assignment", elses[0].name);
	gotoxy(PANEL_X, 2); printf("%s: %s\n", "Idea", elses[1].name);
	gotoxy(PANEL_X, 3); printf("%s: %s\n", "Meal supply", elses[2].name);
	gotoxy(PANEL_X, 4); printf("%s: %s\n", "The one who is sleepy", elses[3].name);
	gotoxy(PANEL_X, 5); printf("%s: %s\n", "Assignment", elses[4].name);
	gotoxy(PANEL_X, 6); printf("Special Thanks\n");
	for (i = 0; i < 4; i++) {
		gotoxy(PANEL_X, i + 7);
		printf("%s\n", special[i].name);
	}
	getch();   // 아무 키나 누르면 종료
	cls();
	axis();
}

//다항함수 출력
int polynomial(float a, float b, float c, float d, float e) {
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * pow(i, 4) + b * pow(i, 3) + c * pow(i, 2) + d * i + e) == j)
				printf("*");
		}
	}
	fflush(stdout);
	return 0;
}

//삼각함수  예시: 10 0.25 0 0
void trisin(float a, float b, float c, float d) {
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * sin(b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void tricos(float a, float b, float c, float d) {
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * cos(b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void tritan(float a, float b, float c, float d) {
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * tan(b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void exponential(float a, float b, float c, float d) { //예시: 2 0.1 2.5 -25
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(pow(a, b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void naturalLog(float a, float b, float c, float d) { //예시: 5 1 1 1
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * log(b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void logarithm(float a, float b, float c, float d, float e) { //예시: 10 2 1 25 -25
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * (log(c * i + d) / log(b)) + e) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void rational(float a, float b, float c, float d) { // 예시: 1 0 50 0
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(c / (a * i + b) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

void irrational(float a, float b, float c, float d) {
	float i = 0;
	int j = 0;
	for (i = -49; i <= 50; i += 0.1) {
		for (j = 24; j >= -25; j -= 1) {
			gotoxy(49 + i, 24 - j);
			if (row_of(a * sqrt(b * i + c) + d) == j)
				printf("*");
		}
	}
	fflush(stdout);
}

// 패널의 한 줄을 공백으로 지움
static void clear_panel_line(int y) {
	int k;
	gotoxy(PANEL_X, y);
	for (k = 0; k < PANEL_W; k++)
		printf(" ");
}

// 터미널이 너무 작으면 경고
static void check_terminal_size(void) {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
		if (ws.ws_col < PANEL_X + PANEL_W || ws.ws_row < 51) {
			fprintf(stderr, "경고: 터미널이 작습니다 (현재 %dx%d, 필요 %dx%d 이상).\n"
			        "창을 키우거나 글자 크기를 줄인 뒤 실행하세요. 계속하려면 Enter...\n",
			        ws.ws_col, ws.ws_row, PANEL_X + PANEL_W, 51);
			getchar();
		}
	}
}

int main(void) {
	int i = 0;
	char input[100] = {0};
	float a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0;

	check_terminal_size();
	cls();
	gotoxy(0, 0);
	axis();

	while (1) {
		clear_panel_line(0);
		for (i = 0; i < 100; i++)
			input[i] = 0;
		gotoxy(PANEL_X, 0);
		printf("> ");
		fflush(stdout);

		if (scanf("%99s", input) != 1)   // EOF(Ctrl+D)면 종료
			break;

		if (strcmp(input, "quit") == 0)
			break;
		else if (strcmp(input, "help") == 0)
			inst();
		else if (strcmp(input, "clear") == 0) {
			cls();
			gotoxy(0, 0);
			axis();
		}
		else if (strcmp(input, "polynomial") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f %f", &a1, &a2, &a3, &a4, &a5);
			polynomial(a1, a2, a3, a4, a5);
			clear_panel_line(1);
		}
		else if (strcmp(input, "sin") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			trisin(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "cos") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			tricos(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "tan") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			tritan(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "exponential") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			exponential(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "ln") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			naturalLog(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "logarithm") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f %f", &a1, &a2, &a3, &a4, &a5);
			logarithm(a1, a2, a3, a4, a5);
			clear_panel_line(1);
		}
		else if (strcmp(input, "rational") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);
			rational(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "irrational") == 0) {
			gotoxy(PANEL_X, 1); printf("Enter a coefficient: "); fflush(stdout);
			scanf("%f %f %f %f", &a1, &a2, &a3, &a4);   // 원본은 %f 3개만 받아서 버그였음
			irrational(a1, a2, a3, a4);
			clear_panel_line(1);
		}
		else if (strcmp(input, "credits") == 0)
			credits();
	}

	gotoxy(0, 51);   // 종료 시 커서를 그래프 아래로
	printf("\n");
	return 0;
}

int inst() {
	int i;
	gotoxy(PANEL_X, 1);  printf("help: Print information about commands");
	gotoxy(PANEL_X, 2);  printf("polynomial(a,b,c,d,e): Draw a polynomial function (ax^4+bx^3+cx^2+dx+e)");
	gotoxy(PANEL_X, 3);  printf("sin(a,b,c,d): Draw sin graph (asin(bx+c)+d)");
	gotoxy(PANEL_X, 4);  printf("cos(a,b,c,d): Draw cos graph (acos(bx+c)+d)");
	gotoxy(PANEL_X, 5);  printf("tan(a,b,c,d): Draw tan graph (atan(bx+c)+d)");
	gotoxy(PANEL_X, 6);  printf("exponential(a,b,c,d): Draw exponential function (a^(bx+c)+d)");
	gotoxy(PANEL_X, 7);  printf("ln(a,b,c,d): Draw nature logarithm function (a*ln(bx+c)+d)");
	gotoxy(PANEL_X, 8);  printf("logarithm(a,b,c,d,e): Draw logarithm function (a*logb(cx+d)+e)");
	gotoxy(PANEL_X, 9);  printf("rational(a,b,c,d): Draw rational function (c/(ax+b)+d)");
	gotoxy(PANEL_X, 10); printf("irrational(a,b,c,d): Draw irrational function (a*sqrt(bx+c)+d)");
	gotoxy(PANEL_X, 11); printf("clear: Clear the graph");
	gotoxy(PANEL_X, 12); printf("quit: Exit Program");
	gotoxy(PANEL_X, 13); printf("Press Enter to continue");
	fflush(stdout);

	// Enter가 눌릴 때까지 대기 (원본의 while(getch()=='\n')은 Windows에서 Enter가 '\r'이라 의도와 다르게 동작)
	while (1) {
		int c = getch();
		if (c == '\n' || c == '\r' || c == EOF)
			break;
	}

	for (i = 1; i <= 20; i++)
		clear_panel_line(i);
	return 0;
}
