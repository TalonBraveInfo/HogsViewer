/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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
#include "pork_engine.h"
#include "pork_input.h"
#include "pork_map.h"
#include "pork_language.h"

#include "client/client_font.h"
#include "client/client_frontend.h"

#include "server/server_actor.h"

#include <PL/platform_graphics_camera.h>

/*****************************************************/

typedef struct CallbackConstruct {
    const char *cmd;

    void(*Get)(unsigned int argc, char *argv[]);
    void(*Set)(unsigned int argc, char *argv[]);
    void(*Add)(unsigned int argc, char *argv[]);
} CallbackConstruct;

CallbackConstruct callbacks[]={
        {"menu", NULL, SetFEObjectCommand, NULL},
        {"actor", NULL, NULL, NULL},
        {"map", NULL, NULL, NULL},
};

void GetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        LogWarn("invalid number of arguments, ignoring!\n");
        return;
    }

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Get == NULL) {
                break;
            }
            callbacks[j].Get(argc, argv);
            return;
        }
    }
    LogWarn("invalid GET command, %s!\n", cmd);
}

void AddCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        LogWarn("invalid number of arguments, ignoring!\n");
        return;
    }

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Add == NULL) {
                break;
            }
            callbacks[j].Add(argc, argv);
            return;
        }
    }
    LogWarn("invalid ADD command, %s!\n", cmd);
}

void SetCommand(unsigned int argc, char *argv[]) {
    if(argc < 1) {
        LogWarn("invalid number of arguments, ignoring!\n");
        return;
    }

    const char *cmd = argv[1];
    for(unsigned int j = 0; j < plArrayElements(callbacks); ++j) {
        if(pl_strcasecmp(callbacks[j].cmd, cmd) == 0) {
            if(callbacks[j].Add == NULL) {
                break;
            }
            callbacks[j].Add(argc, argv);
            return;
        }
    }
    LogWarn("invalid ADD command, %s!\n", cmd);

#if 0
    for(unsigned int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("actor", argv[i], 5) == 0) {
            const char *actor_name = argv[++i];
            if(actor_name == NULL || actor_name[0] == '\0') {
                LogWarn("invalid actor name, aborting\n");
                return;
            }

            if(g_state.is_host) {
                Actor *actor = SVGetActor(actor_name);
                if (actor == NULL) {
                    LogWarn("unable to find actor %s, aborting\n", actor_name);
                    return;
                }

                const char *modifier = argv[++i];
                if (modifier == NULL || modifier[0] == '\0') {
                    LogWarn("no modifier provided for actor %s, aborting\n", actor_name);
                    return;
                }

                /* todo, add table of string + parm so we don't need to hard-code most of this */
                if (pl_strncasecmp("name", modifier, 4) == 0) {
                    const char *name = argv[++i];
                    if (name == NULL || name[0] == '\0') {
                        LogWarn("invalid name, aborting\n");
                        return;
                    }

                    strncpy(actor->name, name, sizeof(actor->name));
                    return;
                } else if (pl_strncasecmp("position", modifier, 8) == 0) {

                } else if (pl_strncasecmp("bounds", modifier, 6) == 0) {

                }
            }
            return;
        } else if(pl_strncasecmp(CMD_GROUP_MAP, argv[i], 3) == 0) {
            /* todo, set map properties */
            const char *name = argv[++i];
            if(name == NULL || name[0] == '\0') {
                LogWarn("invalid map name, aborting\n");
                return;
            }

            LoadMap(name, MAP_MODE_DEATHMATCH);
            return;
        }
    }

    LogWarn("invalid parameters provided for set command\n");
#endif
}

void QuitCommand(unsigned int argc, char *argv[]) {
    g_launcher.ShutdownLauncher();
}

void DisconnectCommand(unsigned int argc, char *argv[]) {
    UnloadMap();
}

/*****************************************************/

#define MAX_BUFFER_SIZE 512

struct {
    char buffer[MAX_BUFFER_SIZE];
    unsigned int buffer_pos;
} console_state;

bool console_enabled = false;

void ConvertImageCallback(unsigned int argc, char *argv[]);

void InitConsole(void) {
    plRegisterConsoleCommand("convert", ConvertImageCallback, "Convert TIM textures to PNG");
    plRegisterConsoleCommand("set", SetCommand, "");
    plRegisterConsoleCommand("get", GetCommand, "");
    plRegisterConsoleCommand("add", AddCommand, "");
    plRegisterConsoleCommand("exit", QuitCommand, "Closes the game");
    plRegisterConsoleCommand("quit", QuitCommand, "Closes the game");
    plRegisterConsoleCommand("disconnect", DisconnectCommand, "Disconnects and unloads current map");

    plRegisterConsoleVariable("language", "eng", pl_string_var, SetLanguageCallback, "Current language");
    cv_camera_mode = plRegisterConsoleVariable("camera", "0", pl_int_var, NULL, "0 = default, 1 = debug");
}

void DrawConsole(void) {
    if(!console_enabled) {
        return;
    }

    plSetBlendMode(PL_BLEND_DEFAULT);
    plDrawFilledRectangle(plCreateRectangle(
            PLVector2(0, 0),
            PLVector2(GetViewportWidth(), 32),
            PLColour(0, 0, 0, 255),
            PLColour(0, 0, 0, 0),
            PLColour(0, 0, 0, 255),
            PLColour(0, 0, 0, 0)
    ));

    char msg_buf[MAX_BUFFER_SIZE];
    strncpy(msg_buf, console_state.buffer, sizeof(msg_buf));
    pl_strtoupper(msg_buf);

    plSetBlendMode(PL_BLEND_ADDITIVE);

    unsigned int x = 20;
    unsigned int w = g_fonts[FONT_GAME_CHARS]->chars[0].w;
    DrawBitmapCharacter(g_fonts[FONT_GAME_CHARS], x, 10, 1.f, PL_COLOUR_RED, '>');
    if(console_state.buffer_pos > 0) {
        DrawBitmapString(g_fonts[FONT_GAME_CHARS], x + w + 5, 10, 1, 1.f, PL_COLOUR_WHITE, msg_buf);
    }

    /* DrawBitmapString disables blend - no need to call again here */
}

void ResetConsole(void) {
    memset(console_state.buffer, 0, sizeof(console_state.buffer));
    console_state.buffer_pos = 0;
}

void ConsoleInputCallback(int key, bool is_pressed);
void ToggleConsole(void) {
    console_enabled = !console_enabled;
    if(console_enabled) {
        SetKeyboardFocusCallback(ConsoleInputCallback);
        ResetConsole();
    } else {
        SetKeyboardFocusCallback(NULL);
    }

    ResetInputStates();
}

void ConsoleInputCallback(int key, bool is_pressed) {
    if(!is_pressed || !console_enabled) {
        return;
    }

    if(key == '\r') {
        /* todo, allow multiple commands, seperated with ';' */
        plParseConsoleString(console_state.buffer);
        ToggleConsole();
        return;
    }

    if(key == '\b' && console_state.buffer_pos > 0) {
        console_state.buffer[--console_state.buffer_pos] = '\0';
        return;
    }

    if(console_state.buffer_pos > MAX_BUFFER_SIZE) {
        LogWarn("hit console buffer limit, aborting!\n");
        return;
    }

    if(isprint(key) == 0) {
        return;
    }

    console_state.buffer[console_state.buffer_pos++] = (char) key;
}