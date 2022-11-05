
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();

    return ret;
}

void Map::Draw()
{
    if(mapLoaded == false)
        return;

    /*
    // L04: DONE 6: Iterate all tilesets and draw all their 
    // images in 0,0 (you should have only one tileset for now)

    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;

    while (tileset != NULL) {
        app->render->DrawTexture(tileset->data->texture,0,0);
        tileset = tileset->next;
    }
    */

    // L05: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        //L06: DONE 7: use GetProperty method to ask each layer if your “Draw” property is true.
        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = mapLayerItem->data->Get(x, y);

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }

        mapLayerItem = mapLayerItem->next;

    }
    
}

void Map::DrawPlatformCollider() {

    if (mapLoaded == false)
        return;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    ListItem<PhysBody*>* bodyItem;
    bodyItem = listBodies.start;

    int contador = 0;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Block") != NULL && mapLayerItem->data->properties.GetProperty("Block")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    
                    if (mapLayerItem->data->data[contador]!=NULL) {

                        int gid = mapLayerItem->data->Get(x, y);

                        TileSet* tileset = GetTilesetFromTileId(gid);

                        SDL_Rect r = tileset->GetTileRect(gid);

                        iPoint pos = MapToWorld(x, y);

                        PhysBody* collider;

                        collider = app->physics->CreateRectangle(pos.x + r.w / 2, pos.y + r.h / 2, r.w, r.h, bodyType::STATIC);

                        collider->ctype = ColliderType::PLATFORM;

                        listBodies.Add(collider);
                      
                    }
                
                  
                }

               
            }

        }
        mapLayerItem = mapLayerItem->next;

    }

}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    // L05: DONE 7: Get relative Tile rectangle
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}


// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }


    ListItem<PhysBody*>* bodyItem;
    bodyItem = listBodies.start;

    while (bodyItem != NULL)
    {
        RELEASE(bodyItem->data);
        bodyItem = bodyItem->next;
    }

    return true;
}

