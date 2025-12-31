#include <stdlib.h>

typedef struct {
  size_t path_len;
  char *path;
  size_t ext_len;
  char *ext;
} CmonPath;

#define MAX_PATH_LEN 4096

CmonPath *cmon_path_new(const char *str_path);

CmonPath *cmon_path_new_full_path(const char *path);

void cmon_path_free(CmonPath *path);

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
#define cmon_path_cast_ext_as_cmon_string(path) \
  (CmonString *)((char *)path + sizeof(CmonString))


