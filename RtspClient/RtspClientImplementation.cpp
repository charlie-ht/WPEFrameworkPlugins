#include "Module.h"
#include <interfaces/IRtspClient.h>
#include <interfaces/IMemory.h>

#include "RtspSession.h"

namespace WPEFramework {
namespace Plugin {

    class RtspClientImplementation : public Exchange::IRtspClient {
    private:

        class Config : public Core::JSON::Container {
        private:
            Config(const Config&) = delete;
            Config& operator=(const Config&) = delete;

        public:
            Config()
                : Core::JSON::Container()
                , TestNum(0)
            {
                Add(_T("testNum"), &TestNum);
                Add(_T("testStr"), &TestStr);
            }
            ~Config()
            {
            }

        public:
            Core::JSON::DecUInt16 TestNum;
            Core::JSON::String TestStr;
        };

    private:
        RtspClientImplementation(const RtspClientImplementation&) = delete;
        RtspClientImplementation& operator=(const RtspClientImplementation&) = delete;

    public:
        RtspClientImplementation()
            : _observers()
            , str("Nothing set")
        {
            TRACE_L1("%s: Initializing", __FUNCTION__);
        }

        virtual ~RtspClientImplementation()
        {
        }

        uint32_t Configure(PluginHost::IShell* service)
        {
            ASSERT(service != nullptr);

            Config config;
            config.FromString(service->ConfigLine());

            uint32_t result = 0;

            return (result);
        }

        uint32_t Setup(const string& assetId, uint32_t position)
        {
            RtspReturnCode rc = ERR_OK;

            string host = "Heisenberg";        // XXX: Move it to config file
            uint16_t port = 5554;
            rc = _rtspSession.Initialize(host, port);

            if (rc == ERR_OK) {
                rc = _rtspSession.Open(assetId, position);
            }
            return rc;
        }

        uint32_t Play(int32_t scale, uint32_t position)
        {
            RtspReturnCode rc = ERR_OK;

            TRACE_L2( "%s: scale=%d position=%d", __FUNCTION__, scale, position);
            rc = _rtspSession.Play((float_t) scale/1000, position);

            return rc;
        }

        uint32_t Teardown()
        {
            RtspReturnCode rc = ERR_OK;

            rc = _rtspSession.Close();
            _rtspSession.Terminate();

            return rc;
        }

        void Set(const string& name, const string& value)
        {
        }

        string Get(const string& name) const
        {
            return (str);
        }

        BEGIN_INTERFACE_MAP(RtspClientImplementation)
        INTERFACE_ENTRY(Exchange::IRtspClient)
        END_INTERFACE_MAP

    private:
        RtspSession _rtspSession;
        std::list<PluginHost::IStateControl::INotification*> _observers;
        string str;
    };

    SERVICE_REGISTRATION(RtspClientImplementation, 1, 0);

} // namespace Plugin

} // namespace WPEFramework
