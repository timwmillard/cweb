#include <stdio.h>
#include <stdint.h>

#include "ecewo.h"

#define SLOG_IMPLEMENTATION
#include "slog.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#define VERSION "0.0.1"

void hello_world(Req *req, Res *res) {
  send_json(res, OK, "{\"name\": \"Tim Millard\"}\n");
}
void usage(FILE *stream)
{
    fprintf(stream, "Usage: ./example [OPTIONS] [--] [ARGS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

void handle_ecewo_log(LogLevel level, const char *file, int line,
                  const char *fmt, va_list args) {
    char msg[1024];
    vsnprintf(msg, sizeof(msg), fmt, args);

    slog_logger *logger = slog_get_default();
    slog_log_impl(logger, (slog_level)level, msg, 
            slog_bool("ecewo_log", true),
#ifdef DEBUG
            slog_string("file", file),
            slog_int("line", line),
#endif
            NULL);
}

int main(int argc, char *argv[]) {
    bool        *help = flag_bool("help", false, "Print this help to stdout and exit with 0");
    uint64_t    *port = flag_uint64("port", 8080, "server port number");
    char **log_format = flag_str("log", "text", "logging format");

    if (!flag_parse(argc, argv)) {
        usage(stderr);
        flag_print_error(stderr);
        exit(1);
    }
    argc = flag_rest_argc();
    argv = flag_rest_argv();
    if (*help) {
        usage(stdout);
        exit(0);
    }

    slog_handler *log_handler;
    if (strcmp(*log_format, "json") == 0)
        log_handler = slog_json_handler_new(stdout, SLOG_INFO);
    else 
        log_handler = slog_text_handler_new(stdout, SLOG_INFO);

    slog_logger *logger = slog_new(log_handler);
    slog_set_default(logger);

    server_set_log_handler(handle_ecewo_log);
    server_set_log_level(LOG_LEVEL_INFO);

    int err;
    int backlog = 0;

    slog_info("Server started",
                 slog_string("version", VERSION),
                 slog_string("log_format", *log_format)
                 );

    if (server_init() != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return -1;
    }

    get("/", hello_world);

    if (server_listen(*port) != 0) {
        fprintf(stderr, "Failed to start server\n");
        return -1;
    }

    slog_info("Server started",
             slog_int("port", *port)
             );

    server_run();
    return 0;
}
