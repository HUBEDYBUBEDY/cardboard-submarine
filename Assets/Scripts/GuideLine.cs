using UnityEngine;
using UnityEngine.Splines;
using Unity.Mathematics;

using Spline = UnityEngine.Splines.Spline;
using System.Linq;
using Unity.VisualScripting;

public class GuideLine : MonoBehaviour
{
  [Tooltip("The end of the line will show the estimated position after this many seconds, " +
    "if maintaining current velocity and angular velocity.")]
  [SerializeField] private int guideTime = 30;

  [Tooltip("Number of samples to generate guideline")]
  [SerializeField] private int samples = 10;

  [SerializeField] private SplineContainer splineContainer;
  [SerializeField] private Spline guideSpline;

  [SerializeField] private Spline TEMPangleSpline;

  void Start()
  {
    splineContainer = GetComponent<SplineContainer>();

    // Remove default spline
    splineContainer.RemoveSplineAt(0);
    guideSpline = splineContainer.AddSpline();

    // Replace with spline starting from sub centre + 1 knot for each sample
    BezierKnot[] knots = new BezierKnot[samples];
    guideSpline.Knots = knots;

    // Set mode to automatically calculate bezier tangents for each knot
    guideSpline.SetTangentMode(TangentMode.AutoSmooth);
  }

  // Generate an arc guideline of the path the sub will follow (if velocity & angular velocity constant)
  public void updateLine(Vector2 velocity, float angularVelocity)
  {
    // Calculate radius of the arc path
    float angularVelocityRad = angularVelocity * guideTime * Mathf.Deg2Rad;
    float arcRadius = velocity.magnitude * guideTime / angularVelocityRad;

    Vector2 samplePosition = Vector2.zero;
    Vector2 radiusVectToPivot;
    Vector2 radiusVectToArc;
    Vector2 pivot;

    // Determine vector to the arc centre (i.e. pivot)
    if (angularVelocity > 0f)
    {
      // Rotating anticlockwise
      radiusVectToPivot = new Vector2(-velocity.y, velocity.x).normalized * arcRadius;
      pivot = samplePosition + radiusVectToPivot;
    }
    else
    {
      // Rotating clockwise
      radiusVectToPivot = new Vector2(velocity.y, -velocity.x).normalized * arcRadius;
      pivot = samplePosition - radiusVectToPivot;
    }

    // For each sample
    for (int sample = 1; sample < samples; sample++)
    {
      // Determine angle of arc as proportion of total arc angle to be followed
      float sampleAngle = angularVelocity * sample / (samples - 1) * guideTime;

      if (angularVelocity > 0f)
      {
        // Rotating anticlockwise
        radiusVectToArc = Quaternion.Euler(0f, 0f, sampleAngle) * radiusVectToPivot;
        samplePosition = pivot - radiusVectToArc;
      }
      else if (angularVelocity < 0f)
      {
        // Rotating clockwise
        radiusVectToArc = Quaternion.Euler(0f, 0f, sampleAngle) * radiusVectToPivot;
        samplePosition = pivot + radiusVectToArc;
      }
      else
      {
        // Not rotating (edge case)
        samplePosition = samplePosition + velocity;
      }

      // Update guideline with the sampled position
      guideSpline.SetKnot(sample, new BezierKnot(new float3(samplePosition.x, samplePosition.y, 0f)));
    }
  }
}
