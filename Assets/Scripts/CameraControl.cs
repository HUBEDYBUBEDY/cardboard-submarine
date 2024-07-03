using UnityEngine;

public class CameraControl : MonoBehaviour
{
    [SerializeField] private float HUDwidth = 400f;
    [SerializeField] private Transform target;
    [SerializeField] private Vector3 offset;

    // Start is called before the first frame update
    void Start() {
        transform.position = target.position;
        Camera camera = GetComponent<Camera>();
        float midpoint = (camera.pixelWidth-HUDwidth) / 2;
        midpoint = (camera.pixelWidth/2) - midpoint;

        offset = new Vector3(camera.ScreenToWorldPoint(new Vector3((camera.pixelWidth/2)-midpoint, 0f, 0f)).x, 0f, -10f);
    }

    // Update is called once per frame
    void LateUpdate() {
        transform.position = target.position + offset;
    }
}
