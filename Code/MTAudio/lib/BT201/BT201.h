#ifndef BT201_H
#define BT201_H

#include <Arduino.h>
#include <string.h>
#include <Queue.h>

#ifdef DEBUG
#define DEBUG_PRINT(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif

enum class BluetoothStatus
{
    Pairing = 0,
    Connected = 1,
    MusicPlaying = 2,
    Phone = 3,
    PhoneTalking = 4,
};

enum class AudioMode
{
    Bluetooth = 1,
    UDisk = 2,
    TFCard = 3,
};

class BT201
{
public:
    BT201()
    {
        this->commandQueue = new Queue<String>(20);
    }

    ~BT201()
    {
        delete this->commandQueue;
    }

    /// @brief Get state from device and initializes all internal variables
    void init(HardwareSerial *serial)
    {
        this->serial = serial;
        getAudioMode(true);
        getBluetoothStatus(true);
        updateOnlineDevices();
    }

    void update()
    {
        readSerialToCommandBuffer();
        proccessPendingCommands();
    }

    void sendCommand(String cmd)
    {
        this->serial->printf("%s\r\n", cmd.c_str());
    }

    bool setVolume(uint8_t volume)
    {

        volume = constrain(volume, 0, 30);
        DEBUG_PRINT("Sending Command: AT+CA%02u\r\n", volume);
        this->serial->printf("AT+CA%02u\r\n", volume);

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool increaseVolume()
    {
        DEBUG_PRINT("Sending Command: AT+CE\r\n");
        this->serial->printf("AT+CE\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool decreaseVolume()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+CF\r\n");
        this->serial->printf("AT+CF\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool togglePlayPause()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+CB\r\n");
        this->serial->printf("AT+CB\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool playNextSong()
    {
        DEBUG_PRINT("Sending Command: AT+CC\r\n");
        this->serial->printf("AT+CC\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool playPreviousSong()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+CD\r\n");
        this->serial->printf("AT+CD\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    bool setAudioMode(AudioMode mode)
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+CM%02d\r\n", (int)mode);
        this->serial->printf("AT+CM%02d\r\n", (int)mode);

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");

            // Here we need to poll for some time to make sure the mode has actually changed.
            unsigned long timeout = millis() + 300; // 300 ms timeout for waiting until mode changes.
            while (true)
            {
                if (timeout <= millis())
                {
                    DEBUG_PRINT("Did not receive QM result - Timed Out\n");
                    return false;
                }

                while (serial->available())
                {
                    String received = serial->readStringUntil('\n');

                    // Check if it is the response, else just move it to the command buffer for processing later.
                    if (received.substring(0, 2) == "QM")
                    {
                        uint8_t receivedMode = extractValueFromQueryResponse(received);
                        this->audioMode = (AudioMode)receivedMode;
                        if (this->audioMode == mode)
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        commandQueue->push(received);
                    }
                }

                delay(1);
            }

            return true;
        }
        DEBUG_PRINT("ACK is Timed Out or ERROR\n");
        return false;
    }

    // --- Getter Functions ---

    AudioMode getAudioMode(bool forceUpdate = false)
    {
        if (!forceUpdate)
        {
            return this->audioMode;
        }

        DEBUG_PRINT("Sending Command: AT+QM\r\n");
        this->serial->printf("AT+QM\r\n");

        // Wait for response
        String response = waitUntilResponseReceived(1000, "QM");
        if (response != "")
        {
            DEBUG_PRINT("Response is %s\n", response.c_str());
            uint8_t mode = extractValueFromQueryResponse(response);

            // Wait for ACK
            if (waitUntilACKReceived(500))
            {
                DEBUG_PRINT("Response is OK\n");
                this->audioMode = (AudioMode)mode;
                return this->audioMode;
            }

            DEBUG_PRINT("ACK is Timed Out or ERROR\n");
            return this->audioMode;
        }

        DEBUG_PRINT("Response Timed Out or ERROR\n");
        return this->audioMode;
    }

    BluetoothStatus getBluetoothStatus(bool forceUpdate = false)
    {
        if (!forceUpdate)
        {
            return this->bluetoothStatus;
        }

        DEBUG_PRINT("Sending Command: AT+TS\r\n");
        this->serial->printf("AT+TS\r\n");

        // Wait for response
        String response = waitUntilResponseReceived(500, "TS");
        if (response != "")
        {
            DEBUG_PRINT("Response is %s\n", response.c_str());
            uint8_t status = extractValueFromQueryResponse(response);

            // Wait for ACK
            if (waitUntilACKReceived(500))
            {
                DEBUG_PRINT("Response is OK\n");
                this->bluetoothStatus = (BluetoothStatus)status;
                return this->bluetoothStatus;
            }

            DEBUG_PRINT("ACK is Timed Out or ERROR\n");
            return this->bluetoothStatus;
        }

        DEBUG_PRINT("Response Timed Out or ERROR\n");
        return this->bluetoothStatus;
    }

    /// @brief Gets the volume from the BT201 board.
    /// @return Returns the volume.
    uint8_t getVolume()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+QA\r\n");
        this->serial->printf("AT+QA\r\n");

        // Wait for response
        String response = waitUntilResponseReceived(500, "QA");
        if (response != "")
        {
            DEBUG_PRINT("Response is %s\n", response.c_str());
            uint8_t volume = extractValueFromQueryResponse(response);

            // Wait for ACK
            if (waitUntilACKReceived(500))
            {
                DEBUG_PRINT("Response is OK\n");
                return volume;
            }
            DEBUG_PRINT("ACK is Timed Out or ERROR\n");
            return 0;
        }

        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return 0;
    }

    bool getTFCardStatus(bool forceUpdate = false)
    {
        if (forceUpdate)
        {
            this->updateOnlineDevices();
        }

        // The TF Card is the second BIT in the onlineDevices byte
        return (this->onlineDevices & 0b010) >> 1;
    }

    bool getUDiskStatus(bool forceUpdate = false)
    {
        if (forceUpdate)
        {
            this->updateOnlineDevices();
        }

        // The UDisk is the second BIT in the onlineDevices byte
        return (this->onlineDevices & 0b001);
    }

    String getCurrentFilePlaying()
    {
        return this->currentFilePlaying;
    }

    String getCallerPhoneNumber()
    {
        return this->phoneNumber;
    }

    // Phone Call Functions

    /// @brief Calls the specified phone number.
    /// @param phoneNumber The phone number to call.
    /// @return true on successful call.
    bool phoneCall(String phoneNumber)
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+BT%s\r\n", phoneNumber.c_str());
        this->serial->printf("AT+BT%s\r\n", phoneNumber.c_str());

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return false;
    }

    /// @brief Call back a phone call.
    /// @return
    bool phoneBack()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+BA00\r\n");
        this->serial->printf("AT+BA00\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return false;
    }

    /// @brief Hangs up the incoming phone call.
    /// @return true if successful.
    bool phoneHangUp()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+BA03\r\n");
        this->serial->printf("AT+BA03\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return false;
    }

    bool phoneRefuseAccept()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+BA02\r\n");
        this->serial->printf("AT+BA02\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return false;
    }

    /// @brief Picks up the incoming phone call.
    /// @return true if successful.
    bool phonePickUp()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+BA04\r\n");
        this->serial->printf("AT+BA04\r\n");

        // Wait for response
        if (waitUntilACKReceived(500))
        {
            DEBUG_PRINT("Response is OK\n");
            return true;
        }
        DEBUG_PRINT("Response is Timed Out or ERROR\n");
        return false;
    }

private:
    HardwareSerial *serial;
    AudioMode audioMode;
    BluetoothStatus bluetoothStatus;
    uint8_t onlineDevices = 0; // BIT values of online devices ([2]TF-Card,[1]- ,[0]U-Disk)

