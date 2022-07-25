#include "../SoC_abstraction.h"

#include "dsMgr.h"
#include "libIBusDaemon.h"
#include "host.hpp"
#include "exception.hpp"
#include "audioOutputPort.hpp"
#include "audioOutputPortType.hpp"
#include "audioOutputPortConfig.hpp"
#include "manager.hpp"
#include "dsUtl.h"
#include "dsError.h"
#include "list.hpp"
#include "libIBus.h"

#include "utils.h"

void Soc_Initialize()
{
    Utils::IARM::init();
    device::Manager::Initialize();
}

void Soc_Deinitialize()
{
    device::Manager::DeInitialize();
}

void SoC_ChangePrimaryVol(MixGain gain, int volume)
{
    const string audioPort = "HDMI0";
    device::AudioOutputPort aPort = device::Host::getInstance().getAudioOutputPort(audioPort);
    aPort.setLevel(volume);
}
