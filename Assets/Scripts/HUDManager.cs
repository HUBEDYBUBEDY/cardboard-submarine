using System;
using TMPro;
using UnityEngine;

public class HUDManager : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI speedText;
    [SerializeField] private TextMeshProUGUI bearingText;
    [SerializeField] private TextMeshProUGUI depthText;

    public void updateText(float speed, float bearing, float depth) {
        // Average speed is 25kn or 12.85m/s, each kn is 0.514m/s
        speedText.text = "Speed: " + Math.Truncate(speed).ToString() + "kn";
        // Angle clockwise from north 0-360
        bearingText.text = "Bearing: " + Math.Truncate(bearing).ToString() + "°";
        // Nuclear submarines operational depth 300m, up to 500m
        depthText.text = "Depth: " + Math.Truncate(depth).ToString() + "m";
    }
}
