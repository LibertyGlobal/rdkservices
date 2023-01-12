/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2019 RDK Management
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

/**
 *  Utility functions used in plugins.
 *  This is IARM-less version of ../helpers/utils.cpp
 */

#include <string.h>
#include <sstream>
#include "utils.h"
#include <curl/curl.h>
#include <utility>
#include <ctype.h>
#include <mutex>

#define MAX_STRING_LENGTH 2048

#define SERVER_DETAILS  "127.0.0.1:9998"

using namespace WPEFramework;
using namespace std;

/***
 * @brief	: Execute shell script and get response
 * @param1[in]	: script to be executed with args
 * @return		: string; response.
 */
std::string Utils::cRunScript(const char *cmd)
{
    std::string totalStr = "";
    FILE *pipe = NULL;
    char buff[1024] = {'\0'};

    if ((pipe = popen(cmd, "r"))) {
        memset(buff, 0, sizeof(buff));
        while (fgets(buff, sizeof(buff), pipe)) {
            totalStr += buff;
            memset(buff, 0, sizeof(buff));
        }
        pclose(pipe);
    } else {
        /* popen failed. */
    }
    return totalStr;
}

using namespace WPEFramework;

/***
 * @brief	: Checks that file exists
 * @param1[in]	: pFileName name of file
 * @return		: true if file exists.
 */
bool Utils::fileExists(const char *pFileName)
{
    struct stat fileStat;
    return 0 == stat(pFileName, &fileStat);
}

/***
 * @brief	: Checks that file exists and modified at least pointed seconds ago
 * @param1[in]	: pFileName name of file
 * @param1[in]	: age modification age in seconds
 * @return		: true if file exists and modifies 'age' seconds ago.
 */
bool Utils::isFileExistsAndOlderThen(const char *pFileName, long age /*= -1*/)
{
    struct stat fileStat;
    int res = stat(pFileName, &fileStat);
    if (0 != res)
        return false;

    if (-1 == age)
        return true;

    time_t currentTime = time(nullptr);
    //LOGWARN("current time of %s: %lu", pFileName, currentTime);

    time_t modifiedSecondsAgo = difftime(currentTime, fileStat.st_mtime);
    //LOGWARN("elapsed time is %lu, %s", modifiedSecondsAgo, modifiedSecondsAgo <= age ? "updated recently (doesn't exists)" : "updated long time ago (exists)");

    return modifiedSecondsAgo > age;
}

/*
void Utils::SecurityToken::getSecurityToken(std::string& token)
{
    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    if(m_sThunderSecurityChecked)
    {
        token = m_sToken;
        return;
    }

    // Thunder Security is enabled by Default.
    bool thunderSecurityRFCEnabled = true;
    RFC_ParamData_t param;
    if (getRFCConfig("Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ThunderSecurity.Enable", param))
    {
        if (param.type == WDMP_BOOLEAN && (strncasecmp(param.value,"false",5) == 0))
        {
            thunderSecurityRFCEnabled = false;
        }
    }
    std::cout << "Thunder Security RFC enabled: " << thunderSecurityRFCEnabled << std::endl;
    if(!isThunderSecurityConfigured() || !thunderSecurityRFCEnabled)
    {
        m_sThunderSecurityChecked = true;
        std::cout << "Thunder Security is not enabled. Not getting token\n";
        return;
    }
    m_sThunderSecurityChecked = true;
    unsigned char buffer[MAX_STRING_LENGTH] = {0};

    int ret = GetSecurityToken(MAX_STRING_LENGTH,buffer);
    if(ret < 0)
    {
        std::cout << "Error in getting token\n";
    }
    else
    {
        std::cout << "retrieved token successfully\n";
        token = (char*)buffer;
        m_sToken = token;
    }
}
*/
static size_t writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream)
{
    size_t realsize = size * nmemb;
    string temp(static_cast<const char*>(ptr), realsize);
    stream.append(temp);
    return realsize;
}

