#ifndef OPENAI_REALTIME_H
#define OPENAI_REALTIME_H

/*
Date: 2025-12-31
This is a sample code for a SIP gateway bridging SIP call endpoint to OpenAI realtime API, support multiple simultaneous lines
The code provided here has no gurantee anyhow to a PROD usage
Please reach out to the Author(Yonge Liu) for any issues or bugs
Author: Yonge Liu
Email: yonge.liu@gmail.com
Enviroment tested: Ubuntu 24.02
How to run the application:
$./openai_realtime
Then press key 'q' or 'e' to exit the program.
*/

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <thread>
#include <mutex>
#include <deque>
#include <vector>
#include <atomic>

using namespace pj;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

// --- Configuration ---
const std::string OPENAI_HOST = "api.openai.com";
const std::string OPENAI_PATH = "/v1/realtime?model=gpt-4o-realtime-preview-2024-10-01";
const std::string OPENAI_API_KEY = "YOUR_OPENAI_API_KEY"; // <--- PUT KEY HERE

// SIP Config
const std::string SIP_USER = "user";
const std::string SIP_PASS = "password";
const std::string SIP_DOMAIN = "127.0.0.1"; // Or your PBX IP

// Audio Config (OpenAI Native)
const int CLOCK_RATE = 24000; 
const int PTIME = 20;
const int SAMPLES_PER_FRAME = CLOCK_RATE * PTIME / 1000;
const int CHANNELS = 1;

std::string base64_encode(const std::vector<int16_t>& data) ;
std::vector<int16_t> base64_decode(const std::string &in);

// --- Audio & PJSIP Setup ---
class AudioQueue {
    std::deque<int16_t> buffer;
    std::mutex mtx;
public:
    void push(const std::vector<int16_t>& data);
    bool pop(std::vector<int16_t>& out, size_t count);
    void reset() {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.clear();
    }
    void clear()
    {
        reset();
    }
};

class BotMediaPort : public pj::AudioMediaPort 
{
public:
    AudioQueue incoming_q; // OpenAI -> SIP
    AudioQueue outgoing_q; // SIP -> OpenAI

public:
    BotMediaPort();
    virtual void onFrameRequested(pj::MediaFrame &frame) override;
    virtual void onFrameReceived(pj::MediaFrame &frame) override;
};


class BotCall : public pj::Call 
{
public:
    BotMediaPort* port;
    bool isCallActive;
    std::thread* pOpenAIThread;

public:
    BotCall(pj::Account &acc, int call_id = PJSUA_INVALID_ID);
    ~BotCall();

    virtual void onCallState(pj::OnCallStateParam &prm) override;
    virtual void onCallMediaState(pj::OnCallMediaStateParam &prm) override;
};


class BotAccount : public pj::Account 
{
public:
    virtual void onIncomingCall(pj::OnIncomingCallParam &iprm) override;
};

#endif // OPENAI_REALTIME_H