/* infinity-pedal, a linux driver for the Infinity IN-USB-2 foot pedal
 * Copyright (C) 2012 Scott Squires
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

#include <stdio.h>
#include <string.h>
#include "event.h"

void event_callback(pedal_context *context, int event, void *client_data)
{
    FILE *mplayer_pipe = (FILE*) client_data;

    if (NULL == mplayer_pipe)
    {
        perror("pipe is null\n");
        return;
    }

    switch(event)
    {
        case PEDAL_EVENT_UP_LEFT:
            fprintf(mplayer_pipe, "seek -5\n");
            break;

        case PEDAL_EVENT_UP_MIDDLE:
            fprintf(mplayer_pipe, "pause\n");
            break;

        case PEDAL_EVENT_UP_RIGHT:
            fprintf(mplayer_pipe, "seek +5\n");
            break;
    }

    fflush(mplayer_pipe);
}

int main(int argc, char *argv[])
{
    int result = 0;
    char cmd[1024];
    char buf[1024];
    FILE *mplayer_pipe = NULL;

    snprintf(cmd, 1024, "mplayer -slave");

    int i;
    for (i = 1; i < argc; i++)
    {
        strncpy(buf, cmd, 1024);
        /* TODO: escape parameter for the new shell call */
        /* (prefix any single quotes with \, and wrap whole parameter in single quotes) */
        snprintf(cmd, 1024, "%s %s", buf, argv[i]);
    }

    printf("%s\n", cmd);
    mplayer_pipe = popen(cmd, "w");

    pedal_context *context = pedal_new();
    pedal_subscribe(context, PEDAL_EVENT_UP_LEFT | PEDAL_EVENT_UP_MIDDLE | PEDAL_EVENT_UP_RIGHT);
    pedal_set_callback(context, &event_callback);
    pedal_set_client_data(context, mplayer_pipe);
    pedal_event_loop(context);

    return result;
}

