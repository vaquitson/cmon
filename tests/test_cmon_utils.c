#include <string.h>
#include <stdio.h>

#include "c_utils_buffer.h"

int test1(){
  char *test_buf_1 = "hello hello";
  size_t test_buf_1_len = strlen(test_buf_1);

  CmonBuffer *cmon_buffer_1;

  cmon_buffer_1 = c_u_buffer_new(test_buf_1, test_buf_1_len);
  if (cmon_buffer_1 == NULL){
    printf("test1: error while creating CmonBuffer\n");
    return -1;
  }

  if (memcmp(cmon_buffer_1->buf, test_buf_1, cmon_buffer_1->len) != 0){
    printf("test1: the buffer does not conatin the correct characters\n");
    return -1;
  }

  if (cmon_buffer_1->len != strlen(test_buf_1)){
    printf("test1: the len of the CmonBuffer is incorrect\n");
    return -1;
  }
  

  /* vieww */
  
  CmonBuffer *cmon_buffer_2;
  
  CmonBufferView *cmon_buffer_view_2;
  CmonBufferView *cmon_buffer_view_3;

  char *test_buf_2 = "hello hello|"; 
  size_t test_buf_2_len = strlen(test_buf_2); 

  cmon_buffer_2 = c_u_buffer_new(test_buf_2, test_buf_2_len);

  cmon_buffer_view_2 = c_u_view_buffer_new(cmon_buffer_2->buf, cmon_buffer_2->len, 6, 5);
  if (memcmp("hello", cmon_buffer_view_2->buf, cmon_buffer_view_2->len) != 0){
    printf("test1: the CmonBufferView dosent contain the write data\n");
    return -1;
  }

  cmon_buffer_view_3 = c_u_view_buffer_new(cmon_buffer_2->buf, cmon_buffer_2->len, 6, C_U_BUFFER_VIEW_END);
  if (memcmp("hello|", cmon_buffer_view_3->buf, cmon_buffer_view_3->len) != 0){
    printf("test1: the CmonBufferView dosent contain the write data\n");
    return -1;
  }

  if (cmon_buffer_view_3->len != 6){
    printf("test1: the CmonBufferView have incorrect length\n");
    return -1;
  }

  return 0;
}

int test_2(){
  char *txt = "abcdeabcde|";
  char *payload = "12345";
  char *payload2 = "123456789 123456789 123456789";

  int rc;
  CmonBuffer *buf = c_u_buffer_new(txt, strlen(txt));
  
  rc = c_u_buffer_insert_at(buf, payload, strlen(payload), 5);
  if (memcmp(buf->buf, "abcde12345abcde|", strlen("abcde12345abcde|")) != 0){
    printf("test2: the insert is not corret\n");
    return -1;
  }

  c_u_buffer_free(buf);
  buf = c_u_buffer_new(txt, strlen(txt));

  rc = c_u_buffer_insert_at(buf, payload2, strlen(payload2), 5);
  if (memcmp(buf->buf, 
        "abcde123456789 123456789 123456789abcde|", 
        strlen("abcde123456789 123456789 123456789abcde|")) != 0){

    printf("test2: the insert is not correct for the second buffer\n");
    return -1;
  }

  c_u_buffer_free(buf);
  buf = c_u_buffer_new(txt, strlen(txt));
  rc = c_u_buffer_insert_at(buf, payload, strlen(payload), 0);
  if (memcmp(buf->buf,
        "12345abcdeabcde|",
        strlen("12345abcdeabcde|")) != 0){
    printf("test2: faild to index at 0\n");
    return -1;
  }

  c_u_buffer_free(buf);
  buf = c_u_buffer_new(txt, strlen(txt));
  rc = c_u_buffer_insert_at(buf, payload, strlen(payload), buf->len);
  if (memcmp(buf->buf,
        "abcdeabcde|12345",
        strlen("abcdeabcde|12345")) != 0){
    printf("test2: faild to index at the end\n");
    return -1;
  }


  c_u_buffer_free(buf);
  buf = c_u_buffer_new(txt, strlen(txt));
  rc = c_u_buffer_insert_at(buf, payload2, strlen(payload2), 0);
  if (memcmp(buf->buf,
        "123456789 123456789 123456789abcdeabcde|",
        strlen("123456789 123456789 123456789abcdeabcde|")) != 0){
    printf("test2: faild to index at 0 when overflow\n");
    return -1;
  }


  c_u_buffer_free(buf);
  buf = c_u_buffer_new(txt, strlen(txt));
  rc = c_u_buffer_insert_at(buf, payload2, strlen(payload2), buf->len);
  if (memcmp(buf->buf,
        "abcdeabcde|123456789 123456789 123456789",
        strlen("abcdeabcde|123456789 123456789 123456789")) != 0){
    printf("test2: faild to index at 0 when overflow\n");
    return -1;
  }
  return 0;
}

