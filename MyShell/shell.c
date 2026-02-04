#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RL_BUFSIZE 1024 // Tahminen kullanýcýnýn 1024 karakterden daha uzun yazmayacaðýný varsayýyoruz. Eðer yazarsa realloc ile halledeceðiz ama þimdilik 1024'lük bir yer ayýrýyoruz.
#define TOK_BUFSIZE 64 // Tahminen kullanýcýnýn BÝR SATIRDA 64 karakterden fazla yazmayacaðýný varsayýyyoruz.
#define TOK_DELIM " \t\r\n\a"

char* read_line_func() {

	int bufsize = RL_BUFSIZE;
	int position = 0; // Bu bizim sayacýmýz. Kullanýcý harf yazdýkça buffer[0], buffer[1] diye ilerleyeceðiz. Baþlangýçta 0. sýradayýz.
	char* buffer = malloc(sizeof(char) * bufsize);
	int c; // EOF için.

	if (!buffer) { // Eðer boþ alan yoksa.
		fprintf(stderr, "allocation error\n"); // stderr'e yazýlanlar dosyaya gitmez, her zaman ekrana basýlýr. Hata mesajý kaybolmasýn diye profesyonel kodlarda hata basarken hep stderr kullanýlýr.
		exit(EXIT_FAILURE); // return programý çaðýran yere geri dönerken exit programý o anda, olduðu yerde sonlandýrýr.
	}

	while (1) { // Kullanýcýnýn kaç harf yazacaðýný bilmiyoruz. Kullanýcý "Enter" tuþuna basana kadar döngü dönecek.
		c = getchar();

		if (c == EOF || c == '\n') { // Kullanýcý akýþý kesti mi veya kullanýcý "Enter" tuþuna bastý mý?
			buffer[position] = '\0';
			return buffer;
		}

		else {
			buffer[position] = c;
		}

		position++;
		
		if (position >= bufsize) {
			bufsize += RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);

			if (!buffer) {
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char** split_line_func(char *line) {

	int bufsize = TOK_BUFSIZE;
	int position = 0;
	char** tokens = malloc(sizeof(char*) * bufsize);
	char* token;

	if (!tokens) {
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOK_DELIM);

	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*)); 

			if (!tokens) {
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOK_DELIM);
	}

	tokens[position] = NULL; // C dilinde bir dizinin uzunluðunu bilmek zordur. Birazdan yazacaðýmýz execute fonksiyonu bu listeyi okurken nerede duracaðýný bilemez. O yüzden listenin sonuna biz elle bir "DUR" tabelasý koyuyoruz: tokens = ["ls", "-la", "/home", NULL].

		return tokens;

}

