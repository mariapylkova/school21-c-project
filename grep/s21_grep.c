#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned char i_option = 1;
const unsigned char v_option = 2;
const unsigned char c_option = 4;
const unsigned char l_option = 8;
const unsigned char n_option = 16;
const unsigned char h_option = 32;
const unsigned char s_option = 64;
const unsigned char o_option = 128;

struct f_e_array {
  regex_t* regular_array;
  int length;
  int max_lenght;
};

struct f_e_array f_e_array_create() {
  struct f_e_array result;
  result.length = 0;
  result.max_lenght = 10;
  result.regular_array = malloc(10 * sizeof(regex_t));
  return result;
}

void f_e_array_add(struct f_e_array* result, char* argument,
                   int flag_template) {
  if (result->length + 1 == result->max_lenght)
    result->regular_array = realloc(
        result->regular_array,
        sizeof(regex_t) * (result->max_lenght = result->max_lenght * 3 / 2));
  regex_t* current_regex = &result->regular_array[result->length];
  int ret = regcomp(current_regex, argument, flag_template);
  if (ret != 0) {
    char err_buf[256];
    regerror(ret, current_regex, err_buf, sizeof(err_buf));
    fprintf(stderr, "Error compiling regex: %s\n", err_buf);
    exit(1);
  }
  result->length++;
}

void f_e_array_clean(struct f_e_array* result) {
  for (int i = 0; i < result->length; i++) regfree(&result->regular_array[i]);
  free(result->regular_array);
}

int regular_check(regex_t regex_c, char* str, char v_flag) {
  if (v_flag)
    return regexec(&regex_c, str, 0, NULL, 0);
  else
    return !regexec(&regex_c, str, 0, NULL, 0);
}

char i_option_parse(int argc, char* argv[]) {
  char flag_result = 0;
  char next_e_file = 0;
  char next_f_file = 0;
  for (int i_argument = 1; i_argument < argc; i_argument++) {
    if (next_e_file) {
      next_e_file = 0;
      continue;
    }
    if (next_f_file) {
      next_f_file = 0;
      continue;
    }
    if (argv[i_argument][0] == '-') {
      for (size_t i = 1; i < strlen(argv[i_argument]); i++) {
        if (argv[i_argument][i] == 'i') flag_result |= i_option;
        if (argv[i_argument][i] == 'e') next_e_file = 1;
        if (argv[i_argument][i] == 'f') next_f_file = 1;
      }
    } else
      break;
  }
  return flag_result;
}

char options_parse(int argc, char* argv[], int* i_argument,
                   struct f_e_array* arguments) {
  char flag_result = i_option_parse(argc, argv);
  char next_e_file = 0;
  char next_f_file = 0;
  for (int i = *i_argument; i < argc; i++) {
    *i_argument = i;
    if (next_e_file) {
      f_e_array_add(arguments, argv[*i_argument],
                    (flag_result & i_option) ? REG_ICASE : 0);
      next_e_file = 0;
      continue;
    }
    if (next_f_file) {
      FILE* fp = fopen(argv[*i_argument], "r");
      if (!fp) {
        if (!s_option)
          fprintf(stderr, "grep: %s: No such file or directory\n",
                  argv[*i_argument]);
        next_f_file = 0;
        continue;
      }
      char f_array_size[4096];
      while (fgets(f_array_size, 4096, fp)) {
        if (f_array_size[strlen(f_array_size) - 1] == '\n')
          f_array_size[strlen(f_array_size) - 1] = '\0';
        f_e_array_add(arguments, f_array_size,
                      (flag_result & i_option) ? REG_ICASE : 0);
      }
      fclose(fp);
      next_f_file = 0;
      continue;
    }
    if (argv[*i_argument][0] == '-') {
      for (size_t i = 1; i < strlen(argv[*i_argument]); i++) {
        if (argv[*i_argument][i] == 'v') flag_result |= v_option;
        if (argv[*i_argument][i] == 'c') flag_result |= c_option;
        if (argv[*i_argument][i] == 'l') flag_result |= l_option;
        if (argv[*i_argument][i] == 'n') flag_result |= n_option;
        if (argv[*i_argument][i] == 'h') flag_result |= h_option;
        if (argv[*i_argument][i] == 's') flag_result |= s_option;
        if (argv[*i_argument][i] == 'o') flag_result |= o_option;
        if (argv[*i_argument][i] == 'e') next_e_file = 1;
        if (argv[*i_argument][i] == 'f') next_f_file = 1;
      }
    } else
      break;
  }
  return flag_result;
}

int fail_scan(char* filename, int file_count, char flags,
              struct f_e_array arguments) {
  FILE* fp = fopen(filename, "r");
  int match_lines = 0;
  int line_number = 1;
  if (!fp) return !(flags & s_option);
  char line_from_file[4096];
  char exit_file_flag = 0;
  while (fgets(line_from_file, 4096, fp)) {
    if (line_from_file[strlen(line_from_file) - 1] == '\n')
      line_from_file[strlen(line_from_file) - 1] = '\0';
    for (int arguments_count = 0; arguments_count < arguments.length;
         arguments_count++) {
      if (regular_check(arguments.regular_array[arguments_count],
                        line_from_file, flags & v_option)) {
        if (flags & l_option) {
          if (flags & c_option) match_lines++;
          exit_file_flag = 1;
          break;
        }
        if (flags & c_option) {
          match_lines++;
          break;
        }
        if ((file_count > 1) && !(flags & h_option)) printf("%s:", filename);
        if (flags & n_option) printf("%d:", line_number);
        if (flags & o_option) {
          regmatch_t match;
          regexec(&(arguments.regular_array[arguments_count]), line_from_file,
                  1, &match, 0);
          printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                 &line_from_file[match.rm_so]);
        } else
          puts(line_from_file);
        break;
      }
    }
    if (flags & l_option)
      if (exit_file_flag) break;
    line_number++;
  }
  if (flags & c_option) {
    if ((file_count > 1) && !(flags & h_option)) printf("%s:", filename);
    printf("%d\n", match_lines);
  }
  if (exit_file_flag) printf("%s\n", filename);
  fclose(fp);
  return 0;
}

int main(int argc, char* argv[]) {
  struct f_e_array arguments = f_e_array_create();
  int arg_num = 1;
  char flag_result = options_parse(argc, argv, &arg_num, &arguments);
  if (arguments.length == 0) {
    char* argument = argv[arg_num];
    f_e_array_add(&arguments, argument,
                  (flag_result & i_option) ? REG_ICASE : 0);
    arg_num++;
  }
  int file_count = argc - arg_num;
  for (; arg_num < argc; arg_num++) {
    if (fail_scan(argv[arg_num], file_count, flag_result, arguments))
      if (!s_option)
        fprintf(stderr, "grep: %s: No such file or directory\n", argv[arg_num]);
  }
  f_e_array_clean(&arguments);
  return 0;
}