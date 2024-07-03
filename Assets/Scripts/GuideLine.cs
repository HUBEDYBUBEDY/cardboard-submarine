using UnityEngine;

public class GuideLine : MonoBehaviour
{
    [SerializeField] private float maxLength = 3f;
    [SerializeField] private LineRenderer lineRenderer;

    void Start() {
        lineRenderer = GetComponent<LineRenderer>();
    }

    public void updateLine(Vector3 velocity, float speedProp, float angularVelocity) {
        // Scale line to match current speed
        float newLength = maxLength * speedProp;
        velocity = Vector3.Normalize(velocity) * newLength;
        lineRenderer.SetPositions(new Vector3[] {Vector3.zero, velocity});

        // Rotate by angular velocity
        transform.rotation = Quaternion.Euler(0f, 0f, angularVelocity);
    }
}
