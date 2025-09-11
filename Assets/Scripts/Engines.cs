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
  [SerializeField] private float velocityKNMin = -5f;
  [SerializeField] private float velocityKNAcc = 0f;
  [SerializeField] private float velocityKNAccMax = 3f;
  [Tooltip("Proportion of max acceleration increased/decreased per second, " +
    "e.g. 0.1 AccRate = 10 seconds to reach max acceleration")]
  [SerializeField] private float velocityAccRate = 0.1f;
  [Tooltip("1 unit/s = 1 cell/s = 100m/s")]
  [SerializeField] private float velocityUS;
  [SerializeField] private float velocityUSMax;
  [SerializeField] private float velocityUSMin;
  [SerializeField] private float velocityUSAcc;
  [SerializeField] private float velocityUSAccMax;

  // Bearing/Steering
  [Tooltip("1 unit = 1 deg")]
  [SerializeField] private float bearing = 0f;
  [Tooltip("1 unit = 1 deg/s")]
  [SerializeField] private float angularVelocity;
  [SerializeField] private float angularVelocityMax = 5f;
  [Tooltip("Proportion of max angular velocity increased/decreased per second, " +
    "e.g. 0.1 Acc = 10 seconds to reach max angular velocity")]
  [SerializeField] private float angularVelocityAcc = 0.1f;

  // Depth/Ballast
  [Tooltip("1 unit = 1m")]
  [SerializeField] private float depth = 0f;
  [Tooltip("1 unit = 1 m/s")]
  [SerializeField] private float depthSpeed;
  [SerializeField] private float depthSpeedMax = 5f;
  [Tooltip("Proportion of max depth speed increased/decreased per second, " +
    "e.g. 0.1 Acc = 10 seconds to reach max depth speed")]
  [SerializeField] private float depthAcc = 0.1f;


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

    // Update angular velocity (display update separate as it relies on rb physics)
    float targetAngularVelocity = enginesSerial.GetSteerVal() * angularVelocityMax;
    UpdateAngularVelocity(targetAngularVelocity);

    // Update velocity (display update separate as it relies on rb physics)
    float targetAcc = enginesSerial.GetThrustVal() * velocityUSAccMax;
    UpdateVelocity(targetAcc);

    guideLine.updateLine(rb.velocity, velocityUS / velocityUSMax, rb.angularVelocity);

    UpdateDisplays();
  }

  // Update depth speed & depth based on given target speed
  void UpdateDepth(float targetDepthSpeed)
  {
    int oldDepth = (int)Math.Round(GetDepth(), 0);

    if (targetDepthSpeed > depthSpeed)
    {
      // Increase/decrease acceleration at constant rate with respect to time
      depthSpeed += depthAcc * depthSpeedMax * Time.deltaTime;
      depthSpeed = Mathf.Clamp(depthSpeed, -depthSpeedMax, targetDepthSpeed);
    }
    else if (targetDepthSpeed < depthSpeed)
    {
      depthSpeed -= depthAcc * depthSpeedMax * Time.deltaTime;
      depthSpeed = Mathf.Clamp(depthSpeed, targetDepthSpeed, depthSpeedMax);
    }

    // Increase/decrease speed at constant rate with respect to time
    depth -= depthSpeed * Time.deltaTime;
    depth = Mathf.Clamp(depth, 0f, depthCheck.GetDepth());

    // Update display if changed
    int newDepth = (int)Math.Round(GetDepth(), 0);
    if (oldDepth != newDepth)
    {
      enginesSerial.UpdateDisplay(newDepth, EnginesSerial.DisplayType.DEPTH);
    }
  }

  // Update angular velocity based on given target (-ve is AC, +ve is C)
  void UpdateAngularVelocity(float targetAngularVelocity)
  {
    if (targetAngularVelocity > angularVelocity)
    {
      // Increase/decrease at constant rate with respect to time
      angularVelocity += angularVelocityAcc * angularVelocityMax * Time.deltaTime;
      angularVelocity = Mathf.Clamp(angularVelocity, -angularVelocityMax, targetAngularVelocity);
    }
    else if (targetAngularVelocity < angularVelocity)
    {
      angularVelocity -= angularVelocityAcc * angularVelocityMax * Time.deltaTime;
      angularVelocity = Mathf.Clamp(angularVelocity, targetAngularVelocity, angularVelocityMax);
    }

    rb.angularVelocity = -angularVelocity;
  }

  // Update acceleration & velocity based on given target acceleration
  void UpdateVelocity(float targetAcc)
  {
    if (targetAcc > velocityUSAcc)
    {
      // Increase/decrease at constant rate with respect to time
      velocityUSAcc += velocityAccRate * velocityUSAccMax * Time.deltaTime;
      velocityUSAcc = Mathf.Clamp(velocityUSAcc, -velocityUSAccMax, targetAcc);
    }
    else if (targetAcc < velocityUSAcc)
    {
      velocityUSAcc -= velocityAccRate * velocityUSAccMax * Time.deltaTime;
      velocityUSAcc = Mathf.Clamp(velocityUSAcc, targetAcc, velocityUSAccMax);
    }

    // Increase/decrease velocity by acceleration
    velocityUS += velocityUSAcc * Time.deltaTime;
    velocityUS = Mathf.Clamp(velocityUS, velocityUSMin, velocityUSMax);

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

  float convertUStoKN(float valueUS)
  {
    return valueUS * 100 / KN_TO_MS;
  }

  void updateUSValues()
  {
    velocityUSMax = convertKNtoUS(velocityKNMax);
    velocityUSMin = convertKNtoUS(velocityKNMin);
    velocityUSAccMax = convertKNtoUS(velocityKNAccMax);

    velocityKNAcc = convertUStoKN(velocityUSAcc);
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
