#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmon_parser.h"

#define MAX_LEXEM_LEN 100

// create a token in an empty position of the token list 
// in succes return a pointer to the new allocated token in the token list
// in error return NULL
static struct Token *_cmon_new_token(struct TokenList *tokenList, int type, char *string){
  struct Token *token;

  if (!string){
    return NULL;
  }

  if (string[0] == '\0'){
    return NULL;
  }
  
  if (!tokenList){
    return NULL;
  }

  token = (struct Token *)malloc(sizeof(struct Token));
  if (!token){
    return NULL;
  }

  switch (type){
    case LIST_MARK:
      tokenList->tokenList[tokenList->length].value = (char *)malloc(2);
      if (!tokenList->tokenList[tokenList->length].value){
        return NULL;
      }
      tokenList->tokenList[tokenList->length].value[0] = string[0];
      tokenList->tokenList[tokenList->length].value[1] = '\0';
      break;

    case STRING:
      tokenList->tokenList[tokenList->length].value = strdup(string);
      break;

    case CONFIG_ENTRY:
      tokenList->tokenList[tokenList->length].value = strdup(string);
      break;
  }
  
  tokenList->tokenList[tokenList->length].type = type; 
  tokenList->length++;
  
  return &tokenList->tokenList[tokenList->length-1]; 
}


// this function takes the config file and make a malloc allocated struct tokenList
// the caller is responsible to free the memory
struct TokenList *cmon_tokenizer(FILE *configFile){
  int buffSize = 2048;
  char buff[buffSize];
  char lexem[MAX_LEXEM_LEN];
  char *buffPtr;
  char *lexemPtr;
  
  buffPtr = buff;
  lexemPtr = lexem;

  struct TokenList *tokenList = (struct TokenList *)malloc(sizeof(struct TokenList));
  tokenList->length = 0;

  while(fgets(buff, buffSize, configFile)){
    buffPtr = buff;
    while(*buffPtr != '\0'){
      
      if ((*buffPtr >= 'a' && *buffPtr <= 'z') || (*buffPtr >= 'A' && *buffPtr <= 'Z') ||
          (*buffPtr == '_') || (*buffPtr) == '.' || *buffPtr == '/'){
        *lexemPtr = *buffPtr;
        lexemPtr++;
      } 

      else if (*buffPtr >= '0' && *buffPtr <= '9'){
        
      }

      else if (*buffPtr == '{' || *buffPtr == '}'){
        if (lexemPtr > lexem){
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
        _cmon_new_token(tokenList, LIST_MARK, buffPtr);
      }

      else if (*buffPtr == ' '){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ','){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ':'){
        if (lexemPtr > lexem){ 
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, CONFIG_ENTRY, lexem);
          lexemPtr = lexem;
        }
      }
      buffPtr++;
    }
  } 
  return tokenList;
}
