using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace UnityEngine.Tilemaps {

public class WaterTile : Tile
{
    public bool isReference = true;

    [SerializeField] private Sprite tileSprite;
    [SerializeField] private float depth = 0;

    public override void RefreshTile(Vector3Int position, ITilemap tilemap)
    {
        base.RefreshTile(position, tilemap);
    }

    public override void GetTileData(Vector3Int position, ITilemap tilemap, ref TileData tileData)
    {
        base.GetTileData(position, tilemap, ref tileData);
        tileData.sprite = tileSprite;
    }

    public float GetDepth() {
        return depth;
    }

#if UNITY_EDITOR
    [MenuItem("Assets/Create/HubedyTools/Custom Tiles/Water Tile")]
    public static void CreateCountingTile()
    {
        string path = EditorUtility.SaveFilePanelInProject("Save Water Tile", "New Water Tile", "Asset", "Save Water Tile", "Assets");
        if (path == "") return;

        AssetDatabase.CreateAsset(CreateInstance<WaterTile>(), path);
    }
#endif
}

}
