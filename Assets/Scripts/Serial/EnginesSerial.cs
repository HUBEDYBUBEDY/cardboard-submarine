using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
    [SerializeField] private float thrust;

    [SerializeField] private SerialController serialController;

    public float getThrust() {
        return thrust;
    }

    void OnMessageArrived(string message) {
        float.TryParse(message, out thrust);
    }

    void OnConnectionEvent(bool success) {
        Debug.Log(success ? "Engines connected":"Engines disconnected");
    }
}
