#pragma once
#include <bluefruit.h>
#include <ArduinoJson.h>

class Bluetooth
{
public:
    void init();
    void updateBatPercent(int batPercent);
    String readMessage();

private:
    // BLE Service
    BLEDfu bledfu_;   // OTA DFU service
    BLEDis bledis_;   // device information
    BLEUart bleuart_; // uart over ble
    BLEBas blebas_;   // battery

    void startAdvertising();
    static void onConnect(uint16_t conn_handle);
    static void onDisconnect(uint16_t conn_handle, uint8_t reason);
    static Bluetooth *instance_;
};

void Bluetooth::init()
{
    Bluefruit.autoConnLed(true);
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
    Bluefruit.begin();
    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.setName("PlantMan7");

    Bluefruit.Periph.setConnectCallback(Bluetooth::onConnect);
    Bluefruit.Periph.setDisconnectCallback(Bluetooth::onDisconnect);

    bledfu_.begin();
    bledis_.setManufacturer("Adafruit Industries");
    bledis_.setModel("Bluefruit Feather52");
    bledis_.begin();

    bleuart_.begin();
    bleuart_.setTimeout(10);

    blebas_.begin();
    blebas_.write(100);

    startAdvertising();
    instance_ = this;
}

void Bluetooth::updateBatPercent(int batPercent)
{
    blebas_.write(batPercent);
}

String Bluetooth::readMessage()
{
    if (!bleuart_.available())
    {
        return {};
    }
    return bleuart_.readStringUntil('\n');
}

void Bluetooth::startAdvertising()
{
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addService(bleuart_);
    Bluefruit.ScanResponse.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void Bluetooth::onConnect(uint16_t conn_handle)
{
    BLEConnection *connection = Bluefruit.Connection(conn_handle);

    char central_name[32] = {0};
    connection->getPeerName(central_name, sizeof(central_name));

    Serial.print("Connected to ");
    Serial.println(central_name);
}

void Bluetooth::onDisconnect(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.println();
    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

Bluetooth *Bluetooth::instance_ = nullptr;