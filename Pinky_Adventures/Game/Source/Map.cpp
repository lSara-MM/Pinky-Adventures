
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
                if (tileId == 246) {
                    map[i] = 1;
                }

                else {
                    map[i] = 0;

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
                   
                    if (gid == 248 && !secretCoins)
                    {
                        coins->CreateCoins(gid, x, y);
                    }
                    else if (gid != 248 && gid != 0)
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
    secretCoins = true;
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

void Map::DrawPaths()
{
    if (mapLoaded == false)
        return;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);

                    if (gid != 0)
                    {
                        TileSet* tileset = GetTilesetFromTileId(gid);

                        SDL_Rect r = tileset->GetTileRect(gid);
                        iPoint pos = MapToWorld(x, y);

                        app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);
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

iPoint Map::WorldToMap(int x, int y)
{
    iPoint ret(0, 0);

    ret.x = x / mapData.tileWidth;

    ret.y = y / mapData.tileHeight;

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
    secretCoins = false;

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
  
    PhysBody * c1 = app->physics->CreateRectangle(1 + 240 / 2, 290 + 95 / 2, 240, 95, bodyType::STATIC);
    c1->ctype = ColliderType::PLATFORM;

    listBodies.Add(c1);

    PhysBody* c2 = app->physics->CreateRectangle(305 + 175 / 2, 290 + 95 / 2, 175, 95, bodyType::STATIC);
    c2->ctype = ColliderType::PLATFORM;
    listBodies.Add(c2);
    PhysBody* c3 = app->physics->CreateRectangle(256, 704 + 32, 576, 64, bodyType::STATIC);
    c3->ctype = ColliderType::PLATFORM;
    listBodies.Add(c3);
    PhysBody* c4 = app->physics->CreateRectangle(528 + 64 / 2, 240 + 32 / 2, 64, 32, bodyType::STATIC);
    c4->ctype = ColliderType::PLATFORM;
    listBodies.Add(c4);
    PhysBody* c5 = app->physics->CreateRectangle(624 + 16 / 2, 368 + 16 / 2, 16, 16, bodyType::STATIC);
    c5->ctype = ColliderType::PLATFORM;
    listBodies.Add(c5);
    PhysBody* c6 = app->physics->CreateRectangle(624 + 144 / 2, 352 + 16 / 2, 144, 16, bodyType::STATIC);
    c6->ctype = ColliderType::PLATFORM;
    listBodies.Add(c6);
    PhysBody* c7 = app->physics->CreateRectangle(752 + 48 / 2, 336 + 16 / 2, 48, 16, bodyType::STATIC);
    c7->ctype = ColliderType::PLATFORM;
    listBodies.Add(c7);


    PhysBody* c8 = app->physics->CreateRectangle(785 + 32 / 2, 320 + 16 / 2, 32, 16, bodyType::STATIC);
    c8->ctype = ColliderType::PLATFORM;
    listBodies.Add(c8);

    PhysBody* c9 = app->physics->CreateRectangle(800 + 32 / 2, 304 + 16 / 2, 32, 16, bodyType::STATIC);
    c9->ctype = ColliderType::PLATFORM;
    listBodies.Add(c9);

    PhysBody* c10 = app->physics->CreateRectangle(816 + 32 / 2, 288 + 16 / 2, 32, 16, bodyType::STATIC);
    c10->ctype = ColliderType::PLATFORM;
    listBodies.Add(c10);
    PhysBody* c11 = app->physics->CreateRectangle(832 + 32 / 2, 272 + 16 / 2, 32, 16, bodyType::STATIC);
    c11->ctype = ColliderType::PLATFORM;
    listBodies.Add(c11);

    PhysBody* c12 = app->physics->CreateRectangle(848 + 48 / 2, 256 + 16 / 2, 48, 16, bodyType::STATIC);
    c12->ctype = ColliderType::PLATFORM;
    listBodies.Add(c12);
    PhysBody* c13 = app->physics->CreateRectangle(880 + 16 / 2, 256 + 96 / 2, 16, 96, bodyType::STATIC);
    c13->ctype = ColliderType::PLATFORM;
    listBodies.Add(c13);
    PhysBody* c14 = app->physics->CreateRectangle(896 + 144 / 2, 336 + 16 / 2, 144, 16, bodyType::STATIC);
    c14->ctype = ColliderType::PLATFORM;
    listBodies.Add(c14);

    PhysBody* c15 = app->physics->CreateRectangle(1024 + 16 / 2, 352 + 32 / 2, 16, 32, bodyType::STATIC);
    c15->ctype = ColliderType::PLATFORM;
    listBodies.Add(c15);

    PhysBody* c16 = app->physics->CreateRectangle(656 + 64 / 2, 304 + 16 / 2, 64, 16, bodyType::STATIC);
    c16->ctype = ColliderType::PLATFORM;
    listBodies.Add(c16);

    PhysBody* c17 = app->physics->CreateRectangle(704 + 32 / 2, 288 + 16 / 2, 32, 16, bodyType::STATIC);
    c17->ctype = ColliderType::PLATFORM;
    listBodies.Add(c17);

    PhysBody* c18 = app->physics->CreateRectangle(720 + 32 / 2, 272 + 16 / 2, 32, 16, bodyType::STATIC);
    c18->ctype = ColliderType::PLATFORM;
    listBodies.Add(c18);
    PhysBody* c19 = app->physics->CreateRectangle(736 + 48 / 2, 256 + 16 / 2, 48, 16, bodyType::STATIC);
    c19->ctype = ColliderType::PLATFORM;
    listBodies.Add(c19);
    PhysBody* c20 = app->physics->CreateRectangle(768 + 32 / 2, 240 + 16 / 2, 32, 16, bodyType::STATIC);
    c20->ctype = ColliderType::PLATFORM;
    listBodies.Add(c20);
    PhysBody* c21 = app->physics->CreateRectangle(784 + 32 / 2, 224 + 16 / 2, 32, 16, bodyType::STATIC);
    c21->ctype = ColliderType::PLATFORM;
    listBodies.Add(c21);
    PhysBody* c22 = app->physics->CreateRectangle(816 + 80 / 2, 208 + 16 / 2, 80, 16, bodyType::STATIC);
    c22->ctype = ColliderType::PLATFORM;
    listBodies.Add(c22);
    PhysBody* c24 = app->physics->CreateRectangle(832 + 64 / 2, 192 + 16 / 2, 64, 16, bodyType::STATIC);
    c24->ctype = ColliderType::PLATFORM;
    listBodies.Add(c24);
    PhysBody* c25 = app->physics->CreateRectangle(640 + 208 / 2, 176 + 16 / 2, 208, 16, bodyType::STATIC);
    c25->ctype = ColliderType::PLATFORM;
    listBodies.Add(c25);
    PhysBody* c26 = app->physics->CreateRectangle(640 + 16 / 2, 192 + 128 / 2, 16, 128, bodyType::STATIC);
    c26->ctype = ColliderType::PLATFORM;
    listBodies.Add(c26);

    PhysBody* c27 = app->physics->CreateRectangle(1072 + 32 / 2, 320 + 32 / 2, 32, 32, bodyType::STATIC);
    c27->ctype = ColliderType::PLATFORM;
    listBodies.Add(c27);
    PhysBody* c28 = app->physics->CreateRectangle(1200 + 32 / 2, 256 + 32 / 2, 32, 32, bodyType::STATIC);
    c28->ctype = ColliderType::PLATFORM;
    listBodies.Add(c28);
    PhysBody* c29 = app->physics->CreateRectangle(1136 + 32 / 2, 288 + 32 / 2, 32, 32, bodyType::STATIC);
    c29->ctype = ColliderType::PLATFORM;
    listBodies.Add(c29);


    PhysBody* c30 = app->physics->CreateRectangle(1264 + 16 / 2, 288 + 96 / 2, 16, 96, bodyType::STATIC);
    c30->ctype = ColliderType::PLATFORM;
    listBodies.Add(c30);
    PhysBody* c31 = app->physics->CreateRectangle(1280 + 80 / 2, 288 + 16 / 2, 80, 16, bodyType::STATIC);
    c31->ctype = ColliderType::PLATFORM;
    listBodies.Add(c31);
    PhysBody* c32 = app->physics->CreateRectangle(1344 + 80 / 2, 304 + 16 / 2, 80, 16, bodyType::STATIC);
    c32->ctype = ColliderType::PLATFORM;
    listBodies.Add(c32);
    PhysBody* c33 = app->physics->CreateRectangle(1408 + 64 / 2, 320 + 16 / 2, 64, 16, bodyType::STATIC);
    c33->ctype = ColliderType::PLATFORM;
    listBodies.Add(c33);
    PhysBody* c34 = app->physics->CreateRectangle(1392 + 32 / 2, 64 + 192 / 2, 32, 192, bodyType::STATIC);
    c34->ctype = ColliderType::PLATFORM;
    listBodies.Add(c34);


    PhysBody* c35 = app->physics->CreateRectangle(1472 + 16 / 2, 64 + 272 / 2, 16, 272, bodyType::STATIC);
    c35->ctype = ColliderType::PLATFORM;
    listBodies.Add(c35);
    PhysBody* c36 = app->physics->CreateRectangle(1488 + 16 / 2, 224 + 160 / 2, 16, 160, bodyType::STATIC);
    c36->ctype = ColliderType::PLATFORM;
    listBodies.Add(c36);

    PhysBody* c37 = app->physics->CreateRectangle(1504 + 128 / 2, 224 + 16 / 2, 128, 16, bodyType::STATIC);
    c37->ctype = ColliderType::PLATFORM;
    listBodies.Add(c37);

    PhysBody* c38 = app->physics->CreateRectangle(1616 + 16 / 2, 96 + 144 / 2, 16, 144, bodyType::STATIC);
    c38->ctype = ColliderType::PLATFORM;
    listBodies.Add(c38);


    PhysBody* c39 = app->physics->CreateRectangle(1504 + 128 / 2, 80 + 16 / 2, 128, 16, bodyType::STATIC);
    c39->ctype = ColliderType::PLATFORM;
    listBodies.Add(c39);

    PhysBody* c40 = app->physics->CreateRectangle(1488 + 16 / 2, 64 + 32 / 2, 16, 32, bodyType::STATIC);
    c40->ctype = ColliderType::PLATFORM;
    listBodies.Add(c40);


    PhysBody* c41 = app->physics->CreateRectangle(1664 + 32 / 2, 128 + 32 / 2, 32, 32, bodyType::STATIC);
    c41->ctype = ColliderType::PLATFORM;
    listBodies.Add(c41);
    PhysBody* c42 = app->physics->CreateRectangle(1728 + 32 / 2, 192 + 32 / 2, 32, 32, bodyType::STATIC);
    c42->ctype = ColliderType::PLATFORM;
    listBodies.Add(c42);


    PhysBody* c44 = app->physics->CreateRectangle(1792 + 48 / 2, 240 + 16 / 2, 48, 16, bodyType::STATIC);
    c44->ctype = ColliderType::PLATFORM;
    listBodies.Add(c44);
    PhysBody* c45 = app->physics->CreateRectangle(1792 + 64 / 2, 256 + 16 / 2, 64, 16, bodyType::STATIC);
    c45->ctype = ColliderType::PLATFORM;
    listBodies.Add(c45);

    PhysBody* c46 = app->physics->CreateRectangle(1808 + 80 / 2, 272 + 16 / 2, 80, 16, bodyType::STATIC);
    c46->ctype = ColliderType::PLATFORM;
    listBodies.Add(c46);



    PhysBody* c47 = app->physics->CreateRectangle(1824 + 96 / 2, 288 + 16 / 2, 96, 16, bodyType::STATIC);
    c47->ctype = ColliderType::PLATFORM;
    listBodies.Add(c47);
    PhysBody* c48 = app->physics->CreateRectangle(1856 + 96 / 2, 304 + 16 / 2, 96, 16, bodyType::STATIC);
    c48->ctype = ColliderType::PLATFORM;
    listBodies.Add(c48);
    PhysBody* c49 = app->physics->CreateRectangle(1920 + 80 / 2, 336 + 16 / 2, 80, 16, bodyType::STATIC);
    c49->ctype = ColliderType::PLATFORM;
    listBodies.Add(c49);
    PhysBody* c50 = app->physics->CreateRectangle(1888 + 112 / 2, 320 + 16 / 2, 112, 16, bodyType::STATIC);
    c50->ctype = ColliderType::PLATFORM;
    listBodies.Add(c50);

    PhysBody* c51 = app->physics->CreateRectangle(1952 + 32 / 2, 64 + 208 / 2, 32, 208, bodyType::STATIC);
    c51->ctype = ColliderType::PLATFORM;
    listBodies.Add(c51);
    PhysBody* c52 = app->physics->CreateRectangle(2032 + 32 / 2, 64 + 272 / 2, 32, 272, bodyType::STATIC);
    c52->ctype = ColliderType::PLATFORM;
    listBodies.Add(c52);
    PhysBody* c53 = app->physics->CreateRectangle(2032 + 16 / 2, 320 + 64 / 2, 16, 64, bodyType::STATIC);
    c53->ctype = ColliderType::PLATFORM;
    listBodies.Add(c53);
    PhysBody* c54 = app->physics->CreateRectangle(2048 + 80 / 2, 320 + 16 / 2, 80, 16, bodyType::STATIC);
    c54->ctype = ColliderType::PLATFORM;
    listBodies.Add(c54);
    PhysBody* c55 = app->physics->CreateRectangle(2112 + 16 / 2, -80 + 400 / 2, 16, 400, bodyType::STATIC);
    c55->ctype = ColliderType::PLATFORM;
    listBodies.Add(c55);
    PhysBody* c56 = app->physics->CreateRectangle(2128 + 16 / 2, 0 + 16 / 2, 16, 16, bodyType::STATIC);
    c56->ctype = ColliderType::PLATFORM;
    listBodies.Add(c56);

    /*
    {
    PhysBody* c57 = app->physics->CreateRectangle(160 + 16 / 2, 272 + 12, 16, 8, bodyType::STATIC);
    c57->ctype = ColliderType::SPIKE;
    listBodies.Add(c57);

    PhysBody* c58 = app->physics->CreateRectangle(368 + 48 / 2, 272 + 12 , 48, 8, bodyType::STATIC);
    c58->ctype = ColliderType::SPIKE;
    listBodies.Add(c58);

    PhysBody* c59 = app->physics->CreateRectangle(480 + 144 / 2, 360 + 16 / 2, 144, 16, bodyType::STATIC);
    c59->ctype = ColliderType::SPIKE;
    listBodies.Add(c59);


    PhysBody* c61 = app->physics->CreateRectangle(688 + 32 / 2, 160 + 12 , 32, 8, bodyType::STATIC);
    c61->ctype = ColliderType::SPIKE;
    listBodies.Add(c61);
    PhysBody* c62 = app->physics->CreateRectangle(800 + 16 / 2, 160 + 12 , 16, 8, bodyType::STATIC);
    c62->ctype = ColliderType::SPIKE;
    listBodies.Add(c62);
    PhysBody* c63 = app->physics->CreateRectangle(896 + 16 / 2, 320 + 12 , 16, 8, bodyType::STATIC);
    c63->ctype = ColliderType::SPIKE;
    listBodies.Add(c63);

    PhysBody* c64 = app->physics->CreateRectangle(1264 + 32 / 2, 272 + 12 , 32, 8, bodyType::STATIC);
    c64->ctype = ColliderType::SPIKE;
    listBodies.Add(c64);
    PhysBody* c65 = app->physics->CreateRectangle(1504 + 32 / 2, 64 + 12, 32, 8, bodyType::STATIC);
    c65->ctype = ColliderType::SPIKE;
    listBodies.Add(c65);
    PhysBody* c66 = app->physics->CreateRectangle(1824 + 16 / 2, 224 + 12 , 16, 8, bodyType::STATIC);
    c66->ctype = ColliderType::SPIKE;
    listBodies.Add(c66);

    PhysBody* c67 = app->physics->CreateRectangle(1504 + 528 / 2, 361 + 16 / 2, 528, 16, bodyType::STATIC);
    c67->ctype = ColliderType::SPIKE;
    listBodies.Add(c67);
    */
    PhysBody* c68 = app->physics->CreateRectangleSensor(641 + 1 / 2, 320 + 32 / 2, 1, 32, bodyType::STATIC);
    c68->ctype = ColliderType::CHANGE;
    listBodies.Add(c68);

    PhysBody* c69 = app->physics->CreateRectangleSensor(240 + 64 / 2, 360 + 1 / 2, 64, 1, bodyType::STATIC);
    c69->ctype = ColliderType::FALL;
    listBodies.Add(c69);

    PhysBody* c70 = app->physics->CreateRectangleSensor(480 + 144 / 2, 360 + 1 / 2, 144, 1, bodyType::STATIC);
    c70->ctype = ColliderType::FALL;
    listBodies.Add(c70);

    PhysBody* c71 = app->physics->CreateRectangleSensor(1040 + 224 / 2, 360 + 1 / 2, 224, 1, bodyType::STATIC);
    c71->ctype = ColliderType::FALL;
    listBodies.Add(c71);

    PhysBody* c72 = app->physics->CreateRectangleSensor(1504 + 528 / 2, 360 + 1 / 2, 528, 1, bodyType::STATIC);
    c72->ctype = ColliderType::FALL;
    listBodies.Add(c72);

    // Sensors enemy
    PhysBody* c73 = app->physics->CreateRectangle(645, 175, 2, 3, bodyType::STATIC);
    c73->ctype = ColliderType::ENEMY_LIMIT;
    listBodies.Add(c73);

    PhysBody* c74 = app->physics->CreateRectangle(845, 175, 2, 3, bodyType::STATIC);
    c74->ctype = ColliderType::ENEMY_LIMIT;
    listBodies.Add(c74);

    PhysBody* c75 = app->physics->CreateRectangle(1537, 80, 2, 3, bodyType::STATIC);
    c75->ctype = ColliderType::ENEMY_LIMIT;
    listBodies.Add(c75);

   /* PhysBody* c76 = app->physics->CreateRectangle(1630, 80, 2, 3, bodyType::STATIC);
    c76->ctype = ColliderType::ENEMY_LIMIT;
    listBodies.Add(c76);*/

  
    // Map limits
    PhysBody* c = app->physics->CreateRectangle(2, 0, 0, 5000, bodyType::STATIC);
    c->ctype = ColliderType::PLATFORM;
    listBodies.Add(c);

    PhysBody* d = app->physics->CreateRectangle(2112, 0, 0, 5000, bodyType::STATIC);
    d->ctype = ColliderType::PLATFORM;
    listBodies.Add(d);

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


    //DrawPlatformCollider();problema de dejar espacios entre los colliders no solucionado
    DrawSpikes();

    int w, h;
    uchar* data = NULL;

    //walkability map aquí o a start scene?
    bool retWalkMap = CreateWalkabilityMap(w, h, &data);
    if (retWalkMap) app->pathfinding->SetMap(w, h, data);

    RELEASE_ARRAY(data);

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

