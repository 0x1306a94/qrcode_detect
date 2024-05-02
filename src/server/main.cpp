#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include <hv/HttpServer.h>
#include <hv/hlog.h>

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "daemon.hpp"
#include "router.hpp"
#include "server_context.hpp"

#include <qrcode_detect_server/version.hpp>

#include <argparse/argparse.hpp>
#include <filesystem>

namespace fs = std::filesystem;

const char *ENV_DISABLE_CONSOLE_LOG_NAME = "DISABLE_CONSOLE_LOG";
static int LOG_verbosity = SPDLOG_LEVEL_OFF;

void setup_log(const std::string &log_dir) {
    if (log_dir.empty()) {
        spdlog::set_level(spdlog::level::off);
        return;
    }

    auto path = fs::path(log_dir);
    bool exists = fs::exists(path);
    if (!exists) {
        if (!fs::create_directories(path)) {
            std::cerr << "create_directories fail: " << log_dir << std::endl;
            exit(EXIT_FAILURE);
            return;
        }
    }

    /// 单个日志文件最大 16M
    size_t max_size = 1024 * 1024 * 16;
    size_t max_files = 10;

    auto log_path = path.append("log.log");
    auto str_path = log_path.string();

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        str_path, max_size, max_files);

    std::vector<spdlog::sink_ptr> m_sinks{{file_sink}};
    const char *tmp = std::getenv(ENV_DISABLE_CONSOLE_LOG_NAME);
    std::string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        m_sinks.push_back(console_sink);
    }

    auto logger = std::make_shared<spdlog::logger>("multi_sink", m_sinks.begin(),
                                                   m_sinks.end());
    logger->flush_on(spdlog::level::trace);
    if (LOG_verbosity < 0) {
        logger->set_level(spdlog::level::err);
        file_sink->set_level(spdlog::level::err);
    } else {
        file_sink->set_level(static_cast<spdlog::level::level_enum>(LOG_verbosity));
        logger->set_level(static_cast<spdlog::level::level_enum>(LOG_verbosity));
    }

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

void libhv_log_handler(int loglevel, const char *buf, int len) {
    if (len == 0) {
        return;
    }
    std::string_view msg(buf, len);
    if (buf[len - 1] == '\n') {
        msg = std::string_view(buf, len - 1);
    }
    switch (loglevel) {
        case LOG_LEVEL_DEBUG: {
            SPDLOG_DEBUG("{}", msg);
            break;
        }
        case LOG_LEVEL_INFO: {
            SPDLOG_INFO("{}", msg);
            break;
        }
        case LOG_LEVEL_WARN: {
            SPDLOG_WARN("{}", msg);
            break;
        }
        case LOG_LEVEL_ERROR: {
            SPDLOG_ERROR("{}", msg);
            break;
        }
        case LOG_LEVEL_FATAL: {
            SPDLOG_CRITICAL("{}", msg);
            break;
        }
    }
}

void create_pidfile(const std::string &path) {
    pid_t pid = getpid();
    std::ofstream pidFile(path);
    if (pidFile.is_open()) {
        pidFile << pid;
        pidFile.close();
        std::cerr << "create pidfile " << path << " pid " << pid << std::endl;
    } else {
        std::cerr << "Failed to open PID file " << path << std::endl;
    }
}

void delete_pidfile(const std::string &path) {
    if (fs::exists(path)) {
        fs::remove(path);
        std::cerr << "delete pidfile " << path << std::endl;
    }
}

static hv::HttpServer *server = nullptr;
void signal_exit_handler(int sig) {
    SPDLOG_INFO("signal_exit_handler");
    if (server != nullptr) {
        server->stop();
    }
}

int main(int argc, char *argv[]) {
    std::stringstream ss;
    ss << "qrcode_detect_server " << QRCODE_DETECT_SERVER_VERSION << " " << QRCODE_DETECT_SERVER_GIT_BRANCH << " " << QRCODE_DETECT_SERVER_GIT_HASH;
    argparse::ArgumentParser program("qrcode_detect_server", ss.str(), argparse::default_arguments::all);
    program.add_argument("-p", "--port")
        .required()
        .scan<'i', uint16_t>()
        .help("service port.");

    program.add_argument("-m", "--model")
        .required()
        .help("model file directory.");

    program.add_argument("-l", "--log")
        .help("log file directory.");

    program.add_argument("-d", "--daemon")
        .default_value(false)
        .implicit_value(true)
        .help("run daemon.");

    program.add_argument("--pid")
        .help("pidfile path.");

    auto &group = program.add_mutually_exclusive_group();
    group.add_argument("-V")
        .action([&](const auto &) { --LOG_verbosity; })
        .append()
        .implicit_value(true)
        .nargs(0)
        .help("Set log level to critical, err, warn, info, debug, trace.");

    group.add_argument("--verbose")
        .action([&](const auto &) { LOG_verbosity = SPDLOG_LEVEL_TRACE; })
        .default_value(false)
        .implicit_value(true)
        .help("Set log level to trace.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    std::uint16_t port = program.get<std::uint16_t>("--port");
    std::string model_dir = program.get<std::string>("--model");
    std::string log_dir;
    if (program.is_used("--log")) {
        log_dir = program.get<std::string>("--log");
    }

    bool daemon = program.get<bool>("daemon");
    if (daemon && !qrcode::start_daemon()) {
        return EXIT_FAILURE;
    }

    std::string pidfile;
    do {
        if (!daemon) {
            break;
        }
        if (!program.is_used("--pid")) {
            break;
        }
        pidfile = program.get<std::string>("--pid");
        if (pidfile.empty()) {
            break;
        }

        delete_pidfile(pidfile);
        create_pidfile(pidfile);
    } while (0);

    //    signal(SIGINT, signal_exit_handler);
    signal(SIGQUIT, signal_exit_handler);

    setup_log(log_dir);
    context::Context::Init(model_dir);

    SPDLOG_INFO("start http server 0.0.0.0: {}", port);

    hlog_set_handler(libhv_log_handler);

    hv::HttpService service;
    qrcode::Router::Register(service);
    server = new hv::HttpServer();
    server->registerHttpService(&service);
    server->setPort(port);
    server->setThreadNum(4);
    server->run();
    SPDLOG_INFO("stop http server 0.0.0.0: {}", port);
    if (!pidfile.empty()) {
        delete_pidfile(pidfile);
    }
    spdlog::shutdown();
    return EXIT_SUCCESS;
}
