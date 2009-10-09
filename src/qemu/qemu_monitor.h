/*
 * qemu_monitor.h: interaction with QEMU monitor console
 *
 * Copyright (C) 2006-2009 Red Hat, Inc.
 * Copyright (C) 2006 Daniel P. Berrange
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Author: Daniel P. Berrange <berrange@redhat.com>
 */


#ifndef QEMU_MONITOR_H
#define QEMU_MONITOR_H

#include "internal.h"

#include "domain_conf.h"

typedef struct _qemuMonitor qemuMonitor;
typedef qemuMonitor *qemuMonitorPtr;

typedef void (*qemuMonitorEOFNotify)(qemuMonitorPtr mon,
                                     virDomainObjPtr vm,
                                     int withError);

/* XXX we'd really like to avoid virCOnnectPtr here
 * It is required so the callback can find the active
 * secret driver. Need to change this to work like the
 * security drivers do, to avoid this
 */
typedef int (*qemuMonitorDiskSecretLookup)(qemuMonitorPtr mon,
                                           virConnectPtr conn,
                                           virDomainObjPtr vm,
                                           const char *path,
                                           char **secret,
                                           size_t *secretLen);

qemuMonitorPtr qemuMonitorOpen(virDomainObjPtr vm,
                               int reconnect,
                               qemuMonitorEOFNotify eofCB);

void qemuMonitorClose(qemuMonitorPtr mon);

void qemuMonitorRegisterDiskSecretLookup(qemuMonitorPtr mon,
                                         qemuMonitorDiskSecretLookup secretCB);

int qemuMonitorWrite(qemuMonitorPtr mon,
                     const char *data,
                     size_t len);

int qemuMonitorWriteWithFD(qemuMonitorPtr mon,
                           const char *data,
                           size_t len,
                           int fd);

int qemuMonitorRead(qemuMonitorPtr mon,
                    char *data,
                    size_t len);

int qemuMonitorWaitForInput(qemuMonitorPtr mon);

int qemuMonitorGetDiskSecret(qemuMonitorPtr mon,
                             virConnectPtr conn,
                             const char *path,
                             char **secret,
                             size_t *secretLen);


int qemuMonitorStartCPUs(qemuMonitorPtr mon,
                         virConnectPtr conn);
int qemuMonitorStopCPUs(qemuMonitorPtr mon);

int qemuMonitorSystemPowerdown(qemuMonitorPtr mon);

int qemuMonitorGetCPUInfo(qemuMonitorPtr mon,
                          int **pids);
int qemuMonitorGetBalloonInfo(qemuMonitorPtr mon,
                              unsigned long *currmem);
int qemuMonitorGetBlockStatsInfo(qemuMonitorPtr mon,
                                 const char *devname,
                                 long long *rd_req,
                                 long long *rd_bytes,
                                 long long *wr_req,
                                 long long *wr_bytes,
                                 long long *errs);


int qemuMonitorSetVNCPassword(qemuMonitorPtr mon,
                              const char *password);
int qemuMonitorSetBalloon(qemuMonitorPtr mon,
                          unsigned long newmem);

/* XXX should we pass the virDomainDiskDefPtr instead
 * and hide devname details inside monitor. Reconsider
 * this when doing the QMP implementation
 */
int qemuMonitorEjectMedia(qemuMonitorPtr mon,
                          const char *devname);
int qemuMonitorChangeMedia(qemuMonitorPtr mon,
                           const char *devname,
                           const char *newmedia);


int qemuMonitorSaveVirtualMemory(qemuMonitorPtr mon,
                                 unsigned long long offset,
                                 size_t length,
                                 const char *path);
int qemuMonitorSavePhysicalMemory(qemuMonitorPtr mon,
                                  unsigned long long offset,
                                  size_t length,
                                  const char *path);

int qemuMonitorSetMigrationSpeed(qemuMonitorPtr mon,
                                 unsigned long bandwidth);

enum {
    QEMU_MONITOR_MIGRATION_STATUS_INACTIVE,
    QEMU_MONITOR_MIGRATION_STATUS_ACTIVE,
    QEMU_MONITOR_MIGRATION_STATUS_COMPLETED,
    QEMU_MONITOR_MIGRATION_STATUS_ERROR,
    QEMU_MONITOR_MIGRATION_STATUS_CANCELLED,

    QEMU_MONITOR_MIGRATION_STATUS_LAST
};

int qemuMonitorGetMigrationStatus(qemuMonitorPtr mon,
                                  int *status,
                                  unsigned long long *transferred,
                                  unsigned long long *remaining,
                                  unsigned long long *total);

int qemuMonitorMigrateToHost(qemuMonitorPtr mon,
                             int background,
                             const char *hostname,
                             int port);

int qemuMonitorMigrateToCommand(qemuMonitorPtr mon,
                                int background,
                                const char * const *argv,
                                const char *target);

int qemuMonitorMigrateToUnix(qemuMonitorPtr mon,
                             int background,
                             const char *unixfile);

int qemuMonitorMigrateCancel(qemuMonitorPtr mon);


/* XXX disk driver type eg,  qcow/etc.
 * XXX cache mode
 */
int qemuMonitorAddUSBDisk(qemuMonitorPtr mon,
                          const char *path);

int qemuMonitorAddUSBDeviceExact(qemuMonitorPtr mon,
                                 int bus,
                                 int dev);
int qemuMonitorAddUSBDeviceMatch(qemuMonitorPtr mon,
                                 int vendor,
                                 int product);


int qemuMonitorAddPCIHostDevice(qemuMonitorPtr mon,
                                unsigned hostDomain,
                                unsigned hostBus,
                                unsigned hostSlot,
                                unsigned hostFunction,
                                unsigned *guestDomain,
                                unsigned *guestBus,
                                unsigned *guestSlot);

/* XXX disk driver type eg,  qcow/etc.
 * XXX cache mode
 */
int qemuMonitorAddPCIDisk(qemuMonitorPtr mon,
                          const char *path,
                          const char *bus,
                          unsigned *guestDomain,
                          unsigned *guestBus,
                          unsigned *guestSlot);

/* XXX do we really want to hardcode 'nicstr' as the
 * sendable item here
 */
int qemuMonitorAddPCINetwork(qemuMonitorPtr mon,
                             const char *nicstr,
                             unsigned *guestDomain,
                             unsigned *guestBus,
                             unsigned *guestSlot);

int qemuMonitorRemovePCIDevice(qemuMonitorPtr mon,
                               unsigned guestDomain,
                               unsigned guestBus,
                               unsigned guestSlot);


int qemuMonitorSendFileHandle(qemuMonitorPtr mon,
                              const char *fdname,
                              int fd);

int qemuMonitorCloseFileHandle(qemuMonitorPtr mon,
                               const char *fdname);


/* XXX do we really want to hardcode 'netstr' as the
 * sendable item here
 */
int qemuMonitorAddHostNetwork(qemuMonitorPtr mon,
                              const char *netstr);

int qemuMonitorRemoveHostNetwork(qemuMonitorPtr mon,
                                 int vlan,
                                 const char *netname);


#endif /* QEMU_MONITOR_H */
