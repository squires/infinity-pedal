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

#define PEDAL_HIDEV_LEFT        0x90001
#define PEDAL_HIDEV_MIDDLE      0x90002
#define PEDAL_HIDEV_RIGHT       0x90003

#define PEDAL_NUM_LEFT          0
#define PEDAL_NUM_MIDDLE        1
#define PEDAL_NUM_RIGHT         2

#define PEDAL_EVENT_BLOCK_UP    0
#define PEDAL_EVENT_BLOCK_DOWN  3

#define PEDAL_EVENT_UP_LEFT     (1 << 0)
#define PEDAL_EVENT_UP_MIDDLE   (1 << 1)
#define PEDAL_EVENT_UP_RIGHT    (1 << 2)
#define PEDAL_EVENT_DOWN_LEFT   (1 << 3)
#define PEDAL_EVENT_DOWN_MIDDLE (1 << 4)
#define PEDAL_EVENT_DOWN_RIGHT  (1 << 5)

typedef struct pedal_context_t
{
    long subscriptions;
    void (*event_callback)(struct pedal_context_t *context, int event, void *value);
    void *client_data;
    unsigned short pedal_state;
} pedal_context;

void pedal_event_loop(pedal_context *context);

void pedal_generate_events(pedal_context *context, int pedal_num, int value);

void pedal_event(pedal_context *context, int pedal_num, long event_block);

pedal_context* pedal_new();

void pedal_delete(pedal_context* context);

void pedal_subscribe(pedal_context *context, long events);

void pedal_unsubscribe(pedal_context *context, long events);

void pedal_set_callback(pedal_context *context,
                        void (*event_callback)(pedal_context *context,
                                               int event,
                                               void *value));

void pedal_set_client_data(pedal_context *context, void *client_data);

