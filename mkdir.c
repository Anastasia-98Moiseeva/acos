#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <err.h>


//Функция вызова getopt.
bool Call_getopt(int argc, char **argv)
{
	// Показывает была ли передана опция "p".
	bool flag = false;
	int opt = getopt(argc, argv, "p");
	// Если передана озция "p". 
	if (opt != -1)
	{
		// Отметим, что опцию "p" передали.
		flag = true;
	}
	return flag;
}

// Функция склеивания двух строк.
char* Add(char* string1, char* string2)
{
	char* string = calloc(strlen(string1) + strlen(string2) + 1, 1);
	strcat(string, string1);
	strcat(string, string2);
	return string;
}

// Функция копирования строки с начала.
char* Copy_string_start(char* string1, int i)
{
	// Строка номер 2, куда будем записывать строку номер 1.
	char* string2;
	// Выделение памяти.
	string2=malloc(sizeof(char)*strlen(string1)*2);
	//Скопируем строку 1 в строку 2.
	strncpy(string2, string1, i);
	return string2;
}

// Функция копирования строки c конца.
char* Copy_string_end(char* string1, int i)
{
	// Строка номер 2, куда будем записывать строку номер 1.
	char* string2;
	// Выделение памяти.
	string2=malloc(sizeof(char)*strlen(string1)*2);
	string1 = string1 + i;
	//Скопируем строку 1 в строку 2.
	strcpy(string2, string1);
	return string2; 
}


// Функция системного вызова mkdir.
int Call_Sys_mkdir(const char* string)
{
	int status = 2;
	status = mkdir(string, 0777);
	return status;
}

// Функция системного вызова rm.
int Sys_rm(char* path)
{
	char* string1 = "rm -R ";
	int i = 0;
	int l = strlen(path);
	while (i < l && path[i] != '/')
	{
		i++;
	}
	char* string2 = Copy_string_start(path, i);
	char* string = Add(string1, string2);
	bool flag = system(string);
	return flag;
}

//Функция mkdir.
int Call_mkdir(int argc, char **argv)
{
	// Показывает была ли передана опция "p".
	bool flag = false;
	// Проверяем передавалиась ли опция "р".
	flag = Call_getopt(argc, argv);
	// Путь до каталога, в котором нужно создать новую директорию.
	char* path = " ";
	// Текущий каталог.
	char* dir = " ";
	int status = 0;
	char* str = "-p";
	// Длина рассматриваемой строки.
	int len = 0;
        // Если опцию передали.
        if (flag == true)
        {
		//  Если кроме опции ничего не передали.
		if (argc == 2){
			status = 3;
			return status;
		}
		// Дириктория, которую необходимо получить.
	        char* directory = " ";
		int icount = 1;
		while (icount < argc)
		{
			if (strcmp(argv[icount], str) != 0)
			{
				int i = 0;
				directory = argv[icount];
				len = strlen(directory);
				// Пока не дошли до конца дирректории.
				while (i < len)
				{
					// Номер счётчика, на котором был встречен "/" в прошлый раз.
					int i_ = i;
					// Пока не встретили "/" и не дошли до конца дирректории.
					while (directory[i] != '/' && i < len || i == 0)
					{
						i++;
					}
					// Запомним текущее значение пути.
					path = Copy_string_start(directory, i);
					dir = Copy_string_end(path, i_);
					status = Call_Sys_mkdir(path);		
					// Если данная папка не существует.
					if (errno == EEXIST)
					{
						if (i == len && status == -1){
							status = 4;
						}
					}
					i++; 
				}
			}
			icount++;
			if (status == 3)
			{
				// Выводим сообщение о том, что аргументы не были введены.
				printf("%s\n", "Not arguments");
			}else{
				if (status == 4)
				{
					// Выводим сообщение о том, что такая папка уже существует.
					printf("%s\n", "Directory exists");
				}
			}
		}
	}

	// Если опцию не передали.
        else{
		// Дириктория, которую необходимо получить.
	        char* directory;
		int icount = 1;
		char* past_path;
		while (icount < argc)
		{
			int i = 0;
			directory = argv[icount];
			len = strlen(directory);
			bool f = false;
			// Пока не дошли до конца дирректории.
			while (i < len)
			{
				// Номер счётчика, на котором был встречен "/" в прошлый раз.
				int i_ = i;
				// Пока не встретили "/" и не дошли до конца дирректории.
				while (directory[i] != '/' && i < len || i == 0)
				{
					i++;
				}
				if (directory[i] == '/')
				{
					// Отметим, что встретили "/".
					f = true;
				}
				// Запомним текущее значение пути.
				// Длина пути.
				int l_path = strlen(path);
				path = Copy_string_start(directory, i);
				dir = Copy_string_end(path, i_);
				status = Call_Sys_mkdir(path);	
				int l = 0;
				if (strcmp(dir, "") != 0)
				{
					l = strlen(dir);
				}
				// Если данная папка не существует.
				if (errno != EEXIST)
				{
					if (i != len || f == true)
					{
						status = 2;
						if (strcmp(past_path, " ") != 0)
						{
							int f_ = Sys_rm(past_path);
						}
					}
				}else{

					if (i == len && status == -1){
						status = 4;
					}
				}
				i++;
			}
			icount++;
			if (status == 2)
			{
				// Выводим сообщение о том, что были введены неправильные аргументы.
				printf("%s\n", "No such file or directory");
			}
			else{
				if (status == 3)
				{
					// Выводим сообщение о том, что аргументы не были введены.
					printf("%s\n", "Not arguments");
				}else{
					if (status == 4)
					{
						// Выводим сообщение о том, что такая папка уже существует.
						printf("%s\n", "Directory exists");
					}
				}
			}

		}
	}	
	return status;
}

int main(int argc, char **argv)
{
	// Если передаём аргументы. 
	if (argc > 1)
	{
		int status = 1;
		status = Call_mkdir(argc, argv);
	}
	// Если не передааём аргументы.
	else{
		// Выводим сообщение о том, что аргументы не были введены.
		printf("%s\n", "Not arguments");
	}
	
	return 0;
}
