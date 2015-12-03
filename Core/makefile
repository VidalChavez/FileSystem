prueba: FS_core_functions.o FS_high_level_functions.o FS_API.o pruebas.o
	gcc FS_core_functions.o FS_high_level_functions.o FS_API.o pruebas.o -o prueba
pruebas.o: pruebas.c
	gcc -c pruebas.c -o pruebas.o
FS_core_functions.o: FS_core_functions.c
	gcc -c FS_core_functions.c -o FS_core_functions.o
FS_high_level_functions.o: FS_high_level_functions.c
	gcc -c FS_high_level_functions.c -o FS_high_level_functions.o
FS_API.o: FS_API.c
	gcc -c FS_API.c -o FS_API.o
clean:
	rm *.o *.c~
