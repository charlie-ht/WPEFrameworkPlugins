#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include <plugins/Logging.h>
#include <core/Queue.h>
#include <core/Thread.h>

#include "RtspParser.h"
#include "RtspSocket.h"

class RtspParser;

namespace WPEFramework {
namespace Plugin {

#define KB(x)   (x*1000)
#define MB(x)   (x*1000)


enum RtspReturnCode {
    ERR_OK,
    ERR_UNKNOWN,
    ERR_ACTIVE,
    ERR_CONNECT_FAILED,
    ERR_SESSION_FAILED,
    ERR_NO_MORE,
    ERR_TIMED_OUT,

};

class RtspMessage
{
    public:
    string message;
    bool bStreamRequest;
};

typedef Core::QueueType<RtspMessage> MessageQueue;


class RtspSession : public Core::Thread
{
    public:
        RtspSession();
        ~RtspSession();
        RtspReturnCode Initialize(const string& hostname, uint16_t port);
        RtspReturnCode Terminate();
        RtspReturnCode Open(const string assetId, uint32_t position = 0, const string &reqCpeId = "", const string &remoteIp = "");
        RtspReturnCode Close();
        RtspReturnCode Play(float scale, uint32_t position = 0);
        RtspReturnCode GetProp(const string name, string &value);

        RtspReturnCode GetParam(bool bSRM);
        RtspReturnCode SendHeartbeat(bool bSRM);
        RtspReturnCode SendHeartbeats();

        RtspReturnCode Check(bool bSRM, int timeout, string *pStr = NULL);
        RtspReturnCode CheckResponse(bool bStream);
        RtspReturnCode CheckAnnouncement();
        RtspReturnCode ProcessAnnouncement(const std::string &response, bool bSRM);
        RtspReturnCode SendResponse(int respSeq, bool bSRM);
        RtspReturnCode SendAnnouncement(int code, const string &reason);

        enum Type {
            RTSP_SETUP = 1,
            RTSP_PLAY,
            RTSP_TEARDOWN,
            RTSP_GETPARAM
        };

        enum AnnounceCode {
            PlayoutStalled              = 1103,
            PlayoutResumed              = 1104,
            NewScale                    = 1500,
            EosReached                  = 2101,
            BosReached                  = 2104,
            StreamStateChange           = 2199,
            TicketExpired               = 2401,
            SessionTornDown_heartbeat   = 2403,
            SessionTornDown             = 2406,
            DataError                   = 4400,
            SvrRsrcNotAvailable         = 5200,
            InternalServerError         = 5502,
            ServerShutdown              = 5403,
            StreamChange                = 5999
        };

    private:
        uint32_t Worker ();
        inline  RtspSocket& GetSocket(bool bSRM)    {
            return (bSRM || _sessionInfo.bSrmIsRtspProxy) ? _srmSocket : _controlSocket;
        }

        inline bool IsSrmRtspProxy() {
            return _sessionInfo.bSrmIsRtspProxy;
        }

    private:
        RtspSocket _srmSocket;          // XXX: Replace RtspSocket with Core::Socket
        RtspSocket _controlSocket;
        RtspParser _parser;
        RtspSessionInfo _sessionInfo;
        MessageQueue _requestQueue;
        MessageQueue _responseQueue;
        Core::CriticalSection _adminLock;

        bool _isSessionActive;
        int _nextSRMHeartbeatMS;
        int _nextPumpHeartbearMS;
        int _playDelay;

};

}} // WPEFramework::Plugin

#endif
