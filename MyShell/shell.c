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

char* builtin_str[] = {
	"cd",
	"help",
	"exit"
}; // Shell'in tanýdýðý kelimelerin listesidir. Kullanýcý bir þey yazdýðýnda, Shell bu listeye bakacak: "Kullanýcýnýn yazdýðý kelime bu listede var mý?"

int (*builtin_func[]) (char**) = {
	&cd_func,
	&help_func,
	&exit_func
}; // Tabeladaki ismin, arka plandaki "gerçek iþi yapan koda" baðlanmasýdýr. Sýralama önemli.

int num_builtins_func() {
	return sizeof(builtin_str) / sizeof(char*);
}

/* Aþaðýdaki þekilde de yapýlabilirdi ama biraz daha amatör kaçardý :
	
if (strcmp(komut, "cd") == 0) 
	cd_func(args);
else if (strcmp(komut, "help") == 0) 
	help_func(args);
else if (strcmp(komut, "exit") == 0) 
	exit_func(args);

Benim yaptýðým yöntem ise þu þekilde çalýþýyor :

1. Elimizde komut isimleri listesi var.
2. Elimizde fonksiyon adresleri listesi var.
3. Bir döngümüz var.
	
	-> "Kullanýcýnýn yazdýðý kelime, 1. listede kaçýncý sýrada? (Diyelim ki 2. sýrada)."
	-> "O zaman git, 2. listedeki 2. sýradaki adresteki kodu çalýþtýr."

Bu sayede 100 tane yeni komut eklesek bile, ana kodumuz (execute_func) tek satýr bile deðiþmez sadece listeye ekleme yaparýz.

*/ 

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

int cd_func(char** args) { // Bu fonksiyonun amacý: Terminalde cd Masaüstü yazdýðýnda seni oraya götürmek.

	// Kontrol: Kullanýcý gideceði yeri yazdý mý? 
	if (args[1] == NULL) {
		fprintf(stderr, "myshell: expected argument to \"cd\"\n"); // printf kullanmadýk çünkü stderr (Standard Error), hata mesajlarý için özel bir kanaldýr. Profesyonel programlarda hatalar normal çýktýlarla karýþmasýn diye buraya yazýlýr.
	}
	
	// Eylem: Yazdýysa oraya gitmeye çalýþ.
	else {
		if (chdir(args[1]) != 0) { // chdir (Change Directory), C'nin iþletim sistemine verdiði bir emirdir. Beni args[1] klasörüne taþý der.
			perror("myshell"); // C de sistem fonksiyonlarý her þey yolundaysa 0 döndürür eðer 0 döndürmediyse hata vardýr (klasör yoktur izin yoktur vs.). 
		}
	}

	return 1; // Shell kapanmasýn devam etsin diye 1 döndürüyoruz.
	
	// NOT: Kullanýcý kaynaklý hatalarda fprintf, sistem kaynaklý hatalarda sebebini detaylandýran perror tercih edilir.

}

int help_func(char** args) { // Bu fonksiyonun amacý: Ekrana menü yazdýrmak.
	
	int i;

	printf("Aleyna Kabuloglu's SHELL\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for (i = 0; i < num_builtins_func(); i++) {
		printf(" %s", builtin_str[i]);
	}

	printf("\nUse the man command for information on other programs.\n");

	return 1;

}

int exit_func(char** args) { // Bu fonksiyonun amacý: Shell'i kapatmak.
	return 0; // 1 Shell i kapatma yeni komut iste demekti, 0 ise Shell i kapat demek.
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

int execute_func(char** args) {

	int i;

	if (args[0] == NULL) {
		return 1; 
	}

	for (i = 0; i < num_builtins_func(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return launch_func(args);

}
