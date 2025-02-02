#include <gtest/gtest.h>
#include "async_file_sink.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <tbox/util/fs.h>

using namespace std;
using namespace tbox;
using namespace tbox::log;

TEST(AsyncFileSink, Format)
{
    AsyncFileSink ch;

    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("test");
    ch.enable();

    LogInfo("%s, %d, %f", "hello", 123456, 12.345);
    LogInfo("%d, %f, %s", 123456, 12.345, "world");

    ch.cleanup();
}

TEST(AsyncFileSink, LongString)
{
    AsyncFileSink ch;

    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("test");
    ch.enable();

    for (size_t s = 900; s < 1200; ++s) {
        std::string tmp(s, 'z');
        LogInfo("%s", tmp.c_str());
    }

    std::string tmp(4096, 'x');
    LogInfo("%s", tmp.c_str());

    ch.cleanup();
}

TEST(AsyncFileSink, FileDivide)
{
    AsyncFileSink ch;

    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("test");
    ch.setFileMaxSize(100);
    ch.enable();

    std::string tmp(30, 'z');

    LogInfo("start");
    for (size_t s = 0; s < 20; ++s)
        LogInfo("%s", tmp.c_str());
    LogInfo("end");

    ch.cleanup();
}

//! 参数规范化
TEST(AsyncFileSink, ParamNormalize)
{
    AsyncFileSink ch;

    ch.setFilePath("  /tmp/tbox ");
    ch.setFilePrefix(" test ");
    ch.enable();
    LogInfo("Test LogPath");
    ch.cleanup();
}

TEST(AsyncFileSink, CreateFileInInit)
{
    AsyncFileSink ch;
    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("create_file_init");
    ch.enable();
    std::string name = ch.currentFilename();
    EXPECT_TRUE(util::fs::IsFileExist(name));
    ch.cleanup();
}

TEST(AsyncFileSink, RemoveLogFileDuringWriting)
{
    AsyncFileSink ch;
    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("remove_log_file_during_writing");
    ch.enable();
    util::fs::RemoveFile(ch.currentFilename());
    LogInfo("Hello");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(util::fs::IsFileExist(ch.currentFilename()));
    ch.cleanup();
}

#include <tbox/event/loop.h>
using namespace tbox::event;

TEST(AsyncFileSink, Benchmark)
{
    AsyncFileSink ch;
    ch.setFilePath("/tmp/tbox");
    ch.setFilePrefix("test");
    ch.enable();
    std::string tmp(30, 'x');

    auto sp_loop = Loop::New();

    int counter = 0;
    function<void()> func = [&] {
        for (int i = 0; i < 100; ++i)
            LogInfo("%d %s", i, tmp.c_str());
        sp_loop->run(func);
        counter += 100;
    };
    sp_loop->run(func);

    sp_loop->exitLoop(chrono::seconds(10));
    sp_loop->runLoop();

    delete sp_loop;
    cout << "count in sec: " << counter/10 << endl;
    ch.cleanup();
}

