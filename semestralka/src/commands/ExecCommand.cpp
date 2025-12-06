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
    throw jkfilesystem_error("Exec command requires at least one argument.");
  }

  // Reconstruct the entire command into a single string.
  std::string command_line;
  for (const auto &arg : args) {
    // If the original input was 'exec ls -l > file.txt', args is {"ls", "-l",
    // ">", "file.txt"}.
    command_line += arg;
    command_line += " ";
  }
  if (!command_line.empty()) {
    command_line.pop_back(); // Remove the trailing space
  }

  // Define the new execution parameters.
  // execute /bin/bash with two arguments: flags and the command line string.
  const char *shell_path = "/bin/bash";
  // -c is neccessarry for reading command from string
  // -x is to see what is executed
  const char *shell_flags = "-cx";

  std::vector<char *> new_args = {const_cast<char *>(shell_path),
                                  const_cast<char *>(shell_flags),
                                  // Passed to bash as one argument
                                  const_cast<char *>(command_line.c_str()),
                                  // Null terminator for execvp
                                  nullptr};

  // Fork process
  pid_t pid = fork();
  if (pid < 0) {
    throw jkfilesystem_error("Fork failed.");
  }

  // Child process: Execute the shell
  if (pid == 0) {
    execvp(new_args[0], new_args.data());

    // shouldn't be here, but if execvp fails...
    perror("execvp (bash)");
    _exit(127);
  }

  // Parent process: Wait for the shell to finish
  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    throw jkfilesystem_error("waitpid failed.");
  }

  // Check exit status
  if (!WIFEXITED(status)) {
    throw jkfilesystem_error("Command failed (process did not exit normally).");
  } else if (WEXITSTATUS(status) != 0) {
    throw jkfilesystem_error("Command returned non-zero exit code: " +
                             std::to_string(WEXITSTATUS(status)));
  }
}

} // namespace jkfs
