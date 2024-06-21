#include "client/cmd_processor.h"

#include <expected>
#include <iostream>
#include <sstream>

#include "client/cmd.h"

namespace tftp {
namespace client {

void CmdProcessor::PrintError(std::string_view err_msg) const {
  std::cout << "error: " << err_msg << std::endl;
}

void CmdProcessor::Exec(std::string_view cmdline) {
  /* Parse the command ID from the command line. */
  std::istringstream is(cmdline.data());
  Id cmd_id;
  is >> cmd_id;

  /* Create the command object. */
  std::expected<CmdPtr, ParseStatus> cmd;
  if (cmd_id == CmdId::kGet) {
    cmd = LoadCmd<GetCmd>(cmdline);
  } else if (cmd_id == CmdId::kPut) {
    cmd = LoadCmd<PutCmd>(cmdline);
  } else if (cmd_id == CmdId::kHelp) {
    cmd = LoadCmd<HelpCmd>(cmdline);
  } else if (cmd_id == CmdId::kMode) {
    cmd = LoadCmd<ModeCmd>(cmdline);
  } else if (cmd_id == CmdId::kQuit) {
    cmd = LoadCmd<QuitCmd>();
  } else if (cmd_id == CmdId::kStatus) {
    cmd = LoadCmd<StatusCmd>();
  } else if (cmd_id == CmdId::kConnect) {
    cmd = LoadCmd<ConnectCmd>(cmdline);
  } else if (cmd_id == CmdId::kLiteral) {
    cmd = LoadCmd<LiteralCmd>();
  } else if (cmd_id == CmdId::kTimeout) {
    cmd = LoadCmd<TimeoutCmd>(cmdline);
  } else if (cmd_id == CmdId::kRexmt) {
    cmd = LoadCmd<RexmtCmd>(cmdline);
  } else {
    PrintError("unknown command '" + cmd_id + "'");
    return;
  }

  if (!cmd) { /* Unable to parse the commandline successfully. */
    PrintError(kParseStatusToStr[cmd.error()]);
    return;
  }

  /* Execute the command. */
  ExecStatus exec_stat = (*cmd)->Execute(conf_);
  if (exec_stat != ExecStatus::kSuccessfulExec) {
    PrintError(kExecStatusToStr[exec_stat]);
  }
}

}  // namespace client
}  // namespace tftp