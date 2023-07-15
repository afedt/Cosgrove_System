all: lionel mcGruder paquita


utils.o: ../libs/utils.c ../libs/utils.h
	gcc -c ../libs/utils.c -Wall -Wextra -ggdb3 -lrt

logic.o: ../McGruder/logic.c ../McGruder/logic.h
	gcc -c logic.c -Wall -Wextra -ggdb3

mcGruder.o: ../McGruder/mcGruder.c ../libs/utils.h ../libs/types.h ../McGruder/logic.h
	gcc -c ../McGruder/mcGruder.c -Wall -Wextra -ggdb3

mcGruder: utils.o ../McGruder/mcGruder.o ../McGruder/logic.o
	gcc utils.o ../McGruder/mcGruder.o ../McGruder/logic.o -o ../McGruder/McGruder -ggdb3

paquita.o: ../Paquita/Paquita.c ../libs/utils.h ../libs/types.h
	gcc -c ../Paquita/Paquita.c -Wall -Wextra

paquita: utils.o ../Paquita/Paquita.o
	gcc utils.o ../Paquita/Paquita.o -o ../Paquita/paquita

lionel.o: lionel.c ../libs/utils.h ../libs/types.h
	gcc -c lionel.c -Wall -Wextra -lpthread -ggdb3 -lrt

lionel: utils.o lionel.o
	gcc utils.o lionel.o -o Lionel -lpthread -ggdb3 -lrt


clean: 
	rm -f *.o lionel
	rm -f ../McGruder/*.o mcGruder
	rm -f ../Paquita/*.o paquita
