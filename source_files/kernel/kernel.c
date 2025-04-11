
extern void main()
{
	*(char*)0xb800 = 'L';

	while (1)
	{}

	return ;
}