/*
// Thunder plugins communication
std::shared_ptr<WPEFramework::JSONRPC::LinkType<WPEFramework::Core::JSON::IElement> > Utils::getThunderControllerClient(std::string callsign)
{
    string token;
    Utils::SecurityToken::getSecurityToken(token);
    string query = "token=" + token;

    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T(SERVER_DETAILS)));
    static std::shared_ptr<WPEFramework::JSONRPC::LinkType<WPEFramework::Core::JSON::IElement> > thunderClient = make_shared<WPEFramework::JSONRPC::LinkType<WPEFramework::Core::JSON::IElement> >(callsign.c_str(), "",false,query);
    return thunderClient;
}

void Utils::activatePlugin(const char* callSign)
{
    JsonObject joParams;
    joParams.Set("callsign",callSign);
    JsonObject joResult;

    if(!isPluginActivated(callSign))
    {
        LOGINFO("Activating %s", callSign);
        uint32_t status = getThunderControllerClient()->Invoke<JsonObject, JsonObject>(2000, "activate", joParams, joResult);
        string strParams;
        string strResult;
        joParams.ToString(strParams);
        joResult.ToString(strResult);
        LOGINFO("Called method %s, with params %s, status: %d, result: %s"
                , "activate"
                , C_STR(strParams)
                , status
                , C_STR(strResult));
        if (status == Core::ERROR_NONE)
        {
            LOGINFO("%s Plugin activation status ret: %d ", callSign, status);
        }
    }
}

bool Utils::isPluginActivated(const char* callSign)
{
    string method = "status@" + string(callSign);
    Core::JSON::ArrayType<PluginHost::MetaData::Service> joResult;
    uint32_t status = getThunderControllerClient()->Get<Core::JSON::ArrayType<PluginHost::MetaData::Service> >(2000, method.c_str(),joResult);
    bool pluginActivated = false;
    if (status == Core::ERROR_NONE)
    {
        LOGINFO("Getting status for callSign %s, result: %s", callSign, joResult[0].JSONState.Data().c_str());
        pluginActivated = joResult[0].JSONState == PluginHost::IShell::ACTIVATED;
    }
    else
    {
        LOGWARN("Getting status for callSign %s, status: %d", callSign, status);
    }

    if(!pluginActivated){
        LOGWARN("Plugin %s is not active", callSign);
    } else {
        LOGINFO("Plugin %s is active ", callSign);
    }
    return pluginActivated;
}
*/

//Thread RAII
Utils::ThreadRAII::ThreadRAII(std::thread&& t): t(std::move(t)) {}

Utils::ThreadRAII::~ThreadRAII()
{
    try
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    catch(const std::system_error& e)
    {
        LOGERR("system_error exception in thread join %s", e.what());
    }
    catch(const std::exception& e)
    {
        LOGERR("exception in thread join %s", e.what());
    }
}

bool Utils::isValidInt(char* x)
{
    bool Checked = true;
    int i = 0;

    if(x[0] == '-') {
        i = 1;
    }

    do
    {
        //valid digit?
        if (isdigit(x[i]))
        {
            //to the next character
            i++;
            Checked = true;
        }
        else
        {
            //to the next character
            i++;
            Checked = false;
            break;
        }
    } while (x[i] != '\0');
    return Checked;
}

bool Utils::isValidUnsignedInt(char* x)
{
    bool Checked = true;
    int i = 0;

    do
    {
        //valid digit?
        if (isdigit(x[i]))
        {
            //to the next character
            i++;
            Checked = true;
        }
        else
        {
            //to the next character
            i++;
            Checked = false;
            break;
        }
    } while (x[i] != '\0');
    return Checked;
}

void Utils::syncPersistFile (const string file) {
    FILE * fp = NULL;
    fp = fopen(file.c_str(), "r");
    if (fp == NULL) {
        printf("fopen NULL\n");
        return;
    }
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
}

void Utils::persistJsonSettings(const string strFile, const string strKey, const JsonValue& jsValue)
{
    Core::File file;
    file = strFile.c_str();

    file.Open(false);
    if (!file.IsOpen())
        file.Create();

    JsonObject cecSetting;
    cecSetting.IElement::FromFile(file);
    file.Destroy();
    file.Create();
    cecSetting[strKey.c_str()] = jsValue;
    cecSetting.IElement::ToFile(file);

    file.Close();

    //Sync the settings
    Utils::syncPersistFile (strFile);

    return;
}

