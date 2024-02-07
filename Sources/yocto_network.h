/*********************************************************************
 *
 *  $Id: yocto_network.h 54332 2023-05-02 08:35:37Z seb $
 *
 *  Declares yFindNetwork(), the high-level API for Network functions
 *
 *  - - - - - - - - - License information: - - - - - - - - -
 *
 *  Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 *  Yoctopuce Sarl (hereafter Licensor) grants to you a perpetual
 *  non-exclusive license to use, modify, copy and integrate this
 *  file into your software for the sole purpose of interfacing
 *  with Yoctopuce products.
 *
 *  You may reproduce and distribute copies of this file in
 *  source or object form, as long as the sole purpose of this
 *  code is to interface with Yoctopuce products. You must retain
 *  this notice in the distributed source file.
 *
 *  You should refer to Yoctopuce General Terms and Conditions
 *  for additional information regarding your rights and
 *  obligations.
 *
 *  THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
 *  WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 *  WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *  EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 *  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
 *  SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
 *  LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *  CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *  BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *  WARRANTY, OR OTHERWISE.
 *
 *********************************************************************/


#ifndef YOCTO_NETWORK_H
#define YOCTO_NETWORK_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YNetwork return codes)
//--- (end of YNetwork return codes)
//--- (YNetwork yapiwrapper)
//--- (end of YNetwork yapiwrapper)
//--- (YNetwork definitions)
class YNetwork; // forward declaration

typedef void (*YNetworkValueCallback)(YNetwork *func, const string& functionValue);
#ifndef _Y_READINESS_ENUM
#define _Y_READINESS_ENUM
typedef enum {
    Y_READINESS_DOWN = 0,
    Y_READINESS_EXISTS = 1,
    Y_READINESS_LINKED = 2,
    Y_READINESS_LAN_OK = 3,
    Y_READINESS_WWW_OK = 4,
    Y_READINESS_INVALID = -1,
} Y_READINESS_enum;
#endif
#ifndef _Y_DISCOVERABLE_ENUM
#define _Y_DISCOVERABLE_ENUM
typedef enum {
    Y_DISCOVERABLE_FALSE = 0,
    Y_DISCOVERABLE_TRUE = 1,
    Y_DISCOVERABLE_INVALID = -1,
} Y_DISCOVERABLE_enum;
#endif
#ifndef _Y_CALLBACKMETHOD_ENUM
#define _Y_CALLBACKMETHOD_ENUM
typedef enum {
    Y_CALLBACKMETHOD_POST = 0,
    Y_CALLBACKMETHOD_GET = 1,
    Y_CALLBACKMETHOD_PUT = 2,
    Y_CALLBACKMETHOD_INVALID = -1,
} Y_CALLBACKMETHOD_enum;
#endif
#ifndef _Y_CALLBACKENCODING_ENUM
#define _Y_CALLBACKENCODING_ENUM
typedef enum {
    Y_CALLBACKENCODING_FORM = 0,
    Y_CALLBACKENCODING_JSON = 1,
    Y_CALLBACKENCODING_JSON_ARRAY = 2,
    Y_CALLBACKENCODING_CSV = 3,
    Y_CALLBACKENCODING_YOCTO_API = 4,
    Y_CALLBACKENCODING_JSON_NUM = 5,
    Y_CALLBACKENCODING_EMONCMS = 6,
    Y_CALLBACKENCODING_AZURE = 7,
    Y_CALLBACKENCODING_INFLUXDB = 8,
    Y_CALLBACKENCODING_MQTT = 9,
    Y_CALLBACKENCODING_YOCTO_API_JZON = 10,
    Y_CALLBACKENCODING_PRTG = 11,
    Y_CALLBACKENCODING_INFLUXDB_V2 = 12,
    Y_CALLBACKENCODING_INVALID = -1,
} Y_CALLBACKENCODING_enum;
#endif
#ifndef _Y_CALLBACKTEMPLATE_ENUM
#define _Y_CALLBACKTEMPLATE_ENUM
typedef enum {
    Y_CALLBACKTEMPLATE_OFF = 0,
    Y_CALLBACKTEMPLATE_ON = 1,
    Y_CALLBACKTEMPLATE_INVALID = -1,
} Y_CALLBACKTEMPLATE_enum;
#endif
#define Y_MACADDRESS_INVALID            (YAPI_INVALID_STRING)
#define Y_IPADDRESS_INVALID             (YAPI_INVALID_STRING)
#define Y_SUBNETMASK_INVALID            (YAPI_INVALID_STRING)
#define Y_ROUTER_INVALID                (YAPI_INVALID_STRING)
#define Y_CURRENTDNS_INVALID            (YAPI_INVALID_STRING)
#define Y_IPCONFIG_INVALID              (YAPI_INVALID_STRING)
#define Y_PRIMARYDNS_INVALID            (YAPI_INVALID_STRING)
#define Y_SECONDARYDNS_INVALID          (YAPI_INVALID_STRING)
#define Y_NTPSERVER_INVALID             (YAPI_INVALID_STRING)
#define Y_USERPASSWORD_INVALID          (YAPI_INVALID_STRING)
#define Y_ADMINPASSWORD_INVALID         (YAPI_INVALID_STRING)
#define Y_HTTPPORT_INVALID              (YAPI_INVALID_UINT)
#define Y_HTTPSPORT_INVALID             (YAPI_INVALID_UINT)
#define Y_DEFAULTPAGE_INVALID           (YAPI_INVALID_STRING)
#define Y_WWWWATCHDOGDELAY_INVALID      (YAPI_INVALID_UINT)
#define Y_CALLBACKURL_INVALID           (YAPI_INVALID_STRING)
#define Y_CALLBACKCREDENTIALS_INVALID   (YAPI_INVALID_STRING)
#define Y_CALLBACKINITIALDELAY_INVALID  (YAPI_INVALID_UINT)
#define Y_CALLBACKSCHEDULE_INVALID      (YAPI_INVALID_STRING)
#define Y_CALLBACKMINDELAY_INVALID      (YAPI_INVALID_UINT)
#define Y_CALLBACKMAXDELAY_INVALID      (YAPI_INVALID_UINT)
#define Y_POECURRENT_INVALID            (YAPI_INVALID_UINT)
//--- (end of YNetwork definitions)

