#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"

#include <math.h>
#include <sstream> // <- LÍNEA AÑADIDA
#include <string>  // <- LÍNEA AÑADIDA

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{
}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded) {
        for (const auto& mapLayer : mapData.layers) {
            // Se elimina la comprobación de la propiedad "Draw" para que se dibujen todas las capas
            for (int i = 0; i < mapData.height; i++) {
                for (int j = 0; j < mapData.width; j++) {
                    int gid = mapLayer->Get(i, j);
                    if (gid != 0) {
                        TileSet* tileSet = GetTilesetFromTileId(gid);
                        if (tileSet != nullptr) {
                            SDL_Rect tileRect = tileSet->GetRect(gid);
                            Vector2D mapCoord = MapToWorld(i, j);
                            Engine::GetInstance().render->DrawTexture(tileSet->texture, (int)mapCoord.getX(), (int)mapCoord.getY(), &tileRect);
                        }
                    }
                }
            }
        }
    }

    return ret;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = nullptr;
    for (const auto& tileset : mapData.tilesets) {
        // Se corrige la lógica para que devuelva el tileset correcto
        if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
            set = tileset;
            break;
        }
    }
    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    for (const auto& tileset : mapData.tilesets) {
        // Se añade una comprobación para evitar borrar una textura que no existe
        if (tileset->texture) {
            Engine::GetInstance().textures->UnLoad(tileset->texture);
            tileset->texture = nullptr;
        }
        delete tileset;
    }
    mapData.tilesets.clear();

    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {

        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

            std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

            mapData.tilesets.push_back(tileSet);
        }

        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            LoadProperties(layerNode, mapLayer->properties);

            // Se utiliza un stringstream para parsear los datos de la capa
            std::string data = layerNode.child("data").text().get();
            std::stringstream ss(data);
            std::string token;
            while (std::getline(ss, token, ',')) {
                mapLayer->tiles.push_back(std::stoi(token));
            }


            mapData.layers.push_back(mapLayer);
        }

        for (const auto& mapLayer : mapData.layers) {
            // Se cambia el nombre de la capa a "Capa de patrones 1" y se crean colisiones para cualquier tile no vacío
            if (mapLayer->name == "Capa de patrones 1") {
                for (int i = 0; i < mapData.height; i++) {
                    for (int j = 0; j < mapData.width; j++) {
                        int gid = mapLayer->Get(i, j);
                        if (gid != 0) {
                            Vector2D mapCoord = MapToWorld(i, j);
                            PhysBody* c1 = Engine::GetInstance().physics.get()->CreateRectangle(mapCoord.getX() + mapData.tileWidth / 2, mapCoord.getY() + mapData.tileHeight / 2, mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::PLATFORM;
                        }
                    }
                }
            }
        }

        ret = true;

        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);
            LOG("Tilesets----");

            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }

            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
                LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }

        if (mapFileXML) mapFileXML.reset();

    }

    mapLoaded = ret;
    return ret;
}

Vector2D Map::MapToWorld(int i, int j) const
{
    Vector2D ret;

    ret.setX((float)(j * mapData.tileWidth));
    ret.setY((float)(i * mapData.tileHeight));

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

        properties.propertyList.push_back(p);
    }

    return ret;
}