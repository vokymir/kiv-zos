#include <cstdlib>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.hpp"
#include "errors.hpp"

namespace jkfs {

ExecCommand::ExecCommand() {
  id_ = "exec";
  name_ = "Execute outwordly command";
  desc_ = "Execute any command current shell is capable of.";
  how_ = "exec top";
}

void ExecCommand::execute_inner(const std::vector<std::string> &args) {
  if (args.empty()) {
    throw jkfilesystem_error("Exec command requires one argument.");
  }

  // collect arguments
  std::vector<char *> args_c;
  args_c.reserve(args.size() + 1);

  for (const auto &arg : args) {
    args_c.push_back(const_cast<char *>(arg.c_str()));
  }
  args_c.push_back(nullptr); // end \0

  // fork process
  pid_t pid = fork();
  if (pid < 0) {
    throw jkfilesystem_error("Fork failed.");
  }

  // child
  if (pid == 0) {
    execvp(args_c[0], args_c.data());

    // we shouldn't be here, but...
    perror("execvp");
    _exit(127);
  }

  // parent
  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    throw jkfilesystem_error("waitpid failed.");
  }

  if (!WIFEXITED(status)) {
    throw jkfilesystem_error("Command failed.");
  } else if (WEXITSTATUS(status) != 0) {
    throw jkfilesystem_error("Command returned non‑zero exit code: " +
                             std::to_string(WEXITSTATUS(status)));
  }
}

} // namespace jkfs
