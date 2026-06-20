#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 20
#define LENGTH 20

typedef struct
{
	int site_value;
} map_site;

void Initmap_site(map_site* map_site)
{
	map_site->site_value = 0;
}

char sandchar(int value)
{
	switch (value)
	{
		case 0: return ' ';
		case 1: return '.';
		case 2: return 'o';
		case 3: return '#';
		default: return '#';
	}
}

const char* sandcolor(int value)
{
	switch (value)
	{
		case 0: return "\033[0m";
		case 1: return "\033[34m";
		case 2: return "\033[33m";
		case 3: return "\033[31m";
		default: return "\033[31m";
	}
}

void printmap(map_site map[WIDTH][LENGTH])
{
	printf("\033[H\033[J");

	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < LENGTH; j++)
		{
			printf("%s%c\033[0m ", sandcolor(map[i][j].site_value), sandchar(map[i][j].site_value));
		}
		printf("\n");
	}

	fflush(stdout);
}

void checksite(map_site map[WIDTH][LENGTH], int i, int j)
{
	map_site* mapsite = &map[i][j];

	mapsite->site_value += 1;
	printmap(map);
	usleep(100);

	if (mapsite->site_value >= 4)
	{
		mapsite->site_value = 0;
		printmap(map);
		usleep(100);

		if (i > 0)          checksite(map, i - 1, j);
		if (i < WIDTH - 1)  checksite(map, i + 1, j);
		if (j > 0)          checksite(map, i, j - 1);
		if (j < LENGTH - 1) checksite(map, i, j + 1);
	}
}

void run(map_site map[WIDTH][LENGTH], int start_i, int start_j, int num_grains)
{
	for (int n = 0; n < num_grains; n++)
	{
		checksite(map, start_i, start_j);
	}
}

int main(int argc, char* argv[])
{
	map_site map[WIDTH][LENGTH];

	int start_i = WIDTH / 2;
	int start_j = LENGTH / 2;
	int num_grains = 1000;

	if (argc >= 3)
	{
		start_i = atoi(argv[1]);
		start_j = atoi(argv[2]);
	}

	if (argc >= 4)
	{
		num_grains = atoi(argv[3]);
	}

	if (start_i < 0 || start_i >= WIDTH || start_j < 0 || start_j >= LENGTH)
	{
		printf("starting position must be between 0 and %d for row, 0 and %d for column\n", WIDTH - 1, LENGTH - 1);
		return 1;
	}

	if (num_grains < 0)
	{
		printf("number of grains must be 0 or greater\n");
		return 1;
	}

	for (int i = 0; i < WIDTH; i++)
	{
		for(int j = 0; j< LENGTH; j++)
		{
			Initmap_site(&map[i][j]);
		}
	}

	run(map, start_i, start_j, num_grains);

	return 0;
}
