#ifndef CMD_H_
#define CMD_H_

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "client/config.h"
#include "common/parse.h"
#include "common/types.h"

namespace tftp {
namespace client {

class Cmd;

using Id = std::string;
using File = std::string;
using FileList = std::vector<File>;
using CmdPtr = std::unique_ptr<Cmd>;
template <typename T>
using ExpectedCmd = std::expected<std::unique_ptr<T>, ParseStatus>;

namespace CmdId {
constexpr Id kConnect = "connect";
constexpr Id kGet = "get";
constexpr Id kPut = "put";
constexpr Id kLiteral = "literal";
constexpr Id kMode = "mode";
constexpr Id kStatus = "status";
constexpr Id kTimeout = "timeout";
constexpr Id kRexmt = "rexmt";
constexpr Id kHelp = "help";
}  // namespace CmdId

enum ExecStatus : int {
  kSuccessfulExec = 0,
  kNotImplemented,
  kUnknownCmdHelp,
  kExecStatusCnt,
};

constexpr std::array<const char*, ExecStatus::kExecStatusCnt> kExecStatusToStr =
    {"success", "command not implemented",
     "cannot output help message, unknown cmd"};

class Cmd {
 public:
  virtual ExecStatus Execute([[gnu::unused]] Config& conf) {
    return ExecStatus::kNotImplemented;
  }

  virtual ~Cmd() = default;

  const Id& Id() const { return id_; }

 protected:
  Cmd() = delete;
  explicit Cmd(const tftp::client::Id& id) : id_(id) {}

  tftp::client::Id id_;
};

class GetCmd : public Cmd {
 public:
  static ExpectedCmd<GetCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~GetCmd() = default;

  ExecStatus Execute(Config& conf) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const FileList& Files() const { return files_; }

 private:
  GetCmd() = delete;
  GetCmd(const File& remote_file, const File& local_file, const FileList& files)
      : Cmd(CmdId::kGet),
        remote_file_(remote_file),
        local_file_(local_file),
        files_(files) {}

  File remote_file_;
  File local_file_;
  FileList files_;
};

class PutCmd : public Cmd {
 public:
  static ExpectedCmd<PutCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~PutCmd() = default;

  ExecStatus Execute(Config& conf) final;

  const File& RemoteFile() const { return remote_file_; }
  const File& LocalFile() const { return local_file_; }
  const File& RemoteDir() const { return remote_dir_; }
  const FileList& Files() const { return files_; }

 private:
  PutCmd() = delete;
  PutCmd(const File& remote_file, const File& local_file,
         const File& remote_dir, const FileList& files)
      : Cmd(CmdId::kPut),
        remote_file_(remote_file),
        local_file_(local_file),
        remote_dir_(remote_dir),
        files_(files) {}

  File remote_file_;
  File local_file_;
  File remote_dir_;
  FileList files_;
};

class ConnectCmd : public Cmd {
 public:
  static ExpectedCmd<ConnectCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~ConnectCmd() = default;

  ExecStatus Execute(Config& conf) final;

  std::string Host() const { return host_; }
  uint16_t Port() const { return port_; }

 private:
  ConnectCmd() = delete;
  ConnectCmd(const Hostname& host, uint16_t port)
      : Cmd(CmdId::kConnect), host_(host), port_(port) {}

  std::string host_;
  uint16_t port_ = 0;
};

class LiteralCmd : public Cmd {
 public:
  static ExpectedCmd<LiteralCmd> Create();
  static void PrintUsage();

  virtual ~LiteralCmd() = default;

  ExecStatus Execute(Config& conf) final;

 private:
  LiteralCmd() : Cmd(CmdId::kLiteral) {}
};

class ModeCmd : public Cmd {
 public:
  static ExpectedCmd<ModeCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~ModeCmd() = default;

  ExecStatus Execute(Config& conf) final;

  const Mode& Mode() const { return mode_; }

 private:
  ModeCmd() = delete;
  explicit ModeCmd(const tftp::Mode mode) : Cmd(CmdId::kMode), mode_(mode) {}

  tftp::Mode mode_;
};

class StatusCmd : public Cmd {
 public:
  static ExpectedCmd<StatusCmd> Create();
  static void PrintUsage();

  virtual ~StatusCmd() = default;

  ExecStatus Execute(Config& conf) final;

 private:
  StatusCmd() : Cmd(CmdId::kStatus) {}
};

class TimeoutCmd : public Cmd {
 public:
  static ExpectedCmd<TimeoutCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~TimeoutCmd() = default;

  ExecStatus Execute(Config& conf) final;

  Seconds Timeout() const { return timeout_; }

 private:
  TimeoutCmd() = delete;
  explicit TimeoutCmd(Seconds timeout)
      : Cmd(CmdId::kTimeout), timeout_(timeout) {}

  Seconds timeout_;
};

class RexmtCmd : public Cmd {
 public:
  static ExpectedCmd<RexmtCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~RexmtCmd() = default;

  ExecStatus Execute(Config& conf) final;

  Seconds RexmtTimeout() const { return rexmt_timeout_; }

 private:
  RexmtCmd() = delete;
  explicit RexmtCmd(Seconds rexmt_timeout)
      : Cmd(CmdId::kRexmt), rexmt_timeout_(rexmt_timeout) {}

  Seconds rexmt_timeout_;
};

class HelpCmd : public Cmd {
 public:
  static ExpectedCmd<HelpCmd> Create(std::string_view cmdline);
  static void PrintUsage();

  virtual ~HelpCmd() = default;

  ExecStatus Execute(Config& conf) final;

  const tftp::client::Id& TargetCmd() const { return target_cmd_; }

 private:
  HelpCmd() = delete;
  explicit HelpCmd(const tftp::client::Id& target_cmd)
      : Cmd(CmdId::kHelp), target_cmd_(target_cmd) {}

  tftp::client::Id target_cmd_;
};

}  // namespace client
}  // namespace tftp

#endif
