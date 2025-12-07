#include <gtest/gtest.h>

#include "cpr/callback.h"

#include "cpr/response.h"
#include "cpr/coroutine/coroutine.h"
#include "cpr/coroutine/sync_wait.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

bool write_data(std::string_view /*data*/, intptr_t /*userdata*/)
{
    return true;
}

TEST(CoroutineTests, CoroutineCallbackGetLambdaStatusTest)
{
    Url url{server->GetBaseUrl() + "/hello.html"};
    int status_code = 0;
    auto task = cpr::coroutine::CoGetCallback(
        [&status_code](Response r) {
            status_code = r.status_code;
            return r.status_code;
        },
        url
    );
    auto response = cpr::coroutine::sync_wait(task);
    EXPECT_EQ(status_code, response);
}

TEST(CoroutineTests, CoroutineAsyncGetTest)
{
    Url url{server->GetBaseUrl() + "/hello.html"};
    auto task = cpr::coroutine::CoGetAsync(url);
    cpr::Response response = cpr::coroutine::sync_wait(task);
    std::string expected_text{"Hello world!"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(response.primary_ip, "127.0.0.1");
    EXPECT_EQ(response.primary_port, server->GetPort());
}

TEST(CoroutineTests, CoroutineAsyncDeleteTest)
{
    Url url{server->GetBaseUrl() + "/delete.html"};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoDeleteAsync(url));
    std::string expected_text{"Delete success"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncDownloadTest)
{
    cpr::Url url{ server->GetBaseUrl() + "/download_gzip.html" };
    auto task = cpr::coroutine::CoDownloadAsync(
        fs::path{ "/tmp/async_download" },
        url,
        cpr::Header{
            { "Accept-Encoding", "gzip"}
        },
        cpr::WriteCallback{ write_data, 0 }
    );
    cpr::Response response = cpr::coroutine::sync_wait(task);
    
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(cpr::ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncHeadTest)
{
    Url url{server->GetBaseUrl() + "/hello.html"};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoHeadAsync(url));
    EXPECT_EQ(std::string{}, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"text/html"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncOptionsTest)
{
    Url url{server->GetBaseUrl() + "/"};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoOptionsAsync(url));
    std::string expected_text{""};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"GET, POST, PUT, DELETE, PATCH, OPTIONS"}, response.header["Access-Control-Allow-Methods"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncPatchTest)
{
    Url url{server->GetBaseUrl() + "/patch.html"};
    Payload payload{{"x", "5"}};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoPatchAsync(url, payload));
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncPostTest)
{
    Url url{server->GetBaseUrl() + "/url_post.html"};
    Payload payload{{"x", "5"}};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoPostAsync(url, payload));
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(201, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

TEST(CoroutineTests, CoroutineAsyncPutTest)
{
    Url url{server->GetBaseUrl() + "/put.html"};
    Payload payload{{"x", "5"}};
    cpr::Response response = cpr::coroutine::sync_wait(cpr::coroutine::CoPutAsync(url, payload));
    std::string expected_text{
            "{\n"
            "  \"x\": 5\n"
            "}"};
    EXPECT_EQ(expected_text, response.text);
    EXPECT_EQ(url, response.url);
    EXPECT_EQ(std::string{"application/json"}, response.header["content-type"]);
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(ErrorCode::OK, response.error.code);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}