using UnityEngine;
using UnityEngine.UI;

public class Engines : MonoBehaviour
{
    private const float KN_TO_MS = 0.514f;

    [SerializeField] private float thrustSpeed;
    [SerializeField] private float steeringSpeed;
    [SerializeField] private float depthSpeed;
    [SerializeField] private float depth;

    // Values here are most realistic, change depending on gameplay preference
    [SerializeField] private float thrustAcc = 0.1f;
    [SerializeField] private float thrustMax = 0.1285f;         // 1 unit is 1cell/s
    [SerializeField] private float steeringAcc = 0.1f;
    [SerializeField] private float steeringMax = 2f;            // 1 unit is 1deg/s
    [SerializeField] private float depthAcc = 0.1f;
    [SerializeField] private float depthSpeedMax = 5f;         // 1 unit is 1m/s

    [SerializeField] private EnginesSerial enginesSerial;
    [SerializeField] private Slider steering;
    [SerializeField] private Slider depthControl;
    [SerializeField] private Rigidbody2D rb;
    [SerializeField] private GuideLine guideLine;
    [SerializeField] private HUDManager hudManager;

    // Update is called once per frame
    void FixedUpdate() {
        float targetSteering = enginesSerial.getSteerVal() * steeringMax;
        updateSteering(targetSteering);

        float targetThrust = enginesSerial.getThrustVal() * thrustMax;
        updateThrust(targetThrust);

        float targetDepthSpeed = enginesSerial.getDepthVal() * depthSpeedMax;
        updateDepth(targetDepthSpeed);

        guideLine.updateLine(rb.velocity, thrustSpeed/thrustMax, rb.angularVelocity);

        float bearing = Vector2.Angle(rb.GetRelativeVector(transform.up), Vector2.up);
        if(rb.velocity.x < 0) bearing = 360f - bearing;
        hudManager.updateText(thrustSpeed*100*(1/KN_TO_MS), bearing, depth);
    }

    // Update thrust based on given target
    void updateThrust(float targetThrust) {
        if(targetThrust > thrustSpeed) {
            // Increase/decrease at constant rate with respect to time
            thrustSpeed += thrustAcc*thrustMax*Time.deltaTime;
            thrustSpeed = Mathf.Clamp(thrustSpeed, 0f, targetThrust);
        } else if(targetThrust < thrustSpeed) {
            thrustSpeed -= thrustAcc*thrustMax*Time.deltaTime;
            thrustSpeed = Mathf.Clamp(thrustSpeed, targetThrust, thrustMax);
        }
        rb.velocity = thrustSpeed * rb.GetRelativeVector(transform.up);
    }

    // Update steering based on given target (-ve is AC, +ve is C)
    void updateSteering(float targetSteering) {
        if(targetSteering > steeringSpeed) {
            // Increase/decrease at constant rate with respect to time
            steeringSpeed += steeringAcc*steeringMax*Time.deltaTime;
            steeringSpeed = Mathf.Clamp(steeringSpeed, -steeringMax, targetSteering);
        } else if(targetSteering < steeringSpeed) {
            steeringSpeed -= steeringAcc*steeringMax*Time.deltaTime;
            steeringSpeed = Mathf.Clamp(steeringSpeed, targetSteering, steeringMax);
        }
        rb.angularVelocity = -steeringSpeed;
    }

    void updateDepth(float targetDepthSpeed) {
        if(targetDepthSpeed > depthSpeed) {
            // Increase/decrease at constant rate with respect to time
            depthSpeed += depthAcc*depthSpeedMax*Time.deltaTime;
            depthSpeed = Mathf.Clamp(depthSpeed, -depthSpeedMax, targetDepthSpeed);
        } else if(targetDepthSpeed < depthSpeed) {
            depthSpeed -= depthAcc*depthSpeedMax*Time.deltaTime;
            depthSpeed = Mathf.Clamp(depthSpeed, targetDepthSpeed, depthSpeedMax);
        }
        // Increase/decrease at constant rate with respect to time
        depth -= depthSpeed*Time.deltaTime;
    }
}
