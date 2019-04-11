
assembler: assembler.o firstPass.o models.o secondPass.o utils.o assembler.h firstPass.h models.h secondPass.h utils.h
	gcc -g -Wall -ansi -pedantic assembler.o firstPass.o models.o secondPass.o utils.o -o assembler -lm
	
assembler.o: assembler.c assembler.h models.h firstPass.h secondPass.h utils.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
	
firstPass.o: firstPass.c firstPass.h models.h utils.h secondPass.h
	gcc -c -Wall -ansi -pedantic firstPass.c -o firstPass.o
	
models.o: models.c models.h firstPass.h utils.h
	gcc -c -Wall -ansi -pedantic models.c -o models.o
	
secondPass.o: secondPass.c secondPass.h firstPass.h models.h utils.h
	gcc -c -Wall -ansi -pedantic secondPass.c -o secondPass.o
	
utils.o: utils.c utils.h models.h firstPass.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o