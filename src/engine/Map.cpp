/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <PL/platform_filesystem.h>
#include <PL/platform_graphics_camera.h>

#include "engine.h"
#include "Map.h"
#include "model.h"
#include "ModSupport.h"

#include "script/script.h"

#include "client/frontend.h"
#include "client/display.h"

#include "game/ActorManager.h"
#include "script/ScriptConfig.h"
#include "MapManager.h"

#if 0
/* for now these are hard-coded, but
 * eventually we'll do this through a
 * script instead */
MapManifest map_descriptors[]={
        // Single-player

        //{"camp", "Boot camp", MAP_MODE_SINGLEPLAYER},
        {"estu", "The War Foundation", MAP_MODE_SINGLEPLAYER},
        {"road", "Routine Patrol", MAP_MODE_SINGLEPLAYER},
        {"trench", "Trench Warfare", MAP_MODE_SINGLEPLAYER},
        {"devi", "Morning Glory!", MAP_MODE_SINGLEPLAYER},
        {"rumble", "Island Invasion", MAP_MODE_SINGLEPLAYER},
        {"zulus", "Under Siege", MAP_MODE_SINGLEPLAYER},
        {"twin", "Communication Breakdown", MAP_MODE_SINGLEPLAYER},
        {"sniper", "The Spying Game", MAP_MODE_SINGLEPLAYER},
        {"mashed", "The Village People", MAP_MODE_SINGLEPLAYER},
        {"guns", "Bangers 'N' Mash", MAP_MODE_SINGLEPLAYER},
        {"liberate", "Saving Private Rind", MAP_MODE_SINGLEPLAYER},
        {"oasis", "Just Deserts", MAP_MODE_SINGLEPLAYER},
        {"fjords", "Glacier Guns", MAP_MODE_SINGLEPLAYER},
        {"eyrie", "Battle Stations", MAP_MODE_SINGLEPLAYER},
        {"bay", "Fortified Swine", MAP_MODE_SINGLEPLAYER},
        {"medix", "Over The Top", MAP_MODE_SINGLEPLAYER},
        {"bridge", "Geneva Convention", MAP_MODE_SINGLEPLAYER},
        {"desval", "I Spy...", MAP_MODE_SINGLEPLAYER},
        {"snake", "Chemical Compound", MAP_MODE_SINGLEPLAYER},
        {"emplace", "Achilles Heal", MAP_MODE_SINGLEPLAYER},
        {"supline", "High And Dry", MAP_MODE_SINGLEPLAYER},
        {"keep", "Assassination", MAP_MODE_SINGLEPLAYER},
        {"tester", "Hero Warship", MAP_MODE_SINGLEPLAYER},
        {"foot", "Hamburger Hill", MAP_MODE_SINGLEPLAYER},
        {"final", "Well, Well, Well!", MAP_MODE_SINGLEPLAYER},

        // Multi-player

        {"iceflow", "Ice-Flow", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        //{"archi", "You Hillock", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"dbowl", "Death Bowl", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"mlake", "Frost Fight", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"lake", "The Lake", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ice", "Chill Hill", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"sepia1", "Square Off", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"oneway", "One Way System", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ridge", "Ridge Back", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"island", "Island Hopper", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},

        {"play1", "Play Pen", MAP_MODE_SURVIVAL_NOVICE},
        {"play2", "Duvet Fun", MAP_MODE_SURVIVAL_NOVICE},
        {"lunar1", "Pigs in Space", MAP_MODE_SURVIVAL_NOVICE},
        {"hell3", "Skulduggery", MAP_MODE_SURVIVAL_NOVICE},
        {"hell2", "Pigin' Hell", MAP_MODE_SURVIVAL_NOVICE},
        {"creepy2", "Graveyard Shift", MAP_MODE_SURVIVAL_NOVICE},

        {"boom", "Friendly Fire", MAP_MODE_SURVIVAL_STRATEGY},
        {"lecprod", "Bridge The Gap", MAP_MODE_SURVIVAL_STRATEGY},
        {"bhill", "Dam Busters", MAP_MODE_SURVIVAL_STRATEGY},
        {"bute", "Moon Buttes", MAP_MODE_SURVIVAL_STRATEGY},
        {"maze", "Hedge Maze", MAP_MODE_SURVIVAL_STRATEGY},
        {"cmass", "Cratermass", MAP_MODE_SURVIVAL_STRATEGY},
        {"artgun", "Doomed", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval", "Death Valley", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval2", "Death Valley 2", MAP_MODE_SURVIVAL_STRATEGY},

        {"demo", NULL, 0},
        {"demo2", NULL, 0},
        {"easy", NULL, 0},

        {"genbrack", NULL, MAP_MODE_GENERATED},
        {"genchalk", NULL, MAP_MODE_GENERATED},
        {"gendesrt", NULL, MAP_MODE_GENERATED},
        {"genlava", NULL, MAP_MODE_GENERATED},
        {"genmud", NULL, MAP_MODE_GENERATED},
        {"gensnow", NULL, MAP_MODE_GENERATED},

        {"hillbase", NULL, 0},

        {"airaid", NULL, 0},
        {"canal", NULL, 0},
        {"climb", NULL, 0},
        {"faceoff", NULL, 0},
        {"hell1", NULL, 0},
        {"hiroad", NULL, 0},
        {"hwd", NULL, 0},
        {"newsnu", NULL, 0},
        {"pdrag", NULL, 0},
        {"quack", NULL, 0},
        {"tdd", NULL, 0},
};
#endif

//Precalculated vertices for chunk rendering
//TODO: Share one index buffer instance between all chunks
const static uint16_t chunkIndices[96] = {
     0,  5,  1,  1,  5,  6,
     1,  6,  2,  2,  6,  7,
     2,  7,  3,  3,  7,  8,
     3,  8,  4,  4,  8,  9,

     5, 10,  6,  6, 10, 11,
     6, 11,  7,  7, 11, 12,
     7, 12,  8,  8, 12, 13,
     8, 13,  9,  9, 13, 14,

    10, 15, 11, 11, 15, 16,
    11, 16, 12, 12, 16, 17,
    12, 17, 13, 13, 17, 18,
    13, 18, 14, 14, 18, 19,

    15, 20, 16, 16, 20, 21,
    16, 21, 17, 17, 21, 22,
    17, 22, 18, 18, 22, 23,
    18, 23, 19, 19, 23, 24,
};

Map::Map(const std::string &name) {
    LogDebug("Loading map, %s...\n", name.c_str());

    manifest_ = MapManager::GetInstance()->GetManifest(name);
    if(manifest_ == nullptr) {
        throw std::runtime_error("Failed to get map descriptor, \"" + name + "\"\n");
    }

#if 0
    if(mode.game_mode != MAP_MODE_EDITOR && !(desc->flags & mode.game_mode)) {
        throw std::runtime_error("This mode is unsupported by this map, " + std::string(mode.map) + ", aborting!\n");
    }
#endif

    std::string base_path = "maps/" + name + "/";
    std::string p = u_find(std::string(base_path + name + ".pmg").c_str());
    if(!plFileExists(p.c_str())) {
        throw std::runtime_error("PMG, " + p + ", doesn't exist!\n");
    }

    LoadTiles(p);

    p = u_find(std::string(base_path + name + ".pog").c_str());
    if (!plFileExists(p.c_str())) {
        throw std::runtime_error("POG, " + p + ", doesn't exist!\n");
    }

    LoadSpawns(p);
    LoadTextures(p);

    sky_model_ = LoadModel("skys/skydome", true);

    GenerateOverview();
}

Map::~Map() {
    if(!tile_textures_.empty()) {
        LogDebug("Freeing %u textures...\n", tile_textures_.size());
        for(auto texture : tile_textures_) {
            plDeleteTexture(texture, true);
        }
    }

    if(overview_ != nullptr) {
        plDeleteTexture(overview_, true);
    }

    for (auto &sky_texture : sky_textures_) {
        if(sky_texture == nullptr) {
            break;
        }

        plDeleteTexture(sky_texture, true);
    }

    for(auto & chunk : chunks_) {
        if(chunk.model != nullptr) {
            plDeleteModel(chunk.model);
        }
    }

    if(sky_model_ != nullptr) {
        plDeleteModel(sky_model_);
    }
}

MapChunk *Map::GetChunk(const PLVector2 &pos) {
    if( pos.x < 0 || std::floor(pos.x) >= MAP_PIXEL_WIDTH ||
        pos.y < 0 || std::floor(pos.y) >= MAP_PIXEL_WIDTH) {
        LogWarn("Invalid width / height range (%fx%f) vs (%dx%d)!\n", pos.x, pos.y, MAP_PIXEL_WIDTH, MAP_PIXEL_WIDTH);
        return nullptr;
    }

    uint idx = ((uint)(pos.x) / MAP_CHUNK_PIXEL_WIDTH) + (((uint)(pos.y) / MAP_CHUNK_PIXEL_WIDTH) * MAP_CHUNK_ROW);
    if(idx >= chunks_.size()) {
        LogWarn("Attempted to get an out of bounds chunk index!\n");
        return nullptr;
    }

    return &chunks_[idx];
}

MapTile *Map::GetTile(const PLVector2 &pos) {
    MapChunk *chunk = GetChunk(pos);
    if(chunk == nullptr) {
        return nullptr;
    }

    uint idx = (((uint)(pos.x) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) +
               ((((uint)(pos.y) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) * MAP_CHUNK_ROW_TILES);
    if(idx >= MAP_CHUNK_TILES) {
        LogWarn("attempted to get an out of bounds tile index!\n");
        return nullptr;
    }

    return &chunk->tiles[idx];
}

float Map::GetHeight(const PLVector2 &pos) {
    MapTile *tile = GetTile(pos);
    if(tile == nullptr) {
        return 0;
    }

    float tile_x = pos.x - std::floor(pos.x);
    float tile_y = pos.y - std::floor(pos.y);

    float x = tile->height[0] + ((tile->height[1] - tile->height[0]) * tile_x);
    float y = tile->height[2] + ((tile->height[3] - tile->height[2]) * tile_x);
    float z = x + ((y - x) * tile_y);

    return z;
}

void Map::LoadSpawns(const std::string &path) {
    std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
    if(!ifs.is_open()) {
        Error("Failed to open actor data, \"%s\", aborting!\n", path.c_str());
    }

    uint16_t num_indices;
    try {
        ifs.read(reinterpret_cast<char *>(&num_indices), sizeof(uint16_t));
    } catch(const std::ifstream::failure &err) {
        Error("Failed to read POG indices count, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
    }

    spawns_.resize(num_indices);

    try {
        ifs.read(reinterpret_cast<char *>(spawns_.data()), sizeof(MapSpawn) * num_indices);
    } catch(const std::ifstream::failure &err) {
        Error("Failed to read POG spawns, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
    }
}

void Map::LoadTiles(const std::string &path) {
    FILE *fh = std::fopen(path.c_str(), "rb");
    if(fh == nullptr) {
        Error("Failed to open tile data, \"%s\", aborting\n", path.c_str());
    }

    chunks_.resize(MAP_CHUNKS);

    for(unsigned int chunk_y = 0; chunk_y < MAP_CHUNK_ROW; ++chunk_y) {
        for(unsigned int chunk_x = 0; chunk_x < MAP_CHUNK_ROW; ++chunk_x) {
            MapChunk &current_chunk = chunks_[chunk_x + chunk_y * MAP_CHUNK_ROW];

            struct __attribute__((packed)) {
                /* offsets */
                uint16_t x{0};
                uint16_t y{0};
                uint16_t z{0};

                uint16_t unknown0{0};
            } chunk;
            if(std::fread(&chunk, sizeof(chunk), 1, fh) != 1) {
                Error("unexpected end of file, aborting!\n");
            }
            current_chunk.offset = PLVector3(chunk.x, chunk.y, chunk.z);

            struct __attribute__((packed)) {
                int16_t     height{0};
                uint16_t    lighting{0};
            } vertices[25];
            if(std::fread(vertices, sizeof(*vertices), 25, fh) != 25) {
                Error("Unexpected end of file, aborting!\n");
            }

            // Find the maximum and minimum points
            for (auto &vertex : vertices) {
                if(vertex.height > max_height_) {
                    max_height_ = vertex.height;
                }
                if(vertex.height < min_height_) {
                    min_height_ = vertex.height;
                }
            }

            std::fseek(fh, 4, SEEK_CUR);

            for(unsigned int tile_y = 0; tile_y < MAP_CHUNK_ROW_TILES; ++tile_y) {
                for(unsigned int tile_x = 0; tile_x < MAP_CHUNK_ROW_TILES; ++tile_x) {
                    struct __attribute__((packed)) {
#if 0
                        int8_t      unused0[10];
                        uint16_t    info;
                        int8_t      unused1[2];
                        uint8_t     rotation;
                        uint8_t     texture;
#else
                        int8_t      unused0[6]{0,0,0,0,0,0};
                        uint8_t     type{0};
                        uint8_t     slip{0};
                        int16_t     unused1{0};
                        uint8_t     rotation{0};
                        uint32_t    texture{0};
                        uint8_t     unused2{0};
#endif
                    } tile;
                    if(std::fread(&tile, sizeof(tile), 1, fh) != 1) {
                        Error("unexpected end of file, aborting!\n");
                    }

                    /* todo, load texture and apply it to texture atlas
                     * let's do this per-block? i'm paranoid about people
                     * trying to add massive textures for each tile... :(
                     */

                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type     = (tile.type & 31U);
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].flags    = (tile.type & ~31U);
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].flip     = tile.rotation;
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].slip     = 0;
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].tex      = tile.texture;

                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[0] = vertices[
                            (tile_y * 5) + tile_x].height;
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[1] = vertices[
                            (tile_y * 5) + tile_x + 1].height;
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[2] = vertices[
                            ((tile_y + 1) * 5) + tile_x].height;
                    current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[3] = vertices[
                            ((tile_y + 1) * 5) + tile_x + 1].height;

                    u_assert(current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type < MAX_TILE_TYPES,
                             "invalid tile type!\n");
                }
            }

            {
                //Generate model for this chunk
                current_chunk.model = (PLModel*)pl_malloc(sizeof(PLModel));
                if(current_chunk.model == nullptr) {
                    Error("Unable to create map chunk mesh, aborting!\n");
                }

                current_chunk.model->meshes = (PLModelMesh*)pl_malloc(sizeof(PLModelMesh));
                if(current_chunk.model->meshes == nullptr) {
                    Error("Unable to create map chunk mesh, aborting!\n");
                }

                current_chunk.model->meshes[0].mesh = plCreateMeshInit(PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, 32, 25, (void*)chunkIndices, nullptr);
                if(current_chunk.model->meshes[0].mesh == nullptr) {
                    Error("Unable to create map chunk mesh, aborting!\n");
                }

                snprintf(current_chunk.model->name, sizeof(current_chunk.model->name), "map_chunk_%d_%d", chunk_x, chunk_y);
                current_chunk.model->num_meshes = 1;
                current_chunk.model->model_matrix = plTranslateMatrix( PLVector3((float)(chunk_x * MAP_CHUNK_PIXEL_WIDTH), 0.0f, (float)(chunk_y * MAP_CHUNK_PIXEL_WIDTH)) );

                for(unsigned int vz = 0; vz < 5; ++vz) {
                    for(unsigned int vx = 0; vx < 5; ++vx) {
                        unsigned int idx = (vz*5)+vx;
                        plSetMeshVertexPosition(current_chunk.model->meshes[0].mesh, idx, PLVector3(vx * MAP_TILE_PIXEL_WIDTH, vertices[idx].height, vz * MAP_TILE_PIXEL_WIDTH ) );
                    }
                }
            }

        }
    }

    std::fclose(fh);
}

void Map::LoadTextures(const std::string &path) {
    // TODO
}

void Map::GenerateOverview() {
    static const PLColour colours[MAX_TILE_TYPES]={
            { 60, 50, 40 },     // Mud
            { 40, 70, 40 },     // Grass
            { 128, 128, 128 },  // Metal
            { 153, 94, 34 },    // Wood
            { 90, 90, 150 },    // Water
            { 50, 50, 50 },     // Stone
            { 50, 50, 50 },     // Rock
            { 100, 80, 30 },    // Sand
            { 180, 240, 240 },  // Ice
            { 100, 100, 100 },  // Snow
            { 60, 50, 40 },     // Quagmire
            { 100, 240, 53 }    // Lava/Poison
    };

    // Create our storage

    PLImage image;
    memset(&image, 0, sizeof(PLImage));
    image.width         = 64;
    image.height        = 64;
    image.format        = PL_IMAGEFORMAT_RGB8;
    image.colour_format = PL_COLOURFORMAT_RGB;
    image.size          = plGetImageSize(image.format, image.width, image.height);
    image.levels        = 1;

    image.data = static_cast<uint8_t **>(u_alloc(image.levels, sizeof(uint8_t *), true));
    image.data[0] = static_cast<uint8_t *>(u_alloc(1, image.size, true));

    // Now write into the image buffer

    uint8_t *buf = image.data[0];
    for(uint8_t y = 0; y < 64; ++y) {
        for(uint8_t x = 0; x < 64; ++x) {
            PLVector2 position(x * (MAP_PIXEL_WIDTH / 64), y * (MAP_PIXEL_WIDTH / 64));
            MapTile *tile = GetTile(position);
            u_assert(tile != nullptr, "Hit an invalid tile during overview generation!\n");

            auto mod = static_cast<int>((GetHeight(position) + ((GetMaxHeight() + GetMinHeight()) / 2)) / 255);
            PLColour rgb = PLColour(
                    std::min((colours[tile->type].r / 9) * mod, 255),
                    std::min((colours[tile->type].g / 9) * mod, 255),
                    std::min((colours[tile->type].b / 9) * mod, 255)
                    );
            if(tile->flags & TILE_FLAG_MINE) {
                rgb = PLColour(255, 0, 0);
            }

            *(buf++) = rgb.r;
            *(buf++) = rgb.g;
            *(buf++) = rgb.b;
        }
    }

    if((overview_ = plCreateTexture()) == nullptr) {
        Error("Failed to generate overview texture slot!\n%s\n", plGetError());
    }

    plUploadTextureImage(overview_, &image);
    plFreeImage(&image);
}

void Map::Draw() {
    // TODO
}
