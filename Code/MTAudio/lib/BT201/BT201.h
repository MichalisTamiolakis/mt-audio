#ifndef BT201_H
#define BT201_H

#include <Arduino.h>
#include <string.h>

#define DEBUG

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
    BT201(HardwareSerial* serial)
    {
        this->serial = serial;
    }

    void loop()
    {
        readSerial();
    }

    // U-Disk & TF card in/out functions
    void onUDiskStatusChanged(void (*callback)(bool inserted))
    {
        this->uDiskStatusChanged = callback;
    }

    void onTFCardStatusChanged(void (*callback)(bool inserted))
    {
        this->tfCardStatusChanged = callback;
    }

    void sendCommand(String command)
    {
        this->serial->printf("%s\r\n", command.c_str());
    }
    
    void onAudioModeChanged(void (*callback)(AudioMode mode))
    {
        this->audioModeChanged = callback;
    }

    void onFilePlayingChanged(void (*callback)(String filename))
    {
        filePlayingChanged = callback;
    }

    // Audio Related Functions
    void onBluetoothConnectionStatusChanged(void (*callback)(BluetoothStatus status))
    {
        this->connectionStatusChanged = callback;
    }

    bool setVolume(uint8_t volume)
    {
        // Read pending serial data
        readSerial();
        
        volume = constrain(volume, 0, 30);
        DEBUG_PRINT("Sending Command: AT+CA%02u\r\n", volume);
        this->serial->printf("AT+CA%02u\r\n", volume);

        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = this->serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());
            if(isSuccess(response))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }

    /// @brief Gets the volume from the BT201 board.
    /// @return Returns the volume.
    uint8_t getVolume()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+QA\r\n");
        this->serial->printf("AT+QA\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if (!isError(response))
            {
                uint8_t volume = extractValueFromQueryResponse(response);

                // Parse ok command
                if(waitUntilSerialAvailable(50))
                {
                    String secondResponse = serial->readStringUntil('\n');
                    DEBUG_PRINT("Response is %s\n", secondResponse.c_str());
                    if(isSuccess(secondResponse))
                    {
                        DEBUG_PRINT("Volume: %u\n", volume);
                        return volume;
                    }
                    else
                    {
                        DEBUG_PRINT("Response: Error\n");
                        return 0;
                    }
                }

                DEBUG_PRINT("Unexpected response received\n");
                return 0;
            }
            
            DEBUG_PRINT("Response: Error\n");
            return 0;
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return 0;
    }

    bool increaseVolume()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+CE\r\n");
        this->serial->printf("AT+CE\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Volume increased\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool decreaseVolume()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+CF\r\n");
        this->serial->printf("AT+CF\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Volume decreased\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool togglePlayPause()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+CB\r\n");
        this->serial->printf("AT+CB\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Toggled play/pause\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool playNextSong()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+CC\r\n");
        this->serial->printf("AT+CC\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Playing next song.\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool playPreviousSong()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+CD\r\n");
        this->serial->printf("AT+CD\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Playing previous song.\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool setAudioMode(AudioMode mode)
    {
        // Read pending serial data
        readSerial();
        
        DEBUG_PRINT("Sending Command: AT+CM%02d\r\n", (int)mode);
        this->serial->printf("AT+CM%02d\r\n", (int)mode);

        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = this->serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());
            if(isSuccess(response))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }

    AudioMode getAudioMode()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+QM\r\n");
        this->serial->printf("AT+QM\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if (!isError(response))
            {
                uint8_t mode = extractValueFromQueryResponse(response);

                // Parse ok command
                if(waitUntilSerialAvailable(50))
                {
                    String secondResponse = serial->readStringUntil('\n');
                    DEBUG_PRINT("Response is %s\n", secondResponse.c_str());
                    if(isSuccess(secondResponse))
                    {
                        DEBUG_PRINT("Mode: %u\n", mode);
                        return (AudioMode)constrain(mode, 1, 3);
                    }
                    else
                    {
                        DEBUG_PRINT("Response: Error\n");
                        return AudioMode::Bluetooth;
                    }
                }

                DEBUG_PRINT("Unexpected response received\n");
                return AudioMode::Bluetooth;
            }
            
            DEBUG_PRINT("Response: Error\n");
            return AudioMode::Bluetooth;
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return AudioMode::Bluetooth;
    }

    // Phone Call Functions
    
    void onPhoneCallReceived(void (*callback)(String phoneNumber))
    {
        this->phoneCallReceived = callback;
    }

    /// @brief Calls the specified phone number.
    /// @param phoneNumber The phone number to call.
    /// @return true on successful call.
    bool phoneCall(String phoneNumber)
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+BT%s\r\n", phoneNumber.c_str());
        this->serial->printf("AT+BT%s\r\n", phoneNumber.c_str());
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Phone Number Called\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    /// @brief Call back a phone call.
    /// @return 
    bool phoneBack()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+BA00\r\n");
        this->serial->printf("AT+BA00\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Phone Number Called Back\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    /// @brief Hangs up the incoming phone call.
    /// @return true if successful.
    bool phoneHangUp()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+BA03\r\n");
        this->serial->printf("AT+BA03\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Phone Hang Up\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    bool phoneRefuseAccept()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+BA02\r\n");
        this->serial->printf("AT+BA02\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Phone Refused\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

    /// @brief Picks up the incoming phone call.
    /// @return true if successful.
    bool phonePickUp()
    {
        // Read pending serial data
        readSerial();

        DEBUG_PRINT("Sending Command: AT+BA04\r\n");
        this->serial->printf("AT+BA04\r\n");
        
        // Wait for response
        if(waitUntilSerialAvailable(50))
        {
            String response = serial->readStringUntil('\n');
            DEBUG_PRINT("Response is %s\n", response.c_str());

            if(isSuccess(response))
            {
                DEBUG_PRINT("Phone Pick Up\n");
                return true;
            }
            else
            {
                DEBUG_PRINT("Response: Error\n");
                return false;
            }
            
        }
        
        DEBUG_PRINT("Response: Timed out\n");
        return false;
    }

private:
    HardwareSerial* serial;
    // String** buffer;
    // uint8_t bufferSize = 0;
    // uint8_t usedBufferSize = 0;
    void (*connectionStatusChanged)(BluetoothStatus status);
    void (*audioModeChanged)(AudioMode mode);
    void (*phoneCallReceived)(String phoneNumber);
    void (*uDiskStatusChanged)(bool inserted);
    void (*tfCardStatusChanged)(bool inserted);
    void (*filePlayingChanged)(String filename);

    void readSerial()
    {
        while(serial->available())
        {
            String commandReceived = serial->readStringUntil('\n');

            // Parse command
            parseReceivedCommand(commandReceived);
        }
    }

    void parseReceivedCommand(String cmd)
    {
        DEBUG_PRINT("Received command: %s\n", cmd.c_str());

        String plainCmd = cmd.substring(0, 2);
        // Status
        if(plainCmd == "TS")
        {
            uint8_t status = extractValueFromQueryResponse(cmd);
            DEBUG_PRINT("Status changed %u\n", status);
            if(connectionStatusChanged != nullptr)
            {
                connectionStatusChanged((BluetoothStatus)status);
            }
        }
        else if(plainCmd == "TT")
        {
            String phoneNumber = cmd.substring(3, cmd.length() - 1);
            DEBUG_PRINT("Phone call received %s\n", phoneNumber.c_str());
            if(phoneCallReceived != nullptr)
            {
                phoneCallReceived(phoneNumber);
            }
        }

        else if(plainCmd == "MU")
        {
            uint8_t code = extractValueFromQueryResponse(cmd);
            switch(code)
            {
                case 1:
                    DEBUG_PRINT("U-Disk inserted\n");
                    if(uDiskStatusChanged != nullptr)
                    {
                        uDiskStatusChanged(true);
                    }
                    break;
                case 2:
                    DEBUG_PRINT("U-Disk removed\n");
                    if(uDiskStatusChanged != nullptr)
                    {
                        uDiskStatusChanged(false);
                    }
                    break;
                case 3:
                    DEBUG_PRINT("TF-Card inserted\n");
                    if(tfCardStatusChanged != nullptr)
                    {
                        tfCardStatusChanged(true);
                    }
                    break;
                case 4:
                    DEBUG_PRINT("TF-Card removed\n");
                    if(tfCardStatusChanged != nullptr)
                    {
                        tfCardStatusChanged(false);
                    }
                    break;
            }
        }
    
        else if(plainCmd == "QM")
        {
            uint8_t mode = extractValueFromQueryResponse(cmd);
            DEBUG_PRINT("Audio mode changed %u\n", mode);
            if(audioModeChanged != nullptr)
            {
                audioModeChanged((AudioMode)mode);
            }
        }

        else if(plainCmd == "MF")
        {
            String filename = cmd.substring(3);
            DEBUG_PRINT("File playing changed %s\n", filename.c_str());
            if(filePlayingChanged != nullptr)
            {
                filePlayingChanged(filename);
            }
        }
    }

    /// @brief Checks if the response is the success string "OK"
    /// @param response The response to check.
    /// @return true if the repsponse is the success string "OK", else false.
    bool isSuccess(String response)
    {
        if(response.substring(0, 2) == "OK")
        {
            return true;
        }

        return false;
    }

    /// @brief Tries to parse the response as an error
    /// @param response The response string to check.
    /// @return 0 if no error, else error code.
    uint8_t isError(String response)
    {
        if(response.substring(0, 2) == "ER")
        {
            return response.substring(3, 4).toInt();
        }
        return 0;
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
    /// @return true if data is available, else false, if timed out.
    bool waitUntilSerialAvailable(unsigned long timeout)
    {
        timeout += millis();
        while(true)
        {
            if(timeout <= millis())
            {
                return false;
            }

            if(serial->available())
            {
                return true;
            }

            delay(1);
        }

        return false;
    }
};

#endif