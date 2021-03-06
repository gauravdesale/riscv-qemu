/*
 * common code shared by usb tests
 *
 * Copyright (c) 2014 Red Hat, Inc
 *
 * Authors:
 *     Gerd Hoffmann <kraxel@redhat.com>
 *     John Snow <jsnow@redhat.com>
 *     Igor Mammedov <imammedo@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#include "qemu/osdep.h"
#include "libqtest.h"
#include "hw/usb/uhci-regs.h"
#include "libqos/usb.h"

void qusb_pci_init_one(QPCIBus *pcibus, struct qhc *hc, uint32_t devfn, int bar)
{
    hc->dev = qpci_device_find(pcibus, devfn);
    g_assert(hc->dev != NULL);
    qpci_device_enable(hc->dev);
    hc->bar = qpci_iomap(hc->dev, bar, NULL);
}

void uhci_deinit(struct qhc *hc)
{
    g_free(hc->dev);
}

void uhci_port_test(struct qhc *hc, int port, uint16_t expect)
{
    uint16_t value = qpci_io_readw(hc->dev, hc->bar, 0x10 + 2 * port);
    uint16_t mask = ~(UHCI_PORT_WRITE_CLEAR | UHCI_PORT_RSVD1);

    g_assert((value & mask) == (expect & mask));
}

void usb_test_hotplug(const char *hcd_id, const int port,
                      void (*port_check)(void))
{
    char  *id = g_strdup_printf("usbdev%d", port);

    qtest_qmp_device_add("usb-tablet", id, "'port': '%d', 'bus': '%s.0'",
                         port, hcd_id);

    if (port_check) {
        port_check();
    }

    qtest_qmp_device_del(id);

    g_free(id);
}