    // TF-UDisk variables
    String currentFilePlaying = "";

    // Phone call variables
    bool isReceivingPhoneCall = false;
    String phoneNumber = "";

    Queue<String> *commandQueue;

    // Helper functions

    /// @brief Updates the online devices variable.
    void updateOnlineDevices()
    {
        // Read pending serial data
        proccessPendingCommands();

        DEBUG_PRINT("Sending Command: AT+MV\r\n");
        this->serial->printf("AT+MV\r\n");

        // Wait for response
        String response = waitUntilResponseReceived(500, "MV");
        if (response != "")
        {
            DEBUG_PRINT("Response is %s\n", response.c_str());
            uint8_t onlineDevices = extractValueFromQueryResponse(response);

            // Wait for ACK
            if (waitUntilACKReceived(500))
            {
                DEBUG_PRINT("Response is OK\n");
                this->onlineDevices = onlineDevices;
                return;
            }

            DEBUG_PRINT("ACK is Timed Out or ERROR\n");
            return;
        }
        else
        {
            DEBUG_PRINT("Response Timed Out or ERROR\n");
        }
    }

    // Serial Communication helping functions

    void readSerialToCommandBuffer()
    {
        while (serial->available())
        {
            String received = serial->readStringUntil('\n');
            commandQueue->push(received);
        }
    }

