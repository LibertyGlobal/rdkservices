/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2020 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include "LEDControl.h"
#include <algorithm>

#include "rdk/iarmmgrs-hal/pwrMgr.h"

#include "UtilsJsonRpc.h"
#include "UtilsIarm.h"
#include "dsFPD.h"

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 0
#define API_VERSION_NUMBER_PATCH 1

#define FPD_LED_DEVICE_NONE "NONE"
#define FPD_LED_DEVICE_ACTIVE "ACTIVE"
#define FPD_LED_DEVICE_STANDBY "STANDBY"
#define FPD_LED_DEVICE_WPS_CONNECTING "WPS_CONNECTING"
#define FPD_LED_DEVICE_WPS_CONNECTED "WPS_CONNECTED"
#define FPD_LED_DEVICE_WPS_ERROR "WPS_ERROR"
#define FPD_LED_DEVICE_WIFI_ERROR "WIFI_ERROR"
#define FPD_LED_DEVICE_BOOT_IN_PROGRESS "BOOT_IN_PROGRESS"
#define FPD_LED_DEVICE_COLDSTANDBY "COLDSTANDBY"
#define FPD_LED_DEVICE_PSU_FAILURE "PSU_FAILURE"
#define FPD_LED_DEVICE_WPS_SES_OVERLAP "WPS_SES_OVERLAP"
#define FPD_LED_DEVICE_IP_ACQUIRED "IP_ACQUIRED"
#define FPD_LED_DEVICE_NO_IP "NO_IP"
#define FPD_LED_DEVICE_RCU_COMMAND "RCU_COMMAND"


namespace WPEFramework
{
    namespace {

