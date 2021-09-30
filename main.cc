#include <sys/uio.h>
#include <unistd.h>

#include "json-rpc-server.h"
#include "message-stream-splitter.h"

int main() {
  MessageStreamSplitter::ReadFun read_fun =
    [](char *buf, int size) -> int {
      return read(STDIN_FILENO, buf, size);
    };

  JsonRpcServer::WriteFun write_fun =
    [](absl::string_view reply) {
      std::cout << reply;
    };

  MessageStreamSplitter source(1 << 20);
  JsonRpcServer server(write_fun);

  source.SetMessageProcessor([&server](absl::string_view /*header*/,
                                       absl::string_view body) {
    return server.DispatchMessage(body);
  });

  absl::Status status = absl::OkStatus();
  while (status.ok()) {
    status = source.PullFrom(read_fun);
  }
  if (!status.ok())
    std::cerr << status.message() << std::endl;

  fprintf(stderr, "--------------- Statistic Counters Stats ---------------\n");
  fprintf(stderr, "Total bytes : %9ld\n", source.StatTotalBytesRead());
  fprintf(stderr, "Largest body: %9ld\n", source.StatLargestBodySeen());

  fprintf(stderr, "\n--- Methods called ---\n");
  int longest = 0;
  for (const auto &stats : server.GetStatCounters()) {
    longest = std::max(longest, (int)stats.first.length());
  }
  for (const auto &stats : server.GetStatCounters()) {
    fprintf(stderr, "%*s %9d\n", longest, stats.first.c_str(), stats.second);
  }
  return status.ok() ? 0 : 1;
}
