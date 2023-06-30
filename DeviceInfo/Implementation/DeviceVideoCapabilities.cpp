#include "DeviceVideoCapabilities.h"

#include "exception.hpp"
#include "host.hpp"
#include "manager.hpp"
#include "videoOutputPortConfig.hpp"

#include "UtilsIarm.h"

#include "util.h"

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(DeviceVideoCapabilities, 1, 0);

    DeviceVideoCapabilities::DeviceVideoCapabilities()
    {
        Utils::IARM::init();

        try {
            device::Manager::Initialize();
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
        } catch (...) {
        }
    }

    uint32_t DeviceVideoCapabilities::VideoOutputs(IVideoOutputIterator*& videoOutputs /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::list<VideoOutput> list;

        try {
            const auto& vPorts = device::Host::getInstance().getVideoOutputPorts();
            for (size_t i = 0; i < vPorts.size(); i++) {

                /**
                 * There's N:1 relation between VideoOutputPort and AudioOutputPort.
                 * When there are multiple Audio Ports on the Video Port,
                 * there are multiple VideoOutputPort-s as well.
                 * Those VideoOutputPort-s are the same except holding a different Audio Port id.
                 * As a result, a list of Video Ports has multiple Video Ports
                 * that represent the same Video Port, but different Audio Port.
                 * A list of VideoOutputPort-s returned from DS
                 * needs to be filtered by name.
                 */

                auto port = str_to_enum<VideoOutput>(vPorts.at(i).getName());
                if (std::find(list.begin(), list.end(), port) != list.end())
                    continue;

                list.emplace_back(port);
            }
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        if (result == Core::ERROR_NONE) {
            videoOutputs = (Core::Service<RPC::IteratorType<IVideoOutputIterator>>::Create<IVideoOutputIterator>(list));
        }

        return result;
    }

    uint32_t DeviceVideoCapabilities::HostEDID(string& edid /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::vector<uint8_t> edidVec({ 'u', 'n', 'k', 'n', 'o', 'w', 'n' });
        try {
            std::vector<unsigned char> edidVec2;
            device::Host::getInstance().getHostEDID(edidVec2);
            edidVec = edidVec2;
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        if (result == Core::ERROR_NONE) {
            // convert to base64
            if (edidVec.size() > (size_t)std::numeric_limits<uint16_t>::max()) {
                result = Core::ERROR_GENERAL;
            } else {
                string base64String;
                Core::ToString((uint8_t*)&edidVec[0], edidVec.size(), true, base64String);
                edid = base64String;
            }
        }

        return result;
    }

    uint32_t DeviceVideoCapabilities::DefaultResolution(const VideoOutput videoOutput /* @in */, ScreenResolution& defaultResolution /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        try {
            std::string strVideoPort = enum_to_str(videoOutput);
            if (strVideoPort.empty()) strVideoPort = device::Host::getInstance().getDefaultVideoPortName();
            auto& vPort = device::Host::getInstance().getVideoOutputPort(strVideoPort);
            defaultResolution = str_to_enum<ScreenResolution>(vPort.getDefaultResolution().getName().c_str());
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        return result;
    }

    uint32_t DeviceVideoCapabilities::Resolutions(const VideoOutput videoOutput /* @in */, IScreenResolutionIterator*& resolutions /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::list<ScreenResolution> list;

        try {
            std::string strVideoPort = enum_to_str(videoOutput);
            if (strVideoPort.empty()) strVideoPort = device::Host::getInstance().getDefaultVideoPortName();
            auto& vPort = device::Host::getInstance().getVideoOutputPort(strVideoPort);
            const auto resolutions = device::VideoOutputPortConfig::getInstance().getPortType(vPort.getType().getId()).getSupportedResolutions();
            for (size_t i = 0; i < resolutions.size(); i++) {
                list.emplace_back(str_to_enum<ScreenResolution>(resolutions.at(i).getName()));
            }
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        if (result == Core::ERROR_NONE) {
            resolutions = (Core::Service<RPC::IteratorType<IScreenResolutionIterator>>::Create<IScreenResolutionIterator>(list));
        }

        return result;
    }

    uint32_t DeviceVideoCapabilities::Hdcp(const VideoOutput videoOutput /* @in */, CopyProtection& hdcpVersion /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        try {
            std::string strVideoPort = enum_to_str(videoOutput);
            if (strVideoPort.empty()) strVideoPort = device::Host::getInstance().getDefaultVideoPortName();
            auto& vPort = device::Host::getInstance().getVideoOutputPort(strVideoPort);

            switch (vPort.getHDCPProtocol()) {
            case dsHDCP_VERSION_2X:
                hdcpVersion = Exchange::IDeviceVideoCapabilities::CopyProtection::HDCP_22;
                break;
            case dsHDCP_VERSION_1X:
                hdcpVersion = Exchange::IDeviceVideoCapabilities::CopyProtection::HDCP_14;
                break;
            default:
                result = Core::ERROR_GENERAL;
            }
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        return result;
    }

    uint32_t DeviceVideoCapabilities::HDR(bool& supportsHDR /*@out*/) const
    {
        uint32_t result = Core::ERROR_GENERAL;
        supportsHDR = false;
        return result;
    }
    uint32_t DeviceVideoCapabilities::Atmos(bool& supportsAtmos /*@out*/) const
    {
        uint32_t result = Core::ERROR_GENERAL;
        supportsAtmos = false;
        return result;
    }
    uint32_t DeviceVideoCapabilities::CEC(bool& supportsCEC /*@out*/) const
    {
        uint32_t result = Core::ERROR_GENERAL;
        supportsCEC = false;
        return result;
    }
}
}
