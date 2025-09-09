using System;
using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
  [SerializeField] private float depthVal;
  [SerializeField] private float steerVal;
  [SerializeField] private float thrustVal;

  [SerializeField] private SerialControllerCustomDelimiter serialController;
  [SerializeField] private bool connected = false;

  private const byte DEPTH =      0b01000000;
  private const byte BEARING =    0b10000000;
  private const byte SPEED =      0b11000000;

  public enum DisplayType {
    DEPTH,
    BEARING,
    SPEED
  }

  void Start() {
    depthVal = 50f;
    steerVal = 50f;
    thrustVal = 0f;
  }

  public float GetDepthVal() {
    return (50f-depthVal) / 50f;
  }

  public float GetSteerVal() {
    return (50f-steerVal) / 50f;
  }

  public float GetThrustVal() {
    return thrustVal / 100f;
  }

  // Read joystick values between 0-100
  void OnMessageArrived(byte[] message) {
    if (message.Length < 3) return;
    if (!connected) {
      Debug.Log("Engines connected");
      connected = true;
    }

    depthVal = message[0];
    steerVal = message[1];
    thrustVal = message[2];
  }

  public void UpdateDisplay(int value, DisplayType type) {
    byte[] valueBytes = BitConverter.GetBytes(value);

    if (type.Equals(DisplayType.DEPTH)) {
      valueBytes[1] = (byte)(valueBytes[1] | DEPTH);
    } else if (type.Equals(DisplayType.BEARING)) {
      valueBytes[1] = (byte)(valueBytes[1] | BEARING);
    } else if (type.Equals(DisplayType.SPEED)) {
      valueBytes[1] = (byte)(valueBytes[1] | SPEED);
    }

    byte[] message = {valueBytes[0], valueBytes[1], serialController.separator};
    serialController.SendSerialMessage(message);
  }
}
