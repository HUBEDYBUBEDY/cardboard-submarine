using System;
using UnityEngine;
using UnityEngine.UI;

public class Engines : MonoBehaviour
{
  private const float KN_TO_MS = 0.514f;

  [SerializeField] private float thrustSpeed;
  [SerializeField] private float speed = 0f;
  [SerializeField] private float steeringSpeed;
  [SerializeField] private float bearing = 0f;
  [SerializeField] private float depthSpeed;
  [SerializeField] private float depth = 0f;

  // Values here are most realistic, change depending on gameplay preference
  [SerializeField] private float thrustAcc = 0.1f;
  [SerializeField] private float thrustMax = 0.1285f;         // 1 unit is 1cell/s
  [SerializeField] private float steeringAcc = 0.1f;
  [SerializeField] private float steeringMax = 2f;            // 1 unit is 1deg/s
  [SerializeField] private float depthAcc = 0.1f;
  [SerializeField] private float depthSpeedMax = 5f;          // 1 unit is 1m/s

  [SerializeField] private EnginesSerial enginesSerial;
  [SerializeField] private DepthCheck depthCheck;
  [SerializeField] private Rigidbody2D rb;
  [SerializeField] private GuideLine guideLine;
  [SerializeField] private HUDManager hudManager;

  // Update is called once per frame
  void FixedUpdate() {
    // Update depth and display (not based on rb physics)
    float targetDepthSpeed = enginesSerial.GetDepthVal() * depthSpeedMax;
    UpdateDepth(targetDepthSpeed);

    // Update steering (display update separate as it relies on rb physics)
    float targetSteering = enginesSerial.GetSteerVal() * steeringMax;
    UpdateSteering(targetSteering);

    // Update thrust (display update separate as it relies on rb physics)
    float targetThrust = enginesSerial.GetThrustVal() * thrustMax;
    UpdateThrust(targetThrust);

    guideLine.updateLine(rb.velocity, thrustSpeed/thrustMax, rb.angularVelocity);

    UpdateDisplays();
  }

  void UpdateDepth(float targetDepthSpeed)
  {
    int oldDepth = (int)Math.Round(GetDepth(), 0);

    if (targetDepthSpeed > depthSpeed)
    {
      // Increase/decrease at constant rate with respect to time
      depthSpeed += depthAcc * depthSpeedMax * Time.deltaTime;
      depthSpeed = Mathf.Clamp(depthSpeed, -depthSpeedMax, targetDepthSpeed);
    }
    else if (targetDepthSpeed < depthSpeed)
    {
      depthSpeed -= depthAcc * depthSpeedMax * Time.deltaTime;
      depthSpeed = Mathf.Clamp(depthSpeed, targetDepthSpeed, depthSpeedMax);
    }

    // Increase/decrease at constant rate with respect to time
    depth -= depthSpeed * Time.deltaTime;
    depth = Mathf.Clamp(depth, 0f, depthCheck.GetDepth());

    // Update display if changed
    int newDepth = (int)Math.Round(GetDepth(), 0);
    if(oldDepth != newDepth) {
      enginesSerial.UpdateDisplay(newDepth, EnginesSerial.DisplayType.DEPTH);
    }
  }

  // Update steering based on given target (-ve is AC, +ve is C)
  void UpdateSteering(float targetSteering) {
    if (targetSteering > steeringSpeed)
    {
      // Increase/decrease at constant rate with respect to time
      steeringSpeed += steeringAcc * steeringMax * Time.deltaTime;
      steeringSpeed = Mathf.Clamp(steeringSpeed, -steeringMax, targetSteering);
    }
    else if (targetSteering < steeringSpeed)
    {
      steeringSpeed -= steeringAcc * steeringMax * Time.deltaTime;
      steeringSpeed = Mathf.Clamp(steeringSpeed, targetSteering, steeringMax);
    }

    rb.angularVelocity = -steeringSpeed;
  }

  // Update thrust based on given target
  void UpdateThrust(float targetThrust) {
    if (targetThrust > thrustSpeed)
    {
      // Increase/decrease at constant rate with respect to time
      thrustSpeed += thrustAcc * thrustMax * Time.deltaTime;
      thrustSpeed = Mathf.Clamp(thrustSpeed, 0f, targetThrust);
    }
    else if (targetThrust < thrustSpeed)
    {
      thrustSpeed -= thrustAcc * thrustMax * Time.deltaTime;
      thrustSpeed = Mathf.Clamp(thrustSpeed, targetThrust, thrustMax);
    }

    rb.velocity = thrustSpeed * rb.GetRelativeVector(transform.up);
  }

  void UpdateDisplays()
  {
    int newBearing = (int)Math.Round(GetBearing(), 0);
    if (bearing != newBearing)
    {
      enginesSerial.UpdateDisplay(newBearing, EnginesSerial.DisplayType.BEARING);
      bearing = newBearing;
    }

    int newSpeed = (int)Math.Round(GetSpeed(), 0);
    if (speed != newSpeed)
    {
      enginesSerial.UpdateDisplay(newSpeed, EnginesSerial.DisplayType.SPEED);
      speed = newSpeed;
    }

    hudManager.updateText(
      (int)Math.Round(GetSpeed(), 0),
      (int)Math.Round(GetBearing(), 0),
      (int)Math.Round(GetDepth(), 0));
  }

  // Return depth in metres
  public float GetDepth()
  {
    return depth;
  }

  // Return bearing in degrees 0-359
  public float GetBearing() {
    Vector2 direction = rb.GetRelativeVector(transform.up);
    float bearing = Vector2.Angle(direction, Vector2.up);

    if (direction.x < 0) bearing = 360f - bearing;
    return bearing;
  }

  // Return speed in knots
  public float GetSpeed() {
    return thrustSpeed * 100 / KN_TO_MS;
  }
}
