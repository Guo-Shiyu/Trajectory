#include <graphics.h>
#include <conio.h>
#include "../../header/client/test.h"

int main() 
{
	initgraph(640, 480);
	circle(200, 200, 100);
	_getch();
	closegraph();
	sub_test(4, 3);
	return 0;
}