#include <iostream>
#include <vector>
#include <windows.h>
#include <ctime>
#include <conio.h>
#include <regex>
#include <algorithm>
#include <iomanip>
using namespace std;

void gotoxy(SHORT x, SHORT y) {
	HANDLE stdhandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	COORD xy = { x, y };
	::SetConsoleCursorPosition(stdhandle, xy);
}
template<size_t XSize, size_t YSize>
class board {
	static const char blank = ' ';
	static const int ESC = 0x1B;
	static const int PLUS = 0x2B;
	static const int MINUS = 0x2D;
	static const int ENTER = 0x0D;
	static const int SPACE = 0x20;
	static const int default_speed = 10;	// dot per second
	static const int max_speed = 500;
	static const int min_speed = 1;
	static const int SEC2MILLI = 1000;

	vector<char> screen;
	vector<int> pos;
	string target;
	int speed;
	int index;
	clock_t need_to_update;
public :
	board()
		: screen(XSize*YSize, blank)
		, pos(XSize*YSize, 0)
		, speed(default_speed)
		, index(0)
		, need_to_update(0)
	{
		shuffle(pos);
	}
	~board() {}
	void run() {
		clock_t s = clock();
		clock_t e = s;
		clock_t dt = e - s;
		while (true) {
			if (input()) {
				break;
			}
			if (update(dt)) {
				draw();
			}

			s = e;
			e = clock();
			dt = e - s;
		}
	}
private :
	void shuffle(vector<int>& cont) {
		int i = 0;
		for (int& e : cont) {
			e = i++;
		}
		::srand((unsigned int)::time(nullptr));
		for (int i = cont.size() - 1; i > 0; --i) {
			int s = ::rand() % i;
			swap(cont[i], cont[s]);
		}
	}
	bool input() {
		bool stop = false;
		if (::_kbhit()) {
			int command = ::_getch();
			switch (command) {
			case ESC : // quit
				stop = true;
				break;
			case PLUS : // increase speed
				++speed;
				if (speed > max_speed) {
					speed = max_speed;
				}
				break;
			case MINUS : // decrease speed
				--speed;
				if (speed < min_speed) {
					speed = min_speed;
				}
				break;
			case SPACE : // reset speed
				reset();
				break;
			case ENTER : // change string
				reset();
				target = prompt();
				break;
			default : // do nothing
				break;
			}
		}
		return stop;
	}
	bool update(clock_t& dt) {

		bool need_to_draw = false;
		need_to_update += dt;
		int current_speed = speed;

		if (need_to_update >= SEC2MILLI / current_speed) {
			while (need_to_update >= SEC2MILLI / current_speed) {
				if (screen[pos[index]] == blank) {
					screen[pos[index]] = '#';
				}
				else {
					screen[pos[index]] = blank;
				}
				++index;
				if (index >= XSize*YSize) {
					index = 0;
					shuffle(pos);
				}

				need_to_update -= SEC2MILLI / current_speed;
			}
			need_to_draw = true;
		}

		return need_to_draw;
	}
	void draw() {
		gotoxy(0, 0);
		size_t i = 0;
		for (auto& c : screen) {
			cout << c;
			if (++i == XSize) {
				cout << endl;
				i = 0;
			}
		}
		cout << "+(speed up) -(speed down) space(reset) esc(quit)" << endl;
		cout << "current speed : " << setw(3) << setfill(' ') << speed << " dot/second";
	}
	void reset() {
		speed = default_speed;
		index = 0;
	}
	string prompt() {
		gotoxy(XSize / 4, YSize / 2);
		string buffer;
		cin >> buffer;

		buffer = confirm(buffer);

		return buffer;
	}
	string confirm(const string& buffer) {
		string converted;
		cmatch cm;
		converted = regex_replace(buffer, regex("[^a-zA-Z]"), string(""));
		return converted;
	}
};
int main(void) {
	board<40, 20> screen;
	screen.run();

//	while (1) {
//		int c = _getch();
//		cout << hex << c << endl;
//	}

	return 0;
}