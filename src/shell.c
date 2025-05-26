// shell.c
#include "shell.h"
#include <string.h>

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
    print("SeaOS> ");
}

void shell() {
    char input[MAX_INPUT];
    int pos;
    clear_screen();
    print("SeaOS Shell. Digita 'help' per i comandi.\n");
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
            print("Comandi disponibili: help, clear\n");
        } else if (!strcmp(input, "clear")) {
            clear_screen();
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

// Nota: questa shell è molto basilare e supporta solo lettere minuscole e numeri, senza maiuscole o simboli.
