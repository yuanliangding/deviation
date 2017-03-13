/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "model.h"
#include "../music.h"

#define MATCH_SECTION(s) (strcasecmp(section, s) == 0)
#define MATCH_KEY(s)     (strcasecmp(name,    s) == 0)

#if HAS_EXTENDED_AUDIO

const char SECTION_VOICE_GLOBAL[] = "global";
const char SECTION_VOICE_CUSTOM[] = "custom";

static const char* getfield(char* value, int num) {
    const char* tok;
    for (tok = strtok(value, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
        if (!--num)
            return tok;
    }
    return NULL;
}

static int ini_handler(void* user, const char* section, const char* name, const char* value)
{
    (void) user;
    char tmp[100];
    strlcpy(tmp, value, 100);
    int duration = atoi(getfield(tmp,2));
#if HAS_MUSIC_CONFIG
    char label[MAX_MUSIC_LABEL];
    strlcpy(label,getfield(tmp, 1), MAX_MUSIC_LABEL);
#endif

    if (MATCH_SECTION(SECTION_VOICE_GLOBAL)) {
        for (int i = 0; i < CUSTOM_ALARM_ID; i++) {
            snprintf(tempstring, 4, "%d", i);
            if (MATCH_KEY(tempstring)) {
                music_map[i].duration = duration;
                music_map[i].musicid = i;
#if HAS_MUSIC_CONFIG
                strcpy(music_map[i].label, label);
#endif
                return 1;
            }
        }
    }
    if (MATCH_SECTION(SECTION_VOICE_CUSTOM)) {
        music_map[music_map_entries].duration = duration;
        music_map[music_map_entries].musicid = atoi(name);
#if HAS_MUSIC_CONFIG
        strcpy(music_map[music_map_entries].label, label);
#endif
        music_map_entries++;
        return 1;
    }
    printf("Unknown entry in voice.ini: %s\n", value);
    return 0;
}

void CONFIG_VoiceParse()
{
    music_map_entries = CUSTOM_ALARM_ID;
    if (CONFIG_IniParse("media/voice.ini", ini_handler, NULL)) {
        printf("Failed to parse voice.ini\n");
    }
}
#endif