//--- (YNetwork declaration)
/**
 * YNetwork Class: network interface control interface, available for instance in the
 * YoctoHub-Ethernet, the YoctoHub-GSM-4G, the YoctoHub-Wireless-SR or the YoctoHub-Wireless-n
 *
 * YNetwork objects provide access to TCP/IP parameters of Yoctopuce
 * devices that include a built-in network interface.
 */
class YOCTO_CLASS_EXPORT YNetwork: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YNetwork declaration)
protected:
    //--- (YNetwork attributes)
    // Attributes (function value cache)
    Y_READINESS_enum _readiness;
    string          _macAddress;
    string          _ipAddress;
    string          _subnetMask;
    string          _router;
    string          _currentDNS;
    string          _ipConfig;
    string          _primaryDNS;
    string          _secondaryDNS;
    string          _ntpServer;
    string          _userPassword;
    string          _adminPassword;
    int             _httpPort;
    int             _httpsPort;
    string          _defaultPage;
    Y_DISCOVERABLE_enum _discoverable;
    int             _wwwWatchdogDelay;
    string          _callbackUrl;
    Y_CALLBACKMETHOD_enum _callbackMethod;
    Y_CALLBACKENCODING_enum _callbackEncoding;
    Y_CALLBACKTEMPLATE_enum _callbackTemplate;
    string          _callbackCredentials;
    int             _callbackInitialDelay;
    string          _callbackSchedule;
    int             _callbackMinDelay;
    int             _callbackMaxDelay;
    int             _poeCurrent;
    YNetworkValueCallback _valueCallbackNetwork;

    friend YNetwork *yFindNetwork(const string& func);
    friend YNetwork *yFirstNetwork(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindNetwork factory function to instantiate
    YNetwork(const string& func);
    //--- (end of YNetwork attributes)

public:
    virtual ~YNetwork();
    //--- (YNetwork accessors declaration)

    static const Y_READINESS_enum READINESS_DOWN = Y_READINESS_DOWN;
    static const Y_READINESS_enum READINESS_EXISTS = Y_READINESS_EXISTS;
    static const Y_READINESS_enum READINESS_LINKED = Y_READINESS_LINKED;
    static const Y_READINESS_enum READINESS_LAN_OK = Y_READINESS_LAN_OK;
    static const Y_READINESS_enum READINESS_WWW_OK = Y_READINESS_WWW_OK;
    static const Y_READINESS_enum READINESS_INVALID = Y_READINESS_INVALID;
    static const string MACADDRESS_INVALID;
    static const string IPADDRESS_INVALID;
    static const string SUBNETMASK_INVALID;
    static const string ROUTER_INVALID;
    static const string CURRENTDNS_INVALID;
    static const string IPCONFIG_INVALID;
    static const string PRIMARYDNS_INVALID;
    static const string SECONDARYDNS_INVALID;
    static const string NTPSERVER_INVALID;
    static const string USERPASSWORD_INVALID;
    static const string ADMINPASSWORD_INVALID;
    static const int HTTPPORT_INVALID = YAPI_INVALID_UINT;
    static const int HTTPSPORT_INVALID = YAPI_INVALID_UINT;
    static const string DEFAULTPAGE_INVALID;
    static const Y_DISCOVERABLE_enum DISCOVERABLE_FALSE = Y_DISCOVERABLE_FALSE;
    static const Y_DISCOVERABLE_enum DISCOVERABLE_TRUE = Y_DISCOVERABLE_TRUE;
    static const Y_DISCOVERABLE_enum DISCOVERABLE_INVALID = Y_DISCOVERABLE_INVALID;
    static const int WWWWATCHDOGDELAY_INVALID = YAPI_INVALID_UINT;
    static const string CALLBACKURL_INVALID;
    static const Y_CALLBACKMETHOD_enum CALLBACKMETHOD_POST = Y_CALLBACKMETHOD_POST;
    static const Y_CALLBACKMETHOD_enum CALLBACKMETHOD_GET = Y_CALLBACKMETHOD_GET;
    static const Y_CALLBACKMETHOD_enum CALLBACKMETHOD_PUT = Y_CALLBACKMETHOD_PUT;
    static const Y_CALLBACKMETHOD_enum CALLBACKMETHOD_INVALID = Y_CALLBACKMETHOD_INVALID;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_FORM = Y_CALLBACKENCODING_FORM;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_JSON = Y_CALLBACKENCODING_JSON;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_JSON_ARRAY = Y_CALLBACKENCODING_JSON_ARRAY;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_CSV = Y_CALLBACKENCODING_CSV;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_YOCTO_API = Y_CALLBACKENCODING_YOCTO_API;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_JSON_NUM = Y_CALLBACKENCODING_JSON_NUM;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_EMONCMS = Y_CALLBACKENCODING_EMONCMS;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_AZURE = Y_CALLBACKENCODING_AZURE;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_INFLUXDB = Y_CALLBACKENCODING_INFLUXDB;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_MQTT = Y_CALLBACKENCODING_MQTT;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_YOCTO_API_JZON = Y_CALLBACKENCODING_YOCTO_API_JZON;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_PRTG = Y_CALLBACKENCODING_PRTG;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_INFLUXDB_V2 = Y_CALLBACKENCODING_INFLUXDB_V2;
    static const Y_CALLBACKENCODING_enum CALLBACKENCODING_INVALID = Y_CALLBACKENCODING_INVALID;
    static const Y_CALLBACKTEMPLATE_enum CALLBACKTEMPLATE_OFF = Y_CALLBACKTEMPLATE_OFF;
    static const Y_CALLBACKTEMPLATE_enum CALLBACKTEMPLATE_ON = Y_CALLBACKTEMPLATE_ON;
    static const Y_CALLBACKTEMPLATE_enum CALLBACKTEMPLATE_INVALID = Y_CALLBACKTEMPLATE_INVALID;
    static const string CALLBACKCREDENTIALS_INVALID;
    static const int CALLBACKINITIALDELAY_INVALID = YAPI_INVALID_UINT;
    static const string CALLBACKSCHEDULE_INVALID;
    static const int CALLBACKMINDELAY_INVALID = YAPI_INVALID_UINT;
    static const int CALLBACKMAXDELAY_INVALID = YAPI_INVALID_UINT;
    static const int POECURRENT_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the current established working mode of the network interface.
     * Level zero (DOWN_0) means that no hardware link has been detected. Either there is no signal
     * on the network cable, or the selected wireless access point cannot be detected.
     * Level 1 (LIVE_1) is reached when the network is detected, but is not yet connected.
     * For a wireless network, this shows that the requested SSID is present.
     * Level 2 (LINK_2) is reached when the hardware connection is established.
     * For a wired network connection, level 2 means that the cable is attached at both ends.
     * For a connection to a wireless access point, it shows that the security parameters
     * are properly configured. For an ad-hoc wireless connection, it means that there is
     * at least one other device connected on the ad-hoc network.
     * Level 3 (DHCP_3) is reached when an IP address has been obtained using DHCP.
     * Level 4 (DNS_4) is reached when the DNS server is reachable on the network.
     * Level 5 (WWW_5) is reached when global connectivity is demonstrated by properly loading the
     * current time from an NTP server.
     *
     * @return a value among YNetwork::READINESS_DOWN, YNetwork::READINESS_EXISTS,
     * YNetwork::READINESS_LINKED, YNetwork::READINESS_LAN_OK and YNetwork::READINESS_WWW_OK corresponding to
     * the current established working mode of the network interface
     *
     * On failure, throws an exception or returns YNetwork::READINESS_INVALID.
     */
    Y_READINESS_enum    get_readiness(void);

    inline Y_READINESS_enum readiness(void)
    { return this->get_readiness(); }

    /**
     * Returns the MAC address of the network interface. The MAC address is also available on a sticker
     * on the module, in both numeric and barcode forms.
     *
     * @return a string corresponding to the MAC address of the network interface
     *
     * On failure, throws an exception or returns YNetwork::MACADDRESS_INVALID.
     */
    string              get_macAddress(void);

    inline string       macAddress(void)
    { return this->get_macAddress(); }

    /**
     * Returns the IP address currently in use by the device. The address may have been configured
     * statically, or provided by a DHCP server.
     *
     * @return a string corresponding to the IP address currently in use by the device
     *
     * On failure, throws an exception or returns YNetwork::IPADDRESS_INVALID.
     */
    string              get_ipAddress(void);

    inline string       ipAddress(void)
    { return this->get_ipAddress(); }

    /**
     * Returns the subnet mask currently used by the device.
     *
     * @return a string corresponding to the subnet mask currently used by the device
     *
     * On failure, throws an exception or returns YNetwork::SUBNETMASK_INVALID.
     */
    string              get_subnetMask(void);

    inline string       subnetMask(void)
    { return this->get_subnetMask(); }

    /**
     * Returns the IP address of the router on the device subnet (default gateway).
     *
     * @return a string corresponding to the IP address of the router on the device subnet (default gateway)
     *
     * On failure, throws an exception or returns YNetwork::ROUTER_INVALID.
     */
    string              get_router(void);

    inline string       router(void)
    { return this->get_router(); }

    /**
     * Returns the IP address of the DNS server currently used by the device.
     *
     * @return a string corresponding to the IP address of the DNS server currently used by the device
     *
     * On failure, throws an exception or returns YNetwork::CURRENTDNS_INVALID.
     */
    string              get_currentDNS(void);

    inline string       currentDNS(void)
    { return this->get_currentDNS(); }

    /**
     * Returns the IP configuration of the network interface.
     *
     * If the network interface is setup to use a static IP address, the string starts with "STATIC:" and
     * is followed by three
     * parameters, separated by "/". The first is the device IP address, followed by the subnet mask
     * length, and finally the
     * router IP address (default gateway). For instance: "STATIC:192.168.1.14/16/192.168.1.1"
     *
     * If the network interface is configured to receive its IP from a DHCP server, the string start with
     * "DHCP:" and is followed by
     * three parameters separated by "/". The first is the fallback IP address, then the fallback subnet
     * mask length and finally the
     * fallback router IP address. These three parameters are used when no DHCP reply is received.
     *
     * @return a string corresponding to the IP configuration of the network interface
     *
     * On failure, throws an exception or returns YNetwork::IPCONFIG_INVALID.
     */
    string              get_ipConfig(void);

    inline string       ipConfig(void)
    { return this->get_ipConfig(); }

    int             set_ipConfig(const string& newval);
    inline int      setIpConfig(const string& newval)
    { return this->set_ipConfig(newval); }

    /**
     * Returns the IP address of the primary name server to be used by the module.
     *
     * @return a string corresponding to the IP address of the primary name server to be used by the module
     *
     * On failure, throws an exception or returns YNetwork::PRIMARYDNS_INVALID.
     */
    string              get_primaryDNS(void);

    inline string       primaryDNS(void)
    { return this->get_primaryDNS(); }

    /**
     * Changes the IP address of the primary name server to be used by the module.
     * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param newval : a string corresponding to the IP address of the primary name server to be used by the module
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_primaryDNS(const string& newval);
    inline int      setPrimaryDNS(const string& newval)
    { return this->set_primaryDNS(newval); }

    /**
     * Returns the IP address of the secondary name server to be used by the module.
     *
     * @return a string corresponding to the IP address of the secondary name server to be used by the module
     *
     * On failure, throws an exception or returns YNetwork::SECONDARYDNS_INVALID.
     */
    string              get_secondaryDNS(void);

    inline string       secondaryDNS(void)
    { return this->get_secondaryDNS(); }

    /**
     * Changes the IP address of the secondary name server to be used by the module.
     * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param newval : a string corresponding to the IP address of the secondary name server to be used by the module
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_secondaryDNS(const string& newval);
    inline int      setSecondaryDNS(const string& newval)
    { return this->set_secondaryDNS(newval); }

    /**
     * Returns the IP address of the NTP server to be used by the device.
     *
     * @return a string corresponding to the IP address of the NTP server to be used by the device
     *
     * On failure, throws an exception or returns YNetwork::NTPSERVER_INVALID.
     */
    string              get_ntpServer(void);

    inline string       ntpServer(void)
    { return this->get_ntpServer(); }

    /**
     * Changes the IP address of the NTP server to be used by the module. Use an empty
     * string to restore the factory set  address.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param newval : a string corresponding to the IP address of the NTP server to be used by the module
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_ntpServer(const string& newval);
    inline int      setNtpServer(const string& newval)
    { return this->set_ntpServer(newval); }

    /**
     * Returns a hash string if a password has been set for "user" user,
     * or an empty string otherwise.
     *
     * @return a string corresponding to a hash string if a password has been set for "user" user,
     *         or an empty string otherwise
     *
     * On failure, throws an exception or returns YNetwork::USERPASSWORD_INVALID.
     */
    string              get_userPassword(void);

    inline string       userPassword(void)
    { return this->get_userPassword(); }

    /**
     * Changes the password for the "user" user. This password becomes instantly required
     * to perform any use of the module. If the specified value is an
     * empty string, a password is not required anymore.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the password for the "user" user
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_userPassword(const string& newval);
    inline int      setUserPassword(const string& newval)
    { return this->set_userPassword(newval); }

    /**
     * Returns a hash string if a password has been set for user "admin",
     * or an empty string otherwise.
     *
     * @return a string corresponding to a hash string if a password has been set for user "admin",
     *         or an empty string otherwise
     *
     * On failure, throws an exception or returns YNetwork::ADMINPASSWORD_INVALID.
     */
    string              get_adminPassword(void);

    inline string       adminPassword(void)
    { return this->get_adminPassword(); }

    /**
     * Changes the password for the "admin" user. This password becomes instantly required
     * to perform any change of the module state. If the specified value is an
     * empty string, a password is not required anymore.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the password for the "admin" user
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_adminPassword(const string& newval);
    inline int      setAdminPassword(const string& newval)
    { return this->set_adminPassword(newval); }

    /**
     * Returns the TCP port used to serve the hub web UI.
     *
     * @return an integer corresponding to the TCP port used to serve the hub web UI
     *
     * On failure, throws an exception or returns YNetwork::HTTPPORT_INVALID.
     */
    int                 get_httpPort(void);

    inline int          httpPort(void)
    { return this->get_httpPort(); }

    /**
     * Changes the the TCP port used to serve the hub web UI. The default value is port 80,
     * which is the default for all Web servers. Regardless of the value set here,
     * the hub will always reply on port 4444, which is used by default by Yoctopuce
     * API library. When you change this parameter, remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the the TCP port used to serve the hub web UI
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_httpPort(int newval);
    inline int      setHttpPort(int newval)
    { return this->set_httpPort(newval); }

    /**
     * Returns the secure TCP port used to serve the hub web UI.
     *
     * @return an integer corresponding to the secure TCP port used to serve the hub web UI
     *
     * On failure, throws an exception or returns YNetwork::HTTPSPORT_INVALID.
     */
    int                 get_httpsPort(void);

    inline int          httpsPort(void)
    { return this->get_httpsPort(); }

    /**
     * Changes the secure TCP port used to serve the hub web UI. The default value is port 4443,
     * which is the default for all Web servers. When you change this parameter, remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the secure TCP port used to serve the hub web UI
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_httpsPort(int newval);
    inline int      setHttpsPort(int newval)
    { return this->set_httpsPort(newval); }

    /**
     * Returns the HTML page to serve for the URL "/"" of the hub.
     *
     * @return a string corresponding to the HTML page to serve for the URL "/"" of the hub
     *
     * On failure, throws an exception or returns YNetwork::DEFAULTPAGE_INVALID.
     */
    string              get_defaultPage(void);

    inline string       defaultPage(void)
    { return this->get_defaultPage(); }

    /**
     * Changes the default HTML page returned by the hub. If not value are set the hub return
     * "index.html" which is the web interface of the hub. It is possible to change this page
     * for file that has been uploaded on the hub. The maximum filename size is 15 characters.
     * When you change this parameter, remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : a string corresponding to the default HTML page returned by the hub
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_defaultPage(const string& newval);
    inline int      setDefaultPage(const string& newval)
    { return this->set_defaultPage(newval); }

    /**
     * Returns the activation state of the multicast announce protocols to allow easy
     * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
     *
     * @return either YNetwork::DISCOVERABLE_FALSE or YNetwork::DISCOVERABLE_TRUE, according to the
     * activation state of the multicast announce protocols to allow easy
     *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
     *
     * On failure, throws an exception or returns YNetwork::DISCOVERABLE_INVALID.
     */
    Y_DISCOVERABLE_enum get_discoverable(void);

    inline Y_DISCOVERABLE_enum discoverable(void)
    { return this->get_discoverable(); }

    /**
     * Changes the activation state of the multicast announce protocols to allow easy
     * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : either YNetwork::DISCOVERABLE_FALSE or YNetwork::DISCOVERABLE_TRUE, according to the
     * activation state of the multicast announce protocols to allow easy
     *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_discoverable(Y_DISCOVERABLE_enum newval);
    inline int      setDiscoverable(Y_DISCOVERABLE_enum newval)
    { return this->set_discoverable(newval); }

    /**
     * Returns the allowed downtime of the WWW link (in seconds) before triggering an automated
     * reboot to try to recover Internet connectivity. A zero value disables automated reboot
     * in case of Internet connectivity loss.
     *
     * @return an integer corresponding to the allowed downtime of the WWW link (in seconds) before
     * triggering an automated
     *         reboot to try to recover Internet connectivity
     *
     * On failure, throws an exception or returns YNetwork::WWWWATCHDOGDELAY_INVALID.
     */
    int                 get_wwwWatchdogDelay(void);

    inline int          wwwWatchdogDelay(void)
    { return this->get_wwwWatchdogDelay(); }

    /**
     * Changes the allowed downtime of the WWW link (in seconds) before triggering an automated
     * reboot to try to recover Internet connectivity. A zero value disables automated reboot
     * in case of Internet connectivity loss. The smallest valid non-zero timeout is
     * 90 seconds. Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the allowed downtime of the WWW link (in seconds)
     * before triggering an automated
     *         reboot to try to recover Internet connectivity
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_wwwWatchdogDelay(int newval);
    inline int      setWwwWatchdogDelay(int newval)
    { return this->set_wwwWatchdogDelay(newval); }

    /**
     * Returns the callback URL to notify of significant state changes.
     *
     * @return a string corresponding to the callback URL to notify of significant state changes
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKURL_INVALID.
     */
    string              get_callbackUrl(void);

    inline string       callbackUrl(void)
    { return this->get_callbackUrl(); }

    /**
     * Changes the callback URL to notify significant state changes. Remember to call the
     * saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : a string corresponding to the callback URL to notify significant state changes
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackUrl(const string& newval);
    inline int      setCallbackUrl(const string& newval)
    { return this->set_callbackUrl(newval); }

    /**
     * Returns the HTTP method used to notify callbacks for significant state changes.
     *
     * @return a value among YNetwork::CALLBACKMETHOD_POST, YNetwork::CALLBACKMETHOD_GET and
     * YNetwork::CALLBACKMETHOD_PUT corresponding to the HTTP method used to notify callbacks for
     * significant state changes
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKMETHOD_INVALID.
     */
    Y_CALLBACKMETHOD_enum get_callbackMethod(void);

    inline Y_CALLBACKMETHOD_enum callbackMethod(void)
    { return this->get_callbackMethod(); }

    /**
     * Changes the HTTP method used to notify callbacks for significant state changes.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among YNetwork::CALLBACKMETHOD_POST, YNetwork::CALLBACKMETHOD_GET and
     * YNetwork::CALLBACKMETHOD_PUT corresponding to the HTTP method used to notify callbacks for
     * significant state changes
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackMethod(Y_CALLBACKMETHOD_enum newval);
    inline int      setCallbackMethod(Y_CALLBACKMETHOD_enum newval)
    { return this->set_callbackMethod(newval); }

    /**
     * Returns the encoding standard to use for representing notification values.
     *
     * @return a value among YNetwork::CALLBACKENCODING_FORM, YNetwork::CALLBACKENCODING_JSON,
     * YNetwork::CALLBACKENCODING_JSON_ARRAY, YNetwork::CALLBACKENCODING_CSV,
     * YNetwork::CALLBACKENCODING_YOCTO_API, YNetwork::CALLBACKENCODING_JSON_NUM,
     * YNetwork::CALLBACKENCODING_EMONCMS, YNetwork::CALLBACKENCODING_AZURE,
     * YNetwork::CALLBACKENCODING_INFLUXDB, YNetwork::CALLBACKENCODING_MQTT,
     * YNetwork::CALLBACKENCODING_YOCTO_API_JZON, YNetwork::CALLBACKENCODING_PRTG and
     * YNetwork::CALLBACKENCODING_INFLUXDB_V2 corresponding to the encoding standard to use for
     * representing notification values
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKENCODING_INVALID.
     */
    Y_CALLBACKENCODING_enum get_callbackEncoding(void);

    inline Y_CALLBACKENCODING_enum callbackEncoding(void)
    { return this->get_callbackEncoding(); }

    /**
     * Changes the encoding standard to use for representing notification values.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among YNetwork::CALLBACKENCODING_FORM, YNetwork::CALLBACKENCODING_JSON,
     * YNetwork::CALLBACKENCODING_JSON_ARRAY, YNetwork::CALLBACKENCODING_CSV,
     * YNetwork::CALLBACKENCODING_YOCTO_API, YNetwork::CALLBACKENCODING_JSON_NUM,
     * YNetwork::CALLBACKENCODING_EMONCMS, YNetwork::CALLBACKENCODING_AZURE,
     * YNetwork::CALLBACKENCODING_INFLUXDB, YNetwork::CALLBACKENCODING_MQTT,
     * YNetwork::CALLBACKENCODING_YOCTO_API_JZON, YNetwork::CALLBACKENCODING_PRTG and
     * YNetwork::CALLBACKENCODING_INFLUXDB_V2 corresponding to the encoding standard to use for
     * representing notification values
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackEncoding(Y_CALLBACKENCODING_enum newval);
    inline int      setCallbackEncoding(Y_CALLBACKENCODING_enum newval)
    { return this->set_callbackEncoding(newval); }

    /**
     * Returns the activation state of the custom template file to customize callback
     * format. If the custom callback template is disabled, it will be ignored even
     * if present on the YoctoHub.
     *
     * @return either YNetwork::CALLBACKTEMPLATE_OFF or YNetwork::CALLBACKTEMPLATE_ON, according to the
     * activation state of the custom template file to customize callback
     *         format
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKTEMPLATE_INVALID.
     */
    Y_CALLBACKTEMPLATE_enum get_callbackTemplate(void);

    inline Y_CALLBACKTEMPLATE_enum callbackTemplate(void)
    { return this->get_callbackTemplate(); }

    /**
     * Enable the use of a template file to customize callbacks format.
     * When the custom callback template file is enabled, the template file
     * will be loaded for each callback in order to build the data to post to the
     * server. If template file does not exist on the YoctoHub, the callback will
     * fail with an error message indicating the name of the expected template file.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : either YNetwork::CALLBACKTEMPLATE_OFF or YNetwork::CALLBACKTEMPLATE_ON
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackTemplate(Y_CALLBACKTEMPLATE_enum newval);
    inline int      setCallbackTemplate(Y_CALLBACKTEMPLATE_enum newval)
    { return this->set_callbackTemplate(newval); }

    /**
     * Returns a hashed version of the notification callback credentials if set,
     * or an empty string otherwise.
     *
     * @return a string corresponding to a hashed version of the notification callback credentials if set,
     *         or an empty string otherwise
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKCREDENTIALS_INVALID.
     */
    string              get_callbackCredentials(void);

    inline string       callbackCredentials(void)
    { return this->get_callbackCredentials(); }

    /**
     * Changes the credentials required to connect to the callback address. The credentials
     * must be provided as returned by function get_callbackCredentials,
     * in the form username:hash. The method used to compute the hash varies according
     * to the the authentication scheme implemented by the callback, For Basic authentication,
     * the hash is the MD5 of the string username:password. For Digest authentication,
     * the hash is the MD5 of the string username:realm:password. For a simpler
     * way to configure callback credentials, use function callbackLogin instead.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the credentials required to connect to the callback address
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackCredentials(const string& newval);
    inline int      setCallbackCredentials(const string& newval)
    { return this->set_callbackCredentials(newval); }

    /**
     * Connects to the notification callback and saves the credentials required to
     * log into it. The password is not stored into the module, only a hashed
     * copy of the credentials are saved. Remember to call the
     * saveToFlash() method of the module if the modification must be kept.
     *
     * @param username : username required to log to the callback
     * @param password : password required to log to the callback
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             callbackLogin(string username,string password);

    /**
     * Returns the initial waiting time before first callback notifications, in seconds.
     *
     * @return an integer corresponding to the initial waiting time before first callback notifications, in seconds
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKINITIALDELAY_INVALID.
     */
    int                 get_callbackInitialDelay(void);

    inline int          callbackInitialDelay(void)
    { return this->get_callbackInitialDelay(); }

    /**
     * Changes the initial waiting time before first callback notifications, in seconds.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the initial waiting time before first callback
     * notifications, in seconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackInitialDelay(int newval);
    inline int      setCallbackInitialDelay(int newval)
    { return this->set_callbackInitialDelay(newval); }

    /**
     * Returns the HTTP callback schedule strategy, as a text string.
     *
     * @return a string corresponding to the HTTP callback schedule strategy, as a text string
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKSCHEDULE_INVALID.
     */
    string              get_callbackSchedule(void);

    inline string       callbackSchedule(void)
    { return this->get_callbackSchedule(); }

    /**
     * Changes the HTTP callback schedule strategy, as a text string.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : a string corresponding to the HTTP callback schedule strategy, as a text string
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackSchedule(const string& newval);
    inline int      setCallbackSchedule(const string& newval)
    { return this->set_callbackSchedule(newval); }

    /**
     * Returns the minimum waiting time between two HTTP callbacks, in seconds.
     *
     * @return an integer corresponding to the minimum waiting time between two HTTP callbacks, in seconds
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKMINDELAY_INVALID.
     */
    int                 get_callbackMinDelay(void);

    inline int          callbackMinDelay(void)
    { return this->get_callbackMinDelay(); }

    /**
     * Changes the minimum waiting time between two HTTP callbacks, in seconds.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the minimum waiting time between two HTTP callbacks, in seconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackMinDelay(int newval);
    inline int      setCallbackMinDelay(int newval)
    { return this->set_callbackMinDelay(newval); }

    /**
     * Returns the waiting time between two HTTP callbacks when there is nothing new.
     *
     * @return an integer corresponding to the waiting time between two HTTP callbacks when there is nothing new
     *
     * On failure, throws an exception or returns YNetwork::CALLBACKMAXDELAY_INVALID.
     */
    int                 get_callbackMaxDelay(void);

    inline int          callbackMaxDelay(void)
    { return this->get_callbackMaxDelay(); }

    /**
     * Changes the waiting time between two HTTP callbacks when there is nothing new.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the waiting time between two HTTP callbacks when there
     * is nothing new
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_callbackMaxDelay(int newval);
    inline int      setCallbackMaxDelay(int newval)
    { return this->set_callbackMaxDelay(newval); }

    /**
     * Returns the current consumed by the module from Power-over-Ethernet (PoE), in milliamps.
     * The current consumption is measured after converting PoE source to 5 Volt, and should
     * never exceed 1800 mA.
     *
     * @return an integer corresponding to the current consumed by the module from Power-over-Ethernet
     * (PoE), in milliamps
     *
     * On failure, throws an exception or returns YNetwork::POECURRENT_INVALID.
     */
    int                 get_poeCurrent(void);

    inline int          poeCurrent(void)
    { return this->get_poeCurrent(); }

    /**
     * Retrieves a network interface for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the network interface is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the network interface is
     * indeed online at a given time. In case of ambiguity when looking for
     * a network interface by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the network interface, for instance
     *         YHUBETH1.network.
     *
     * @return a YNetwork object allowing you to drive the network interface.
     */
    static YNetwork*    FindNetwork(string func);

    /**
     * Registers the callback function that is invoked on every change of advertised value.
     * The callback is invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
     * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and the character string describing
     *         the new advertised value.
     * @noreturn
     */
    virtual int         registerValueCallback(YNetworkValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Changes the configuration of the network interface to enable the use of an
     * IP address received from a DHCP server. Until an address is received from a DHCP
     * server, the module uses the IP parameters specified to this function.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param fallbackIpAddr : fallback IP address, to be used when no DHCP reply is received
     * @param fallbackSubnetMaskLen : fallback subnet mask length when no DHCP reply is received, as an
     *         integer (e.g. 24 means 255.255.255.0)
     * @param fallbackRouter : fallback router IP address, to be used when no DHCP reply is received
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         useDHCP(string fallbackIpAddr,int fallbackSubnetMaskLen,string fallbackRouter);

    /**
     * Changes the configuration of the network interface to enable the use of an
     * IP address received from a DHCP server. Until an address is received from a DHCP
     * server, the module uses an IP of the network 169.254.0.0/16 (APIPA).
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         useDHCPauto(void);

    /**
     * Changes the configuration of the network interface to use a static IP address.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param ipAddress : device IP address
     * @param subnetMaskLen : subnet mask length, as an integer (e.g. 24 means 255.255.255.0)
     * @param router : router IP address (default gateway)
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         useStaticIP(string ipAddress,int subnetMaskLen,string router);

    /**
     * Pings host to test the network connectivity. Sends four ICMP ECHO_REQUEST requests from the
     * module to the target host. This method returns a string with the result of the
     * 4 ICMP ECHO_REQUEST requests.
     *
     * @param host : the hostname or the IP address of the target
     *
     * @return a string with the result of the ping.
     */
    virtual string      ping(string host);

    /**
     * Trigger an HTTP callback quickly. This function can even be called within
     * an HTTP callback, in which case the next callback will be triggered 5 seconds
     * after the end of the current callback, regardless if the minimum time between
     * callbacks configured in the device.
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         triggerCallback(void);

    /**
     * Setup periodic HTTP callbacks (simplified function).
     *
     * @param interval : a string representing the callback periodicity, expressed in
     *         seconds, minutes or hours, eg. "60s", "5m", "1h", "48h".
     * @param offset : an integer representing the time offset relative to the period
     *         when the callback should occur. For instance, if the periodicity is
     *         24h, an offset of 7 will make the callback occur each day at 7AM.
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_periodicCallbackSchedule(string interval,int offset);


    inline static YNetwork *Find(string func)
    { return YNetwork::FindNetwork(func); }

    /**
     * Continues the enumeration of network interfaces started using yFirstNetwork().
     * Caution: You can't make any assumption about the returned network interfaces order.
     * If you want to find a specific a network interface, use Network.findNetwork()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YNetwork object, corresponding to
     *         a network interface currently online, or a NULL pointer
     *         if there are no more network interfaces to enumerate.
     */
           YNetwork        *nextNetwork(void);
    inline YNetwork        *next(void)
    { return this->nextNetwork();}

    /**
     * Starts the enumeration of network interfaces currently accessible.
     * Use the method YNetwork::nextNetwork() to iterate on
     * next network interfaces.
     *
     * @return a pointer to a YNetwork object, corresponding to
     *         the first network interface currently online, or a NULL pointer
     *         if there are none.
     */
           static YNetwork *FirstNetwork(void);
    inline static YNetwork *First(void)
    { return YNetwork::FirstNetwork();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YNetwork accessors declaration)
};

//--- (YNetwork functions declaration)

/**
 * Retrieves a network interface for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the network interface is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the network interface is
 * indeed online at a given time. In case of ambiguity when looking for
 * a network interface by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the network interface, for instance
 *         YHUBETH1.network.
 *
 * @return a YNetwork object allowing you to drive the network interface.
 */
inline YNetwork *yFindNetwork(const string& func)
{ return YNetwork::FindNetwork(func);}
/**
 * Starts the enumeration of network interfaces currently accessible.
 * Use the method YNetwork::nextNetwork() to iterate on
 * next network interfaces.
 *
 * @return a pointer to a YNetwork object, corresponding to
 *         the first network interface currently online, or a NULL pointer
 *         if there are none.
 */
inline YNetwork *yFirstNetwork(void)
{ return YNetwork::FirstNetwork();}

//--- (end of YNetwork functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
