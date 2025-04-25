#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
} opt;

opt options = {0};

static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                       {"number", 0, 0, 'n'},
                                       {"squeeze-blank", 0, 0, 's'},
                                       {0, 0, 0, 0}};

void cat_stdin() {
  char name[1000];
  while (1) {
    fgets(name, 1000, stdin);
    printf("%s", name);
  }
}

void parser(int argc, char* argv[], opt* options) {
  int rez = 0;
  while ((rez = getopt_long(argc, argv, "+benstvTE", long_options, 0)) != -1) {
    switch (rez) {
      case 'b':
        options->b = 1;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'T':
        options->T = 1;
        break;
      case 'E':
        options->E = 1;
        break;
      case '?':
        fprintf(stderr, "usage: cat [-belnstuv] [file ...]");
        exit(1);
        break;
    }
  }
}

int s_option(int current_symbol, char prev_symbol, int* tab_flag) {
  int s_flag = 0;
  if (prev_symbol == 0 && current_symbol == '\n')
    *tab_flag = 1;
  else if (prev_symbol == '\n' && current_symbol == '\n' && *tab_flag == 0)
    *tab_flag = 1;
  else if (current_symbol != '\n')
    *tab_flag = 0;
  else if (prev_symbol == '\n' && current_symbol == '\n' && *tab_flag)
    s_flag = 1;
  return s_flag;
}

int b_option(int str_count, int current_symbol, char prev_symbol) {
  if (str_count == 1 && current_symbol != '\n') {
    printf("%6d\t", str_count);
    str_count++;
  } else if (current_symbol != '\n' && prev_symbol == '\n') {
    {
      printf("%6d\t", str_count);
      str_count++;
    }
  }
  return str_count;
}

int n_option(int str_count, char prev_symbol) {
  if (prev_symbol == '\n') {
    printf("%6d\t", str_count);
    str_count++;
  } else {
    if (str_count == 1) {
      printf("%6d\t", str_count);
      str_count++;
    }
  }
  return str_count;
}

int v_option(int current_symbol) {
  int v_flag = 0;
  if ((current_symbol >= 0 && current_symbol < 32 && current_symbol != 9 &&
       current_symbol != 10) ||
      current_symbol == 127) {
    if (current_symbol == 127) {
      printf("^?");
      v_flag = 1;
    } else {
      printf("^%c", current_symbol + 64);
      v_flag = 1;
    }
  }
  return v_flag;
}

void output(int argc, char* argv[], opt* options) {
  char symbol_array[2] = {0, 0};
  int current_symbol;
  int tab_flag = 0;
  int str_count = 1;
  FILE* fp = fopen(argv[argc], "r");
  if (fp) {
    while ((current_symbol = fgetc(fp)) != EOF) {
      symbol_array[0] = symbol_array[1];
      symbol_array[1] = current_symbol;
      if (options->s) {
        int s_flag = 0;
        if ((s_flag = s_option(current_symbol, symbol_array[0], &tab_flag)) ==
            1)
          continue;
      }
      if (options->b) {
        str_count = b_option(str_count, current_symbol, symbol_array[0]);
      }
      if (options->n) {
        str_count = n_option(str_count, symbol_array[0]);
      }
      if (options->v) {
        int v_flag = 0;
        if ((v_flag = v_option(current_symbol)) == 1) continue;
      }
      if (options->e) {
        if (current_symbol == '\n') printf("$");
      }
      if (options->E) {
        if (current_symbol == '\n') printf("$");
      }
      if (options->t) {
        if (current_symbol == '\t') {
          printf("^I");
          continue;
        }
      }
      if (options->T) {
        if (current_symbol == '\t') {
          printf("^I");
          continue;
        }
      }
      printf("%c", current_symbol);
    }
    fclose(fp);
  } else
    fprintf(stderr, "No such file or directory\n");
}

int main(int argc, char* argv[]) {
  if (argc == 1)
    cat_stdin();
  else {
    parser(argc, argv, &options);
    if (options.b) options.n = 0;
    if (options.E) options.e = 0;
    if (options.T) options.t = 0;
    int i = optind;
    while (i < argc) {
      output(i, argv, &options);
      i++;
    }
  }
}