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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>

#include "udev.h"
#include "event.h"

/* EXAMPLE USAGE */
/*
void event_callback(pedal_context *context, int event, void *value)
{
    switch(event)
    {
        case PEDAL_EVENT_UP_LEFT:
            printf("left pedal up\n");
            break;

        case PEDAL_EVENT_UP_MIDDLE:
            printf("middle pedal up\n");
            break;

        case PEDAL_EVENT_UP_RIGHT:
            printf("right pedal up\n");
            break;
    }
}

int main(void)
{
    int result = 0;

    pedal_context *context = pedal_new();
    pedal_set_callback(context, &event_callback);
    pedal_subscribe(context, PEDAL_EVENT_UP_LEFT);
    pedal_event_loop(context);

    return result;
}
*/

void pedal_event_loop(pedal_context *context)
{
    int result = 0;
    char *devnode = NULL;
    int fd = -1;
    struct hiddev_event hid_event;
    unsigned short pedal_num = 0;

    result = pedal_find_devnode(&devnode);
    if (0 == result)
    {
        printf("pedal device node: %s\n", devnode);

        if ((fd = open(devnode, O_RDONLY)) < 0)
        {
            perror("failed to open pedal hid device node\n");
            result = 1;
        }
    }

    if (0 == result)
    {
        while (1)
        {
            read(fd, &hid_event, sizeof(struct hiddev_event));

            switch(hid_event.hid)
            {
                case PEDAL_HIDEV_LEFT:
                    pedal_num = PEDAL_NUM_LEFT;
                    break;

                case PEDAL_HIDEV_MIDDLE:
                    pedal_num = PEDAL_NUM_MIDDLE;
                    break;

                case PEDAL_HIDEV_RIGHT:
                    pedal_num = PEDAL_NUM_RIGHT;
                    break;

                default:
                    pedal_num = 0;
                    break;
            }

            pedal_generate_events(context, pedal_num, hid_event.value);
        }
    }
}

void pedal_generate_events(pedal_context *context, int pedal_num, int value)
{
    unsigned short pedal_mask = 1 << pedal_num;

    if ((context->pedal_state & pedal_mask) != value)
    {
        context->pedal_state ^= pedal_mask;

        if (1 == value)
        {
            pedal_event(context, pedal_num, PEDAL_EVENT_BLOCK_DOWN);
        }
        else
        {
            pedal_event(context, pedal_num, PEDAL_EVENT_BLOCK_UP);
        }
    }
}

void pedal_event(pedal_context *context, int pedal_num, long event_block)
{
    long event_num = event_block + pedal_num;
    long event_id = 1 << event_num;

    (context->event_callback)(context, event_id, context->client_data);
}

pedal_context* pedal_new()
{
    pedal_context *context = calloc(1, sizeof(pedal_context));

    if (context == NULL)
    {
        perror("pedal_new() failed to allocate memory for pedal context\n");
    }

    return context;
}

void pedal_delete(pedal_context* context)
{
    free(context);
}

void pedal_subscribe(pedal_context *context, long events)
{
    int result = 0;

    if (NULL == context)
    {
        perror("pedal_subscribe() received null context\n");
    }

    if (0 == result)
    {
        context->subscriptions |= events;
    }
}

void pedal_unsubscribe(pedal_context *context, long events)
{
    int result = 0;

    if (NULL == context)
    {
        perror("pedal_unsubscribe() received null context\n");
    }

    if (0 == result)
    {
        context->subscriptions &= ~events;
    }
}

void pedal_set_callback(pedal_context *context,
                        void (*event_callback)(pedal_context *context,
                                               int event,
                                               void *value))
{
    int result = 0;

    if (NULL == context)
    {
        perror("pedal_set_callback() received null context\n");
    }

    if (0 == result)
    {
        context->event_callback = event_callback;
    }
}

void pedal_set_client_data(pedal_context *context, void *client_data)
{
    int result = 0;

    if (NULL == context)
    {
        perror("pedal_set_client_data() received null context\n");
    }

    if (0 == result)
    {
        context->client_data = client_data;
    }
}

