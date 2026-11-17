#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>     // read, STDIN_FILENO
#include <termios.h>    // 키 입력(getch) 구현용
#include <sys/ioctl.h>  // 터미널 크기 확인용

/*
 * 함수 그래프 그리기 (POSIX / 80x24 터미널용)
 *
 * 화면 구성 (80열 x 24줄)
 *   0~20줄 : 그래프 (가로 79칸 x 세로 21칸, 원점은 (39,10), 1칸 = 1단위)
 *   21줄   : 상태/안내 메시지
 *   22줄   : 입력줄
 *   23줄   : 비워 둠 (마지막 줄에서 Enter를 치면 화면이 밀리므로)
 *
 * 그래프는 grid[][] 배열에 먼저 그려 두고, 필요할 때 한 번에 출력한다.
 * 그래서 help/credits 화면을 보여 준 뒤에도 그래프가 그대로 복원된다.
 */

#define W   79
#define H   21
#define CX  39   // 원점 열
#define CY  10   // 원점 줄

#define ROW_STATUS 21
#define ROW_INPUT  22
#define ROW_LAST   23

static char grid[H][W + 1];
static char status[128] = "help 를 입력하면 명령어 목록이 나옵니다.";

/* ---------- 터미널 제어 ---------- */

// 커서 이동 (좌표는 0부터, ANSI는 1부터라서 +1)
static void gotoxy(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }
static void erase_line(int y)    { gotoxy(0, y); printf("\033[2K"); }
static void cls(void)            { printf("\033[2J\033[H"); }

