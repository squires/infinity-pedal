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
#include <libudev.h>
#include "udev.h"

int pedal_find_devnode(char **devnode)
{
    int result = 0;
    struct udev *udev = NULL;
    struct udev_enumerate *enumerate = NULL;
    struct udev_list_entry *devices = NULL;
    struct udev_list_entry *dev_list_entry = NULL;
    struct udev_device *dev = NULL;
    struct udev_device *pedal = NULL;
    struct udev_device *hid_dev = NULL;
    const char *hid_devnode = NULL;

    udev = udev_new();
    if (NULL == udev)
    {
        perror("failed to initialize libudev\n");
        result = 1;
    }

    if (0 == result)
    {
        enumerate = udev_enumerate_new(udev);
        if (NULL == enumerate)
        {
            perror("failed to create udev enumeration context\n");
            result = 1;
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_add_match_subsystem(enumerate, "usb");
        if (0 != result)
        {
            perror("failed to add subsystem udev filter\n");
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_add_match_sysattr(enumerate, "idVendor", INFINITY_VENDOR);
        if (0 != result)
        {
            perror("failed to add vendor attribute udev filter\n");
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_add_match_sysattr(enumerate, "idProduct", INFINITY_PRODUCT_PEDAL);
        if (0 != result)
        {
            perror("failed to add product attribute udev filter\n");
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_scan_devices(enumerate);
        if (0 != result)
        {
            perror("failed to scan udev devices\n");
        }
    }

    if (0 == result)
    {
        devices = udev_enumerate_get_list_entry(enumerate);
        /* if devices is null, no matching devices were found; fall through */
    }

    if (0 == result)
    {
        udev_list_entry_foreach(dev_list_entry, devices)
        {
            const char *path;

            path = udev_list_entry_get_name(dev_list_entry);
            dev = udev_device_new_from_syspath(udev, path);

            printf("sysfs path: %s\n", path);
            printf("device path: %s\n", udev_device_get_devnode(dev));

            if (NULL == pedal)
            {
                pedal = dev;
                dev = NULL;
            }
            else
            {
                udev_device_unref(dev);
                dev = NULL;

                perror("multiple usb pedals found: configuration not supported\n");
                result = 1;
            }
        }

        udev_enumerate_unref(enumerate);
        enumerate = NULL;
        dev_list_entry = NULL;
        devices = NULL;
    }

    if ((0 == result) && (NULL == pedal))
    {
        perror("no usb pedal found\n");
        result = 1;
    }

    if (0 == result)
    {
        enumerate = udev_enumerate_new(udev);
        if (NULL == enumerate)
        {
            perror("failed to create udev enumeration context\n");
            result = 1;
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_add_match_parent(enumerate, pedal);
        if (0 != result)
        {
            perror("failed to add parent device udev filter\n");
        }
    }

    if (0 == result)
    {
        result = udev_enumerate_scan_devices(enumerate);
        if (0 != result)
        {
            perror("failed to scan udev devices\n");
        }
    }

    if (0 == result)
    {
        devices = udev_enumerate_get_list_entry(enumerate);
        if (NULL == devices)
        {
            perror("failed to get enumerated udev devices (hiddev)\n");
            result = 1;
        }
    }

    if (0 == result)
    {
        udev_list_entry_foreach(dev_list_entry, devices)
        {
            const char *path;
            char *str_hiddev;

            path = udev_list_entry_get_name(dev_list_entry);
            str_hiddev = strstr(path, "hiddev");
            if (NULL != str_hiddev)
            {
                if (NULL != hid_dev)
                {
                    perror("multiple hiddev nodes found for pedal device. this shouldn't happen.\n");
                    udev_device_unref(hid_dev);
                    hid_dev = NULL;
                }
                hid_dev = udev_device_new_from_syspath(udev, path);
            }
        }

        udev_enumerate_unref(enumerate);
        enumerate = NULL;
        dev_list_entry = NULL;
        devices = NULL;
    }

    if ((0 == result) && (NULL == hid_dev))
    {
        perror("failed to find hiddev node for pedal device\n");
        result = 1;
    }

    if (0 == result)
    {
        hid_devnode = udev_device_get_devnode(hid_dev);
        if (NULL == hid_devnode)
        {
            perror("no device node associated with hiddev pedal device\n");
            result = 1;
        }
    }

    if (0 == result)
    {
        *devnode = strdup(hid_devnode);
    }

    /* cleanup */
    if (NULL != pedal)
    {
        udev_device_unref(pedal);
        pedal = NULL;
    }

    if (NULL != hid_dev)
    {
        udev_device_unref(hid_dev);
        hid_dev = NULL;
    }

    if (NULL != enumerate)
    {
        udev_enumerate_unref(enumerate);
        enumerate = NULL;
    }

    if (NULL != udev)
    {
        udev_unref(udev);
        enumerate = NULL;
    }

    return result;
}

