using UnityEngine;
using UnityEngine.UI;

public class WeaponsSerial : MonoBehaviour
{
    [SerializeField] private Button countermeasures;
    [SerializeField] private SerialController serialController;

    void Start() {
        countermeasures.onClick.AddListener(onCountermeasures);
    }

    void onCountermeasures() {
        Debug.Log("Targeting Started");
        serialController.SendSerialMessage("success");
    }

    void OnMessageArrived(string message) {
        Debug.Log("Recieved: " + message);
    }

    void OnConnectionEvent(bool success) {
        Debug.Log(success ? "Weapons connected":"Weapons disconnected");
    }
}
