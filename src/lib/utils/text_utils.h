#pragma once


int _str_is_number(char *str);

/*
 * return 1 if char is a number, else return
 * 0
*/
int char_is_numeric(char c);

/*
 * return 1 if char is upper case and 2 if 
 * is a lower Case
*/
int char_is_alpha(char c);

/*
 * return a number grater than 0 
 * if the character is alpha numeric, else return 0
 * return 1 for Upper Case 
 * return 2 for Lower Case
 * return 3 for Numerc
*/
int char_is_alphanumeric(char c);
