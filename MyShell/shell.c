#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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

int launch_func(char** args) {
	
	pid_t pid; // Process ID (iþlem numarasý). Unix/Linux sistemlerinde çalýþan her programa bir "TC Kimlik Numarasý" verilir (1024, 5432 gibi tamsayýlar). Bölündükten sonra "Ben anne miyim, çocuk muyum?" sorusunun cevabýný tutacak.
	int status; // Çocuðun iþi bittiðinde nasýl kapandýðýný (baþarýlý mý, hata mý?) tutacak.

	pid = fork(); // Kopyalama.

	if (pid == 0) { // Çocuksa pid = 0, anneyse pid çocuðun numarasýdýr (örneðin 12345).
		
		if (execvp(args[0], args) == -1) { // NOT1: exec ailesinin fonksiyonlarý baþarýlý olduðunda return etmez sadece baþarýsýz olduðunda -1 döndürür ve kod bir alt satýra geçer. NOT2: args[0] yazdýk çünkü 0. indeks çalýþtýrýlacak programý, diðerleri ise detaylarý gösterir.
			perror("myshell"); // perror, hatanýn sebebini otomatik olarak ekrana basar. fprintf kullansaydýk hatanýn sebebini (dosya mý yok, RAM mi yetmedi) bilemezdik.
		}
		
		exit(EXIT_FAILURE); // Eðer execvp bulursa, bulduðu program bu kodun üzerine yazýlacaðýndan artýk ortada exit diye bir komut kalmaz. Bu satýr fiziksel olarak yok olur ve çalýþmaz.
	
	}
	
	else if (pid < 0) { // Kopyalama hatasý.
		perror("myshell");
	}
	
	else {

		do {
			waitpid(pid, &status, WUNTRACED); // pid -> hangisini bekleyeceði, status -> öldü mü, hata mý verdi?, WUNTRACED -> Sadece ölmesini bekleme; eðer çocuk durdurulursa (pause edilirse) da bana haber ver.
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); // !WIFEXITED(status) -> normal yollarla (iþini bitirip) çýkýþ yapmadýysa, !WIFSIGNALED(status) ->  dýþarýdan bir sinyalle öldürülmediyse döngüde beklemeye devam et.

	}

	return 1; // 0 dönerse shell kapanýr, 1 dönerse shell sýradaki komutu almaya devam eder.

}