        static Plugin::Metadata<Plugin::LEDControl> metadata(
            // Version (Major, Minor, Patch)
            API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    namespace Plugin
    {
        SERVICE_REGISTRATION(LEDControl, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

        LEDControl* LEDControl::_instance = nullptr;



        LEDControl::LEDControl()
        : PluginHost::JSONRPC(),
        m_isPlatInitialized (false)
        {
           LOGWARN("ctor");
           LEDControl::_instance = this;
           Register("getSupportedLEDStates", &LEDControl::getSupportedLEDStates, this);
           Register("getLEDState", &LEDControl::getLEDState, this);
           Register("setLEDState", &LEDControl::setLEDState, this);

        }

        LEDControl::~LEDControl()
        {

        }

        const string LEDControl::Initialize(PluginHost::IShell* /* service */)
        {
            string msg;
            if (!m_isPlatInitialized){
                LOGINFO("Doing plat init");
                if (dsERR_NONE != dsFPInit()){
                    msg = "dsFPInit failed";
		    LOGERR("dsFPInit failed");
		    return msg;
		}
                m_isPlatInitialized = true;
            }

            // On success return empty, to indicate there is no error text.
           return msg;
        }

        void LEDControl::Deinitialize(PluginHost::IShell* /* service */)
        {
            LEDControl::_instance = nullptr;

            if (m_isPlatInitialized){
                LOGINFO("Doing plat uninit");
                dsFPTerm();
                m_isPlatInitialized = false;
            }
        }

        void LEDControl::setResponseArray(JsonObject& response, const char* key, const vector<string>& items)
        {
            JsonArray arr;
            for(auto& i : items) arr.Add(JsonValue(i));

            response[key] = arr;

            string json;
            response.ToString(json);
        }

        /**
         * @brief This method returns all the led states supported by the platform.
         *
         * @param: None.
         * @return Returns the success code of underlying method.
         */
        uint32_t LEDControl::getSupportedLEDStates(const JsonObject& parameters, JsonObject& response)
        {
            bool success = false;
            vector<string> supportedLEDStates;

            try {
                unsigned int states = dsFPD_LED_DEVICE_NONE;
                dsError_t err = dsFPGetSupportedLEDStates (&states);
                if (!err) {
                    if(!states)supportedLEDStates.emplace_back(FPD_LED_DEVICE_NONE);
                    if(states & (dsFPD_LED_DEVICE_ACTIVE))supportedLEDStates.emplace_back(FPD_LED_DEVICE_ACTIVE);
                    if(states & (dsFPD_LED_DEVICE_STANDBY))supportedLEDStates.emplace_back(FPD_LED_DEVICE_STANDBY);
                    if(states & (dsFPD_LED_DEVICE_WPS_CONNECTING))supportedLEDStates.emplace_back(FPD_LED_DEVICE_WPS_CONNECTING);
                    if(states & (dsFPD_LED_DEVICE_WPS_CONNECTED))supportedLEDStates.emplace_back(FPD_LED_DEVICE_WPS_CONNECTED);
                    if(states & (dsFPD_LED_DEVICE_WPS_ERROR))supportedLEDStates.emplace_back(FPD_LED_DEVICE_WPS_ERROR);
                    if(states & (dsFPD_LED_DEVICE_WIFI_ERROR))supportedLEDStates.emplace_back(FPD_LED_DEVICE_WIFI_ERROR);
		    if(states & (dsFPD_LED_DEVICE_BOOT_IN_PROGRESS))supportedLEDStates.emplace_back(FPD_LED_DEVICE_BOOT_IN_PROGRESS);
                    if(states & (dsFPD_LED_DEVICE_COLDSTANDBY))supportedLEDStates.emplace_back(FPD_LED_DEVICE_COLDSTANDBY);
                    if(states & (dsFPD_LED_DEVICE_PSU_FAILURE))supportedLEDStates.emplace_back(FPD_LED_DEVICE_PSU_FAILURE);
                    if(states & (dsFPD_LED_DEVICE_WPS_SES_OVERLAP))supportedLEDStates.emplace_back(FPD_LED_DEVICE_WPS_SES_OVERLAP);
                    if(states & (dsFPD_LED_DEVICE_IP_ACQUIRED))supportedLEDStates.emplace_back(FPD_LED_DEVICE_IP_ACQUIRED);
                    if(states & (dsFPD_LED_DEVICE_NO_IP))supportedLEDStates.emplace_back(FPD_LED_DEVICE_NO_IP);
                    if(states & (dsFPD_LED_DEVICE_RCU_COMMAND))supportedLEDStates.emplace_back(FPD_LED_DEVICE_RCU_COMMAND);
                    success = true;
                } else {
                        LOGERR("dsFPGetSupportedLEDStates returned error %d", err);
                }

            } catch (...){
                LOGERR("Exception in supportedLEDStates");
            }
            setResponseArray(response, "supportedLEDStates", supportedLEDStates);
            returnResponse(success);
        }

        /**
         * @brief This method returns current led state of the platform.
         *
         * @param: None.
         * @return Returns the success code of underlying method.
         */
        uint32_t LEDControl::getLEDState(const JsonObject& parameters, JsonObject& response)
        {
            bool success = false;

            try
            {
               dsFPDLedState_t state;
               dsError_t err = dsFPGetLEDState (&state);
               if (!err) {
                    success = true;
		    static const std::unordered_map<dsFPDLedState_t, const char*> stateToString = {
                       {dsFPD_LED_DEVICE_NONE, FPD_LED_DEVICE_NONE},
                       {dsFPD_LED_DEVICE_ACTIVE, FPD_LED_DEVICE_ACTIVE},
                       {dsFPD_LED_DEVICE_STANDBY, FPD_LED_DEVICE_STANDBY},
                       {dsFPD_LED_DEVICE_WPS_CONNECTING, FPD_LED_DEVICE_WPS_CONNECTING},
                       {dsFPD_LED_DEVICE_WPS_CONNECTED, FPD_LED_DEVICE_WPS_CONNECTED},
                       {dsFPD_LED_DEVICE_WPS_ERROR, FPD_LED_DEVICE_WPS_ERROR},
                       {dsFPD_LED_DEVICE_WIFI_ERROR, FPD_LED_DEVICE_WIFI_ERROR},
                       {dsFPD_LED_DEVICE_BOOT_IN_PROGRESS, FPD_LED_DEVICE_BOOT_IN_PROGRESS},
                       {dsFPD_LED_DEVICE_COLDSTANDBY, FPD_LED_DEVICE_COLDSTANDBY},
                       {dsFPD_LED_DEVICE_PSU_FAILURE, FPD_LED_DEVICE_PSU_FAILURE},
                       {dsFPD_LED_DEVICE_WPS_SES_OVERLAP, FPD_LED_DEVICE_WPS_SES_OVERLAP},
                       {dsFPD_LED_DEVICE_IP_ACQUIRED, FPD_LED_DEVICE_IP_ACQUIRED},
                       {dsFPD_LED_DEVICE_NO_IP, FPD_LED_DEVICE_NO_IP},
                       {dsFPD_LED_DEVICE_RCU_COMMAND, FPD_LED_DEVICE_RCU_COMMAND},
                    };
                    auto it = stateToString.find(state);
                    if (it != stateToString.end()) {
                        response["state"] = it->second;
                    } else {
                        LOGERR("Unsupported LEDState %d", state);
                        LOGTRACEMETHODFIN();
                        return WPEFramework::Core::ERROR_BAD_REQUEST;
                    }
                } else {
                    LOGERR("dsFPGetLEDState returned error %d", err);
                    LOGTRACEMETHODFIN();
                    return WPEFramework::Core::ERROR_ILLEGAL_STATE;
                }
            }
            catch(...)
            {
                LOGERR("Exception in dsFPGetLEDState");
                LOGTRACEMETHODFIN();
                return WPEFramework::Core::ERROR_ILLEGAL_STATE;
            }

            returnResponse(success);
        }

        /**
         * @brief This method changes the current led state to the one, mentioned by the user.
         *
         * @param[in] led state to apply.
         * @return Returns the success code of underlying method.
         * @ingroup SERVMGR_FRONTPANEL_API
         */
        uint32_t LEDControl::setLEDState(const JsonObject& parameters, JsonObject& response)
        {
            LOGINFOMETHOD();
            returnIfParamNotFound(parameters, "state");
            string strLedState = parameters["state"].String();
            bool success = false;
            try
            {
	        static const std::unordered_map<std::string, dsFPDLedState_t> stateMap = {
                   {FPD_LED_DEVICE_ACTIVE, dsFPD_LED_DEVICE_ACTIVE},
                   {FPD_LED_DEVICE_STANDBY, dsFPD_LED_DEVICE_STANDBY},
                   {FPD_LED_DEVICE_WPS_CONNECTING, dsFPD_LED_DEVICE_WPS_CONNECTING},
                   {FPD_LED_DEVICE_WPS_CONNECTED, dsFPD_LED_DEVICE_WPS_CONNECTED},
                   {FPD_LED_DEVICE_WPS_ERROR, dsFPD_LED_DEVICE_WPS_ERROR},
                   {FPD_LED_DEVICE_WIFI_ERROR, dsFPD_LED_DEVICE_WIFI_ERROR},
                   {FPD_LED_DEVICE_BOOT_IN_PROGRESS, dsFPD_LED_DEVICE_BOOT_IN_PROGRESS},
                   {FPD_LED_DEVICE_COLDSTANDBY, dsFPD_LED_DEVICE_COLDSTANDBY},
                   {FPD_LED_DEVICE_PSU_FAILURE, dsFPD_LED_DEVICE_PSU_FAILURE},
                   {FPD_LED_DEVICE_WPS_SES_OVERLAP, dsFPD_LED_DEVICE_WPS_SES_OVERLAP},
                   {FPD_LED_DEVICE_IP_ACQUIRED, dsFPD_LED_DEVICE_IP_ACQUIRED},
                   {FPD_LED_DEVICE_NO_IP, dsFPD_LED_DEVICE_NO_IP},
                   {FPD_LED_DEVICE_RCU_COMMAND, dsFPD_LED_DEVICE_RCU_COMMAND}
                };

                auto it = stateMap.find(strLedState);
		if (it == stateMap.end()) {
                    //Invalid parameter
                    LOGERR("UNKNOWN state : %s", strLedState.c_str());
                    LOGTRACEMETHODFIN();
                    return WPEFramework::Core::ERROR_BAD_REQUEST;
                }

		dsFPDLedState_t state = it->second;
		LOGINFO("dsFPSetLEDState state:%s state:%d", strLedState.c_str(), state);
                dsError_t err = dsFPSetLEDState (state);
                if (err == dsERR_NONE) {
                    success = true;
                } else {
                    LOGERR("dsFPSetLEDState returned error %d", err);
                    LOGTRACEMETHODFIN();
                    return WPEFramework::Core::ERROR_ILLEGAL_STATE;
               }
            }
            catch (...)
            {
                LOGERR("Exception in dsFPSetLEDState");
                LOGTRACEMETHODFIN();
                return WPEFramework::Core::ERROR_ILLEGAL_STATE;
            }

            returnResponse(success);
        }


    } // namespace Plugin
} // namespace WPEFramework
