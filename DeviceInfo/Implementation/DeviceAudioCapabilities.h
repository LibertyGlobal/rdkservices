#pragma once

#include "../Module.h"
#include <interfaces/IDeviceInfo.h>

namespace WPEFramework {
namespace Plugin {
    class DeviceAudioCapabilities : public Exchange::IDeviceAudioCapabilities {
    private:
        DeviceAudioCapabilities(const DeviceAudioCapabilities&) = delete;
        DeviceAudioCapabilities& operator=(const DeviceAudioCapabilities&) = delete;

    public:
        DeviceAudioCapabilities();

        BEGIN_INTERFACE_MAP(DeviceAudioCapabilities)
        INTERFACE_ENTRY(Exchange::IDeviceAudioCapabilities)
        END_INTERFACE_MAP

    private:
        // IDeviceAudioCapabilities interface
        uint32_t AudioOutputs(IAudioOutputIterator*& audioOutputs /* @out */) const override;
        uint32_t AudioCapabilities(const AudioOutput audioOutput /* @in */, IAudioCapabilityIterator*& audioCapabilities /* @out */) const override;
        uint32_t MS12Capabilities(const AudioOutput audioOutput /* @in */, IMS12CapabilityIterator*& ms12Capabilities /* @out */) const override;
        uint32_t MS12AudioProfiles(const AudioOutput audioOutput /* @in */, IMS12ProfileIterator*& ms12Profiles /* @out */) const override;
    };
}
}
