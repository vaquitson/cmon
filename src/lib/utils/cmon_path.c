#include <string.h>
#include <cmon_string.h>
#include <unistd.h>

#include "cmon_path.h"
#include "logger.h"

/*
 * Creates a new heap-allocated CmonPath from `str_path`.
 *
 * The input string is duplicated into a newly heap-allocated buffer owned by
 * the returned CmonPath, so the caller does not need to keep `str_path` alive.
 *
 * The `ext` field is set to point inside that duplicated buffer at the last '.'
 * character in the path, so the extension includes the leading dot
 * (e.g. ".c", ".js", ".rs"). No additional allocation is performed for `ext`.
 *
 * If `str_path` has no extension, `ext` is set to NULL and `ext_len` is set to 0.
 */
CmonPath *cmon_path_new(const char *str_path){
  static size_t buff_size = 20; 

  CmonPath *path = malloc(sizeof(CmonPath)); 
  if (path == NULL){
    log_write(LOG_ERROR, "from cmon_path_new: could not allocate memory for the CmonPath");
    return NULL;
  }

  char *path_buffer = malloc(sizeof(char)*buff_size);
  if (path_buffer == NULL){
    log_write(LOG_ERROR, "from cmon_path_new: could not allocate mamory for the CmonPath->path");
    return NULL;
  }

  size_t i = 0;
  size_t last_point_index = 0;

  for (;;){  
    if (i >= buff_size-1){
      buff_size = buff_size * 2;
      path_buffer = realloc(path_buffer, buff_size);
    }
    
    if (i > MAX_PATH_LEN-1) {
      free(path);
      free(path_buffer);
      log_write(LOG_ERROR, "cmon_path_new; The path %s is to large", str_path); 
      return NULL;
    }

    if (str_path[i] == '\0'){
      path_buffer[i] = '\0';
      break;

    } else if (str_path[i] == '.'){
      last_point_index = i;
    }

    path_buffer[i] = str_path[i];
    i++;
  }
  
  path->path = path_buffer;
  path->path_len = i;

  if (last_point_index > 0) {
    path->ext = path_buffer + sizeof(char)*last_point_index;
    path->ext_len = strlen(path->ext);
  } else {
    path->ext = NULL;
    path->ext_len = 0;
  }
  
  return path; 
}



/*
 * Creates a new heap-allocated CmonPath representing an absolute path built
 * from `src` and the current working directory (CWD).
 * 
 * The CWD string and its length are cached on the first call (using static
 * storage) and reused on subsequent calls to avoid repeated `getcwd()` work.
 *
 * The returned CmonPath owns its `path` buffer. The `ext`/`ext_len` fields are
 * derived exactly like `cmon_path_new()`: `ext` points inside the internal `path`
 * buffer at the last '.' (including the dot), or is NULL with `ext_len == 0`
 * if no extension is present.
 */

CmonPath *cmon_path_new_full_path(const char *src){
  static char *cwd = NULL;
  static size_t cwd_len = 0;

  if (cwd == NULL){
    char buff[MAX_PATH_LEN];
    if (getcwd(buff, MAX_PATH_LEN) == NULL){
      log_write(LOG_ERROR, "from cmon_path_new_full_path: could not get the cwd");
      exit(1);
    }

    cwd_len = strnlen(buff, MAX_PATH_LEN);
    if (cwd_len == MAX_PATH_LEN){
      log_write(LOG_WARNING, "from cmon_path_new_full_path: the cwd is probably to large");
    }
    
    cwd = malloc(sizeof(char)*cwd_len);
    if (cwd == NULL){
      log_write(LOG_ERROR, "from cmon_path_new_full_path: could not alloc mamory for the cwd");
      exit(1);
    }
    strncpy(cwd, buff, cwd_len);
  }

  size_t src_len = strnlen(src, MAX_PATH_LEN);
  if (cwd_len == MAX_PATH_LEN){
    log_write(LOG_WARNING, "from cmon_path_new_full_path: the cwd is probably to large");
  }

  CmonPath *path = malloc(sizeof(CmonPath));
  if (path == NULL){
    free(path);
    free(cwd);
    log_write(LOG_ERROR, "from cmon_path_new_full_path: could not allocate memory for the CmonPath");
    exit(1);
  }
  
  path->path_len = src_len + cwd_len + 1;
  // plus the \0 char at the end of the path
  char *full_path_buff = malloc(sizeof(char) * (path->path_len + 1));
  if (full_path_buff == NULL){
    free(path);
    log_write(LOG_ERROR, "from cmon_path_new_full_path: could not allocate memory for the full path buf");
    exit(1);
  }

  snprintf(full_path_buff, path->path_len+1, "%s/%s", cwd, src);
  path->path = full_path_buff;

  for (size_t i = 1; i < src_len; i++){
    if (path->path[path->path_len - i] == '.'){
      path->ext = full_path_buff + path->path_len - i;
      path->ext_len = i;
      return path;
    }
  }

  path->ext = NULL;
  path->ext_len = 0;

  return path;
}
 
/*
 * Returns the file extension of `path` as a CmonString view, including the dot.
 *
 * No memory is allocated and no data is copied. The returned CmonString is a
 * non-owning view into the internal `CmonPath::path` buffer: it starts at
 * `CmonPath::ext` and has length `CmonPath::ext_len`.
 *
 * The extension includes the leading '.' (e.g. ".c", ".js", ".rs").
 *
 * If the path has no extension (i.e., `path->ext == NULL` / `path->ext_len == 0`),
 * this function returns NULL.
 *
 * Lifetime: the returned view is valid only as long as the owning CmonPath
 * remains alive and its `path` buffer is not modified or freed.
 */
#define cmon_path_get_ext_as_cmon_string(path) \
  (CmonString *)((char *)path + sizeof(CmonString))


/* Free the all the memory asiciated with the CmonPath.
 * If path is NULL nothing happens and a warning is write to the logs
*/
void cmon_path_free(CmonPath *path){
  if (path == NULL){
    log_write(LOG_INFO, "from cmon_path_free: trying to free a null path");
    return;
  }
  free(path->path);
  free(path);
}
