/*
	init函数只是供main调用，初始化一些.exe文件用到的数据结构，input的初始化，和内置类型的初始化。不明白为什么要把内置类型的初始化放到这个地方。
*/

void init(int argc, char *argv[]) {
	{extern void input_init(int, char *[]); input_init(argc, argv);}
	{extern void main_init(int, char *[]); main_init(argc, argv);}
	{extern void type_init(int, char *[]); type_init(argc, argv);}
}
