#include "lou/core/exec.h"
#include "lou/core/log.h"
#include "lou/core/vec.h"
#include "lou/core/assertions.h"
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

bool lou_exec(const char *binary, char *const *args, size_t length, int *status) {
  char **built_args = LOU_VEC_NEW(char*);
  *LOU_VEC_PUSH(&built_args) = (char*)binary;
  LOU_VEC_APPEND_RAW(&built_args, args, length);
  *LOU_VEC_PUSH(&built_args) = NULL;

  pid_t pid = fork();
  assert(pid >= 0);
  if (pid == 0) {
    if (execv(binary, built_args) < 0) {
      lou_log_fmt(LOU_LOG_ERROR, "failed to run child process: #E");
      return false;
    }
    UNREACHABLE();
  }

  waitpid(pid, status, 0);
  *status = WEXITSTATUS(*status);
  lou_vec_free(built_args);
  return true;
}