// Load new map
bool Map::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }

    PhysBody* c1 = app->physics->CreateRectangle(1 + 240 / 2, 290 + 95 / 2, 240, 95, bodyType::STATIC);
    c1->ctype = ColliderType::PLATFORM;

    PhysBody* c2 = app->physics->CreateRectangle(305 + 175 / 2, 290 + 95 / 2, 175, 95, bodyType::STATIC);
    c2->ctype = ColliderType::PLATFORM;

    PhysBody* c3 = app->physics->CreateRectangle(256, 704 + 32, 576, 64, bodyType::STATIC);
    c3->ctype = ColliderType::PLATFORM;

    PhysBody* c4 = app->physics->CreateRectangle(528 + 64 / 2, 240 + 32 / 2, 64, 32, bodyType::STATIC);
    c4->ctype = ColliderType::PLATFORM;

    PhysBody* c5 = app->physics->CreateRectangle(624 + 16 / 2, 368 + 16 / 2, 16, 16, bodyType::STATIC);
    c5->ctype = ColliderType::PLATFORM;

    PhysBody* c6 = app->physics->CreateRectangle(624 + 144 / 2, 352 + 16 / 2, 144, 16, bodyType::STATIC);
    c6->ctype = ColliderType::PLATFORM;

    PhysBody* c7 = app->physics->CreateRectangle(752 + 48 / 2, 336 + 16 / 2, 48, 16, bodyType::STATIC);
    c7->ctype = ColliderType::PLATFORM;



    PhysBody* c8 = app->physics->CreateRectangle(785 + 32 / 2, 320 + 16 / 2, 32, 16, bodyType::STATIC);
    c8->ctype = ColliderType::PLATFORM;


    PhysBody* c9 = app->physics->CreateRectangle(800 + 32 / 2, 304 + 16 / 2, 32, 16, bodyType::STATIC);
    c9->ctype = ColliderType::PLATFORM;


    PhysBody* c10 = app->physics->CreateRectangle(816 + 32 / 2, 288 + 16 / 2, 32, 16, bodyType::STATIC);
    c10->ctype = ColliderType::PLATFORM;

    PhysBody* c11 = app->physics->CreateRectangle(832 + 32 / 2, 272 + 16 / 2, 32, 16, bodyType::STATIC);
    c11->ctype = ColliderType::PLATFORM;


    PhysBody* c12 = app->physics->CreateRectangle(848 + 48 / 2, 256 + 16 / 2, 48, 16, bodyType::STATIC);
    c12->ctype = ColliderType::PLATFORM;

    PhysBody* c13 = app->physics->CreateRectangle(880 + 16 / 2, 256 + 96 / 2, 16, 96, bodyType::STATIC);
    c13->ctype = ColliderType::PLATFORM;

    PhysBody* c14 = app->physics->CreateRectangle(896 + 144 / 2, 336 + 16 / 2, 144, 16, bodyType::STATIC);
    c14->ctype = ColliderType::PLATFORM;


    PhysBody* c15 = app->physics->CreateRectangle(1024 + 16 / 2, 352 + 32 / 2, 16, 32, bodyType::STATIC);
    c15->ctype = ColliderType::PLATFORM;


    PhysBody* c16 = app->physics->CreateRectangle(640 + 80 / 2, 304 + 16 / 2, 80, 16, bodyType::STATIC);
    c16->ctype = ColliderType::PLATFORM;


    PhysBody* c17 = app->physics->CreateRectangle(704 + 32 / 2, 288 + 16 / 2, 32, 16, bodyType::STATIC);
    c8->ctype = ColliderType::PLATFORM;


    PhysBody* c18 = app->physics->CreateRectangle(720 + 32 / 2, 272 + 16 / 2, 32, 16, bodyType::STATIC);
    c18->ctype = ColliderType::PLATFORM;

    PhysBody* c19 = app->physics->CreateRectangle(736 + 48 / 2, 256 + 16 / 2, 48, 16, bodyType::STATIC);
    c19->ctype = ColliderType::PLATFORM;

    PhysBody* c20 = app->physics->CreateRectangle(768 + 32 / 2, 240 + 16 / 2, 32, 16, bodyType::STATIC);
    c20->ctype = ColliderType::PLATFORM;

    PhysBody* c21 = app->physics->CreateRectangle(784 + 32 / 2, 224 + 16 / 2, 32, 16, bodyType::STATIC);
    c21->ctype = ColliderType::PLATFORM;

    PhysBody* c22 = app->physics->CreateRectangle(816 + 80 / 2, 208 + 16 / 2, 80, 16, bodyType::STATIC);
    c22->ctype = ColliderType::PLATFORM;

    PhysBody* c24 = app->physics->CreateRectangle(832 + 64 / 2, 192 + 16 / 2, 64, 16, bodyType::STATIC);
    c24->ctype = ColliderType::PLATFORM;

    PhysBody* c25 = app->physics->CreateRectangle(640 + 208 / 2, 176 + 16 / 2, 208, 16, bodyType::STATIC);
    c25->ctype = ColliderType::PLATFORM;

    PhysBody* c26 = app->physics->CreateRectangle(640 + 16 / 2, 192 + 112 / 2, 16, 112, bodyType::STATIC);
    c26->ctype = ColliderType::PLATFORM;


    PhysBody* c27 = app->physics->CreateRectangle(1072 + 32 / 2, 320 + 32 / 2, 32, 32, bodyType::STATIC);
    c27->ctype = ColliderType::PLATFORM;
   
    PhysBody* c28 = app->physics->CreateRectangle(1200 + 32 / 2, 256 + 32 / 2, 32, 32, bodyType::STATIC);
    c28->ctype = ColliderType::PLATFORM;

    PhysBody* c29 = app->physics->CreateRectangle(1136 + 32 / 2, 288 + 32 / 2, 32, 32, bodyType::STATIC);
    c29->ctype = ColliderType::PLATFORM;



    PhysBody* c30 = app->physics->CreateRectangle(1264 + 16 / 2, 288 + 96 / 2, 16, 96, bodyType::STATIC);
    c30->ctype = ColliderType::PLATFORM;

    PhysBody* c31 = app->physics->CreateRectangle(1280 + 80 / 2, 288 + 16 / 2, 80, 16, bodyType::STATIC);
    c31->ctype = ColliderType::PLATFORM;

    PhysBody* c32 = app->physics->CreateRectangle(1344 + 80 / 2, 304 + 16 / 2, 80, 16, bodyType::STATIC);
    c32->ctype = ColliderType::PLATFORM;

    PhysBody* c33 = app->physics->CreateRectangle(1408 + 64 / 2, 320 + 16 / 2, 64, 16, bodyType::STATIC);
    c33->ctype = ColliderType::PLATFORM;

    PhysBody* c34 = app->physics->CreateRectangle(1392 + 32 / 2, 64 + 192 / 2, 32, 192, bodyType::STATIC);
    c34->ctype = ColliderType::PLATFORM;



    PhysBody* c35 = app->physics->CreateRectangle(1472 + 16 / 2, 64 + 272 / 2, 16, 272, bodyType::STATIC);
    c35->ctype = ColliderType::PLATFORM;

    PhysBody* c36 = app->physics->CreateRectangle(1488 + 16 / 2, 224 + 160 / 2, 16, 160, bodyType::STATIC);
    c36->ctype = ColliderType::PLATFORM;


    PhysBody* c37 = app->physics->CreateRectangle(1504 + 128 / 2, 224 + 16 / 2, 128, 16, bodyType::STATIC);
    c37->ctype = ColliderType::PLATFORM;


    PhysBody* c38 = app->physics->CreateRectangle(1616 + 16 / 2, 80 + 160 / 2, 16, 160, bodyType::STATIC);
    c38->ctype = ColliderType::PLATFORM;



    PhysBody* c39 = app->physics->CreateRectangle(1504 + 112 / 2, 80 + 16 / 2, 112, 16, bodyType::STATIC);
    c39->ctype = ColliderType::PLATFORM;


    PhysBody* c40 = app->physics->CreateRectangle(1488 + 16 / 2, 64 + 32 / 2, 16, 32, bodyType::STATIC);
    c40->ctype = ColliderType::PLATFORM;



    PhysBody* c41 = app->physics->CreateRectangle(1648 + 32 / 2, 128 + 32 / 2, 32, 32, bodyType::STATIC);
    c41->ctype = ColliderType::PLATFORM;

    PhysBody* c42 = app->physics->CreateRectangle(1696 + 32 / 2, 160 + 32 / 2, 32, 32, bodyType::STATIC);
    c42->ctype = ColliderType::PLATFORM;
    PhysBody* c43 = app->physics->CreateRectangle(1744 + 32 / 2, 192 + 32 / 2, 32, 32, bodyType::STATIC);
    c43->ctype = ColliderType::PLATFORM;

 

    PhysBody* c44 = app->physics->CreateRectangle(1792 + 48 / 2, 240 + 16 / 2, 48, 16, bodyType::STATIC);
    c44->ctype = ColliderType::PLATFORM;

    PhysBody* c45 = app->physics->CreateRectangle(1792 + 64 / 2, 256 + 16 / 2, 64, 16, bodyType::STATIC);
    c45->ctype = ColliderType::PLATFORM;


    PhysBody* c46 = app->physics->CreateRectangle(1808 + 80 / 2, 272 + 16 / 2, 80, 16, bodyType::STATIC);
    c46->ctype = ColliderType::PLATFORM;




    PhysBody* c47 = app->physics->CreateRectangle(1824 + 96 / 2, 288 + 16 / 2, 96, 16, bodyType::STATIC);
    c47->ctype = ColliderType::PLATFORM;

    PhysBody* c48 = app->physics->CreateRectangle(1856 + 96 / 2, 304 + 16 / 2, 96, 16, bodyType::STATIC);
    c48->ctype = ColliderType::PLATFORM;

    PhysBody* c49 = app->physics->CreateRectangle(1920 + 80 / 2, 336 + 16 / 2, 80, 16, bodyType::STATIC);
    c49->ctype = ColliderType::PLATFORM;

    PhysBody* c50 = app->physics->CreateRectangle(1888 + 112 / 2, 320 + 16 / 2, 112, 16, bodyType::STATIC);
    c50->ctype = ColliderType::PLATFORM;


    PhysBody* c51 = app->physics->CreateRectangle(1952 + 32 / 2, 64 + 208 / 2, 32, 208, bodyType::STATIC);
    c51->ctype = ColliderType::PLATFORM;

    PhysBody* c52 = app->physics->CreateRectangle(2032 + 32 / 2, 64 + 272 / 2, 32, 272, bodyType::STATIC);
    c52->ctype = ColliderType::PLATFORM;

    PhysBody* c53 = app->physics->CreateRectangle(2032 + 16 / 2, 320 + 64 / 2, 16, 64, bodyType::STATIC);
    c53->ctype = ColliderType::PLATFORM;

    PhysBody* c54 = app->physics->CreateRectangle(2048 + 80 / 2, 320 + 16 / 2, 80, 16, bodyType::STATIC);
    c54->ctype = ColliderType::PLATFORM;

    PhysBody* c55 = app->physics->CreateRectangle(2112 + 16 / 2, 0 + 320 / 2, 16, 320, bodyType::STATIC);
    c55->ctype = ColliderType::PLATFORM;

    PhysBody* c56 = app->physics->CreateRectangle(2128 + 16 / 2, 0 + 16 / 2, 16, 16, bodyType::STATIC);
    c56->ctype = ColliderType::PLATFORM;




    PhysBody* c57 = app->physics->CreateRectangle(160 + 16 / 2, 272 + 12, 16, 8, bodyType::STATIC);
    c57->ctype = ColliderType::SPIKE;


    PhysBody* c58 = app->physics->CreateRectangle(368 + 48 / 2, 272 + 12 , 48, 8, bodyType::STATIC);
    c58->ctype = ColliderType::SPIKE;


    PhysBody* c59 = app->physics->CreateRectangle(480 + 144 / 2, 361 + 28 / 2, 144, 28, bodyType::STATIC);
    c59->ctype = ColliderType::SPIKE;

    PhysBody* c60 = app->physics->CreateRectangle(240 + 64 / 2, 384 , 64, 0, bodyType::STATIC);
    c60->ctype = ColliderType::SPIKE;

    PhysBody* c61 = app->physics->CreateRectangle(688 + 32 / 2, 160 + 12 , 32, 8, bodyType::STATIC);
    c61->ctype = ColliderType::SPIKE;

    PhysBody* c62 = app->physics->CreateRectangle(800 + 16 / 2, 160 + 12 , 16, 8, bodyType::STATIC);
    c62->ctype = ColliderType::SPIKE;

    PhysBody* c63 = app->physics->CreateRectangle(896 + 16 / 2, 320 + 12 , 16, 8, bodyType::STATIC);
    c63->ctype = ColliderType::SPIKE;


    PhysBody* c64 = app->physics->CreateRectangle(1264 + 32 / 2, 272 + 12 , 32, 8, bodyType::STATIC);
    c64->ctype = ColliderType::SPIKE;

    PhysBody* c65 = app->physics->CreateRectangle(1504 + 32 / 2, 64 + 12, 32, 8, bodyType::STATIC);
    c65->ctype = ColliderType::SPIKE;

    PhysBody* c66 = app->physics->CreateRectangle(1824 + 16 / 2, 224 + 12 , 16, 8, bodyType::STATIC);
    c66->ctype = ColliderType::SPIKE;


    PhysBody* c67 = app->physics->CreateRectangle(1504 + 528 / 2, 360 + 28 / 2, 528, 28, bodyType::STATIC);
    c67->ctype = ColliderType::SPIKE;

    PhysBody* c68 = app->physics->CreateRectangle(1040 + 224 / 2, 384, 224, 0, bodyType::STATIC);
    c68->ctype = ColliderType::SPIKE;


    PhysBody* c = app->physics->CreateRectangle(2, 0, 0, 1000, bodyType::STATIC);//limit mapa
    c->ctype = ColliderType::PLATFORM;


    if(ret == true)
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything
       
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //LoadGidValue(node, layer->gids);



    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!
        
        properties.list.Add(p);
    }

    return ret;
}


// L06: DONE 7: Ask for the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}


