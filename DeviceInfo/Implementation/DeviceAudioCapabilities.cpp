#include "DeviceAudioCapabilities.h"

#include "exception.hpp"
#include "host.hpp"
#include "manager.hpp"

#include "UtilsIarm.h"

#include <core/Enumerate.h>

namespace WPEFramework {
namespace Plugin {

    SERVICE_REGISTRATION(DeviceAudioCapabilities, 1, 0);

    DeviceAudioCapabilities::DeviceAudioCapabilities()
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


    template<class enum_type>
    enum_type str_to_enum(const char *cstr) {
        return WPEFramework::Core::EnumerateType<enum_type>(cstr).Value();
    }

    template<class enum_type>
    const char *enum_to_str(enum_type e) {
        return WPEFramework::Core::EnumerateType<enum_type>(e).Data();
    }


    uint32_t DeviceAudioCapabilities::AudioOutputs(IAudioOutputIterator*& audioOutputs /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::list<AudioOutput> list;

        try {
            const auto& aPorts = device::Host::getInstance().getAudioOutputPorts();
            for (size_t i = 0; i < aPorts.size(); i++) {
                list.emplace_back(str_to_enum<AudioOutput>(aPorts.at(i).getName().c_str()));
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
            audioOutputs = (Core::Service<RPC::IteratorType<IAudioOutputIterator>>::Create<IAudioOutputIterator>(list));
        }
        return result;
    }

    uint32_t DeviceAudioCapabilities::AudioCapabilities(const AudioOutput audioOutput /* @in */, IAudioCapabilityIterator*& audioCapabilities /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::list<Exchange::IDeviceAudioCapabilities::AudioCapability> list;

        int capabilities = dsAUDIOSUPPORT_NONE;

        try {
            std::string strAudioPort = enum_to_str(audioOutput);
            if (strAudioPort.empty()) strAudioPort = device::Host::getInstance().getDefaultAudioPortName();

            auto& aPort = device::Host::getInstance().getAudioOutputPort(strAudioPort);
            aPort.getAudioCapabilities(&capabilities);
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        if (!capabilities)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::AUDIOCAPABILITY_NONE);
        if (capabilities & dsAUDIOSUPPORT_ATMOS)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::ATMOS);
        if (capabilities & dsAUDIOSUPPORT_DD)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::DD);
        if (capabilities & dsAUDIOSUPPORT_DDPLUS)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::DDPLUS);
        if (capabilities & dsAUDIOSUPPORT_DAD)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::DAD);
        if (capabilities & dsAUDIOSUPPORT_DAPv2)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::DAPV2);
        if (capabilities & dsAUDIOSUPPORT_MS12)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::AudioCapability::MS12);

        if (result == Core::ERROR_NONE) {
            audioCapabilities = (Core::Service<RPC::IteratorType<Exchange::IDeviceAudioCapabilities::IAudioCapabilityIterator>>::Create<Exchange::IDeviceAudioCapabilities::IAudioCapabilityIterator>(list));
        }

        return result;
    }

    uint32_t DeviceAudioCapabilities::MS12Capabilities(const AudioOutput audioOutput /* @in */, IMS12CapabilityIterator*& ms12Capabilities /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;

        std::list<Exchange::IDeviceAudioCapabilities::MS12Capability> list;

        int capabilities = dsMS12SUPPORT_NONE;

        try {
            std::string strAudioPort = enum_to_str(audioOutput);
            if (strAudioPort.empty()) strAudioPort = device::Host::getInstance().getDefaultAudioPortName();

            auto& aPort = device::Host::getInstance().getAudioOutputPort(strAudioPort);
            aPort.getMS12Capabilities(&capabilities);
        } catch (const device::Exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (const std::exception& e) {
            TRACE(Trace::Fatal, (_T("Exception caught %s"), e.what()));
            result = Core::ERROR_GENERAL;
        } catch (...) {
            result = Core::ERROR_GENERAL;
        }

        if (!capabilities)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::MS12Capability::MS12CAPABILITY_NONE);
        if (capabilities & dsMS12SUPPORT_DolbyVolume)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::MS12Capability::DOLBYVOLUME);
        if (capabilities & dsMS12SUPPORT_InteligentEqualizer)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::MS12Capability::INTELIGENTEQUALIZER);
        if (capabilities & dsMS12SUPPORT_DialogueEnhancer)
            list.emplace_back(Exchange::IDeviceAudioCapabilities::MS12Capability::DIALOGUEENHANCER);

        if (result == Core::ERROR_NONE) {
            ms12Capabilities = (Core::Service<RPC::IteratorType<Exchange::IDeviceAudioCapabilities::IMS12CapabilityIterator>>::Create<Exchange::IDeviceAudioCapabilities::IMS12CapabilityIterator>(list));
        }

        return result;
    }


    uint32_t DeviceAudioCapabilities::MS12AudioProfiles(const AudioOutput audioOutput /* @in */, IMS12ProfileIterator*& ms12Profiles /* @out */) const
    {
        uint32_t result = Core::ERROR_NONE;
        std::list<MS12Profile> list;

        try {
            std::string strAudioPort = enum_to_str(audioOutput);
            if (strAudioPort.empty()) strAudioPort = device::Host::getInstance().getDefaultAudioPortName();
            auto& aPort = device::Host::getInstance().getAudioOutputPort(strAudioPort);
            const auto supportedProfiles = aPort.getMS12AudioProfileList();
            for (size_t i = 0; i < supportedProfiles.size(); i++) {
                list.emplace_back(str_to_enum<MS12Profile>(supportedProfiles.at(i).c_str()));
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
            ms12Profiles = (Core::Service<RPC::IteratorType<Exchange::IDeviceAudioCapabilities::IMS12ProfileIterator>>::Create<Exchange::IDeviceAudioCapabilities::IMS12ProfileIterator>(list));
        }

        return result;
    }
}
}