// 엔터 없이 키 하나 읽기 (conio.h getch 대체)
static int getch(void) {
	struct termios oldt, rawt;
	unsigned char ch = 0;
	ssize_t n;

	fflush(stdout);
	if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
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

/* ---------- 그래프 버퍼 ---------- */

// 축만 있는 빈 그래프로 초기화
static void grid_reset(void) {
	int r, c;
	for (r = 0; r < H; r++) {
		for (c = 0; c < W; c++) {
			if (r == CY && c == CX) grid[r][c] = '+';
			else if (c == CX)       grid[r][c] = '|';
			else if (r == CY)       grid[r][c] = '-';
			else                    grid[r][c] = ' ';
		}
		grid[r][W] = '\0';
	}
}

static void grid_render(void) {
	int r;
	cls();
	for (r = 0; r < H; r++) {
		gotoxy(0, r);
		fputs(grid[r], stdout);
	}
	fflush(stdout);
}

/* ---------- 함수 표 ---------- */
// c[0..4] = a,b,c,d,e

typedef double (*func_t)(const double *c, double x);

static double f_poly(const double *c, double x) { return c[0]*pow(x,4) + c[1]*pow(x,3) + c[2]*x*x + c[3]*x + c[4]; }
static double f_sin (const double *c, double x) { return c[0]*sin(c[1]*x + c[2]) + c[3]; }
static double f_cos (const double *c, double x) { return c[0]*cos(c[1]*x + c[2]) + c[3]; }
static double f_tan (const double *c, double x) { return c[0]*tan(c[1]*x + c[2]) + c[3]; }
static double f_exp (const double *c, double x) { return pow(c[0], c[1]*x + c[2]) + c[3]; }
static double f_ln  (const double *c, double x) { return c[0]*log(c[1]*x + c[2]) + c[3]; }
static double f_log (const double *c, double x) { return c[0]*(log(c[2]*x + c[3]) / log(c[1])) + c[4]; }
static double f_rat (const double *c, double x) { return c[2] / (c[0]*x + c[1]) + c[3]; }
static double f_irr (const double *c, double x) { return c[0]*sqrt(c[1]*x + c[2]) + c[3]; }

struct command {
	const char *name;
	int ncoef;            // 필요한 계수 개수
	const char *formula;  // help에 보여 줄 식
	func_t f;
	const char *example;
};

static const struct command table[] = {
	{"polynomial", 5, "a*x^4+b*x^3+c*x^2+d*x+e", f_poly, "0 0 0.05 0 -5"},
	{"sin",        4, "a*sin(b*x+c)+d",           f_sin,  "10 0.25 0 0"},
	{"cos",        4, "a*cos(b*x+c)+d",           f_cos,  "10 0.25 0 0"},
	{"tan",        4, "a*tan(b*x+c)+d",           f_tan,  "1 0.1 0 0"},
	{"exponential",4, "a^(b*x+c)+d",              f_exp,  "2 0.1 2.5 -10"},
	{"ln",         4, "a*ln(b*x+c)+d",            f_ln,   "5 1 1 1"},
	{"logarithm",  5, "a*log_b(c*x+d)+e",         f_log,  "10 2 1 25 -10"},
	{"rational",   4, "c/(a*x+b)+d",              f_rat,  "1 0 20 0"},
	{"irrational", 4, "a*sqrt(b*x+c)+d",          f_irr,  "3 1 0 0"},
};
#define NCMD ((int)(sizeof(table) / sizeof(table[0])))

/*
 * 그래프 그리기.
 * 칸 하나당 10번씩 촘촘히 x를 샘플링해서 y를 구하고, 화면 안이면 '*'를 찍는다.
 * 기울기가 가파른 구간(x^4, tan 등)도 끊기지 않고, 점근선에서 엉뚱한 세로선도 생기지 않는다.
 */
static void plot(const struct command *cmd, const double *c) {
	int n;
	for (n = -(W / 2) * 10 - 5; n <= (W / 2) * 10 + 5; n++) {
		double x = n / 10.0;
		double y = cmd->f(c, x);
		int col, row;
		if (!isfinite(y) || fabs(y) > 1000.0)
			continue;                       // NaN, 무한대, 너무 큰 값은 건너뜀
		col = CX + (int)lround(x);
		row = CY - (int)lround(y);
		if (col >= 0 && col < W && row >= 0 && row < H)
			grid[row][col] = '*';
	}
}

/* ---------- 입력 ---------- */

// 문자열에서 실수를 최대 need개 읽고, 읽은 개수를 반환
static int parse_nums(const char *s, double *out, int need) {
	int n = 0;
	char *end;
	while (n < need) {
		out[n] = strtod(s, &end);
		if (end == s) break;
		n++;
		s = end;
	}
	return n;
}

// 입력줄 출력 후 한 줄 읽기. EOF면 0
static int read_line(const char *prompt, char *buf, int size) {
	erase_line(ROW_INPUT);
	printf("%s", prompt);
	fflush(stdout);
	if (!fgets(buf, size, stdin))
		return 0;
	buf[strcspn(buf, "\r\n")] = '\0';
	return 1;
}

static void draw_panel(void) {
	erase_line(ROW_STATUS);
	printf("%.79s", status);
	erase_line(ROW_INPUT);
}

/* ---------- 도움말 / 크레딧 ---------- */

static void wait_key(void) {
	gotoxy(0, ROW_LAST - 1);
	printf("아무 키나 누르면 돌아갑니다...");
	getch();
}

static void show_help(void) {
	int i, k, row = 1;
	char vars[8];
	cls();
	gotoxy(0, 0);
	printf("[명령어]  명령어 뒤에 계수를 바로 적거나, 적지 않으면 따로 물어봅니다.");
	for (i = 0; i < NCMD; i++) {
		for (k = 0; k < table[i].ncoef; k++) vars[k] = 'a' + k;
		vars[k] = '\0';
		gotoxy(0, row++);
		printf("%s %s", table[i].name, vars);
		gotoxy(0, row++);
		printf("   %s  (예: %s %s)", table[i].formula, table[i].name, table[i].example);
	}
	gotoxy(0, row++); printf("clear    그래프 지우기");
	gotoxy(0, row++); printf("credits  크레딧");
	gotoxy(0, row++); printf("quit     종료");
	wait_key();
	grid_render();
}

struct credit {
	char name[32];
};

static void show_credits(void) {
	struct credit elses[5] = {{"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}, {"Felix Brown"}};
	struct credit special[4] = {{"Monster Energy"}, {"One of my friend"}, {"Youtube"}, {"Dev-C++"}};
	int i;
	cls();
	gotoxy(0, 1); printf("%s: %s", "Assignment",        elses[0].name);
	gotoxy(0, 2); printf("%s: %s", "Idea",    elses[1].name);
	gotoxy(0, 3); printf("%s: %s", "Meal supply",   elses[2].name);
	gotoxy(0, 4); printf("%s: %s", "The one who is sleepy", elses[3].name);
	gotoxy(0, 5); printf("%s: %s", "Assignment",        elses[4].name);
	gotoxy(0, 7); printf("Special Thanks");
	for (i = 0; i < 4; i++) {
		gotoxy(0, 8 + i);
		printf("%s", special[i].name);
	}
	wait_key();
	grid_render();
}

/* ---------- main ---------- */

static void check_terminal_size(void) {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && (ws.ws_col < 80 || ws.ws_row < 24)) {
		fprintf(stderr, "경고: 터미널이 작습니다 (현재 %dx%d, 필요 80x24 이상). 계속하려면 Enter...\n",
		        ws.ws_col, ws.ws_row);
		getchar();
	}
}

int main(void) {
	char line[256];

	check_terminal_size();
	grid_reset();
	grid_render();

	while (1) {
		char cmdname[64];
		const char *rest;
		int off = 0, i, found = -1;

		draw_panel();
		if (!read_line("> ", line, sizeof(line)))   // EOF(Ctrl+D)
			break;
		if (sscanf(line, "%63s%n", cmdname, &off) != 1)
			continue;                               // 빈 줄
		rest = line + off;
		status[0] = '\0';

		if (strcmp(cmdname, "quit") == 0)         break;
		else if (strcmp(cmdname, "help") == 0)    { show_help();    continue; }
		else if (strcmp(cmdname, "credits") == 0) { show_credits(); continue; }
		else if (strcmp(cmdname, "clear") == 0)   { grid_reset(); grid_render(); continue; }

		for (i = 0; i < NCMD; i++)
			if (strcmp(cmdname, table[i].name) == 0) { found = i; break; }

		if (found < 0) {
			snprintf(status, sizeof(status), "알 수 없는 명령어: %.40s  (help 로 목록 확인)", cmdname);
			continue;
		}

		{
			const struct command *cmd = &table[found];
			double c[5] = {0};
			int n = parse_nums(rest, c, cmd->ncoef);

			if (n < cmd->ncoef) {                   // 계수가 모자라면 따로 입력받기
				char prompt[64], buf[256];
				snprintf(prompt, sizeof(prompt), "계수 %d개 입력: ", cmd->ncoef);
				erase_line(ROW_STATUS);
				printf("%s = %s  (예: %s)", cmd->name, cmd->formula, cmd->example);
				if (!read_line(prompt, buf, sizeof(buf)))
					break;
				n = parse_nums(buf, c, cmd->ncoef);
			}
			if (n < cmd->ncoef) {
				snprintf(status, sizeof(status), "계수가 %d개 필요합니다. (%s)", cmd->ncoef, cmd->example);
				continue;
			}
			plot(cmd, c);
			grid_render();
			snprintf(status, sizeof(status), "%s: %s", cmd->name, cmd->formula);
		}
	}

	gotoxy(0, ROW_LAST);   // 종료 후 셸 프롬프트가 그래프 아래에 나오도록
	fflush(stdout);
	return 0;
}
