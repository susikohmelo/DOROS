#include <vga_tty.h>
#include <stdint.h>

int32_t k_atoi(const uint8_t *nptr)
{
	int32_t	resulting_int;
	int8_t	sign;

	resulting_int = 0;
	sign = 1;
	while (*nptr && (*nptr == ' ' || *nptr == '\t' || *nptr == '\r'
			|| *nptr == '\n' || *nptr == '\v' || *nptr == '\f'))
		++nptr;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
			sign = -1;
		++nptr;
	}
	while (*nptr >= '0' && *nptr <= '9')
	{
		resulting_int = resulting_int * 10 + *nptr - '0';
		++nptr;
	}
	return (resulting_int * sign);
}