int test_3(){
  CmonBuffer *buf = c_u_buffer_new("abcdeabcde", strlen("abcdeabcde"));
  c_u_buffer_shift_from(buf, 5, 4);
  if (memcmp(buf->buf, "abcdeabcdabcde", 14) != 0){
    printf("test_3: the shift dosent have the write values\n");
    return -1;
  }

  if (buf->len != 14){
    printf("test_3: the shift dosent have the write length\n");
    return -1;
  }
  c_u_buffer_free(buf);

  buf = c_u_buffer_new("abcdeabcde", strlen("abcdeabcde"));
  c_u_buffer_shift_from(buf, 0, 4);

  if (memcmp(buf->buf, "abcdabcdeabcde", 14) != 0){
    printf("test_3: the shift at 0 dosent have the write values\n");
    return -1;
  }

  if (buf->len != 14){
    printf("test_3: the shift at 0 dosent have the write length\n");
    return -1;
  }

  c_u_buffer_free(buf);

  buf = c_u_buffer_new("1234ab", strlen("1234ab"));
  c_u_buffer_shift_from(buf, 4, 4);

  // be carefule here with the null bytes that could be in empty spaces, 
  // because they dosen't print
  if (memcmp(buf->buf, "1234ab\0\0ab", 10) != 0){
    printf("test_3: the shift at n dosent have the write values\n");
    return -1;
  }

  if (buf->len != 10){
    printf("test_3: the shift at n dosent have the write length\n");
    return -1;
  }

  c_u_buffer_free(buf);

  buf = c_u_buffer_new("1234ab", strlen("1234ab"));
  c_u_buffer_shift_from(buf, 4, 30);

  // be carefule here with the null bytes that could be in empty spaces, 
  // because they dosen't print
  if (memcmp(buf->buf, "1234\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0ab", 37) != 0){
    printf("test_3: the shift at n with new memory dosent have the write values\n");
    return -1;
  }

  if (buf->len != 36){
    printf("test_3: the shift at n with new memory dosent have the write length\n");
    return -1;
  }

  c_u_buffer_free(buf);

  buf = c_u_buffer_new("_234ab", strlen("_234ab"));
  c_u_buffer_shift_from(buf, 0, 30);


  // be carefule here with the null bytes that could be in empty spaces, 
  // because they dosen't print
  if (memcmp(buf->buf, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0_234ab", 37) != 0){
    printf("test_3: the shift at 0 with new memory dosent have the write values\n");
    return -1;
  }

  if (buf->len != 36){
    printf("test_3: the shift at 0 with new memory dosent have the write length\n");
    return -1;
  }

  return 0;
}

int test_4()
{
  return 0;
}


int main(void){
  if (test1() != 0){
    printf("\ntest_1: Faild\n");
    return -1;
  }

  if (test_2() != 0){
    printf("\ntest_2: Faild\n");
    return -2;
  }
  
  if (test_3() != 0){
    printf("\ntest_3: Faild\n");
    return -3;
  }

  if (test_4() != 0){
    printf("\ntest_4: Faild\n");
    return -3;
  }
  
  printf("\nSuccess\n");
  return 0;
}
