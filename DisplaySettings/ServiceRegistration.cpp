#include "core/Services.h"
#include "DisplaySettings.h"
#include "DisplaySettingsLgiAddons.h"

namespace WPEFramework {
    namespace Plugin {

#ifdef USE_LGI_DEVICESETTINGS_IMPLEMENTATION
        SERVICE_REGISTRATION(DisplaySettingsLgiAddons, 1, 0);
#else
        SERVICE_REGISTRATION(DisplaySettings, 1, 0);
#endif

}}