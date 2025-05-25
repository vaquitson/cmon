#include <text_utils.h>

int char_is_alphanumeric(char c){
  if (c >= 'A' && c <= 'Z'){
    return 1;
  }

  if (c >= 'a' && c <= 'z'){
    return 2;
  }

  if (c >= '0' && c <= '9'){
    return 3;
  }

  return 0;
}



int char_is_alpha(char c){
  if (c >= 'A' && c <= 'Z'){
    return 1;
  }

  if (c >= 'a' && c <= 'z'){
    return 2;
  }

  return 0;
}

int char_is_numeric(char c){
  if (c >= '0' && c <= '9'){
    return 1;
  }

  return 0;
}


int _str_is_number(char *str){
  while (*str != '\0'){
    if (char_is_alpha(*str)){
      return 0;
    }
    str++;
  }
  return 1; 
}


