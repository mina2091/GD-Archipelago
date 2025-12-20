#pragma once
#ifndef GD_ARCHIPELAGO_APCONNECTION_H
#define GD_ARCHIPELAGO_APCONNECTION_H


namespace APConnection {

    void clearItemCallback();
    void itemReceivedCallback(int64_t id, bool notify);
    void locationCheckedCallback(int64_t id);

};


#endif //GD_ARCHIPELAGO_APCONNECTION_H