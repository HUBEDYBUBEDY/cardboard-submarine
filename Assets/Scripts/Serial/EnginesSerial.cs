using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
    [SerializeField] private int depthVal;
    [SerializeField] private int steerVal;
    [SerializeField] private int thrustVal;

    [SerializeField] private SerialControllerCustomDelimiter serialController;

    private const byte DEPTH =      1;
    private const byte STEER =      2;
    private const byte THRUST =     3;

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
            // serialController.SendSerialMessage();
        } else if (type.Equals(STEER)) {
            steerVal = data;
        } else if (type.Equals(THRUST)) {
            thrustVal = data;
        }
    }

    void OnConnectionEvent(bool success) {
        Debug.Log(success ? "Engines connected":"Engines disconnected");
    }
}
