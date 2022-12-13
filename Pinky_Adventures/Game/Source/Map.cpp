
#include "App.h"
#include "Render.h"
#include "Window.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

#include "Pathfinding.h"
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

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;
    ListItem<MapLayer*>* item;
    item = mapData.maplayers.start;

    for (item = mapData.maplayers.start; item != NULL; item = item->next)
    {
        MapLayer* layer = item->data;

        if (!layer->properties.GetProperty("Navigation")->value)
            continue;

        uchar* map = new uchar[layer->width * layer->height];
        memset(map, 1, layer->width * layer->height);

        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                int i = (y * layer->width) + x;

                int tileId = layer->Get(x, y);
                TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

                if (tileset != NULL)
                {
                    map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
                }
            }
        }

        *buffer = map;
        width = mapData.width;
        height = mapData.height;
        ret = true;

        break;
    }

    return ret;
}

void Map::Draw()
{
    if(mapLoaded == false)
        return;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {
            
            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                   
                    int gid = mapLayerItem->data->Get(x, y);

              
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

void Map::DrawSecret()
{
    if (mapLoaded == false)
        return;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Secret") != NULL && mapLayerItem->data->properties.GetProperty("Secret")->value ) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {                
                    int gid = mapLayerItem->data->Get(x, y);
                   
                    if (gid == 248 && a == false)
                    {
                        coins->CreateCoins(gid, x, y);
                    }
                    else if (gid != 248)
                    {
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
        }
        mapLayerItem = mapLayerItem->next;
    }
    a = true;
}

void Map::DrawPlatformCollider() {

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    ListItem<PhysBody*>* bodyItem;
    bodyItem = listBodies.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->name == "collisionsTerrain") {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);

                    if (gid == 244)
                    {
                        TileSet* tileset = GetTilesetFromTileId(gid);

                        SDL_Rect r = tileset->GetTileRect(gid);

                        iPoint pos = MapToWorld(x, y);

                        PhysBody* collider;

                        collider = app->physics->CreateRectangle(pos.x + r.w * 0.5 , pos.y + r.h * 0.5 , r.w, r.h, bodyType::STATIC);

                        collider->ctype = ColliderType::PLATFORM;

                        listBodies.Add(collider);
                    }
                }
            }
        }
        mapLayerItem = mapLayerItem->next;
    }
}

void Map::DrawSpikes()
{
    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    ListItem<PhysBody*>* bodyItem;
    bodyItem = listBodies.start;

    while (mapLayerItem != NULL) {
        if (mapLayerItem->data->name == "collisionsSpike") {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);

                    if (gid == 245)
                    {
                        TileSet* tileset = GetTilesetFromTileId(gid);

                        SDL_Rect r = tileset->GetTileRect(gid);

                        iPoint pos = MapToWorld(x, y);

                        PhysBody* collider;

                        collider = app->physics->CreateRectangle(pos.x + r.w / 2, pos.y + r.h, r.w, r.h, bodyType::STATIC);

                        collider->ctype = ColliderType::SPIKE;

                        listBodies.Add(collider);
                    }
                }
            }
        }
        mapLayerItem = mapLayerItem->next;
    }
}

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

   
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}

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
    a = false;

    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    while (item != NULL)
    {
        app->tex->UnLoad(item->data->texture);
        RELEASE(item->data);
        item = item->next;
    }
    mapData.tilesets.Clear();

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }
    mapData.maplayers.Clear();

    ListItem<PhysBody*>* bodyItem;
    bodyItem = listBodies.start;

    while (bodyItem != NULL)
    {
        app->render->name;
        //RELEASE(bodyItem->data);
        //bodyItem->data->body->GetWorld()->DestroyBody(bodyItem->data->body);
        app->render->active;
        delete bodyItem->data;
        bodyItem = bodyItem->next;
    }
    listBodies.Clear();

    app->render->active;
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

    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }
  

    PhysBody* c68 = app->physics->CreateRectangleSensor(641 + 1 / 2, 320 + 32 / 2, 1, 32, bodyType::STATIC);
    c68->ctype = ColliderType::CHANGE;
    listBodies.Add(c68);

    PhysBody* c69 = app->physics->CreateRectangle(240 + 64 / 2, 383 + 1 / 2, 64, 1, bodyType::STATIC);
    c69->ctype = ColliderType::FALL;
    listBodies.Add(c69);

    PhysBody* c70 = app->physics->CreateRectangle(1040 + 224 / 2, 382 + 1 / 2, 224, 1, bodyType::STATIC);
    c70->ctype = ColliderType::FALL;
    listBodies.Add(c70);

    PhysBody* c = app->physics->CreateRectangle(2, 0, 0, 1000, bodyType::STATIC);//l�mit mapa
    c->ctype = ColliderType::PLATFORM;
    listBodies.Add(c);

    if(ret == true)
    {
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


    DrawPlatformCollider();
    DrawSpikes();

    int w, h;
    uchar* data = NULL;

    //walkability map aqu� o a start scene?
    bool retWalkMap = CreateWalkabilityMap(w, h, &data);
    if (retWalkMap) app->pathfinding->SetMap(w, h, data);

    RELEASE_ARRAY(data);
    //


    return ret;
}

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

bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();
       
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

    
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();


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

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); 
        
        properties.list.Add(p);
    }

    return ret;
}

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
