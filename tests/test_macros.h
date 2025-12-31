#pragma once

#define TEST(func_val, test_name) \
  if ((func_val) != 0){ \
    printf("%s: Faild\n", test_name); \
  } else { \
    printf("%s: Success\n", test_name); \
  }
