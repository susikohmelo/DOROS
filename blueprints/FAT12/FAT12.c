#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct s_bootsector
{
}	t_bootsector

int	main(int argc, char **argv)
{

	if (argc < 3)
	{
		printf("%s\n", "Bad argc");
		return 1;
	}

	// Read in binary mode
	FILE *disk = fopen(argc[1], "rb");

	return 0;
}
