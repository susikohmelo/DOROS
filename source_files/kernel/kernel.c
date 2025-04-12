
extern void main()
{
	*(char*)0xb8000 = 'L';

	while (1)
	{}

	return ;
}
