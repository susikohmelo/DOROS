
extern void main()
{
	*(char*)0xb8000 = 'T';
	*(char*)0xb8002 = 'e';
	*(char*)0xb8004 = 's';
	*(char*)0xb8006 = 't';

	while (1)
	{}

	return ;
}
