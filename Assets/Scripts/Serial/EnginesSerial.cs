using System;
using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
    [SerializeField] private int depthVal;
    [SerializeField] private int steerVal;
    [SerializeField] private int thrustVal;

    [SerializeField] private SerialControllerCustomDelimiter serialController;

    private const byte DEPTH =      0b00100000;
    private const byte STEER =      0b01000000;
    private const byte THRUST =     0b01100000;

    public enum DisplayType {
        DEPTH,
        STEER,
        THRUST
    }

    void Start() {
        depthVal = 50;
        steerVal = 50;
        thrustVal = 100;
    }

    public float GetDepthVal() {
        return (depthVal-50) / 50f;
    }

    public float GetSteerVal() {
        return (50-steerVal) / 50f;
    }

    public float GetThrustVal() {
        return (100-thrustVal) / 100f;
    }

    void OnMessageArrived(byte[] message) {
        if(message.Length < 2) return;
        byte data = message[0];
        byte type = message[1];

        if (type.Equals(DEPTH)) {
            depthVal = data;
        } else if (type.Equals(STEER)) {
            steerVal = data;
        } else if (type.Equals(THRUST)) {
            thrustVal = data;
        }
    }

    void OnConnectionEvent(bool success) {
        Debug.Log(success ? "Engines connected":"Engines disconnected");
    }

    public void UpdateDisplay(int value, DisplayType type) {
        // Due to serial limitations, each byte must be < 128
        byte[] valueBytes = BitConverter.GetBytes(value);

        // Save and clear top bit of lower byte
        byte topBit = (byte)(valueBytes[0] >> 7);
        valueBytes[0] = (byte)(valueBytes[0] - (topBit << 7));

        // Store in lower bit of upper byte
        valueBytes[1] = (byte)((byte)(valueBytes[1] << 1) + topBit);

        if (type.Equals(DisplayType.DEPTH)) {
            valueBytes[1] = (byte)(valueBytes[1] | DEPTH);
        } else if (type.Equals(DisplayType.STEER)) {
            valueBytes[1] = (byte)(valueBytes[1] | STEER);
        } else if (type.Equals(DisplayType.THRUST)) {
            valueBytes[1] = (byte)(valueBytes[1] | STEER);
        }

        byte[] message = {valueBytes[0], valueBytes[1], serialController.separator};
        serialController.SendSerialMessage(message);
        foreach(byte b in message) {
            Debug.Log(b);
        }
    }
}
