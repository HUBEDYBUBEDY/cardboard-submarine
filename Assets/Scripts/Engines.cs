using System;
using UnityEditor.EditorTools;
using UnityEngine;
using UnityEngine.UI;

public class Engines : MonoBehaviour
{
  // 1 kn = 0.514 m/s
  private const float KN_TO_MS = 0.514f;

  /* Values here are most realistic, change depending on gameplay preference */

  // Velocity/Thrust
  [Tooltip("1 kn = 0.514 m/s = 0.00514cell/s")]
  [SerializeField] private float velocityKN = 0f;
  [SerializeField] private float velocityKNMax = 35f;
  [SerializeField] private float velocityKNAcc = 1f;
  [Tooltip("1 unit/s = 1 cell/s = 100m/s")]
  [SerializeField] private float velocityUS;
  [SerializeField] private float velocityUSMax;
  [SerializeField] private float velocityUSAcc;

  // Bearing/Steering
  [SerializeField] private float bearing = 0f;
  [Tooltip("1 unit = 1 deg/s")]
  [SerializeField] private float angularVelocity;
  [SerializeField] private float angularVelocityMax = 2f;
  [SerializeField] private float angularVelocityAcc = 0.1f;

  // Depth/Ballast
  [SerializeField] private float depth = 0f;
  [Tooltip("1 unit = 1 m/s")]
  [SerializeField] private float depthSpeed;
  [SerializeField] private float depthAcc = 0.1f;
  [SerializeField] private float depthSpeedMax = 5f;

  [SerializeField] private EnginesSerial enginesSerial;
  [SerializeField] private DepthCheck depthCheck;
  [SerializeField] private Rigidbody2D rb;
  [SerializeField] private GuideLine guideLine;
  [SerializeField] private HUDManager hudManager;

  void Awake()
  {
    updateUSValues();
  }

  // Update is called once per frame
  void FixedUpdate()
  {
    // Remove once testing complete
    updateUSValues();

    // Update depth and display (not based on rb physics)
    float targetDepthSpeed = enginesSerial.GetDepthVal() * depthSpeedMax;
    UpdateDepth(targetDepthSpeed);

    // Update steering (display update separate as it relies on rb physics)
    float targetSteering = enginesSerial.GetSteerVal() * angularVelocityMax;
    UpdateSteering(targetSteering);

    // Update thrust (display update separate as it relies on rb physics)
    float targetThrust = enginesSerial.GetThrustVal() * velocityUSMax;
    UpdateThrust(targetThrust);

    guideLine.updateLine(rb.velocity, velocityUS / velocityUSMax, rb.angularVelocity);

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
    if (oldDepth != newDepth)
    {
      enginesSerial.UpdateDisplay(newDepth, EnginesSerial.DisplayType.DEPTH);
    }
  }

  // Update steering based on given target (-ve is AC, +ve is C)
  void UpdateSteering(float targetSteering)
  {
    if (targetSteering > angularVelocity)
    {
      // Increase/decrease at constant rate with respect to time
      angularVelocity += angularVelocityAcc * angularVelocityMax * Time.deltaTime;
      angularVelocity = Mathf.Clamp(angularVelocity, -angularVelocityMax, targetSteering);
    }
    else if (targetSteering < angularVelocity)
    {
      angularVelocity -= angularVelocityAcc * angularVelocityMax * Time.deltaTime;
      angularVelocity = Mathf.Clamp(angularVelocity, targetSteering, angularVelocityMax);
    }

    rb.angularVelocity = -angularVelocity;
  }

  // Update thrust based on given target
  void UpdateThrust(float targetThrust)
  {
    if (targetThrust > velocityUS)
    {
      // Increase/decrease at constant rate with respect to time
      velocityUS += velocityUSAcc * velocityUSMax * Time.deltaTime;
      velocityUS = Mathf.Clamp(velocityUS, 0f, targetThrust);
    }
    else if (targetThrust < velocityUS)
    {
      velocityUS -= velocityUSAcc * velocityUSMax * Time.deltaTime;
      velocityUS = Mathf.Clamp(velocityUS, targetThrust, velocityUSMax);
    }

    rb.velocity = velocityUS * rb.GetRelativeVector(transform.up);
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
    if (velocityKN != newSpeed)
    {
      enginesSerial.UpdateDisplay(newSpeed, EnginesSerial.DisplayType.SPEED);
      velocityKN = newSpeed;
    }

    hudManager.updateText(
      (int)Math.Round(GetSpeed(), 0),
      (int)Math.Round(GetBearing(), 0),
      (int)Math.Round(GetDepth(), 0));
  }

  float convertKNtoUS(float valueKN)
  {
    return valueKN * KN_TO_MS / 100f;
  }

  float convertUStoKN(float valueUS) {
    return valueUS * 100 / KN_TO_MS;
  }

  void updateUSValues()
  {
    velocityUSAcc = convertKNtoUS(velocityKNAcc);
    velocityUSMax = convertKNtoUS(velocityKNMax);
  }

  // Return depth in metres
  public float GetDepth()
  {
    return depth;
  }

  // Return bearing in degrees 0-359
  public float GetBearing()
  {
    Vector2 direction = rb.GetRelativeVector(transform.up);
    float bearing = Vector2.Angle(direction, Vector2.up);

    if (direction.x < 0) bearing = 360f - bearing;
    return bearing;
  }

  // Return speed in knots
  public float GetSpeed()
  {
    return convertUStoKN(velocityUS);
  }
}
