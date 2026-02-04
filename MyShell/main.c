#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

void loop_func() {

	char* line; // Kullanýcýnýn terminale yazdýðý bütün satýrý tutacak. (Örn: "ls -la /home")
	char** args; // O satýrýn diziye parçalanmýþ halini tutacak.
	int status; // Döngünün devam edip etmeyeceðini tutacak. (1 devam, 0 çýkýþ)

	do {
		
		printf("> ");

		line = read_line_func(); // Kullanýcýnýn klavyeden girdiði yazýyý okur. Enter'a basana kadar bekler.
		args = split_line_func(line); // Okunan o ham metni boþluklarýndan böler. Bize kelime listesini (char **) verir.
		status = execute_func(args); // Asýl iþ burada yapýlýyor. args[0] içindeki komutu (Örneðin "ls") çalýþtýrýr. Ýþ bitince geriye 1 (devam et) veya 0 (programdan çýk) döndürür.


	} while (status); // Shell açýldýðýnda en az bir kez çalýþmasý lazým o yüzden while yerine do while.

	free(line); // Mallocla açýlan alaný silmeliyiz.
	free(args); 

}


int main(int argc, char** argv)
{

	loop_func();

	return EXIT_SUCCESS; // 0 ile ayný þey.

}