    void proccessPendingCommands()
    {
        while (commandQueue->count() > 0)
        {
            parseReceivedCommand(commandQueue->pop());
        }
    }

    void parseReceivedCommand(String cmd)
    {
        DEBUG_PRINT("Received command: %s\n", cmd.c_str());

        String plainCmd = cmd.substring(0, 2);
        // Status
        if (plainCmd == "TS")
        {
            uint8_t status = extractValueFromQueryResponse(cmd);
            DEBUG_PRINT("Status changed %u\n", status);
            this->bluetoothStatus = (BluetoothStatus)status;
        }
        else if (plainCmd == "TT")
        {
            String phoneNumber = cmd.substring(3, cmd.length() - 1);
            DEBUG_PRINT("Phone call received %s\n", phoneNumber.c_str());
            this->isReceivingPhoneCall = true;
            this->phoneNumber = phoneNumber;
        }

        else if (plainCmd == "MU")
        {
            uint8_t code = extractValueFromQueryResponse(cmd);
            switch (code)
            {
            case 1:
                DEBUG_PRINT("U-Disk inserted\n");
                delay(100);
                updateOnlineDevices();
                break;
            case 2:
                DEBUG_PRINT("U-Disk removed\n");
                delay(100);
                updateOnlineDevices();
                break;
            case 3:
                DEBUG_PRINT("TF-Card inserted\n");
                delay(100);
                updateOnlineDevices();
                break;
            case 4:
                DEBUG_PRINT("TF-Card removed\n");
                delay(100);
                updateOnlineDevices();
                break;
            }
        }

        else if (plainCmd == "QM")
        {
            uint8_t mode = extractValueFromQueryResponse(cmd);
            DEBUG_PRINT("Audio mode changed %u\n", mode);
            this->audioMode = (AudioMode)mode;
        }

        else if (plainCmd == "MF")
        {
            String filename = cmd.substring(3);
            DEBUG_PRINT("File playing changed %s\n", filename.c_str());
            currentFilePlaying = filename;
        }
    }

    /// @brief The query result from which to extract the value.
    /// @param queryResult The query result.
    /// @return The value from the query result.
    uint8_t extractValueFromQueryResponse(String queryResult)
    {
        return queryResult.substring(3, 5).toInt();
    }

    /// @brief Waits until data in the serial buffer is available or timeout is reached.
    /// @param timeout The timeout in milliseconds.
    /// @return The data if available, else empty string, if timed out.
    String waitUntilResponseReceived(unsigned long timeout, String expectedResponse)
    {
        timeout += millis();
        while (true)
        {
            if (timeout <= millis())
            {
                DEBUG_PRINT("Response Timed Out\n");
                return "";
            }

            while (serial->available())
            {
                String received = serial->readStringUntil('\n');

                // Check if it is the response, else just move it to the command buffer for processing later.
                if (received.substring(0, 2) == expectedResponse)
                {
                    return received;
                }
                else
                {
                    DEBUG_PRINT("{Queueing} Response Process Received: %s\n", received.c_str());
                    commandQueue->push(received);
                }
            }

            delay(1);
        }

        return "";
    }

    /// @brief Waits until "OK" message is received or error message is received.
    /// @param timeout The timeout in milliseconds.
    /// @return true if "OK" message is received, else (if timed out or error message received) false.
    bool waitUntilACKReceived(unsigned long timeout)
    {
        timeout += millis();
        while (true)
        {
            if (timeout <= millis())
            {
                DEBUG_PRINT("ACK Timed Out\n");
                return false;
            }

            while (serial->available())
            {
                String received = serial->readStringUntil('\n');

                // Check if it is the response, else just move it to the command buffer for processing later.
                if (received.substring(0, 2) == "OK")
                {
                    return received;
                }
                else if (received.substring(0, 2) == "ER")
                {
                    DEBUG_PRINT("ACK ERROR: %s\n", received.c_str());
                    return false;
                }
                else
                {
                    DEBUG_PRINT("{Queueing} ACK Process Received: %s\n", received.c_str());
                    commandQueue->push(received);
                }
            }

            delay(1);
        }

        return false;
    }
};

#endif