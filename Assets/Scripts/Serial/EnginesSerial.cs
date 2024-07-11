using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
    [SerializeField] private int depthVal;
    [SerializeField] private int steerVal;
    [SerializeField] private int thrustVal;

    [SerializeField] private SerialController serialController;

    public float getDepthVal() {
        return (depthVal-50) / 50f;
    }

    public float getSteerVal() {
        return (50-steerVal) / 50f;
    }

    public float getThrustVal() {
        return (100-thrustVal) / 100f;
    }

    void OnMessageArrived(string message) {
        char type = message[0];
        switch (type) {
            case 'D':
                int.TryParse(message.Substring(1), out depthVal);
                break;
            case 'S':
                int.TryParse(message.Substring(1), out steerVal);
                break;
            case 'T':
                int.TryParse(message.Substring(1), out thrustVal);
                break;
            default:
                break;
        }
    }

    void OnConnectionEvent(bool success) {
        Debug.Log(success ? "Engines connected":"Engines disconnected");
    }
}
