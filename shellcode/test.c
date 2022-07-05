int main(void)
{
	char *code = "<shellcode_string>";
	int (*func)();
	func = (int (*)())code;
	func();
	return (0);
}
