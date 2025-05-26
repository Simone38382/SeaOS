// shell.c
#include "shell.h"
#include <string.h>
#include <stddef.h>

#define VIDEO_MEMORY (char*)0xb8000
#define MAX_INPUT 80

static int cursor = 0;

void print(const char *str) {
    char *vidptr = VIDEO_MEMORY;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            // Vai a inizio riga successiva
            cursor += (160 - (cursor % 160));
        } else {
            vidptr[cursor] = str[i];
            vidptr[cursor + 1] = 0x07;
            cursor += 2;
        }
        i++;
    }
}

void clear_screen() {
    char *vidptr = VIDEO_MEMORY;
    for (int j = 0; j < 80 * 25 * 2; j += 2) {
        vidptr[j] = ' ';
        vidptr[j + 1] = 0x07;
    }
    cursor = 0;
}

void print_prompt() {
    print("user@seaos:~$ ");
}

// --- Simulazione file system Unix-like ---
typedef struct File {
    const char* name;
    const char* content;
} File;

typedef struct Directory {
    const char* name;
    File* files;
    int file_count;
} Directory;

// File di esempio
File root_files[] = {
    {"README.txt", "Benvenuto su SeaOS!\nQuesto è un file di esempio."},
    {"hello.txt", "Hello from SeaOS!"}
};
File etc_files[] = {
    {"os-release", "NAME=SeaOS\nVERSION=0.1"}
};

Directory fs[] = {
    {"/", root_files, 2},
    {"/etc", etc_files, 1}
};
const int dir_count = 2;

// Funzione per ls simulato
void ls_sim(const char* path) {
    for (int i = 0; i < dir_count; i++) {
        if (!strcmp(path, fs[i].name)) {
            for (int j = 0; j < fs[i].file_count; j++) {
                print(fs[i].files[j].name);
                print("  ");
            }
            print("\n");
            return;
        }
    }
    print("ls: directory non trovata\n");
}

// Funzione per cat simulato
void cat_sim(const char* path, const char* filename) {
    for (int i = 0; i < dir_count; i++) {
        if (!strcmp(path, fs[i].name)) {
            for (int j = 0; j < fs[i].file_count; j++) {
                if (!strcmp(filename, fs[i].files[j].name)) {
                    print(fs[i].files[j].content);
                    print("\n");
                    return;
                }
            }
            print("cat: file non trovato\n");
            return;
        }
    }
    print("cat: directory non trovata\n");
}

void shell() {
    char input[MAX_INPUT];
    int pos;
    clear_screen();
    print("SeaOS (Unix-like) [Version 0.1]\n");
    print("(C) 2025 SeaOS Team. All rights reserved.\n");
    print("Type 'help' to list commands.\n\n");
    while (1) {
        print_prompt();
        pos = 0;
        // Lettura input utente (solo lettere, numeri, backspace, invio)
        while (1) {
            char c;
            asm volatile ("inb $0x60, %0" : "=a"(c));
            if (c == 0x1C) { // Invio
                input[pos] = '\0';
                print("\n");
                break;
            } else if (c == 0x0E && pos > 0) { // Backspace
                pos--;
                cursor -= 2;
                print(" ");
                cursor -= 2;
            } else if (c >= 0x02 && c <= 0x0A && pos < MAX_INPUT - 1) { // Numeri 1-9
                char num = '0' + (c - 0x01);
                input[pos++] = num;
                char s[2] = {num, 0};
                print(s);
            } else if (c >= 0x10 && c <= 0x19 && pos < MAX_INPUT - 1) { // QWERTYUIOP
                char letter = 'q' + (c - 0x10);
                input[pos++] = letter;
                char s[2] = {letter, 0};
                print(s);
            } else if (c >= 0x1E && c <= 0x26 && pos < MAX_INPUT - 1) { // ASDFGHJKL
                char letter = 'a' + (c - 0x1E);
                input[pos++] = letter;
                char s[2] = {letter, 0};
                print(s);
            } else if (c >= 0x2C && c <= 0x32 && pos < MAX_INPUT - 1) { // ZXCVBNM
                char letter = 'z' + (c - 0x2C);
                input[pos++] = letter;
                char s[2] = {letter, 0};
                print(s);
            }
        }
        // Comandi
        if (input[0] == '\0') continue;
        if (!strcmp(input, "help")) {
            print("Comandi disponibili: help, clear, echo, ls, cat, uname\n");
        } else if (!strcmp(input, "clear")) {
            clear_screen();
        } else if (!strcmp(input, "ls")) {
            ls_sim("/");
        } else if (!strncmp(input, "ls /etc", 7)) {
            ls_sim("/etc");
        } else if (!strcmp(input, "uname")) {
            print("SeaOS 0.1 (unix-like)\n");
        } else if (!strcmp(input, "cat")) {
            print("cat: manca il file da leggere\n");
        } else if (!strcmp(input, "echo")) {
            print("\n");
        } else if (!strncmp(input, "echo ", 5)) {
            print(input + 5);
            print("\n");
        } else if (!strncmp(input, "cat /etc/", 9)) {
            cat_sim("/etc", input + 9);
        } else if (!strncmp(input, "cat ", 4)) {
            cat_sim("/", input + 4);
        } else {
            print("Comando non riconosciuto\n");
        }
    }
}

// Semplice strcmp per ambienti freestanding
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Semplice strncmp per ambienti freestanding
int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Nota: questa shell è molto basilare e supporta solo lettere minuscole e numeri, senza maiuscole o simboli.
