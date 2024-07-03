using UnityEngine;
using UnityEngine.Tilemaps;

public class DepthCheck : MonoBehaviour
{
    [SerializeField] private Tilemap waterMap;

    public float GetDepth() {
        Vector3Int tilePos = waterMap.layoutGrid.WorldToCell(transform.position);
        TileBase tile = waterMap.GetTile(tilePos);

        if(tile is WaterTile) {
            return ((WaterTile)tile).GetDepth();
        } else {
            return 0f;
        }
    }
}
