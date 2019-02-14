// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

#include "../inc/globals.h"
#include "../inc/utility.h"

#include "SystemWiFi.h"
#include "NTPClient.h"

unsigned char h2int(char c) {
    if (c >= '0' && c <='9') {
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f') {
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F') {
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

unsigned urldecode(const char * url, unsigned length, StringBuffer * outURL) {
    assert(outURL != NULL);
    unsigned resultLength = 0;

    if (length == 0) {
        return resultLength;
    }

    outURL->alloc(length + 1);
    for (unsigned i = 0; i < length; i++) {
        char c = *(url + i);
        if (c == '+') {
            outURL->set(resultLength++, ' ');
        } else {
            if (c == '%') {
                const char code0 = *(url + (++i));
                const char code1 = *(url + (++i));
                c = (h2int(code0) << 4) | h2int(code1);
            }
            outURL->set(resultLength++, c);
        }
    }
    outURL->set(resultLength, 0);
    outURL->setLength(resultLength);

    return resultLength;
}

bool SyncTimeToNTP() {
    static const char* ntpHost[] =
    {
        "pool.ntp.org",
        "cn.pool.ntp.org",
        "europe.pool.ntp.org",
        "asia.pool.ntp.org",
        "oceania.pool.ntp.org"
    };

    for (int i = 0; i < sizeof(ntpHost) / sizeof(ntpHost[0]); i++) {
        NTPClient ntp(WiFiInterface());
        NTPResult res = ntp.setTime((char*)ntpHost[i]);
        if (res == NTP_OK) {
            time_t t = time(NULL);
            LOG_VERBOSE("Time from %s, now is (UTC): %s", ntpHost[i], ctime(&t));
            return true;
        }
    }

    return false;
}