#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "cmon_errors.h"
#include "text_utils.h"

#define MAX_LEXEM_LEN 100

/* return a grater than 0 number if the char is a valid string character */ 
static int _isValidSpecialChar(char c){
  if (c == '.' || c == '_' || c == '/'){
    return 1;
  }
  return 0;
}

struct TokenArr *token_arr_new(){
  struct TokenArr *tokArr = (struct TokenArr *)malloc(sizeof(struct TokenArr)); 
  if (!tokArr){
    cmon_print_error(true, "token_arr_new", "coudl not allocate memory for the new struct TokenArray");
    return NULL;
  }
  tokArr->length = 0;
  tokArr->nextPosition = &tokArr->tokenList[0];
  return tokArr;
}


void token_arr_free(struct TokenArr *tokArr){
  for (int i = 0; i < tokArr->length; i++){
    free(tokArr->tokenList[i].value);
  }
  free(tokArr);
}


// create a token in an empty position of the token list 
// in succes return a pointer to the new allocated token in the token list
// in error return NULL
static struct Token *_cmon_new_token(struct TokenArr *tokenList, int type, char *string){
  struct Token *newToken;

  if (tokenList->length >= 100){
    cmon_print_error(true, "_cmon_new_token", "the token list is full");
    return NULL;
  }
 
  if (!tokenList){
    return NULL;
  }

  if (string){
    if (string[0] != '\0'){
      tokenList->nextPosition->value = strdup(string);
    }
  }
  else {
    tokenList->nextPosition->value = NULL;
  }
  tokenList->nextPosition->type = type; 

  newToken = tokenList->nextPosition;

  tokenList->length++;
  tokenList->nextPosition++;
  
  return newToken; 
}


// this function takes the config file and make a malloc allocated struct tokenList
// the caller is responsible to free the memory
struct TokenArr *cmon_tokenizer(FILE *configFile){
  int buffSize = 2048;
  char buff[buffSize];
  char lexem[MAX_LEXEM_LEN];
  char *buffPtr;
  char *lexemPtr;
  
  buffPtr = buff;
  lexemPtr = lexem;

  struct TokenArr *tokenList = (struct TokenArr *)malloc(sizeof(struct TokenArr));
  tokenList->length = 0;

  while(fgets(buff, buffSize, configFile)){
    buffPtr = buff;
    while(*buffPtr != '\0'){
      
      if (char_is_alphanumeric(*buffPtr) || (*buffPtr == '_') || (*buffPtr) == '.' || *buffPtr == '/'){
        *lexemPtr = *buffPtr;
        lexemPtr++;
      }

      else if (*buffPtr >= '0' && *buffPtr <= '9'){
        *lexemPtr = *buffPtr;
        lexemPtr++;
         
      }

      else if (*buffPtr == '{' || *buffPtr == '}'){
        if (lexemPtr > lexem){
          _cmon_new_token(tokenList, TEXT, lexem);
          lexemPtr = lexem;
        }
        _cmon_new_token(tokenList, CURLY_BARACKETS, buffPtr);
      }

      else if (*buffPtr == ' '){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, TEXT, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ','){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, TEXT, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ':'){
        if (lexemPtr > lexem){ 
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, DOUBLE_COLON, lexem);
          lexemPtr = lexem;
        }
      }
      buffPtr++;
    }
  } 
  return tokenList;
}


int token_arr_print(struct TokenArr *tok_arr){
  char *str_type;

  if (!tok_arr){
    return 1; 
  }

  for (int i = 0; i < tok_arr->length; i++){
    switch (tok_arr->tokenList[i].type) {
      case TEXT:
        str_type = "TEXT";
        break;
      case STRING:
        str_type = "STRING";
        break;
      case DOUBLE_COLON:
        str_type = "DOUBLE_COLON";
        break;
      case O_CURLY_BARACKET:
        str_type = "O_CURLY_BARACKETS";
        break;
      case C_CURLY_BARACKET:
        str_type = "C_CURLY_BARACKETS";
        break;
      case PARENTHESIS:
        str_type = "PARENTHESIS";
        break;
      case COLON:
        str_type = "COLON";
        break;
      case NUMBER:
        str_type = "NUMBER";
        break;
      default:
        str_type = "UNKNOWN";
        break;
    }      
    if (tok_arr->tokenList[i].value){
      printf("(%s: %s)\n", str_type, tok_arr->tokenList[i].value);
    } 
    else {
      printf("(%s: %s)\n", str_type, "NULL");
    } 
  }
  printf("\n");
  return 0;
}


// this function takes the config file and make a malloc allocated struct tokenList
// the caller is responsible to free the memory
struct TokenArr *config_tokenizer(struct TokenArr *tokArr, FILE *configFile){
  int buffSize = 2048;
  char buff[buffSize];
  static char *buffPtr;

  static char lexem[MAX_LEXEM_LEN];
  static char *lexemPtr;
  
  int token_type;

  bool inQuatationMark;

  inQuatationMark = false;
  lexemPtr = lexem;

  while(fgets(buff, buffSize, configFile)){
    buffPtr = buff;
    while(*buffPtr != '\0'){

      if (char_is_alphanumeric(*buffPtr) || _isValidSpecialChar(*buffPtr)){
        *lexemPtr = *buffPtr;
        lexemPtr++;
      }

      else if (*buffPtr == '{'){
        _cmon_new_token(tokArr, O_CURLY_BARACKET, NULL);
      }

      else if (*buffPtr == '}'){
        _cmon_new_token(tokArr, C_CURLY_BARACKET, NULL);
      }

      else if (*buffPtr == '\"' || *buffPtr == '\''){
        if (inQuatationMark){
          token_type = STRING;
          *lexemPtr = '\0';
          _cmon_new_token(tokArr, token_type, lexem);
          lexemPtr = lexem;
        }
        inQuatationMark = !inQuatationMark;
      }

      else if (*buffPtr == ' '){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          token_type = _str_is_number(lexem) ? NUMBER : TEXT;
          _cmon_new_token(tokArr, token_type, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ':'){
        if (lexemPtr > lexem){ 
          *lexemPtr = '\0';
          token_type = _str_is_number(lexem) ? NUMBER : TEXT;
          _cmon_new_token(tokArr, token_type, lexem);
          lexemPtr = lexem;
        }
        _cmon_new_token(tokArr, DOUBLE_COLON, NULL);
      }

      buffPtr++;
    }
  }
  return tokArr;
}
