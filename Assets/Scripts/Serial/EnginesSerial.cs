using System;
using System.Linq;
using UnityEngine;

public class EnginesSerial : MonoBehaviour
{
  [SerializeField] private int depthVal;
  [SerializeField] private int steerVal;
  [SerializeField] private int thrustVal;

  [SerializeField] private SerialControllerCustomDelimiter serialController;
  [SerializeField] private bool connected = false;

  private const byte DEPTH = 0b01000000;
  private const byte BEARING =    0b10000000;
  private const byte SPEED =      0b11000000;

  private const int INPUT_SAMPLES = 10;

  private int[] depthSamples;
  private int[] steerSamples;
  private int[] thrustSamples;

  private int sample = 0;

  public enum DisplayType
  {
    DEPTH,
    BEARING,
    SPEED
  }

  void Awake()
  {
    depthVal = 50;
    steerVal = 50;
    thrustVal = 50;

    sample = 0;
    depthSamples = new int[INPUT_SAMPLES];
    steerSamples = new int[INPUT_SAMPLES];
    thrustSamples = new int[INPUT_SAMPLES];
  }

  public float GetDepthVal()
  {
    return (50f-depthVal) / 50f;
  }

  public float GetSteerVal()
  {
    return (50f-steerVal) / 50f;
  }

  public float GetThrustVal()
  {
    return (thrustVal-50f) / 50f;
  }

  // Read joystick values between 0-100
  void OnMessageArrived(byte[] message)
  {
    if (message.Length != 3) return;
    if (!connected)
    {
      Debug.Log("Engines connected");
      connected = true;
    }

    depthSamples[sample] = message[0];
    steerSamples[sample] = message[1];
    thrustSamples[sample] = message[2];

    if (++sample >= INPUT_SAMPLES)
    {
      sample = 0;

      depthVal = (int) Math.Round(depthSamples.Average(), 0);
      steerVal = (int) Math.Round(steerSamples.Average(), 0);
      thrustVal = (int) Math.Round(thrustSamples.Average(), 0);
    }
    
  }

  public void UpdateDisplay(int value, DisplayType type)
  {
    byte[] valueBytes = BitConverter.GetBytes(value);

    if (type.Equals(DisplayType.DEPTH))
    {
      valueBytes[1] = (byte)(valueBytes[1] | DEPTH);
    }
    else if (type.Equals(DisplayType.BEARING))
    {
      valueBytes[1] = (byte)(valueBytes[1] | BEARING);
    }
    else if (type.Equals(DisplayType.SPEED))
    {
      valueBytes[1] = (byte)(valueBytes[1] | SPEED);
    }

    byte[] message = {valueBytes[0], valueBytes[1], serialController.separator};
    serialController.SendSerialMessage(message);
  }
}
