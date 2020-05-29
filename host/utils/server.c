/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "host_system.h"
#include <host_imgl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <bcm_err.h>
#include <mqueue.h>
#include <server.h>
#include <hipc.h>
#include <sys_ipc_cmds.h>
#include <rpc_cmds.h>
#ifdef ENABLE_HOST_ETS_CMD_HANDLER
#include <ets_ipc.h>
#include <host_ets.h>
#endif
#ifdef ENABLE_HOST_COMMS_CMD_HANDLER
#include <etherswt_ipc.h>
#include <host_comms.h>
#include <host_ether.h>
#include <host_etherswt.h>
#endif
#include <host_system.h>
#ifdef ENABLE_HOST_CFP_CMD_HANDLER
#include <host_cfp.h>
#endif
#include <utils.h>
#include <hlog.h>

#define MAX_NUM_PORTS (8UL)

#define MGMT_CMD_INT_PARSE_STATUS(status)  if(status != BCM_ERR_OK) {  \
    goto done;                    \
}                                \

#define MGMT_AVB_TRACE_LINE(n)                                \
    do {                                                        \
        uint32_t i;                                                 \
        for (i=0; i < (n); i++) { HOST_Log("-"); }                  \
        HOST_Log("\n");                                             \
    } while(0)                                                  \


/*
 * Typedef:     cmd_result_t
 * Purpose:    Type retured from all commands indicating success, fail,
 *        or print usage.
 */
typedef enum cmd_result_e {
    CMD_OK   = 0,            /* Command completed successfully */
    CMD_FAIL = -1,            /* Command failed */
    CMD_USAGE= -2,            /* Command failed, print usage  */
} cmd_result_t;

static int32_t mgmt_arl_show(MgmtInfoType *info);
static int32_t Host_CheckAsyncMsg();

static ETHERSWT_ARLEntryType ARLEntries[1000UL];

static MgmtInfoType info_g;

/*@api
 * parse_integer
 *
 * @brief
 * parse the integer.
 *
 * @param=s
 * @param=status - mask
 * @returns parsed value
 *
 * @desc
 */
uint32_t parse_integer(const char *s, int32_t *status)
{
    uint32_t  n, neg, base = 10;
    int8_t   *ch;

    *status = BCM_ERR_OK;
    s += (neg = (*s == '-'));
    if ((*s == '0') && ((*(s+1) == 'x') || (*(s+1) == 'X'))) {
        if (neg) {
            HOST_Log("Negative hex numbers are not supported\n");
            *status = BCM_ERR_INVAL_PARAMS;
            return -1;
        }
        else {
            base = 16;
            s = (s + 2);
        }
    }

    ch = (int8_t *) s;
    if (*ch == '\0') {
        HOST_Log("Invalid user input\n");
        *status = BCM_ERR_INVAL_PARAMS;
        return -1;
    }

    if (base == 10) {
        while ((*ch != '\0')
                && (*ch != ' ')){
            if (*ch < '0' || *ch > '9') {
                HOST_Log("Invalid decimal number \"%s\"\n", s);
                *status = BCM_ERR_INVAL_PARAMS;
                return -1;
            }
            ch++;
        }
    } else if (base == 16) {
        while ((*ch !='\0')
                && (*ch != ' ')){
            if ((*ch >= '0' && *ch <= '9') || (*ch >= 'A' && *ch <= 'F') || (*ch >= 'a' && *ch <= 'f')){
                ch++;
                continue;
            }
            else {
                HOST_Log("Invalid hex number: \"0x%s\"\n", s);
                *status = BCM_ERR_INVAL_PARAMS;
                return -1;
            }
        }
    }

    for (n = 0; ((*s >= 'a' && *s <= 'f' && base > 10) ||
                (*s >= 'A' && *s <= 'F' && base > 10) ||
                (*s >= '0' && *s <= '9')); s++) {
        n = n * base +
            ((*s >= 'a' && *s <= 'f') ? *s - 'a' + 10 :
             (*s >= 'A' && *s <= 'F') ? *s - 'A' + 10 :
             *s - '0');
    }

    return (neg ? -n : n);
}


/*@api
 * buffer_from_file
 *
 * @brief
 * Create a buffer holding the contents of a file.
 *
 * @param=name - file to read
 * @param=len - pointer to length to return
 * @returns pointer to the data, or NULL
 *
 * @desc
 */
void * buffer_from_file(char *name, uint32_t *len)
{
    FILE *fp;
    uint32_t read_len;
    void *buffer;
    fp = fopen(name, "r");
    if (!fp) {
        HOST_Log("could not open %s\n", name);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) < 0) {
        HOST_Log("error seeking to end of %s\n", name);
        fclose(fp);
        return NULL;
    }

    *len = ftell(fp);
    if(*len < 0 ) {
        HOST_Log("error in getting the current position\n");
        fclose(fp);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        HOST_Log("error seeking to beginning of %s\n", name);
        fclose(fp);
        return NULL;
    }

    buffer = malloc(*len);
    if (!buffer) {
        HOST_Log("could not allocate %d bytes\n", *len);
        fclose(fp);
        return NULL;
    }

    read_len = fread(buffer, 1, *len, fp);
    fclose(fp);
    if (read_len != *len) {
        HOST_Log("error reading %s\n", name);
        free(buffer);
        return NULL;
    }

    return buffer;
}

void split_line(char * const input_string, uint32_t size, char **rem_string_ptr,
        uint32_t * const rem_len_ptr)
{
    uint32_t i = 0UL;
    while ((i < size)
            && ('\0' != input_string[i])
            && (' ' != input_string[i])
            && ('\r' != input_string[i])) {
        ++i;
    }
    if (' ' == input_string[i]) {
        rem_string_ptr[0UL] = &input_string[i + 1UL];
        rem_len_ptr[0UL] = size - (i + 1UL);
        input_string[i] = '\0'; /*don't modify the original string */
    }else {
        /* Empty argument string */
        rem_string_ptr[0UL] = NULL;
        rem_len_ptr[0UL] = 0UL;
    }
}

static const char HOST_Exec_cmd_usage[] =
"\texecute fw <img>\n"
"\texecute bl <img>\n";

void HOST_Exec_cmd_handler(MgmtInfoType *info, char *input_str, uint32_t input_str_len)
{
    char *rem_str;
    uint32_t rem_str_len;
    uint32_t buf_size;
    void* buffer;
    int32_t rv = CMD_USAGE;

    if ((input_str != NULL) && (input_str_len != 0)) {
        split_line(input_str, input_str_len, &rem_str, &rem_str_len);
        if (0UL == strncmp(input_str, "fw", 2)) {
            if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                buffer = buffer_from_file(rem_str, &buf_size);
                if (NULL != buffer) {
                    if (HOST_SysFirmwareExecute(info, buffer, buf_size) != BCM_ERR_OK) {
                        HOST_Log("failed to execute firmware\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else {
                    rv = CMD_FAIL;
                }
            }
        } else if (0UL == strncmp(input_str, "bl", 2)) {
            if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                buffer = buffer_from_file(rem_str, &buf_size);
                if (NULL != buffer) {
                    if (HOST_SysBLExecute(info, buffer, buf_size) != BCM_ERR_OK) {
                        HOST_Log("failed to execute firmware\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else {
                    rv = CMD_FAIL;
                }
            }
        }
    }

    if (rv == CMD_USAGE) {
        HOST_Log(HOST_Exec_cmd_usage);
    }
}

static const char HOST_Install_cmd_usage[] =
"\tinstall all <img>\n"
"\tinstall bl <img>\n"
"\tinstall custom <img>\n";
void HOST_Install_cmd_handler(MgmtInfoType *info, char *input_str, uint32_t input_str_len)
{
    char *rem_str;
    uint32_t rem_str_len;
    int32_t rv = CMD_USAGE;
    uint32_t buf_size;
    void* buffer;
    char *curr_str = input_str;
    uint32_t curr_str_len = input_str_len;

    if ((input_str != NULL) && (input_str_len != 0)) {
        split_line(input_str, input_str_len, &rem_str, &rem_str_len);
        if (0 == strncmp(input_str, "all", 3)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) && (rem_str_len == 0)) {
                    // Read the file and install it
                    buffer = buffer_from_file(curr_str, &buf_size);
                    if (buffer != NULL) {
                        if(HOST_SysFirmwareUpdate(info, INSTALL_MODE_ALL, buffer, buf_size) != 0) {
                            HOST_Log("Failed to install images!\n");
                            rv = CMD_FAIL;
                        } else {
                            rv = CMD_OK;
                        }
                        free(buffer);
                    } else {
                        rv = CMD_FAIL;
                    }
                }
            }
        } else if (0 == strncmp(input_str, "bl", 2)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) && (rem_str_len == 0)) {
                    // Read the file and install it
                    buffer = buffer_from_file(curr_str, &buf_size);
                    if (buffer != NULL) {
                        if(HOST_SysFirmwareUpdate(info, INSTALL_MODE_BL, buffer, buf_size) != 0) {
                            HOST_Log("Failed to install images!\n");
                            rv = CMD_FAIL;
                        } else {
                            rv = CMD_OK;
                        }
                        free(buffer);
                    } else {
                        rv = CMD_FAIL;
                    }
                }
            }
        } else if (0 == strncmp(input_str, "custom", 6)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) && (rem_str_len == 0)) {
                    // Read the file and install it
                    buffer = buffer_from_file(curr_str, &buf_size);
                    if (buffer != NULL) {
                        if (HOST_SysFirmwareUpdate(info, INSTALL_MODE_CUSTOM, buffer, buf_size) != 0) {
                            HOST_Log("Failed to install images!\n");
                            rv = CMD_FAIL;
                        } else {
                            rv = CMD_OK;
                        }
                        free(buffer);
                    } else {
                        rv = CMD_FAIL;
                    }
                }
            }
        }
    }
    if (rv == CMD_USAGE) {
        HOST_Log(HOST_Install_cmd_usage);
    }
    return;
}

static const char HOST_FlashWrite_cmd_usage[] =
"\tflash write <addr>\n";
void HOST_FlashWrite_cmd_handler(MgmtInfoType *info, char *input_str, uint32_t input_str_len)
{
    char *rem_str;
    uint32_t rem_str_len;
    int32_t rv = CMD_USAGE;

    if ((input_str != NULL) && (input_str_len != 0)) {
        split_line(input_str, input_str_len, &rem_str, &rem_str_len);
        if (0 == strncmp(input_str, "write", 5)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                if (HOST_SysFlashWrite(info, (int)strtol(rem_str, NULL, 16)) != 0) {
                    HOST_Log("Failed to write to flash!\n");
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        }
    }
    if (rv == CMD_USAGE) {
        HOST_Log(HOST_FlashWrite_cmd_usage);
    }

    return;
}

static const char mgmt_otp_cmd_usage[] =
"\totp read <row_num (in hex)>\n"
"\totp write <row_num (in hex)> <data (in hex)>\n"
"\totp enable_sec\n"
"\totp key_write <key file>\n"
"\totp mac_addr write <mac address(xx:xx:xx:xx:xx:xx)> <loc(0/1)>\n"
"\totp mac_addr read\n";
void mgmt_otp_cmd_handler(MgmtInfoType *info, char *input_str, uint32_t input_str_len)
{
    char *curr_str;
    uint32_t curr_str_len;
    char *rem_str;
    uint32_t rem_str_len;
    uint32_t buf_size;
    void* buffer;
    int32_t rv = CMD_USAGE;
    uint32_t value;
    uint8_t macaddr1[6UL], macaddr2[6UL];
    int32_t retVal;

    if ((input_str != NULL) && (input_str_len != 0)) {
        split_line(input_str, input_str_len, &rem_str, &rem_str_len);
        if (0 == strncmp(input_str, "read", 4)) {
            if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                value = 0UL;
                if (HOST_SysOTPRead(info, (int)strtol(rem_str, NULL, 16), &value) != 0) {
                    HOST_Log("Failed to read OTP!\n");
                    rv = CMD_FAIL;
                } else {
                    HOST_Log("OTP read: row_num:0x%x, data:0x%x\n",
                    (int)strtol(rem_str, NULL, 16), value);
                    rv = CMD_OK;
                }
            }
        } else if (0 == strncmp(input_str, "write", 5)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                    if (HOST_SysOTPWrite(info,
                                (int)strtol(curr_str, NULL, 16),
                                (int)strtol(rem_str, NULL, 16)
                                ) != 0) {
                        HOST_Log("Failed to write OTP!\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                }
            }
        } else if (0 == strncmp(input_str, "enable_sec", 10)) {
            if ((rem_str == NULL) && (rem_str_len == 0UL)) {
                if (HOST_SysOTPEnableSec(info) != 0) {
                    HOST_Log("Failed to enable Secure OTP bit!\n");
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        } else if (0 == strncmp(input_str, "key_write", 9)) {
            if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                buffer = buffer_from_file(rem_str, &buf_size);
                if (HOST_SysOTPKeyWrite(info, buffer, buf_size) != 0) {
                    HOST_Log("Failed to write key!\n");
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        } else if (0 == strncmp(input_str, "mac_addr", 8)) {
            if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                input_str = rem_str;
                input_str_len = rem_str_len;
                split_line(input_str, input_str_len, &rem_str, &rem_str_len);
                if (0 == strncmp(input_str, "write", 5)) {
                    if ((rem_str != NULL) && (rem_str_len > 0UL)) {
                        input_str = rem_str;
                        input_str_len = rem_str_len;
                        split_line(input_str, input_str_len, &rem_str, &rem_str_len);
                        HOST_EtherSwtConvertMac(input_str, &macaddr1[0], &retVal);
                        if (retVal != 0) {
                            HOST_Log("Invalid MAC\n");
                            rv = CMD_FAIL;
                            goto done;
                        }

                        HOST_Log("OTP MAC addr-1: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                (uint32_t)(macaddr1[0]),
                                (uint32_t)(macaddr1[1]),
                                (uint32_t)(macaddr1[2]),
                                (uint32_t)(macaddr1[3]),
                                (uint32_t)(macaddr1[4]),
                                (uint32_t)(macaddr1[5]));

                        if ((rem_str == NULL) || (rem_str_len == 0)) {
                            goto done;
                        }

                        retVal = HOST_SysOTPMacAddrWrite(info, &macaddr1[0],
                                (int)strtol(rem_str, NULL, 16));
                        if (BCM_ERR_NOPERM == retVal) {
                            HOST_Log("Permission denied to write MAC address to OTP!\n");
                            rv = CMD_FAIL;
                        } else if (BCM_ERR_OK != retVal) {
                            HOST_Log("Failed to write MAC address to OTP, ret:%d\n", retVal);
                            rv = CMD_FAIL;
                        } else {
                            rv = CMD_OK;
                        }
                    }
                } else if (0 == strncmp(input_str, "read", 4)) {
                    if ((rem_str == NULL) && (rem_str_len == 0UL)) {
                        if (HOST_SysOTPMacAddrRead(info,
                                    &macaddr1[0], &macaddr2[0], &value) != 0) {
                            HOST_Log("Failed to read MAC address to OTP!\n");
                            rv = CMD_FAIL;
                        } else {
                            HOST_Log("Location\t MAC Address\t\t Valid\n");
                            HOST_Log("0\t\t %02x:%02x:%02x:%02x:%02x:%02x\t %x\n",
                                    (uint32_t)(macaddr1[0]),
                                    (uint32_t)(macaddr1[1]),
                                    (uint32_t)(macaddr1[2]),
                                    (uint32_t)(macaddr1[3]),
                                    (uint32_t)(macaddr1[4]),
                                    (uint32_t)(macaddr1[5]),
                                    (value & 0x1));
                            HOST_Log("1\t\t %02x:%02x:%02x:%02x:%02x:%02x\t %x\n",
                                    (uint32_t)(macaddr2[0]),
                                    (uint32_t)(macaddr2[1]),
                                    (uint32_t)(macaddr2[2]),
                                    (uint32_t)(macaddr2[3]),
                                    (uint32_t)(macaddr2[4]),
                                    (uint32_t)(macaddr2[5]),
                                    ((value & 0x2) >> 1));
                            rv = CMD_OK;
                        }
                    }
                }
            }
        }
    }

    if (rv == CMD_USAGE) {
        HOST_Log(mgmt_otp_cmd_usage);
    }
done:
    return;
}

#ifdef ENABLE_HOST_COMMS_CMD_HANDLER
static const char mgmt_switch_cmd_usage[] =
"\tswitch read <addr> <0:master|1:slave>\n"
"\tswitch write <addr> <0:master|1:slave>\n"
"\tswitch cl22 read <mdioHwID> <phy> <reg> <0:master|1:slave>\n"
"\tswitch cl22 write <mdioHwID> <phy> <reg> <data> <0:master|1:slave>\n"
"\tswitch cl45 read <mdioHwID> <phy> <devType> <reg> <0:master|1:slave>\n"
"\tswitch cl45 write <mdioHwID> <phy> <devType> <reg> <data> <0:master|1:slave>\n"
"\tswitch port info <port>\n"
"\tswitch port mibs <port> [clear]\n"
"\tswitch age_time set <age_time>\n"
"\tswitch age_time get\n"
"\tswitch dumbfwd set <disable|enable>\n"
"\tswitch dumbfwd get\n"
"\tswitch admin_mode set <port> <disable|enable>\n"
"\tswitch admin_mode get <port>\n"
"\tswitch speed get <port>\n"
"\tswitch master_slave set <port> <slave|master>\n"
"\tswitch master_slave get <port>\n"
"\tswitch phy_lb_mode set <port> <disable|enable>\n"
"\tswitch phy_lb_mode get <port>\n"
"\tswitch jumbo_frame set <port> <disable|enable>\n"
"\tswitch jumbo_frame get <port>\n"
"\tswitch link sqi [port]\n"
"\tswitch pvidset <port> <pvid> [priority]\n"
"\tswitch ifilter [disable|enable]\n"
"\tswitch mirror enable <port bitmap> <probe port> <ingress|egress>\n"
"\tswitch mirror disable\n"
"\tswitch mirror status\n"
"\tswitch vlan get <vlan id>\n"
"\tswitch vlan add <vlan id> <portMask> <tagMask>\n"
"\tswitch vlan del <vlan id> <portMask>\n"
"\tswitch arl add <mac address(xx:xx:xx:xx:xx:xx)> <vlan> <port_mask>\n"
"\tswitch arl del <mac address(xx:xx:xx:xx:xx:xx)> <vlan>\n"
"\tswitch stream policer add <mac address> <vlan> <rate> <burst> <src_mask> <threshold> <interval> <report> <block>\n"
"\tswitch stream policer del <stream index>\n"
"\tswitch stream policer block <stream index>\n"
"\tswitch stream policer resume <stream index>\n"
"\tswitch stream policer show [stream index]\n"
"\tswitch stream policer find <mac address> <vlan> <src_mask>\n"
"\tswitch arl show\n"
"\n";

static void PrintRxStats(ETHER_RxStatsType *rxStats)
{
    HOST_Log("*** RX PKT STATS ***\n");
    HOST_Log("\t gdPkts : 0x%x\n", rxStats->gdPkts );
    HOST_Log("\t octets: %" PRIx64 "\n",
            ((uint64_t)rxStats->octetsHigh << 32UL) | rxStats->octetsLow);
    HOST_Log("\t allPkts: 0x%x\n", rxStats->allPkts);
    HOST_Log("\t brdCast: 0x%x\n", rxStats->brdCast);
    HOST_Log("\t multicast: 0x%x\n", rxStats->mutCast);
    HOST_Log("\t unicast: 0x%x\n", rxStats->uniCast);
    HOST_Log("\t pkts64: 0x%x\n", rxStats->pkts64);
    HOST_Log("\t pkts65_127: 0x%x\n", rxStats->pkts65_127);
    HOST_Log("\t pkts128_255: 0x%x\n", rxStats->pkts128_255);
    HOST_Log("\t pkts256_511: 0x%x\n", rxStats->pkts256_511);
    HOST_Log("\t pkts512_1023: 0x%x\n", rxStats->pkts512_1023);
    HOST_Log("\t pkts1024_1522: 0x%x\n", rxStats->pkts1024_1522);
    HOST_Log("\t pkts1024_MAX: 0x%x\n", rxStats->pkts1024_MAX);
    HOST_Log("\t pkts1523_2047: 0x%x\n", rxStats->pkts1523_2047);
    HOST_Log("\t pkts2048_4095: 0x%x\n", rxStats->pkts2048_4095);
    HOST_Log("\t pkts4096_8191: 0x%x\n", rxStats->pkts4096_8191);
    HOST_Log("\t pkts8192_MAX: 0x%x\n", rxStats->pkts8192_MAX);
    HOST_Log("\t pktsOvrSz: 0x%x\n", rxStats->pktsOvrSz);
    HOST_Log("\t pktsRxDrop: 0x%x\n", rxStats->pktsRxDrop);
    HOST_Log("\t pktsCrcErr: 0x%x\n", rxStats->pktsCrcErr);
    HOST_Log("\t pktsCrcAlignErr: 0x%x\n", rxStats->pktsCrcAlignErr);
    HOST_Log("\t pktsJabber: 0x%x\n", rxStats->pktsJabber);
    HOST_Log("\t pktsFrag: 0x%x\n", rxStats->pktsFrag);
    HOST_Log("\t pktsUndSz: 0x%x\n", rxStats->pktsUndSz);
    HOST_Log("\t pktsRxDiscard: 0x%x\n", rxStats->pktsRxDiscard);
    HOST_Log("\t rxPause: 0x%x\n", rxStats->rxPause);
}

static void PrintTxStats(ETHER_TxStatsType *txStats)
{
    HOST_Log("*** TX PKT STATS ***\n");
    HOST_Log("\t octets: 0x%x\n", txStats->octets);
    HOST_Log("\t brdCast: 0x%x\n", txStats->brdCast);
    HOST_Log("\t multicast: 0x%x\n", txStats->mutCast);
    HOST_Log("\t unicast: 0x%x\n", txStats->uniCast);
    HOST_Log("\t txDropped: 0x%x\n", txStats->txDropped);
    HOST_Log("\t txDroppedErr: 0x%x\n", txStats->txDroppedErr);
    HOST_Log("\t txCollision: 0x%x\n", txStats->txCollision);
    HOST_Log("\t txCollisionSingle: 0x%x\n", txStats->txCollisionSingle);
    HOST_Log("\t txCollisionMulti: 0x%x\n", txStats->txCollisionMulti);
    HOST_Log("\t txLateCollision: 0x%x\n", txStats->txLateCollision);
    HOST_Log("\t txExcessiveCollision: 0x%x\n", txStats->txExcessiveCollision);
    HOST_Log("\t txDeferredTransmit: 0x%x\n", txStats->txDeferredTransmit);
    HOST_Log("\t txFrameInDiscard: 0x%x\n", txStats->txFrameInDiscard);
    HOST_Log("\t txPause: 0x%x\n", txStats->txPause);
    HOST_Log("\t txQ0: 0x%x\n", txStats->txQ0);
    HOST_Log("\t txQ1: 0x%x\n", txStats->txQ1);
    HOST_Log("\t txQ2: 0x%x\n", txStats->txQ2);
    HOST_Log("\t txQ3: 0x%x\n", txStats->txQ3);
    HOST_Log("\t txQ4: 0x%x\n", txStats->txQ4);
    HOST_Log("\t txQ5: 0x%x\n", txStats->txQ5);
    HOST_Log("\t txQ6: 0x%x\n", txStats->txQ6);
    HOST_Log("\t txQ7: 0x%x\n", txStats->txQ7);
    HOST_Log("\t pkts64: 0x%x\n", txStats->pkts64);
    HOST_Log("\t pkts65_127: 0x%x\n", txStats->pkts65_127);
    HOST_Log("\t pkts128_255: 0x%x\n", txStats->pkts128_255);
    HOST_Log("\t pkts256_511: 0x%x\n", txStats->pkts256_511);
    HOST_Log("\t pkts512_1023: 0x%x\n", txStats->pkts512_1023);
    HOST_Log("\t pkts1024_MAX: 0x%x\n", txStats->pkts1024_MAX);
}

static void show_switch_mibs(ETHERSWT_MibType *mibs)
{
    HOST_Log("******Rx Stats******\n");
    PrintRxStats(&mibs->rxStats);
    HOST_Log("******Tx Stats******\n");
    PrintTxStats(&mibs->txStats);
}

void mgmt_switch_cmd_handler(MgmtInfoType *info,
        char *input_str, uint32_t input_str_len)
{
    char *curr_str = input_str;
    char *rem_str;
    uint32_t curr_str_len = input_str_len;
    uint32_t rem_str_len;
    int32_t retVal = 0;
    int32_t rv = CMD_USAGE;
    uint32_t value1, value2, value3;
    uint64_t u64Val;
    int32_t command_parse_status;
    int32_t i;
    ETHERSWT_PortInfoType portInfo;
    ETHERSWT_MibType mibs;
    ETHERSWT_PortMirrorStateType mirrorState;
    uint16_t u16Val;
    uint16_t u16Val2;
    uint8_t mac_addr[6];
    MDIO_RegAccessType access;
    MDIO_SlaveAddrType phy;
    MDIO_DeviceType dev;
    uint32_t write;

    if ((curr_str != NULL) && (curr_str_len != 0)) {
        if (!strncmp(curr_str, "port", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            if (!strncmp(curr_str, "info", 4)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;

                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtPortInfoGet(info, value1, &portInfo);
                if (retVal) {
                    rv = CMD_FAIL;
                    HOST_Log("Failed to get port info, retVal = %d\n", retVal);
                } else {
                    HOST_Log("Port : %u\n", portInfo.port);
                    HOST_Log("AdminMode : %s\n", (portInfo.adminMode ==
                                ETHXCVR_MODE_ACTIVE) ? "Active" : "Down");
                    HOST_Log("LinkStatus : %s\n", (portInfo.linkStatus ==
                                ETHXCVR_LINKSTATE_ACTIVE) ? "Up" : "Down");
                    switch (portInfo.speed) {
                    case ETHXCVR_SPEED_10MBPS:
                        HOST_Log("Speed : 10Mbps\n");
                        break;
                    case ETHXCVR_SPEED_100MBPS:
                        HOST_Log("Speed : 100Mbps\n");
                        break;
                    case ETHXCVR_SPEED_1000MBPS:
                        HOST_Log("Speed : 1000Mbps\n");
                        break;
                    default:
                        HOST_Log("Speed : Invalid\n");
                        break;
                    }
                    if ((portInfo.phyMedia == ETHXCVR_PHYMEDIA_10BASET1) ||
                        (portInfo.phyMedia == ETHXCVR_PHYMEDIA_100BASET1) ||
                        (portInfo.phyMedia == ETHXCVR_PHYMEDIA_1000BASET1)) {
                        HOST_Log("BrMode : %s\n",
                                (portInfo.masterEnable == ETHXCVR_BOOLEAN_TRUE) ?
                                "Master" : "Slave");
                    }
                    HOST_Log("JumboFrameMode : %s\n", (portInfo.jumboEnable ==
                                ETHXCVR_BOOLEAN_TRUE) ? "Enabled" : "Disabled");
                    HOST_Log("LoopBackMode : %s\n", (portInfo.loopbackEnable ==
                                ETHXCVR_BOOLEAN_TRUE) ? "Enabled" : "Disabled");
                    HOST_Log("Autoneg : %s\n", (portInfo.autonegEnable ==
                                ETHXCVR_BOOLEAN_TRUE) ? "Enabled" : "Disabled");
                    switch (portInfo.autonegComplete) {
                    case ETHXCVR_AUTONEGSTATUS_NO_ABILITY:
                        HOST_Log("AutonegComplete : No ability\n");
                        break;
                    case ETHXCVR_AUTONEGSTATUS_INCOMPLETE:
                        HOST_Log("AutonegComplete : Incomplete\n");
                        break;
                    case ETHXCVR_AUTONEGSTATUS_COMPLETE:
                        HOST_Log("AutonegComplete : Complete\n");
                        break;
                    default:
                        HOST_Log("AutonegComplete : Invalid\n");
                        break;
                    }
                    HOST_Log("Duplex : %s\n", (portInfo.duplex ==
                                ETHXCVR_DUPLEXMODE_FULL) ? "Full" : "Half" );
                    HOST_Log("LED : %s\n", portInfo.led ? "On" : "Off");
                    HOST_Log("LinkStateChangeCnt : %u\n", portInfo.linkStateChangeCnt);
                    switch (portInfo.busMode) {
                    case ETHXCVR_BUSMODE_INTG:
                        HOST_Log("Bus Mode : Integrated\n");
                        break;
                    case ETHXCVR_BUSMODE_RGMII:
                        HOST_Log("Bus Mode : RGMII\n");
                        break;
                    case ETHXCVR_BUSMODE_RVMII:
                        HOST_Log("Bus Mode : RVMII\n");
                        break;
                    case ETHXCVR_BUSMODE_RMII:
                        HOST_Log("Bus Mode : RMII\n");
                        break;
                    case ETHXCVR_BUSMODE_MII:
                        HOST_Log("Bus Mode : MII\n");
                        break;
                    case ETHXCVR_BUSMODE_SGMII:
                        HOST_Log("Bus Mode : SGMII\n");
                        break;
                    case ETHXCVR_BUSMODE_PCIE:
                        HOST_Log("Bus Mode : PCIE\n");
                        break;
                    default:
                        HOST_Log("Bus Mode: Unknown\n");
                        break;
                    }
                    switch (portInfo.phyMedia) {
                    case ETHXCVR_PHYMEDIA_10BASET1:
                        HOST_Log("Phy Media : 10Base-T1\n");
                        break;
                    case ETHXCVR_PHYMEDIA_10BASET:
                        HOST_Log("Phy Media : 10Base-T\n");
                        break;
                    case ETHXCVR_PHYMEDIA_100BASET1:
                        HOST_Log("Phy Media : 100Base-T1\n");
                        break;
                    case ETHXCVR_PHYMEDIA_100BASETX:
                        HOST_Log("Phy Media : 100Base-Tx\n");
                        break;
                    case ETHXCVR_PHYMEDIA_1000BASET1:
                        HOST_Log("Phy Media : 1000Base-T1\n");
                        break;
                    case ETHXCVR_PHYMEDIA_1000BASET:
                        HOST_Log("Phy Media : 1000Base-T\n");
                        break;
                    case ETHXCVR_PHYMEDIA_1000BASEX:
                        HOST_Log("Phy Media : 1000Base-X\n");
                        break;
                    default:
                        break;
                    }
                    HOST_Log("LinkSQI : %u\n", portInfo.linkSQI);
                    HOST_Log("PVID : %u\n", portInfo.pvid);
                    HOST_Log("Prio : %u\n", portInfo.prio);
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str, "mibs", 4)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if (rem_str != NULL) {
                    if (!strncmp(rem_str, "clear", 5)) {
                        retVal = HOST_EtherSwtMibClear(info, value1);
                        if (retVal) {
                            HOST_Log("Failed to clear mibs for port=%u, retVal=%d\n", value1, retVal);
                            rv = CMD_FAIL;
                        } else {
                            rv = CMD_OK;
                        }
                    } else {
                        goto done;
                    }
                } else if (rem_str_len != 0) {
                    goto done;
                } else {
                    /* rem_str==NULL and rem_str_len==0 */
                    retVal = HOST_EtherSwtMib(info, value1, &mibs);
                    if (retVal) {
                        HOST_Log("Failed to retrieve mibs for port=%u, retVal=%d\n", value1, retVal);
                        rv = CMD_FAIL;
                    } else {
                        show_switch_mibs(&mibs);
                        rv = CMD_OK;
                    }
                }
            }
        } else if (!strncmp(curr_str, "read", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            curr_str = rem_str;
            curr_str_len = rem_str_len;

            /* Fetch the address to be read */
            value1 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            value2 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            if (MCU_DEVICE_SLAVE_1 < value2) {
                goto done;
            }

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            retVal = HOST_EtherSwtRegRead(info, value2, value1, 1UL, &u64Val);
            if (0 != retVal) {
                HOST_Log("Error in switch read 0x%x\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
                HOST_Log("0x%016" PRIX64 "\n", u64Val);
            }
         } else if (!strncmp(curr_str, "write", 5)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            /* Fetch the address */
            value1 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            /* Fetch dataHigh*/
            value2 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            /* Fetch dataLow */
            value3 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            u64Val = ((uint64_t)value2 << 32UL) | value3;

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            value2 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            if (MCU_DEVICE_SLAVE_1 < value2) {
                goto done;
            }

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            retVal = HOST_EtherSwtRegWrite(info, value2, value1, 1UL, &u64Val);
            if (0 != retVal) {
                HOST_Log("Error in switch write 0x%x\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "cl", 2)) { /* cl22 and cl45 parsers */
            if (!strncmp(curr_str, "cl22", 4)) {
                access = MDIO_REGACCESS_CL22;
            } else if (!strncmp(curr_str, "cl45", 4)) {
                access = MDIO_REGACCESS_CL45;
            } else {
                goto done;
            }

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            if (!strncmp(curr_str, "read", 4)) {
                write = 0;
            } else if (!strncmp(curr_str, "write", 5)) {
                write = 1;
            } else {
                goto done;
            }

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* Fetch the mdio hw ID into value2 */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            value2 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* Fetch the phy address into phy */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            phy = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            if (MDIO_REGACCESS_CL45 == access) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                /* Fetch the dev address into dev */
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                dev = parse_integer(curr_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            }

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* Fetch the register address into u16Val */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            u16Val = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;

            if (write == 0) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                /* Fetch device[master or slave] into value1 */
                value1 = parse_integer(curr_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                retVal = HOST_MDIORead(info, access, value2, phy, dev, u16Val, &u16Val2, value1);
                if(0 != retVal) {
                    HOST_Log("Error in read function err:0x%x\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                    HOST_Log("0x%04X\n", u16Val2);
                }
            } else {
                /* Fetch register value into u16val2 */
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }

                u16Val2 = parse_integer(curr_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                /* Fetch device[master or slave] into value1 */
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                retVal = HOST_MDIOWrite(info, access, value2, phy, dev, u16Val, u16Val2, value1);
                if(0 != retVal) {
                    HOST_Log("Error in memory write 0x%x\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        } else if (!strncmp(curr_str, "age_time", 8)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) && (rem_str_len == 0)
                    && (!strncmp(curr_str,"get", 3))) {
                retVal = HOST_EtherSwtAgeTimeGet(info, &value1);
                if (retVal) {
                    rv = CMD_FAIL;
                    HOST_Log("Failed to get AGE time, retVal = %d\n", retVal);
                }else {
                    HOST_Log("Switch FDB age timeout (secs) =%d\n",value1);
                    rv = CMD_OK;
                }
            } else if ((rem_str != NULL) && (rem_str_len != 0)
                    && (!strncmp(curr_str,"set", 3))) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port bitmap */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                retVal = HOST_EtherSwtAgeTimeSet(info, value1);
                if (retVal) {
                    rv = CMD_FAIL;
                    HOST_Log("Failed to set AGE time, retVal = %d\n", retVal);
                } else {
                    HOST_Log("Switch FDB age timeout set to = %d secs\n",value1);
                    rv = CMD_OK;
                }
            }
        }else if(!strncmp(curr_str, "dumbfwd", 7)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) && (rem_str_len == 0)
                    && (!strncmp(curr_str,"get", 3))) {
                retVal = HOST_EtherSwtDumbFwdModeGet(info, &value1);
                if(retVal){
                    HOST_Log("Failed to get P8 Dumb Forwarding  Mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    HOST_Log("P8 Dumb Forwarding is %s\n", (value1 == ETHERSWT_DUMBFWD_ENABLE) ? "Enabled" : "Disabled");
                    rv = CMD_OK;
                }
            } else if ((rem_str != NULL) && (rem_str_len != 0)
                    && (!strncmp(curr_str,"set", 3))) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if (!strncmp(curr_str,"enable", 6)) {
                    value1 = ETHERSWT_DUMBFWD_ENABLE;
                } else if (!strncmp(curr_str,"disable", 7)) {
                    value1 = ETHERSWT_DUMBFWD_DISABLE;
                } else {
                    goto done;
                }
                retVal = HOST_EtherSwtDumbFwdModeSet(info, value1);
                if (retVal) {
                    rv = CMD_FAIL;
                    HOST_Log("Failed to set P8 Dumb Forward mode, retVal = %d\n", retVal);
                } else {
                    HOST_Log("P8 Dumb Forwarding %s\n", (value1 == ETHERSWT_DUMBFWD_ENABLE) ? "Enabled" : "Disabled");
                    rv = CMD_OK;
                }
            }
        }else if (!strncmp(curr_str, "mirror", 6)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str, "enable", 6)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                value1 = parse_integer(curr_str, &command_parse_status); /* port bitmap */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                value2 = parse_integer(curr_str, &command_parse_status); /* probe port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;

                if(!strncmp(curr_str, "ingress", 7)){
                    value3 = ETHERSWT_TRAFFICDIR_INGRESS;
                }else if(!strncmp(curr_str, "egress", 6)){
                    value3 = ETHERSWT_TRAFFICDIR_EGRESS;
                }else{
                    goto done;
                }

                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                retVal = HOST_EtherSwtMirrorEnable(info, value1, value2, value3);
                if (retVal) {
                    HOST_Log("Failed to enable mirroring mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }else if (!strncmp(curr_str,"disable", 7)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                retVal = HOST_EtherSwtMirrorDisable(info);
                if (retVal) {
                    HOST_Log("Failed to disable mirroring mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }else if (!strncmp(curr_str, "status", 6)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                retVal = HOST_EtherSwtMirrorStatus(info, &mirrorState, &u16Val, &u16Val2, &value1);
                if (retVal) {
                    HOST_Log("Failed to get mirror status, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                    goto done;
                }
                rv = CMD_OK;
                if (mirrorState == ETHERSWT_PORT_MIRROR_STATE_ENABLED) {
                    HOST_Log("Mirroring is enabled for ingress_port_bitmap=0x%x, egress_port_bitmap=0x%x and probe_port=%d\n",
                             u16Val, u16Val2, value1);
                } else {
                    HOST_Log("Mirroring is disabled\n");
                }
            }
        }else if(!strncmp(curr_str, "admin_mode", 10)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str,"get", 3)) {
                ETHXCVR_ModeType mode;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtXcvrAdminModeGet(info, value1, &mode);
                if (retVal) {
                    HOST_Log("Failed to get admin mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    HOST_Log("Admin mode: %s\n", (mode == ETHXCVR_MODE_ACTIVE) ? "Enabled" : "Disabled");
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str,"set", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if(!strncmp(curr_str,"enable", 6)) {
                    value2 = ETHXCVR_MODE_ACTIVE;
                }else if(!strncmp(curr_str,"disable", 7)) {
                    value2 = ETHXCVR_MODE_DOWN;
                }else{
                    HOST_Log("port admin_mode set <port> <disable|enable>\n");
                    goto done;
                }
                retVal = HOST_EtherSwtXcvrAdminModeSet(info, value1, value2);
                if (retVal) {
                    HOST_Log("Failed to set admin mode, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        }else if(!strncmp(curr_str, "speed", 5)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str,"get", 3)) {
                ETHXCVR_SpeedType speed;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtXcvrSpeedGet(info, value1, &speed);
                if (retVal) {
                    HOST_Log("Failed to get speed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    rv = CMD_OK;
                    switch (speed) {
                    case ETHXCVR_SPEED_10MBPS:
                        HOST_Log("Speed: 10mbps\n");
                        break;
                    case ETHXCVR_SPEED_100MBPS:
                        HOST_Log("Speed: 100mbps\n");
                        break;
                    case ETHXCVR_SPEED_1000MBPS:
                        HOST_Log("Speed: 1000mbps\n");
                        break;
                    default:
                        HOST_Log("Speed: Invalid\n");
                        rv = CMD_FAIL;
                        break;
                    }
                }
            }
        }else if(!strncmp(curr_str, "master_slave", 12)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str,"get", 3)) {
                ETHXCVR_BooleanType mode;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtXcvrMasterSlaveGet(info, value1, &mode);
                if ((retVal) && (BCM_ERR_NOSUPPORT != retVal)) {
                    HOST_Log("Failed to get master/slave mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    rv = CMD_OK;
                    switch (mode) {
                    case ETHXCVR_BOOLEAN_TRUE:
                        HOST_Log("Master/Slave: Master\n");
                        break;
                    case ETHXCVR_BOOLEAN_FALSE:
                        HOST_Log("Master/Slave: Slave\n");
                        break;
                    default:
                        HOST_Log("Master/Slave: Invalid\n");
                        rv = CMD_FAIL;
                        break;
                    }
                }
            } else if (!strncmp(curr_str,"set", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if (!strncmp(curr_str,"slave", 5)) {
                    value2 = ETHXCVR_BOOLEAN_FALSE;
                } else if (!strncmp(curr_str,"master", 6)) {
                    value2 = ETHXCVR_BOOLEAN_TRUE;
                } else {
                    HOST_Log("port master_slave set <port> <slave|master>\n");
                    goto done;
                }

                retVal =  HOST_EtherSwtXcvrMasterSlaveSet(info, value1, value2 );
                if (retVal) {
                    HOST_Log("Failed to set BR mode, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        }else if(!strncmp(curr_str, "phy_lb_mode", 11)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str,"get", 3)) {
                ETHXCVR_BooleanType mode;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtXcvrPhyLbGet(info, value1, &mode);
                if (retVal) {
                    HOST_Log("Failed to get phy_lb mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    HOST_Log("Phy_lb mode: %s\n", (mode == ETHXCVR_BOOLEAN_TRUE) ? "Enabled" : "Disabled");
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str,"set", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if(!strncmp(curr_str,"enable", 6)) {
                    value2 = ETHXCVR_BOOLEAN_TRUE;
                } else if(!strncmp(curr_str,"disable", 7)) {
                    value2 = ETHXCVR_BOOLEAN_FALSE;
                } else {
                    HOST_Log("port phy_lb_mode set <port> <disable|enable>\n");
                    goto done;
                }
                retVal = HOST_EtherSwtXcvrPhyLbSet(info, value1, value2 );
                if (retVal) {
                    HOST_Log("Failed to set phy loop back mode, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        }else if(!strncmp(curr_str, "jumbo_frame", 11)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str,"get", 3)) {
                ETHXCVR_BooleanType jumbo;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtXcvrJumboFrameModeGet(info, value1, &jumbo);
                if (retVal) {
                    HOST_Log("Failed to get jumbo frame mode, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    HOST_Log("Jumbo frame mode: %s\n", (jumbo == ETHXCVR_BOOLEAN_TRUE) ? "Enabled" : "Disabled");
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str,"set", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if(!strncmp(curr_str,"enable", 6)) {
                    value2 = ETHXCVR_BOOLEAN_TRUE;
                } else if(!strncmp(curr_str,"disable", 7)) {
                    value2 = ETHXCVR_BOOLEAN_FALSE;
                } else {
                    HOST_Log("port jumbo_frame set <port> <disable|enable>\n");
                    goto done;
                }
                retVal = HOST_EtherSwtXcvrJumboFrameModeSet(info, value1, value2 );
                if (retVal) {
                    HOST_Log("Failed to set jumbo frame mode, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        }else if(!strncmp(curr_str, "link", 4)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str, "sqi", 3)) {
                value2 = 0;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    MGMT_AVB_TRACE_LINE(20);
                    HOST_Log("%7s %10s\n", "Port", "SQI");
                    MGMT_AVB_TRACE_LINE(20);
                    for(i = 0; i < MAX_NUM_PORTS; i++){
                        ETHERSWT_PortInfoType portInfo;
                        memset(&portInfo, 0x00, sizeof(ETHERSWT_PortInfoType));
                        retVal = HOST_EtherSwtPortInfoGet(info, i, &portInfo);
                        if ((portInfo.linkStatus == ETHXCVR_LINKSTATE_DOWN)
                            || (retVal != BCM_ERR_OK)) {
                            continue;
                        }
                        HOST_Log("%7d %10d\n", i, portInfo.linkSQI);
                        rv = CMD_OK;
                    }
                } else {
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    value1 = parse_integer(curr_str, &command_parse_status);
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }
                    if (!HOST_EtherSwtXcvrLinkSQIGet(info, value1, &value2)) {
                        HOST_Log("Signal Quality Indicator(SQI) value of port %d is %d\n", value1, value2);
                        rv = CMD_OK;
                    } else {
                        rv = CMD_FAIL;
                    }
                }
            }
        } else if(!strncmp(curr_str, "arl", 3)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str, "add", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                HOST_EtherSwtConvertMac(curr_str, &mac_addr[0], &retVal);
                if (retVal != 0) {
                    HOST_Log("Invalid MAC\n");
                    rv = CMD_FAIL;
                    goto done;
                }

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_EtherSwtARLAdd(info, &mac_addr[0], u16Val, value1);
                if (retVal) {
                    HOST_Log("Failed to add ARL\n");
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str, "del", 3)){
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                HOST_EtherSwtConvertMac(curr_str, &mac_addr[0], &retVal);
                if (retVal != 0) {
                    HOST_Log("Invalid MAC\n");
                    rv = CMD_FAIL;
                    goto done;
                }

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                u16Val = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_EtherSwtARLDelete(info, &mac_addr[0], u16Val);
                if (BCM_ERR_NOPERM == retVal) {
                    HOST_Log("Permission denied to delete static ARL entry\n");
                    rv = CMD_FAIL;
                } else if (BCM_ERR_OK != retVal){
                    HOST_Log("Failed to delete ARL entry, ret=%d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str, "show", 4)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = mgmt_arl_show(info);
                if (retVal) {
                    HOST_Log("Failed to show ARL\n");
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        } else if(!strncmp(curr_str, "stream", 6)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str, "policer", 7)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if (!strncmp(curr_str, "add", 3)) {
                    uint32_t rate;
                    uint32_t burst;
                    uint32_t src_mask;
                    uint32_t threshold;
                    uint32_t interval;
                    uint32_t report;
                    uint32_t block;
                    uint32_t streamIdx;

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    HOST_EtherSwtConvertMac(curr_str, &mac_addr[0], &retVal);
                    if (retVal != 0) {
                        HOST_Log("Invalid MAC\n");
                        rv = CMD_FAIL;
                        goto done;
                    }

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    rate = parse_integer(curr_str, &command_parse_status); /* rate */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    burst = parse_integer(curr_str, &command_parse_status); /* burst */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    src_mask = parse_integer(curr_str, &command_parse_status); /* src_mask */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    threshold = parse_integer(curr_str, &command_parse_status); /* threshold */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    interval = parse_integer(curr_str, &command_parse_status); /* interval */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    report = parse_integer(curr_str, &command_parse_status); /* report */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    block = parse_integer(curr_str, &command_parse_status); /* block */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }
                    retVal = HOST_EtherSwtStreamPolicerAdd(info, &mac_addr[0], u16Val, rate,
                            burst, src_mask, threshold, interval, report, block, &streamIdx);
                    if (retVal) {
                        HOST_Log("Failed to add stream policer\n");
                        rv = CMD_FAIL;
                    } else {
                        HOST_Log("SUCCESS: Added stream policer id %u\n", streamIdx);
                        rv = CMD_OK;
                    }
                } else if (!strncmp(curr_str, "del", 3)) {
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    value1 = parse_integer(curr_str, &command_parse_status); /* stream index */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }
                    retVal = HOST_EtherSwtStreamPolicerDel(info, value1);
                    if (retVal) {
                        HOST_Log("Failed to delete stream policer\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else if (!strncmp(curr_str, "block", 5)) {
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    value1 = parse_integer(curr_str, &command_parse_status); /* stream index */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }
                    retVal = HOST_EtherSwtBlockStream(info, value1);
                    if (retVal) {
                        HOST_Log("Failed to delete stream policer\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else if (!strncmp(curr_str, "resume", 6)) {
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    value1 = parse_integer(curr_str, &command_parse_status); /* stream index */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }
                    retVal = HOST_EtherSwtResumeStream(info, value1);
                    if (retVal) {
                        HOST_Log("Failed to delete stream policer\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else if (!strncmp(curr_str, "show", 4)) {
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        COMMS_StreamPolicerSnapshotType snapshot;
                        retVal = HOST_EtherSwtStreamPolicerSnapshot(info, &snapshot);
                        if (BCM_ERR_OK == retVal) {
                            rv = CMD_OK;
                            HOST_Log("======= Stream Policer Snapshot =====\n");
                            for (i = 0UL; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
                                if( 0U != (snapshot.policer[i] & COMMS_STREAMPOLICERSTATE_VALID_MASK)) {
                                    HOST_Log("Stream ID %u: Static: %u Blocked: %u\n", i,
                                    (snapshot.policer[i] & COMMS_STREAMPOLICERSTATE_STATIC_MASK) >> COMMS_STREAMPOLICERSTATE_STATIC_SHIFT,
                                    (snapshot.policer[i] & COMMS_STREAMPOLICERSTATE_BLOCKED_MASK) >> COMMS_STREAMPOLICERSTATE_BLOCKED_SHIFT);
                                }
                            }
                        } else {
                            HOST_Log("Stream policer get snapshot failed, retVal = %d\n", retVal);
                            rv = CMD_FAIL;
                        }
                    } else {
                        /* parse stream index */
                        value1 = parse_integer(rem_str, &command_parse_status);
                        MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                        if (value1 >= COMMS_MAX_STREAM_POLICER_ENTRIES) {
                            HOST_Log("Stream policer index must be in the range: [0,%lu]\n", COMMS_MAX_STREAM_POLICER_ENTRIES-1UL);
                            rv = CMD_FAIL;
                            goto done;
                        }
                        COMMS_StreamPolicerStatusType status;
                        status.idx = value1;
                        retVal = HOST_EtherSwtStreamPolicerGetStatus(info, &status);
                        if (BCM_ERR_OK == retVal) {
                            rv = CMD_OK;
                            HOST_Log("Stream Idx:          %u\n", status.idx);
                            HOST_Log("MAC Address:         %02x:%02x:%02x:%02x:%02x:%02x\n",
                                    status.macAddress[0], status.macAddress[1],
                                    status.macAddress[2], status.macAddress[3],
                                    status.macAddress[4], status.macAddress[5]);
                            HOST_Log("VLAN:                %u\n", status.vlan);
                            HOST_Log("PortMask:            0x%x\n", status.portMask);
                            HOST_Log("Blocked:             %s\n", (TRUE == status.blocked) ? "TRUE": "FALSE");
                            HOST_Log("Static:              %s\n", (TRUE == status.isStatic) ? "TRUE": "FALSE");
                            HOST_Log("In-bound stats:      %u\n", status.greenCntr);
                            HOST_Log("Out-of-bound stats:  %u\n", status.redCntr);
                        } else {
                            HOST_Log("Stream policer get failed, retVal = %d\n", retVal);
                            rv = CMD_FAIL;
                        }
                    }
                } else if (!strncmp(curr_str, "find", 4)) {
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    HOST_EtherSwtConvertMac(curr_str, &mac_addr[0], &retVal);
                    if (retVal != 0) {
                        HOST_Log("Invalid MAC\n");
                        rv = CMD_FAIL;
                        goto done;
                    }

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                    value1 = parse_integer(curr_str, &command_parse_status); /* srcMask */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }

                    retVal = HOST_EtherSwtStreamPolicerFindIdx(info, mac_addr, u16Val, value1, &value2);
                    if (retVal) {
                        HOST_Log("Failed to find stream policer\n");
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                        HOST_Log("SUCCESS: Stream policer index %u\n", value2);
                    }
                }
            }
        } else if(!strncmp(curr_str, "vlan", 3)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            if (!strncmp(curr_str, "get", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_EtherSwtVlanGet(info, u16Val, &value1, &value2, &value3);
                if (retVal) {
                    HOST_Log("Failed to get VLAN information retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }else{
                    rv = CMD_OK;
                    HOST_Log("VLAN %u PortMask : 0x%x\n", u16Val, value1);
                    HOST_Log("VLAN %u Tagged Port Mask: 0x%x\n", u16Val, value2);
                    HOST_Log("VLAN %u Static Port Mask: 0x%x\n", u16Val, value3);
                }
            } else if (!strncmp(curr_str, "add", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* portMask */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value2 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_EtherSwtVlanPortAdd(info, u16Val, value1, value2);
                if (retVal) {
                    HOST_Log("Failed to add the port to the VLAN, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            } else if (!strncmp(curr_str, "del", 3)) {
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                u16Val = parse_integer(curr_str, &command_parse_status); /* vlan */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* portMask */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_EtherSwtVlanPortDel(info, u16Val, value1);
                if (BCM_ERR_NOPERM == retVal) {
                    HOST_Log("Permission denied to delete static port from the VLAN\n");
                    rv = CMD_FAIL;
                } else if (BCM_ERR_OK != retVal) {
                    HOST_Log("Failed to delete the port from the VLAN, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
        } else if (!strncmp(curr_str, "pvidset", 7)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            value1 = parse_integer(curr_str, &command_parse_status); /* port */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            value2 = parse_integer(curr_str, &command_parse_status); /* pvid */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            if ((rem_str == NULL) || (rem_str_len == 0)) {
                value3 = 0xFFFFFFFFUL; /* Indicate Default priority */
            } else {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value3 = parse_integer(curr_str, &command_parse_status); /* priority */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            }

            retVal = HOST_EtherSwtVlanPvidSet(info, value1, value2, value3);
            if (retVal) {
                HOST_Log("Failed to set the PVID, retVal = %d\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "ifilter", 7)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                retVal =  HOST_EtherSwtVlanIfilterGet(info, &value1);
                if (retVal) {
                    HOST_Log("Failed to get the VLAN ingress filter, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    HOST_Log("VLAN ingress filter is ");
                    if (value1 == ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED) {
                        HOST_Log("Enabled\n");
                    } else if (value1 == ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED) {
                        HOST_Log("Disabled\n");
                    } else {
                        HOST_Log("Unknown\n");
                    }
                    rv = CMD_OK;
                }
            } else {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                if (!strncmp(curr_str, "enable", 6)) {
                    value1 = ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED;
                } else if(!strncmp(curr_str, "disable", 7)) {
                    value1 = ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED;
                } else {
                    goto done;
                }

                retVal = HOST_EtherSwtVlanIfilterSet(info, value1);
                if (retVal) {
                    HOST_Log("Failed to set the VLAN ingress filter, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    HOST_Log("VLAN ingress filter has been");
                    if (value1 == ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED) {
                        HOST_Log("Enabled\n");
                    } else {
                        HOST_Log("Disabled\n");
                    }
                    rv = CMD_OK;
                }
            }
        }
    }
done:
    if (rv == CMD_USAGE) {
        HOST_Log(mgmt_switch_cmd_usage);
    }
    return;
}
#endif /* ENABLE_HOST_COMMS_CMD_HANDLER */

static int32_t mgmt_arl_show(MgmtInfoType *info)
{
    int32_t retVal;
    uint32_t i;
    uint32_t available = sizeof(ARLEntries)/sizeof(ETHERSWT_ARLEntryType);

    retVal = HOST_EtherSwtARLGet(info, &ARLEntries[0], &available);

    if (available > sizeof(ARLEntries)/sizeof(ETHERSWT_ARLEntryType)) {
        available = sizeof(ARLEntries)/sizeof(ETHERSWT_ARLEntryType);
    }

    if (retVal == 0) {
        HOST_Log("\tMAC\t\tVLAN\tPortMask\n");
        for (i = 0; i < available; i++) {
                HOST_Log("%02x:%02x:%02x:%02x:%02x:%02x\t",
                        ARLEntries[i].macAddr[0], ARLEntries[i].macAddr[1],
                        ARLEntries[i].macAddr[2], ARLEntries[i].macAddr[3],
                        ARLEntries[i].macAddr[4], ARLEntries[i].macAddr[5]);
                HOST_Log("%u\t", ARLEntries[i].vlanID);
                HOST_Log("0x%x\n", ARLEntries[i].portMask);
        }
    }
    return retVal;
}

#ifdef ENABLE_HOST_ETS_CMD_HANDLER
static const char mgmt_ets_cmd_usage[] =
"\tets admin_mode <disable|enable>\n"
"\tets stats <port>\n"
"\tets clearstats <port>\n"
"\tets numpdelaylostresp <port> <val>\n"
"\tets status\n"
"\tets portstatus [port]\n"
"\tets clock mode <gm|slave>\n"
"\tets sync absence timeout <seconds>\n"
"\tets port role <port> <master|slave>\n"
"\tets ascapable <port> <true|false>\n"
"\tets pdelay <port> <pdelay>\n"
"\tets interval <port> <sync|pdelay> <initialLogInterval>\n"
"\tets interval pdelay_oper <port> <logVal>\n"
"\tets config [port]\n"
"\tets time set <sec> <nanosec>\n"
#ifdef ENABLE_RECORD_NOTIFICATION
"\tets show records <port>\n"
"\tets start notification\n"
"\tets stop notification\n"
#endif
"\n";

int32_t show_mgmt_ets_port_stats(MgmtInfoType *info, uint32_t port)
{
    ETS_PortStatsAndStatusType portStatus;
    int32_t rv;

    memset(&portStatus, 0x00, sizeof(portStatus));

    rv = HOST_ETSPortStatus(info, port, &portStatus);
    if (rv != 0){
        goto done;
    }

    HOST_Log("\n");
    HOST_Log("Sync Rx                   : %d\n",portStatus.stats.syncRxCount);
    HOST_Log("Sync Tx                   : %d\n",portStatus.stats.syncTxCount);
    HOST_Log("Sync Tx Resends           : %d\n",portStatus.stats.syncTransmitTimeouts);
    HOST_Log("Followup Rx               : %d\n",portStatus.stats.followUpRxCount);
    HOST_Log("Followup Tx               : %d\n",portStatus.stats.followUpTxCount);
    HOST_Log("Sync Rx Timeouts          : %d\n",portStatus.stats.syncReceiptTimeouts);
    HOST_Log("FollowUp Rx Timeouts      : %d\n",portStatus.stats.followupReceiptTimeouts);
    HOST_Log("Sync Discards             : %d\n",portStatus.stats.syncRxDiscards);
    HOST_Log("FollowUp Rx Discards      : %d\n",portStatus.stats.followUpRxDiscards);

    HOST_Log("\n");
    HOST_Log("Pdelay Req Rx             : %d\n",portStatus.stats.pDelayReqRxCount);
    HOST_Log("Pdelay Req Tx             : %d\n",portStatus.stats.pDelayReqTxCount);
    HOST_Log("Pdelay Resp Rx            : %d\n",portStatus.stats.pDelayRespRxCount);
    HOST_Log("Pdelay Resp Tx            : %d\n",portStatus.stats.pDelayRespTxCount);
    HOST_Log("Pdelay Resp FollowUp Rx   : %d\n",portStatus.stats.pDelayRespFollowUpRxCount);
    HOST_Log("Pdelay Resp FollowUp Tx   : %d\n",portStatus.stats.pDelayRespFollowUpTxCount);
    HOST_Log("Pdelay Req Rx Discards    : %d\n",portStatus.stats.pDelayReqRxDiscards);
    HOST_Log("Pdelay Resp Rx Discards   : %d\n",portStatus.stats.pDelayRespRxDiscards);
    HOST_Log("Pdelay Rx Timeouts        : %d\n",portStatus.stats.pDelayReceiptTimeouts);
    HOST_Log("Bad Pdelay Values         : %d\n",portStatus.stats.badPdelayValues);
    HOST_Log("Pdelay Lost Resp Exceeded : %d\n",portStatus.stats.pDelayLostResponsesExceeded);
    HOST_Log("Pdelay Rx Discards        : %d\n",portStatus.stats.pDelayReqRxDiscards + portStatus.stats.pDelayRespRxDiscards);

    HOST_Log("\n");
    HOST_Log("Signaling Rx              : %d\n",portStatus.stats.signalingRxCount);
    HOST_Log("Signaling Tx              : %d\n",portStatus.stats.signalingTxCount);
    HOST_Log("Signaling Discards        : %d\n",portStatus.stats.signalingRxDiscards);

    HOST_Log("\n");
    HOST_Log("Tx Errors                 : %d\n",portStatus.stats.txErrors);
    HOST_Log("Timestamp Errors          : %d\n",portStatus.stats.tsErrors);
    HOST_Log("Bad headers               : %d\n",portStatus.stats.parseFailed);
    HOST_Log("PTP Discards              : %d\n",portStatus.stats.ptpDiscardCount);
    HOST_Log("Tx Total                  : %d\n",portStatus.stats.txConf);

done:
    return rv;
}

void show_ets_gm_status(ETS_GMStatusType gmStatus)
{
    switch (gmStatus){
        case ETS_GMSTATUS_STARTUP_ABSENT:
            HOST_Log("GM Status                 : Absent At Startup\n");
            break;
        case ETS_GMSTATUS_OPER_ABSENT:
            HOST_Log("GM Status                 : Absent\n");
            break;
        case ETS_GMSTATUS_UNDETERMINED:
            HOST_Log("GM Status                 : Undetermined\n");
            break;
        case ETS_GMSTATUS_DISABLED:
            HOST_Log("GM Status                 : Disabled\n");
            break;
        case ETS_GMSTATUS_NO_SLAVEPORT:
            HOST_Log("GM Status                 : No slave port\n");
            break;
        case ETS_GMSTATUS_PRESENT:
            HOST_Log("GM Status                 : Present\n");
            break;
        default:
            HOST_Log("GM Status                 : Invalid\n");
            break;
    }
}

/* Display ETS global status */
int32_t show_ets_status(MgmtInfoType *info)
{
    ETS_GlobalStatusType status;
    int32_t retVal;
    uint8_t *id = &status.clockId.id[0];

    memset(&status, 0x00, sizeof(status));
    retVal = HOST_ETSGlobalStatus(info, &status);
    if (retVal) {
        HOST_Log("Error: Could not get ETS Global Status\n");
        goto done;
    }

    HOST_Log("\n");
    switch (status.clockState) {
    case ETS_CLOCKSTATE_INIT_GM:
        HOST_Log("Clock state : Yet to be initialized as GM\n");
        break;
    case ETS_CLOCKSTATE_UPDATE_GM:
        HOST_Log("Clock state : Initialized as GM\n");
        break;
    case ETS_CLOCKSTATE_INIT_SLAVE:
        HOST_Log("Clock state : Yet to be initialized as slave\n");
        break;
    case ETS_CLOCKSTATE_UPDATE_SLAVE:
        HOST_Log("Clock state : Initialized as slave\n");
        break;
    default:
        HOST_Log("Clock state : Invalid\n");
        break;
    }
    HOST_Log("Local clock id            : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

    if ((status.clockState == ETS_CLOCKSTATE_INIT_SLAVE) || (status.clockState == ETS_CLOCKSTATE_UPDATE_SLAVE)){
        show_ets_gm_status(status.gmStatus);
        HOST_Log("Slave Port                : %d\n", status.slavePort);
        HOST_Log("Rate Ratio (In)           : 0x%x\n", status.rateRatioIn);
        HOST_Log("Rate Ratio (Out)          : 0x%x\n", status.gmRateRatio);
        HOST_Log("Is Sync Received          : %u\n", status.isSyncReceived);
        HOST_Log("Signaling Sequence Number : %u\n", status.signalingTxSeqId);
        HOST_Log("Requested Sync Interval   : %d\n", status.reqSyncLogInterval);
        HOST_Log("Is Signaling Timer Started: %u\n", status.isSignalingTimerStarted);
    }
    HOST_Log("BridgeLocalTime           : %" PRIu64 ".%u\n",
            status.networkTime.seconds, status.networkTime.nanoseconds);
    switch (status.networkTimeState) {
    case ETS_TIMESTAMPSTATE_SYNC:
        HOST_Log("NetworkTimeState : Network clock is synced to GM\n");
        break;
    case ETS_TIMESTAMPSTATE_UNSYNC:
        HOST_Log("NetworkTimeState : Network clock is not synced to GM\n");
        break;
    case ETS_TIMESTAMPSTATE_UNCERTAIN:
        HOST_Log("NetworkTimeState : Network clock is in holdover\n");
        break;
    default:
        HOST_Log("NetworkTimeState : Invalid\n");
        break;
    }
done:
    return retVal;
}

int32_t show_ets_port_status(MgmtInfoType *info, uint32_t intIfNumStart, uint32_t intIfNumEnd)
{
    int32_t retVal;
    ETS_PortStatsAndStatusType ets_port_status;
    uint32_t port;

    for (port = intIfNumStart; port <= intIfNumEnd; port++) {
        memset(&ets_port_status, 0x00, sizeof(ets_port_status));
        retVal = HOST_ETSPortStatus(info, port, &ets_port_status);
        if(retVal){
            HOST_Log("Error: Could not get ets port status\n");
            continue;
        }

        HOST_Log("\n");
        HOST_Log("Port                      : %d\n", ets_port_status.num);
        HOST_Log("Peer delay                : %d\n", ets_port_status.status.nbrPropDelay);
        HOST_Log("Rate ratio                : 0x%08x\n", ets_port_status.status.nbrRateRatio);
        HOST_Log("Is measuring delay        : %s\n", ets_port_status.status.isMeasuringPdelay ? "True" : "False");
        HOST_Log("Sync interval             : %d\n", ets_port_status.status.currentLogSyncInterval);
        HOST_Log("Pdelay interval           : %d\n", ets_port_status.status.currentLogPdelayInterval);
        HOST_Log("Sync reciept timeout      : %" PRIu64 "\n", ets_port_status.status.syncReceiptTimeoutInterval);
        HOST_Log("Number of lost responses  : %u\n", ets_port_status.status.numPdelayRespLost);
        HOST_Log("Peer delay request state  : %d\n", ets_port_status.status.pDelayReqState);
        HOST_Log("Peer delay response state : %d\n", ets_port_status.status.pDelayRespState);
        HOST_Log("Sync transmit state       : %d\n", ets_port_status.status.syncTxState);
        HOST_Log("Sync receive state        : %d\n", ets_port_status.status.syncRxState);
        HOST_Log("Pdelay Sequence Number    : %u\n", ets_port_status.status.pDelayTxSeqId);
        HOST_Log("Is AVnu Config Saved      : %u\n", ets_port_status.status.isAVnuPdelayConfigSaved);
        HOST_Log("Partner Clock Id          : ");
        HOST_Log("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                ets_port_status.status.partnerClockId.id[0],ets_port_status.status.partnerClockId.id[1],
                ets_port_status.status.partnerClockId.id[2],ets_port_status.status.partnerClockId.id[3],
                ets_port_status.status.partnerClockId.id[4],ets_port_status.status.partnerClockId.id[5],
                ets_port_status.status.partnerClockId.id[6],ets_port_status.status.partnerClockId.id[7]);
        HOST_Log("Partner Port Id           : %u\n", ets_port_status.status.partnerPortId);
        HOST_Log("Rx Sync Sequence Number   : %u\n", ets_port_status.status.syncLastRxSeqId);
        HOST_Log("Rx Correction             : %" PRIu64 "\n", ets_port_status.status.rxFollowupCorrection);
        HOST_Log("Rx Sync PDU interval      : %d\n", ets_port_status.status.rxPduInterval);
        HOST_Log("Precise Origin TimeStamp  : %" PRIu64 ".%u\n", ets_port_status.status.rxPOT.seconds, ets_port_status.status.rxPOT.nanoseconds);
        HOST_Log("Rx Sync Timestamp         : %" PRIu64 ".%u\n", ets_port_status.status.syncRxTimestamp.seconds, ets_port_status.status.syncRxTimestamp.nanoseconds);
        HOST_Log("Tx Sync Timestamp         : %" PRIu64 ".%u\n", ets_port_status.status.syncTxTimestamp.seconds, ets_port_status.status.syncTxTimestamp.nanoseconds);
        HOST_Log("Sync Info Available       : %u\n", ets_port_status.status.syncInfoAvailable);
        HOST_Log("Tx Sync Sequences Number  : %u\n", ets_port_status.status.syncTxSeqId);
        HOST_Log("PDelay Timestamp T1       : %" PRIu64 ".%u\n", ets_port_status.status.pDelayT1.seconds, ets_port_status.status.pDelayT1.nanoseconds);
        HOST_Log("PDelay Timestamp T2       : %" PRIu64 ".%u\n", ets_port_status.status.pDelayT2.seconds, ets_port_status.status.pDelayT2.nanoseconds);
        HOST_Log("PDelay Timestamp T3       : %" PRIu64 ".%u\n", ets_port_status.status.pDelayT3.seconds, ets_port_status.status.pDelayT3.nanoseconds);
        HOST_Log("PDelay Timestamp T4       : %" PRIu64 ".%u\n", ets_port_status.status.pDelayT4.seconds, ets_port_status.status.pDelayT4.nanoseconds);
        HOST_Log("Tx Correction             : %" PRIu64 "\n", ets_port_status.status.txFollowupCorrection);
    }

    return retVal;
}

void show_ets_port_role(ETS_RoleType role) {

    switch (role) {
    case ETS_ROLE_MASTER:
        HOST_Log("Port Role              : Master\n");
        break;
    case ETS_ROLE_SLAVE:
        HOST_Log("Port Role              : Slave\n");
        break;
    case ETS_ROLE_STACKING:
        HOST_Log("Port Role              : Stacking\n");
        break;
    default:
        HOST_Log("Port Role              : Invalid\n");
        break;
    }
}

int32_t show_ets_interface_config(MgmtInfoType *info, uint32_t port)
{
    ETS_ConfigType config;
    int32_t retVal = -1;
    uint32_t idx;

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != 0) {
        HOST_Log("Failed to obtain interface configuration, err:%d\n", retVal);
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        HOST_Log("Invalid port number %d\n", port);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    HOST_Log("\n");
    HOST_Log("HW Port                : %d\n", config.intfCfg[idx].hwPortNum);
    show_ets_port_role (config.intfCfg[idx].role);
    HOST_Log("AsCapable Mode         : %d\n", config.intfCfg[idx].asCapable);
    HOST_Log("PDelay                 : %d\n", config.intfCfg[idx].nbrPropDelay);
    HOST_Log("PDelay Init Interval   : %d\n", config.intfCfg[idx].initLogPdelayInterval);
    HOST_Log("Sync Init Interval     : %d\n", config.intfCfg[idx].initLogSyncInterval);
    HOST_Log("PDelay Oper Interval   : %d\n", config.intfCfg[idx].operLogPdelayInterval);
    HOST_Log("Sync Oper Interval     : %d\n", config.intfCfg[idx].operLogSyncInterval);
    HOST_Log("Sync Timeout Interval  : %d\n", config.intfCfg[idx].syncReceiptTimeout);
    HOST_Log("Allowed Lost Responses : %d\n", config.intfCfg[idx].allowedLostResponses);
    HOST_Log("Neighbor Rate Ratio    : 0x%x\n", config.intfCfg[idx].nbrRateRatio);

done:
    return retVal;
}

int32_t show_ets_global_config(MgmtInfoType *info)
{
    ETS_ConfigType config;
    int32_t retVal = -1;

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != 0) {
        HOST_Log("Failed to obtain global configuration, err:%d\n", retVal);
        goto done;
    }
    HOST_Log("\n");
    switch (config.clockMode) {
    case ETS_CLOCKMODE_GM:
        HOST_Log("Clock mode           : GM\n");
        break;
    case ETS_CLOCKMODE_SLAVE:
        HOST_Log("Clock mode           : Slave\n");
        break;
    default:
        HOST_Log("Clock mode           : Invalid\n");
        break;
    }
    HOST_Log("Sync Absence Timeout : %d\n", config.avnuSyncAbsenceTimeout);
    HOST_Log("Admin mode           : %d\n", config.adminMode);
    HOST_Log("Boundary Mode Enable : %d\n", config.boundaryModeEnable);
    HOST_Log("GM Rate Ratio        : 0x%x\n", config.gmRateRatio);

done:
    return retVal;
}

#ifdef ENABLE_RECORD_NOTIFICATION
void show_ets_port_records(uint32_t port, ETS_RecordType* record, uint32_t size)
{
    uint32_t i;

    HOST_Log("Port %u ::\n", port);
    for (i = 0UL; i < size; ++i) {
        if (((record[i].flag & ETS_RECORD_IS_VALID_MASK) >> ETS_RECORD_IS_VALID_SHIFT) == 1UL) {
            HOST_Log("\n============Record %u==============\n", i);
            HOST_Log("Packet Type            : %lu\n", (record[i].flag & ETS_RECORD_PACKET_TYPE_MASK) >> ETS_RECORD_PACKET_TYPE_SHIFT);
            HOST_Log("Sequence Number        : %u\n", (record[i].flag & ETS_RECORD_SEQ_NUM_MASK) >> ETS_RECORD_SEQ_NUM_SHIFT);
            HOST_Log("GM Time                : %lus:%luns\n", record[i].gmTime.s, record[i].gmTime.ns);
            HOST_Log("Local Time             : %lus:%luns\n", record[i].localTime.s, record[i].localTime.ns);
            show_ets_port_role((record[i].flag & ETS_RECORD_ROLE_MASK) >> ETS_RECORD_ROLE_SHIFT);
            show_ets_gm_status((record[i].flag & ETS_RECORD_GM_STATUS_MASK) >> ETS_RECORD_GM_STATUS_SHIFT);
            HOST_Log("Record Number          : %u\n", (record[i].portAndRecNum & ETS_RECORD_NUM_MASK) >> ETS_RECORD_NUM_SHIFT);
            HOST_Log("LinkUp Status          : %u\n", (record[i].flag & ETS_RECORD_IS_LINK_UP_MASK) >> ETS_RECORD_IS_LINK_UP_SHIFT);
            HOST_Log("IsTx                   : %u\n", (record[i].flag & ETS_RECORD_IS_TX_MASK) >> ETS_RECORD_IS_TX_SHIFT);
        }
    }
}
#endif

void mgmt_ets_cmd_handler(MgmtInfoType *info,
        char *input_str, uint32_t input_str_len)
{
    int32_t retVal = 0;
    int32_t rv = CMD_USAGE;
    char *curr_str = input_str;
    uint32_t curr_str_len = input_str_len;
    char * rem_str = NULL;
    uint32_t rem_str_len;
    uint32_t value1, value2;
    int32_t command_parse_status;

    if ((input_str != NULL) && (input_str_len != 0)){
        if (!strncmp(curr_str, "admin_mode", 10)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if(!strncmp(curr_str, "enable", 6)) {
                value1 = ETS_ADMINMODE_ENABLE;
            } else if (!strncmp(curr_str, "disable", 7)) {
                value1 = ETS_ADMINMODE_DISABLE;
            } else {
                HOST_Log("ets mode <disable|enable>\n");
                goto done;
            }
            retVal = HOST_ETSGlobalAdminModeSet(info, value1);
            if (retVal) {
                HOST_Log("ets global admin mode set failed, ret = %d\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "stats", 5)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }
            value1 = parse_integer(curr_str, &command_parse_status); /* port */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            retVal = show_mgmt_ets_port_stats(info, value1);
            if (retVal) {
                HOST_Log("ets stats get  failed, retVal = %d\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "clearstats", 10)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }
            value1 = parse_integer(curr_str, &command_parse_status); /* port */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            retVal = HOST_ETSPortStatsClear(info, value1);
            if (retVal) {
                HOST_Log("ets stats clear  failed, retVal = %d\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if(!strncmp(curr_str, "status", 6)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }
            retVal = show_ets_status(info);
            if (retVal) {
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "portstatus", 10)){
            value1 = 0UL;
            value2 = MAX_NUM_PORTS;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                value1 = parse_integer(curr_str, &command_parse_status); /* port number*/
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                value2 = value1;
            }
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }
            retVal = show_ets_port_status(info, value1, value2);
            if (retVal) {
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "config", 6)){
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }
                value1 = parse_integer(curr_str, &command_parse_status); /* port number*/
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = show_ets_interface_config(info, value1);
            } else {
                retVal = show_ets_global_config(info);
            }
            if (retVal) {
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "numpdelaylostresp", 17)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            value1 = parse_integer(curr_str, &command_parse_status); /* port */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }
            value2 = parse_integer(curr_str, &command_parse_status); /* last response */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            retVal = HOST_ETSNumLostRespSet(info, value1, value2);
            if (retVal) {
                HOST_Log("ets port num of lost responses set failed, ret = %d\n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "clock", 5)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if(!strncmp(curr_str, "mode", 5)){
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if(!strncmp(curr_str, "gm", 2)){
                    value1 = ETS_CLOCKMODE_GM;
                }else if(!strncmp(curr_str, "slave", 5)){
                    value1 = ETS_CLOCKMODE_SLAVE;
                }else{
                    HOST_Log("ets clock mode <gm|slave>\n");
                    goto done;
                }
                retVal = HOST_ETSClockModeSet(info, value1);
                if(retVal){
                    HOST_Log("Error in configuring clock mode, ret = %d \n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }else{
                HOST_Log("ets clock mode <gm|slave>\n");
            }
        } else if (!strncmp(curr_str, "sync", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if (!strncmp(curr_str, "absence", 7)){
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                if (!strncmp(curr_str, "timeout", 7)){
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }

                    value1 = parse_integer(curr_str, &command_parse_status); /* sync absence timeout */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                    retVal = HOST_ETSSyncAbsenceTimeoutSet(info, value1);
                    if(retVal){
                        HOST_Log("Error in configuring sync absence timeout, ret = %d \n", retVal);
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                }else{
                    HOST_Log("ets sync absence timeout <seconds>\n");
                }
            }else{
                HOST_Log("ets sync absence timeout <seconds>\n");
            }
        } else if (!strncmp(curr_str, "port", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if (!strncmp(curr_str, "role", 4)){
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* port number */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                if (!strncmp(curr_str, "master", 6)) {
                    value2 = ETS_ROLE_MASTER;
                } else if (!strncmp(curr_str, "slave", 5)) {
                    value2 = ETS_ROLE_SLAVE;
                } else {
                    HOST_Log("ets port role <port> <master|slave>\n");
                    goto done;
                }
                retVal = HOST_ETSPortRoleSet(info, value1, value2);
                if(retVal){
                    HOST_Log("Error in configuring port role, ret = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }else{
                HOST_Log("ets port role <port> <master|slave>\n");
            }
        } else if (!strncmp(curr_str, "ascapable", 9)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            value1 = parse_integer(curr_str, &command_parse_status); /* port number */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            if (!strncmp(curr_str, "true", 4)){
                value2 = ETS_ADMINMODE_ENABLE;
            } else if (!strncmp(curr_str, "false", 5)){
                value2 = ETS_ADMINMODE_DISABLE;
            } else {
                HOST_Log("ets ascapable <port> <true|false>\n");
                goto done;
            }
            retVal = HOST_ETSAsCapableModeSet(info, value1, value2);
            if(retVal){
                HOST_Log("Error in configuring AS capability, ret = %d \n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "pdelay", 6)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            value1 = parse_integer(curr_str, &command_parse_status); /* port number */
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            value2 = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            retVal = HOST_ETSNbrPdelaySet(info, value1, value2);
            if(retVal){
                HOST_Log("Error in configuring PDelay value, ret = %d \n", retVal);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        } else if (!strncmp(curr_str, "interval", 8)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            if (!strncmp("pdelay_oper", curr_str, strlen("pdelay_oper"))){
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* port number */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                value2 = parse_integer(curr_str, &command_parse_status); /*pdelay interval */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_ETSOperPdelayIntervalSet(info, value1, (int32_t)value2);
                if(retVal){
                    HOST_Log("Error in configuring Operatonal Log Pdelay, ret = %d \n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            } else {
                value1 = parse_integer(curr_str, &command_parse_status); /* port number */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                if (!strncmp(curr_str, "pdelay", 6)){
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }

                    value2 = parse_integer(curr_str, &command_parse_status); /*pdelay interval */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                    retVal = HOST_ETSInitPdelayIntervalSet(info, value1, (int32_t)value2);
                    if(retVal){
                        HOST_Log("Error in configuring Initial Log Pdelay Interval, ret = %d \n", retVal);
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                } else if(!strncmp(curr_str, "sync", 4)){
                    if ((rem_str == NULL) || (rem_str_len == 0)) {
                        goto done;
                    }
                    curr_str = rem_str;
                    curr_str_len = rem_str_len;
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str != NULL) || (rem_str_len != 0)) {
                        goto done;
                    }

                    value2 = parse_integer(curr_str, &command_parse_status); /*sync interval */
                    MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                    retVal = HOST_ETSInitSyncIntervalSet(info, value1, (int32_t)value2);
                    if(retVal){
                        HOST_Log("Error in configuring Initial Log SYNC Interval, ret = %d \n", retVal);
                        rv = CMD_FAIL;
                    } else {
                        rv = CMD_OK;
                    }
                }
            }
        } else if (!strncmp(curr_str, "time", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if (!strncmp(curr_str, "set", 3)) {
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* sec_low32 */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value2 = parse_integer(curr_str, &command_parse_status); /* nanosec */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);


                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_ETSTimeSet(info, 0UL, value1, value2);
                if (retVal) {
                    HOST_Log("ets time set failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    rv = CMD_OK;
                }
            }
#ifdef ENABLE_RECORD_NOTIFICATION
        } else if (!strncmp(curr_str, "show", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if (!strncmp(curr_str, "records", 7)) {
                ETS_RecordType record[ETS_NUM_RECORDS_PER_INTF];

                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    goto done;
                }
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

                value1 = parse_integer(curr_str, &command_parse_status); /* port */
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_ETSGetRecord(value1, record, ETS_NUM_RECORDS_PER_INTF);
                if (retVal) {
                    HOST_Log("ets show records failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                } else {
                    show_ets_port_records(value1, record, ETS_NUM_RECORDS_PER_INTF);
                    rv = CMD_OK;
                }
            }
        } else if ((!strncmp(curr_str, "start", 5)) || (!strncmp(curr_str, "stop", 4))) {
            uint8_t isStart;
            char *curr_str_store = curr_str;
            isStart = !strncmp(curr_str, "start", 5);
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            curr_str = rem_str;
            curr_str_len = rem_str_len;
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            if (!strncmp(curr_str, "notification", 11)) {

                if ((rem_str != NULL) || (rem_str_len != 0)) {
                    goto done;
                }

                retVal = HOST_ETSStartStopSendingRecord(info, isStart);
                if (retVal) {
                    HOST_Log("ets %s notification failed, retVal = %d\n", strtok(curr_str_store, " "), retVal);
                    rv = CMD_FAIL;
                } else {
                    HOST_Log("ets %s notification completed\n", strtok(curr_str_store, " "));
                    rv = CMD_OK;
                }
            }
        }
#else   /* ENABLE_RECORD_NOTIFICATION */
        }
#endif /* ENABLE_RECORD_NOTIFICATION */
    }

done:
    if (rv == CMD_USAGE) {
        HOST_Log(mgmt_ets_cmd_usage);
    }
    return;
}
#endif /* ENABLE_HOST_ETS_CMD_HANDLER */

#ifdef ENABLE_DBGMEM
static const char mgmt_mem_cmd_usage[] =
"\tmem read <addr> <width 8|16|32> <device ID>\n"
"\t                                 device ID 0: master\n"
"\t                                 device ID 1: slave\n"
"\tmem write <addr> <width 8|16|32> <data> <device ID>\n"
"\t                                         device ID 0: master\n"
"\t                                         device ID 1: slave\n"
"\n";

void mgmt_mem_cmd_handler(MgmtInfoType *info,
        char *input_str, uint32_t input_str_len)
{
    char *curr_str = input_str;
    char *rem_str = NULL;
    uint32_t curr_str_len = input_str_len;
    uint32_t rem_str_len = 0;
    int32_t command_parse_status = 0;
    uint32_t addr = 0;
    uint32_t width = 0;
    uint32_t data = 0;
    int32_t rv = CMD_USAGE;
    uint32_t err = -1;
    uint32_t write = 0;
    DBGMEM_HandleType memHdl;
    uint32_t destn;

    memHdl.u32Ptr = &data;

    if ((curr_str != NULL) && (curr_str_len != 0)) {
        split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
        if ((rem_str == NULL) || (rem_str_len == 0)) {
            goto done;
        }

        if (!strncmp(curr_str, "read", 4)) {
            write = 0;
        } else if (!strncmp(curr_str, "write", 5)) {
            write = 1;
        } else {
            goto done;
        }

        curr_str = rem_str;
        curr_str_len = rem_str_len;
        /* Fetch the address */
        split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
        if ((rem_str == NULL) || (rem_str_len == 0)) {
            goto done;
        }
        addr = parse_integer(curr_str, &command_parse_status);
        MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

        curr_str = rem_str;
        curr_str_len = rem_str_len;

        /* Fetch the width */
        split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
        if ((rem_str == NULL) || (rem_str_len == 0)) {
            goto done;
        }
        width = parse_integer(curr_str, &command_parse_status);
        MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

        if ((width != 8) && (width != 16) && (width != 32)) {
            HOST_Log("Unsupported width:%u\n", width);
            goto done;
        }

        curr_str = rem_str;
        curr_str_len = rem_str_len;

        if (write == 0) {

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            destn = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            if (MCU_DEVICE_SLAVE_1 < destn) {
                HOST_Log("read <addr> <width 8|16|32> <device ID [0|1]>\n");
                goto done;
            }

            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            err = HOST_MemoryRead(info, destn, addr, width, 1UL, memHdl.u8Ptr);
            if(0 != err) {
                HOST_Log("Error in read function err:0x%x\n", err);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
                if (width == 8) {
                    HOST_Log("0x%02X\n", *memHdl.u8Ptr);
                } else if (width == 16) {
                    HOST_Log("0x%04X\n", *memHdl.u16Ptr);
                } else {
                    HOST_Log("0x%08X\n", *memHdl.u32Ptr);
                }
            }
        } else {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            data = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;

            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);

            destn = parse_integer(curr_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
            if (MCU_DEVICE_SLAVE_1 < destn) {
                HOST_Log("write <addr> <width 8|16|32> <device ID [0|1]>\n");
                goto done;
            }

            if ((rem_str != NULL) || (rem_str_len != 0)) {
                goto done;
            }

            //convert to an array of "width" size starting at &data
            if (width == 8) {
                *memHdl.u8Ptr = (uint8_t)data;
            } else if (width == 16) {
                *memHdl.u16Ptr = (uint16_t)data;
            } /* other option is 32 and nothing extra needs to be done for that */

            err = HOST_MemoryWrite(info, destn, addr, width, 1UL, memHdl.u8Ptr);
            if(0 != err) {
                HOST_Log("Error in memory write 0x%x\n", err);
                rv = CMD_FAIL;
            } else {
                rv = CMD_OK;
            }
        }
    }
done:
    if(CMD_USAGE == rv) {
        HOST_Log(mgmt_mem_cmd_usage);
    }
    return;
}
#endif /* ENABLE_DBGMEM */


#ifdef ENABLE_HOST_CFP_CMD_HANDLER
static const char mgmt_cfp_cmd_usage[] =
"\tcfp add <row> <slice> <filename>\n"
"\tcfp delete <row>\n"
"\tcfp update <row> <filename>\n"
"\tcfp stats <row>\n"
"\tcfp show\n"
"\tcfp show <row>\n"
"\tcfp port <num> enable\n"
"\tcfp port <num> disable\n"
"\n";

CFP_L3FramingType formatToL3Framing(uint32_t format)
{
    switch(format) {
        case 0UL:
            return CFP_L3FRAMING_IPV4;
        case 1UL:
            return CFP_L3FRAMING_IPV6;
        case 2UL:
            return CFP_L3FRAMING_NONIP;
        default:
            return CFP_L3FRAMING_NONIP+1U;
    }
}

char* l3FramingToStr(CFP_L3FramingType l3Framing)
{
    switch (l3Framing) {
        case CFP_L3FRAMING_NONIP:
            return "Non-IP";
        case CFP_L3FRAMING_IPV4:
            return "IPv4";
        case CFP_L3FRAMING_IPV6:
            return "IPv6";
        default:
            return "Invalid L3Framing";
    }
}

char* UDFBaseIdToStr(uint32_t baseId)
{
    switch (baseId) {
        case CFP_UDFBASE_SOP:
            return "SOP";
        case CFP_UDFBASE_ENDL2HDR:
            return "End-L2-Header";
        case CFP_UDFBASE_ENDL3HDR:
            return "End-L3-Header";
        default:
            return "Invalid BaseId";
    }
}

void printTag(uint32_t aTag, uint32_t aTagMask)
{
    char *unTagged   = "Un-Tagged";
    char *prioTagged = "Priority-Tagged";
    char *vlanTagged = "VLAN-Tagged";
    uint8_t flag     = FALSE;

    /* check for invalid case
       1. Un-tagged or VLAN-Tagged is not a supported combination
       2. Tag-status bit-map should not be zero */
    if(((aTag & CFP_KEY_TAG_UN_TAGGED_MASK) && (aTag & CFP_KEY_TAG_VLAN_TAGGED_MASK)
       && (!(aTag & CFP_KEY_TAG_PRIO_TAGGED_MASK))) ||
        ((!(aTag & CFP_KEY_TAG_UN_TAGGED_MASK) && (!(aTag & CFP_KEY_TAG_VLAN_TAGGED_MASK)))
       && (!(aTag & CFP_KEY_TAG_PRIO_TAGGED_MASK)))) {
        HOST_Log("%s","Invalid Acceptable Frame Type");
    } else {
        if(aTag & CFP_KEY_TAG_UN_TAGGED_MASK) {
            HOST_Log("%s", unTagged);
            flag = TRUE;
        }
        if(aTag & CFP_KEY_TAG_PRIO_TAGGED_MASK) {
            if(FALSE == flag) {
                HOST_Log("%s", prioTagged);
            } else {
                HOST_Log(", %s", prioTagged);
            }
            flag = TRUE;
        }
        if(aTag & CFP_KEY_TAG_VLAN_TAGGED_MASK) {
            if(FALSE == flag) {
                HOST_Log("%s", vlanTagged);
            } else {
                HOST_Log(", %s", vlanTagged);
            }
        }
    }

    if ((aTag & CFP_KEY_TAG_ID_VALID_MASK) == CFP_KEY_TAG_ID_VALID_MASK) {
        HOST_Log(" VID %lu", (aTag & CFP_KEY_TAG_ID_MASK) >> CFP_KEY_TAG_ID_SHIFT);
        if(0xFFFUL != (aTagMask & CFP_KEY_TAG_IDMASK_MASK) >> CFP_KEY_TAG_IDMASK_SHIFT) {
            HOST_Log(" VID Mask 0x%lx", (aTagMask & CFP_KEY_TAG_IDMASK_MASK) >> CFP_KEY_TAG_IDMASK_SHIFT);
        }
    }
    if ((aTag & CFP_KEY_TAG_DEI_VALID_MASK) == CFP_KEY_TAG_DEI_VALID_MASK) {
        HOST_Log(" DEI %lu", (aTag & CFP_KEY_TAG_DEI_MASK) >> CFP_KEY_TAG_DEI_SHIFT);
    }
    if ((aTag & CFP_KEY_TAG_PCP_VALID_MASK) == CFP_KEY_TAG_PCP_VALID_MASK) {
        HOST_Log(" PCP %lu", (aTag & CFP_KEY_TAG_PCP_MASK) >> CFP_KEY_TAG_PCP_SHIFT);
        if(CFP_KEY_TAG_PCPMASK_MASK != (aTagMask & CFP_KEY_TAG_PCPMASK_MASK)) {
            HOST_Log(" PCP Mask 0x%lx", (aTagMask & CFP_KEY_TAG_PCPMASK_MASK) >> CFP_KEY_TAG_PCPMASK_SHIFT);
        }
    }
}

int32_t show_cfp_rule(MgmtInfoType *info, CFP_RuleType* rule)
{
    uint32_t i;
    int32_t  retVal = -1;
    uint16_t slice = (rule->rowAndSlice & CFP_ROWANDSLICE_SLICE_MASK) >> CFP_ROWANDSLICE_SLICE_SHIFT;

    if (CFP_MAX_SLICES > slice) {
        HOST_Log("\nSlice                    : %u\n", slice);
        HOST_Log("================== Key ==================\n");
        HOST_Log("L3 Framing               : %s\n", l3FramingToStr(rule->key.l3Framing));
        HOST_Log("L2 Framing               : ");
        switch (rule->key.l2Framing) {
            case CFP_L2FRAMING_DIXV2:
                HOST_Log("DIXv2\n");
                break;
            case CFP_L2FRAMING_SNAP_PUB:
                HOST_Log("SNAP Public\n");
                break;
            case CFP_L2FRAMING_SNAP_PVT:
                HOST_Log("SNAP Private\n");
                break;
            case CFP_L2FRAMING_LLC:
                HOST_Log("LLC\n");
                break;
            default:
                break;
        }
        HOST_Log("Ingress Port Bitmap      : 0x%.4x\n", rule->key.ingressPortBitmap);
        HOST_Log("C-Tag                    : ");
        printTag(rule->key.cTagFlags, rule->key.cTagMask);
        HOST_Log("\nS-Tag                    : ");
        printTag(rule->key.sTagFlags, rule->key.sTagMask);

        HOST_Log("\n");
        if (rule->key.l3Framing == CFP_L3FRAMING_NONIP) {
            if ((rule->key.flagsMask & CFP_KEY_ETHTYPE_MASK) != 0UL) {
                HOST_Log("Ethertype                : 0x%lx\n", rule->key.flags & CFP_KEY_ETHTYPE_MASK);
                if(0xFFFF != (rule->key.flagsMask & CFP_KEY_ETHTYPE_MASK)) {
                    HOST_Log("Ethertype Mask           : 0x%lx\n", rule->key.flagsMask & CFP_KEY_ETHTYPE_MASK);
                }
            }
        } else {
            if ((rule->key.flagsMask & CFP_KEY_TTL_MASK) == CFP_KEY_TTL_MASK) {
                HOST_Log("TTL                      : %lu\n", rule->key.flags & CFP_KEY_TTL_MASK);
            }
            if ((rule->key.flagsMask & CFP_KEY_AUTH_MASK) == CFP_KEY_AUTH_MASK) {
                HOST_Log("Auth                     : %lu\n", (rule->key.flags & CFP_KEY_AUTH_MASK) >> CFP_KEY_AUTH_SHIFT);
            }
            if ((rule->key.flagsMask & CFP_KEY_FRAG_MASK) == CFP_KEY_FRAG_MASK) {
                HOST_Log("Fragmentation            : %lu\n", (rule->key.flags & CFP_KEY_FRAG_MASK) >> CFP_KEY_FRAG_SHIFT);
            }
            if ((rule->key.flagsMask & CFP_KEY_NONFIRSTFRAG_MASK) == CFP_KEY_NONFIRSTFRAG_MASK) {
                HOST_Log("Non-First Fragment       : %lu\n", (rule->key.flags & CFP_KEY_NONFIRSTFRAG_MASK) >> CFP_KEY_NONFIRSTFRAG_SHIFT);
            }
            if ((rule->key.flagsMask & CFP_KEY_PROTO_MASK) == CFP_KEY_PROTO_MASK) {
                HOST_Log("Protocol                 : %lu\n", (rule->key.flags & CFP_KEY_PROTO_MASK) >> CFP_KEY_PROTO_SHIFT);
            }
            if ((rule->key.flagsMask & CFP_KEY_TOS_MASK) == CFP_KEY_TOS_MASK) {
                HOST_Log("Type Of Service          : %lu\n", (rule->key.flags & CFP_KEY_TOS_MASK) >> CFP_KEY_TOS_SHIFT);
            }
        }

        for (i = 0UL; i < rule->key.numEnabledUDFs; i++) {
            HOST_Log("UDF[%u] Base: ", i);
            switch ((rule->key.udfList[i].baseAndOffset & CFP_UDF_BASE_MASK) >> CFP_UDF_BASE_SHIFT) {
                case CFP_UDFBASE_SOP:
                    HOST_Log("Start of Packet ");
                    break;
                case CFP_UDFBASE_ENDL2HDR:
                    HOST_Log("End of L2 Header");
                    break;
                case CFP_UDFBASE_ENDL3HDR:
                    HOST_Log("End of L2 Header");
                    break;
                default:
                    break;
            }
            HOST_Log(" Offset: %u Value: 0x%.4x Mask: 0x%.4x\n", (rule->key.udfList[i].baseAndOffset & CFP_UDF_OFFSET_MASK) >> CFP_UDF_OFFSET_SHIFT,
                    rule->key.udfList[i].value, rule->key.udfList[i].mask);
        }

        HOST_Log("\n================ Action =================\n");
        switch ((rule->action.dstMapIBFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >> CFP_ACTION_CHANGE_FWDMAP_SHIFT) {
            case CFP_CHANGEFWDMAP_REM:
                HOST_Log("Destination In-Bound Map : Remove 0x%x\n", (rule->action.dstMapIBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            case CFP_CHANGEFWDMAP_REP:
                HOST_Log("Destination In-Bound Map : Replace 0x%x\n", (rule->action.dstMapIBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            case CFP_CHANGEFWDMAP_ADD:
                HOST_Log("Destination In-Bound Map : Add 0x%x\n", (rule->action.dstMapIBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            default:
                break;
        }
        switch ((rule->action.dstMapOBFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >> CFP_ACTION_CHANGE_FWDMAP_SHIFT) {
            case CFP_CHANGEFWDMAP_REM:
                HOST_Log("Destination Out-Bound Map: Remove 0x%x\n", (rule->action.dstMapOBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            case CFP_CHANGEFWDMAP_REP:
                HOST_Log("Destination Out-Bound Map: Replace 0x%x\n", (rule->action.dstMapOBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            case CFP_CHANGEFWDMAP_ADD:
                HOST_Log("Destination Out-Bound Map: Add 0x%x\n", (rule->action.dstMapOBFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT);
                break;
            default:
                break;
        }
        if ((rule->action.tosIBFlags & CFP_ACTION_CHANGE_TOS_MASK) >> CFP_ACTION_CHANGE_TOS_SHIFT) {
            HOST_Log("In-Bound Type Of Service : %u\n", (rule->action.tosIBFlags & CFP_ACTION_TOS_MASK) >> CFP_ACTION_TOS_SHIFT);
        }
        if ((rule->action.tosOBFlags & CFP_ACTION_CHANGE_TOS_MASK) >> CFP_ACTION_CHANGE_TOS_SHIFT) {
            HOST_Log("Out-Bound Type Of Service: %u\n", (rule->action.tosOBFlags & CFP_ACTION_TOS_MASK) >> CFP_ACTION_TOS_SHIFT);
        }
        if ((rule->action.tcFlags & CFP_ACTION_CHANGE_TC_MASK) >> CFP_ACTION_CHANGE_TC_SHIFT) {
            HOST_Log("Trafic Class             : %u\n", (rule->action.tcFlags & CFP_ACTION_TC_MASK) >> CFP_ACTION_TC_SHIFT);
        }
        if ((rule->action.colorFlags & CFP_ACTION_CHANGE_COLOR_MASK) >> CFP_ACTION_CHANGE_COLOR_SHIFT) {
            HOST_Log("Color                    : ");
            switch((rule->action.colorFlags & CFP_ACTION_COLOR_MASK) >> CFP_ACTION_COLOR_SHIFT) {
                case CFP_COLOR_GREEN:
                    HOST_Log("Green\n");
                    break;
                case CFP_COLOR_YELLOW:
                    HOST_Log("Yellow\n");
                    break;
                case CFP_COLOR_RED:
                    HOST_Log("Red\n");
                    break;
                default:
                    break;
            }
        }
        if ((rule->action.otherFlags & CFP_ACTION_BYPASS_VLAN_MASK) >> CFP_ACTION_BYPASS_VLAN_SHIFT) {
            HOST_Log("Bypass VLAN              : Yes\n");
        }
        if ((rule->action.otherFlags & CFP_ACTION_BYPASS_EAP_MASK) >> CFP_ACTION_BYPASS_EAP_SHIFT) {
            HOST_Log("Bypass EAP               : Yes\n");
        }
        if ((rule->action.otherFlags & CFP_ACTION_BYPASS_STP_MASK) >> CFP_ACTION_BYPASS_STP_SHIFT) {
            HOST_Log("Bypass STP               : Yes\n");
        }
        if ((rule->action.otherFlags & CFP_ACTION_LPBK_EN_MASK) >> CFP_ACTION_LPBK_EN_SHIFT) {
            HOST_Log("Loopback                 : Yes\n");
        }
        if ((rule->action.otherFlags & CFP_ACTION_USE_DFLT_RED_MASK) >> CFP_ACTION_USE_DFLT_RED_SHIFT) {
            HOST_Log("Use default RED profile  : Yes\n");
        }
        if (rule->action.reasonCode != 0U) {
            HOST_Log("Reason code              : %u\n", rule->action.reasonCode);
        }
        if (rule->action.chainID != 0U) {
            HOST_Log("Chain ID                 : %u\n", rule->action.chainID);
        }

        if (((rule->action.meter.policerFlags & CFP_METER_MODE_MASK) >> CFP_METER_MODE_SHIFT) != CFP_POLICERMODE_DISABLED) {
            HOST_Log("Meter mode               : ");
            switch ((rule->action.meter.policerFlags & CFP_METER_MODE_MASK) >> CFP_METER_MODE_SHIFT) {
                case CFP_POLICERMODE_RFC2698:
                    HOST_Log("RFC2698\n");
                    break;
                case CFP_POLICERMODE_RFC4115:
                    HOST_Log("RFC4115\n");
                    break;
                case CFP_POLICERMODE_MEF:
                    HOST_Log("MEF\n");
                    break;
                default:
                    break;
            }
            if ((rule->action.meter.policerFlags & CFP_METER_CF_MASK) >> CFP_METER_CF_SHIFT) {
                HOST_Log("Coupling flag            : Yes\n");
            }
            if ((rule->action.meter.policerFlags & CFP_METER_CM_MASK) >> CFP_METER_CM_SHIFT) {
                HOST_Log("Color Mode               : Blind\n");
            }
            if (((rule->action.meter.policerFlags & CFP_METER_MODE_MASK) >> CFP_METER_MODE_SHIFT) != CFP_POLICERMODE_MEF) {
                if ((rule->action.meter.policerFlags & CFP_METER_ACT_MASK) >> CFP_METER_ACT_SHIFT) {
                    HOST_Log("Red packets treated as   : Out-Bounds\n");
                }
            }
            HOST_Log("CIR                      : %u Kbps\n", (rule->action.meter.cirRefCnt * 1000)/256);
            HOST_Log("CBS                      : %u bytes\n", rule->action.meter.cirBktSize);
            HOST_Log("CTB                      : %u bytes\n", rule->action.meter.cirTkBkt/8);
            HOST_Log("EIR                      : %u Kbps\n", (rule->action.meter.eirRefCnt * 1000) /256);
            HOST_Log("EBS                      : %u bytes\n", rule->action.meter.eirBktSize);
            HOST_Log("ETB                      : %u bytes\n", rule->action.meter.eirTkBkt/8);
        }
    }

    return retVal;
}

void mgmt_cfp_cmd_handler(MgmtInfoType *info,
        char *input_str, uint32_t input_str_len)
{
    char     *curr_str            = input_str;
    char     *rem_str             = NULL;
    uint32_t curr_str_len         = input_str_len;
    uint32_t rem_str_len          = 0;
    int32_t  command_parse_status = 0;
    int32_t  rv                   = CMD_USAGE;
    int32_t  retVal;
    uint32_t value1;
    uint32_t value2;
    uint32_t i;
    uint32_t j;
    uint32_t k;

    if ((curr_str != NULL) && (curr_str_len != 0)) {
        if (!strncmp(input_str, "add", 3)) {
            CFP_ConfigType config;
            uint32_t       buf_size;
            void*          buffer;

            rv = CMD_USAGE;

            /* parse row number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            value1 = parse_integer(rem_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* parse slice number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            value2 = parse_integer(rem_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* parse file name */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }

            buffer = buffer_from_file(rem_str, &buf_size);
            if (buffer != NULL) {
                if (buf_size == sizeof(CFP_ConfigType)) {
                    memcpy(&config, buffer, buf_size);
                    if (config.magicId == uswap32(CFP_CONFIG_MAGIC_ID)) {
                        config.ruleList[0U].rowAndSlice = ((value1 << CFP_ROWANDSLICE_ROW_SHIFT) & CFP_ROWANDSLICE_ROW_MASK) |
                                                          ((value2 << CFP_ROWANDSLICE_SLICE_SHIFT) & CFP_ROWANDSLICE_SLICE_MASK);
                        {
                            /* Since the library is going to perform swap but the generated image is already in the target's endianness */
                            config.ruleList[0].key.flags              = uswap32(config.ruleList[0].key.flags);
                            config.ruleList[0].key.flagsMask          = uswap32(config.ruleList[0].key.flagsMask);
                            config.ruleList[0].key.ingressPortBitmap  = uswap16(config.ruleList[0].key.ingressPortBitmap);
                            config.ruleList[0].key.cTagFlags          = uswap32(config.ruleList[0].key.cTagFlags);
                            config.ruleList[0].key.cTagMask           = uswap16(config.ruleList[0].key.cTagMask);
                            config.ruleList[0].key.sTagFlags          = uswap32(config.ruleList[0].key.sTagFlags);
                            config.ruleList[0].key.sTagMask           = uswap16(config.ruleList[0].key.sTagMask);
                            for (i = 0UL; i < config.ruleList[0].key.numEnabledUDFs; ++i) {
                                config.ruleList[0].key.udfList[i].value  = uswap16(config.ruleList[0].key.udfList[i].value);
                                config.ruleList[0].key.udfList[i].mask  = uswap16(config.ruleList[0].key.udfList[i].mask);
                            }

                            config.ruleList[0].action.dstMapIBFlags      = uswap32(config.ruleList[0].action.dstMapIBFlags);
                            config.ruleList[0].action.dstMapOBFlags      = uswap32(config.ruleList[0].action.dstMapOBFlags);
                            config.ruleList[0].action.meter.cirTkBkt     = uswap32(config.ruleList[0].action.meter.cirTkBkt);
                            config.ruleList[0].action.meter.cirRefCnt    = uswap32(config.ruleList[0].action.meter.cirRefCnt);
                            config.ruleList[0].action.meter.cirBktSize   = uswap32(config.ruleList[0].action.meter.cirBktSize);
                            config.ruleList[0].action.meter.eirTkBkt     = uswap32(config.ruleList[0].action.meter.eirTkBkt);
                            config.ruleList[0].action.meter.eirRefCnt    = uswap32(config.ruleList[0].action.meter.eirRefCnt);
                            config.ruleList[0].action.meter.eirBktSize   = uswap32(config.ruleList[0].action.meter.eirBktSize);
                        }
                        retVal = HOST_CFPAddRule(info, &config.ruleList[0U]);
                        if (retVal != BCM_ERR_OK) {
                            rv = CMD_FAIL;
                            HOST_Log("CFP add rule returned %d\n", retVal);
                        } else {
                            rv = CMD_OK;
                            show_cfp_rule(info, &config.ruleList[0U]);
                        }
                    }
                }
                free(buffer);
                buffer = NULL;
            }
        } else if (!strncmp(input_str, "delete", 6)) {
            /* parse row number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                rv = CMD_USAGE;
            } else {
                value1 = parse_integer(rem_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                if (value1 >= CFP_MAX_RULES) {
                    HOST_Log("CFP row number must be in the range [0,%lu]\n", CFP_MAX_RULES-1UL);
                    rv = CMD_FAIL;
                    goto done;
                }

                retVal = HOST_CFPDeleteRule(info, value1);
                if (BCM_ERR_OK == retVal) {
                    rv = CMD_OK;
                } else {
                    HOST_Log("CFP delete rule failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }
            }
        } else if (!strncmp(input_str, "update", 6)) {
            CFP_ConfigType config;
            uint32_t       buf_size;
            void*          buffer;

            rv = CMD_USAGE;

            /* parse row number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            value1 = parse_integer(rem_str, &command_parse_status);
            MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

            curr_str = rem_str;
            curr_str_len = rem_str_len;
            /* parse file name */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                goto done;
            }
            if (value1 >= CFP_MAX_RULES) {
                HOST_Log("CFP row number must be in the range: [0,%lu]\n", CFP_MAX_RULES-1UL);
                rv = CMD_FAIL;
                goto done;
            }

            buffer = buffer_from_file(rem_str, &buf_size);
            if (buffer != NULL) {
                if (buf_size == sizeof(CFP_ConfigType)) {
                    memcpy(&config, buffer, buf_size);
                    if (config.magicId == uswap32(CFP_CONFIG_MAGIC_ID)) {
                        config.ruleList[0].action.dstMapIBFlags      = uswap32(config.ruleList[0].action.dstMapIBFlags);
                        config.ruleList[0].action.dstMapOBFlags      = uswap32(config.ruleList[0].action.dstMapOBFlags);
                        config.ruleList[0].action.meter.cirTkBkt     = uswap32(config.ruleList[0].action.meter.cirTkBkt);
                        config.ruleList[0].action.meter.cirRefCnt    = uswap32(config.ruleList[0].action.meter.cirRefCnt);
                        config.ruleList[0].action.meter.cirBktSize   = uswap32(config.ruleList[0].action.meter.cirBktSize);
                        config.ruleList[0].action.meter.eirTkBkt     = uswap32(config.ruleList[0].action.meter.eirTkBkt);
                        config.ruleList[0].action.meter.eirRefCnt    = uswap32(config.ruleList[0].action.meter.eirRefCnt);
                        config.ruleList[0].action.meter.eirBktSize   = uswap32(config.ruleList[0].action.meter.eirBktSize);
                        retVal = HOST_CFPUpdateRule(info, value1, &config.ruleList[0U].action);
                        if (retVal != BCM_ERR_OK) {
                            rv = CMD_FAIL;
                            HOST_Log("CFP update rule returned %d\n", retVal);
                        } else {
                            rv = CMD_OK;
                        }
                    }
                }
                free(buffer);
                buffer = NULL;
            }
        } else if (!strncmp(input_str, "show", 4)) {
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                CFP_TableSnapshotType snapShot;
                memset(&snapShot, 0U, sizeof(snapShot));
                retVal = HOST_CFPGetSnapshot(info, &snapShot);
                if (BCM_ERR_OK == retVal) {
                    HOST_Log("Total entries: %u\n", snapShot.numValidEntries);
                    HOST_Log("Ports enabled: 0x%x\n", snapShot.portEnableMask);
                    for (i = 0UL; i < CFP_MAX_RULES; ++i) {
                        if (0U != (snapShot.entry[i] & CFP_ENTRYSNAPSHOT_ENABLE_MASK)) {
                            HOST_Log("%u) Slice %u Format %s Static %u\n", i,
                                    (snapShot.entry[i] & CFP_ENTRYSNAPSHOT_SLICE_MASK) >> CFP_ENTRYSNAPSHOT_SLICE_SHIFT,
                                    l3FramingToStr(formatToL3Framing(
                                        (snapShot.entry[i] & CFP_ENTRYSNAPSHOT_FORMAT_MASK) >> CFP_ENTRYSNAPSHOT_FORMAT_SHIFT)),
                                    (snapShot.entry[i] & CFP_ENTRYSNAPSHOT_STATIC_MASK) >> CFP_ENTRYSNAPSHOT_STATIC_SHIFT);
                        }
                    }
                    HOST_Log("======= UDFs =========\n");
                    for (k = 0UL; k < CFP_NUM_FORMATS; ++k) {
                        HOST_Log("----- Format %s ------\n", l3FramingToStr(formatToL3Framing(k)));
                        for (i = 0UL; i < CFP_MAX_SLICES; ++i) {
                            HOST_Log("Slice %u:\n", i);
                            for (j = 0UL; j < CFP_MAX_UDFS; ++j) {
                                if (0U != snapShot.udfList[k].udfs[i][j].enable) {
                                    HOST_Log("%u) Base %s Offset %u\n", j,
                                    UDFBaseIdToStr((snapShot.udfList[k].udfs[i][j].address & CFP_UDF_BASE_MASK) >> CFP_UDF_BASE_SHIFT),
                                    (snapShot.udfList[k].udfs[i][j].address & CFP_UDF_OFFSET_MASK) >> CFP_UDF_OFFSET_SHIFT);
                                }
                            }
                        }
                    }

                    rv = CMD_OK;
                } else {
                    HOST_Log("CFP get snapshot failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }
            } else {
                CFP_RuleType rowConfig;
                memset(&rowConfig, 0U, sizeof(rowConfig));
                /* parse row number */
                value1 = parse_integer(rem_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                if (value1 >= CFP_MAX_RULES) {
                    HOST_Log("CFP row number must be in the range: [0,%lu]\n", CFP_MAX_RULES-1UL);
                    rv = CMD_FAIL;
                    goto done;
                }
                retVal = HOST_CFPGetRowConfig(info, value1, &rowConfig);
                if (BCM_ERR_OK == retVal) {
                    show_cfp_rule(info, &rowConfig);
                    rv = CMD_OK;
                } else {
                    HOST_Log("CFP get row config failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }
            }
        } else if (!strncmp(input_str, "stats", 5)) {
            CFP_StatsType stats;

            /* parse row number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                rv = CMD_USAGE;
            } else {
                value1 = parse_integer(rem_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HOST_CFPGetStats(info, value1, &stats);
                if (BCM_ERR_OK == retVal) {
                    HOST_Log("Green : %u\n", stats.green);
                    HOST_Log("Yellow: %u\n", stats.yellow);
                    HOST_Log("Red   : %u\n", stats.red);
                    rv = CMD_OK;
                } else {
                    HOST_Log("CFP get stats failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }
            }
        } else if (!strncmp(input_str, "port", 4)) {
            /* parse port number */
            split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
            if ((rem_str == NULL) || (rem_str_len == 0)) {
                rv = CMD_USAGE;
            } else {
                value1 = parse_integer(rem_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);

                curr_str = rem_str;
                curr_str_len = rem_str_len;
                /* parse file name */
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) || (rem_str_len == 0)) {
                    rv = CMD_USAGE;
                    goto done;
                }

                if (!strncmp(rem_str, "enable", 6)) {
                    value2 = 1UL;
                } else if (!strncmp(rem_str, "disable", 7)) {
                    value2 = 0UL;
                } else {
                    rv = CMD_USAGE;
                    goto done;
                }
                retVal = HOST_CFPSetPortMode(info, value1, value2);
                if (BCM_ERR_OK == retVal) {
                    rv = CMD_OK;
                } else {
                    HOST_Log("CFP set port mode failed, retVal = %d\n", retVal);
                    rv = CMD_FAIL;
                }
            }
        } else {
            rv = CMD_USAGE;
            goto done;
        }
    }

done:
    if(CMD_USAGE == rv) {
        HOST_Log(mgmt_cfp_cmd_usage);
    }
    return;
}
#endif /* ENABLE_HOST_CFP_CMD_HANDLER */

static const char mgmt_generic_cmd_usage[] =
"\terase <img>\n"
"\treboot\n"
"\tversion\n"
"\tspi_id [id]\n"
"\tprobe\n"
"\tconn_mode <spi/pcie>\n"
"\tasync count heartbeat\n"
"\thelp\n";
static void print_usage(MgmtInfoType *info)
{
    char str[] = "help";
    HOST_Exec_cmd_handler(info, str, sizeof(str));
    HOST_Install_cmd_handler(info, str, sizeof(str));
#ifdef ENABLE_HOST_COMMS_CMD_HANDLER
    mgmt_switch_cmd_handler(info, str, sizeof(str));
#endif
#ifdef ENABLE_HOST_ETS_CMD_HANDLER
    mgmt_ets_cmd_handler(info, str, sizeof(str));
#endif
#ifdef ENABLE_DBGMEM
    mgmt_mem_cmd_handler(info, str, sizeof(str));
#ifdef ENABLE_HOST_CFP_CMD_HANDLER
    mgmt_cfp_cmd_handler(info, str, sizeof(str));
#endif
#endif
    mgmt_otp_cmd_handler(info, str, sizeof(str));
    HOST_Log(mgmt_generic_cmd_usage);
}

static void process_user_cmd(MgmtInfoType *info, mqd_t mqd)
{
    unsigned int param;
    int retVal;
    char input_str[MGMT_CMD_STR_MAX_LEN];
    char *rem_str;
    char *curr_str;
    uint32_t rem_str_len;
    uint32_t curr_str_len;
    int32_t command_parse_status = 0;
    char version[VERSION_STR_LEN + 1];
    SYS_KeepAliveType keepAlive;

    while (1) {
        /* Fetch the command */
        retVal = mq_receive(mqd, input_str, sizeof(input_str), &param);
        if (retVal == -1) {
            if (errno != EAGAIN) {
                perror("mq_receive");
            }
            break;
        }

        if (strlen(input_str) == 0) {
            break;
        }
        HOST_Log("bcmutild: %s\n", input_str);
        /* Parse the input command and parameters */
        split_line(input_str, strlen(input_str), &rem_str, &rem_str_len);

        if (0 == input_str[0]) {
            /* Do nothing */
        } else if (0 == strncmp(input_str, "install", 7)) {
            HOST_Install_cmd_handler(info, rem_str, rem_str_len);
        } else if (0UL == strncmp(input_str, "execute", 7)) {
            HOST_Exec_cmd_handler(info, rem_str, rem_str_len);
#ifdef ENABLE_HOST_COMMS_CMD_HANDLER
        } else if (0 == strncmp(input_str, "switch", 6)) {
            mgmt_switch_cmd_handler(info, rem_str, rem_str_len);
#endif
#ifdef ENABLE_HOST_ETS_CMD_HANDLER
        } else if (0 == strncmp(input_str, "ets", 3)) {
            mgmt_ets_cmd_handler(info, rem_str, rem_str_len);
        } else if (0 == strncmp(input_str, "avnu", 4)) {
            mgmt_ets_cmd_handler(info, rem_str, rem_str_len);
#endif
#ifdef ENABLE_HOST_CFP_CMD_HANDLER
        } else if (0 == strncmp(input_str, "cfp", 3)) {
            mgmt_cfp_cmd_handler(info, rem_str, rem_str_len);
#endif
        } else if (0 == strncmp(input_str, "erase", 5)) {
            if ((rem_str != NULL) && (rem_str_len > 1UL)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str == NULL) && (rem_str_len == 0)) {
                    // Read the file and install it
                    uint32_t buf_size;
                    void* buffer = buffer_from_file(curr_str, &buf_size);

                    if (buffer != NULL) {
                        retVal = HOST_SysFlashErase(info, buffer, buf_size);
                        if (retVal != BCM_ERR_OK) {
                            HOST_Log("HOST_FlashErase returned %d\n", retVal);
                        }
                        free(buffer);
                    }
                } else {
                    HOST_Log("erase <img>\n");
                }
            } else {
                HOST_Log("bootloader needed\n");
            }
        } else if (0 == strncmp(input_str, "reboot", 6)) {
            HOST_SysReboot(info);
        } else if (0 == strncmp(input_str, "version", 7)) {
            if (!HOST_SysOSVersionGet(info, version)) {
                version[VERSION_STR_LEN] = '\0';
                HOST_Log("%s\n", version);
            }
#ifdef ENABLE_DBGMEM
        } else if (0 == strncmp(input_str, "mem", 3)) {
            mgmt_mem_cmd_handler(info, rem_str, rem_str_len);
#endif
        } else if (0 == strncmp(input_str, "flash", 5)) {
            HOST_FlashWrite_cmd_handler(info, rem_str, rem_str_len);
        } else if (0 == strncmp(input_str, "otp", 3)) {
            mgmt_otp_cmd_handler(info, rem_str, rem_str_len);
        } else if (0 == strncmp(input_str, "probe", 5)) {
            uint32_t activeSpiId[4UL];
            uint32_t count = 4UL;
                    HIPC_ProbeSlaves();
            retVal = HIPC_GetActiveSlaves(&activeSpiId[0], &count);
            if (BCM_ERR_OK != retVal) {
                HOST_Log("Failed to retrieve active slaves, err:%d\n", retVal);
            } else {
                HOST_Log("Active Slave Count: %u\n", count);
                HOST_Log("Ids:[%d %d %d %d]\n", activeSpiId[0], activeSpiId[1],
                    activeSpiId[2], activeSpiId[3]);
            }
        } else if (0 == strncmp(input_str, "spi_id", 6)) {
            if ((rem_str == NULL) || (rem_str_len == 0UL)) {
                retVal = HIPC_GetSlave(&param);
                if (BCM_ERR_OK == retVal) {
                    HOST_Log("Current SPI ID: %u\n", param);
                } else {
                    HOST_Log("Failed to retrieve ID err:%x\n", retVal);
                    goto done;
                }
            } else {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                if ((rem_str != NULL) || (rem_str_len != 0UL)) {
                    HOST_Log("Invalid syntax\n");
                    goto done;
                }
                param = parse_integer(curr_str, &command_parse_status);
                MGMT_CMD_INT_PARSE_STATUS(command_parse_status);
                retVal = HIPC_SetSlave(param);
                if (BCM_ERR_OK != retVal) {
                    HOST_Log("Failed to set ID err:%x\n", retVal);
                    goto done;
                }
            }
        } else if (strncmp(input_str, "conn_mode", 9) == 0UL) {
            if ((rem_str == NULL) || (rem_str_len == 0UL)) {
                if (HIPC_GetConnMode() == MGMT_SPI_CONNECTION) {
                    HOST_Log("current mode: spi\n");
                } else if (HIPC_GetConnMode() == MGMT_PCIE_CONNECTION) {
                    HOST_Log("current mode: pcie\n");
                } else {
                    HOST_Log("current mode: unknown\n");
                }
            } else {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                if (strncmp(curr_str, "spi", 3) == 0UL) {
                    retVal = HIPC_SetConnMode(MGMT_SPI_CONNECTION);
                } else if (strncmp(curr_str, "pcie", 4) == 0UL) {
                    retVal = HIPC_SetConnMode(MGMT_PCIE_CONNECTION);
                } else {
                    HOST_Log("Invalid connection mode\n");
                    retVal = BCM_ERR_INVAL_PARAMS;
                }
            }
        } else if(strncmp(input_str, "async", 5) == 0UL) {
            if ((rem_str != NULL) && (rem_str_len != 0)) {
                curr_str = rem_str;
                curr_str_len = rem_str_len;
                if (strncmp(curr_str, "count", 5) == 0UL) {
                    split_line(curr_str, curr_str_len, &rem_str, &rem_str_len);
                    if ((rem_str != NULL) && (rem_str_len != 0)) {
                        curr_str = rem_str;
                        curr_str_len = rem_str_len;
                        if (strncmp(curr_str, "heartbeat", 9) == 0UL) {
                            retVal = HIPC_GetSlave(&param);
                            if (BCM_ERR_OK != retVal) {
                                HOST_Log("Failed to retrieve slave ID err:%x\n", retVal);
                            } else {
                                retVal = HOST_SysKeepAliveGet(info, param, &keepAlive);
                                if (retVal != BCM_ERR_OK) {
                                    HOST_Log("Failed to get heartbeat information retVal:0x%x\n", retVal);
                                } else {
                                    HOST_Log("Async heartbeats Count:%u Time:%d.%09d\n",
                                            keepAlive.count,
                                            keepAlive.upTime.s,
                                            keepAlive.upTime.ns);
                                }
                            }
                        }
                    }
                }
            }
        } else if (0 == strncmp(input_str, "help", 4)) {
            print_usage(info);
        } else { /**/
            HOST_Log("Unknown Command\n");
            print_usage(info);
        }
        HOST_Log("bcmutild: %s processed successfully\n", input_str);
    }
done:
    return;
}

/*@api
 * main
 *
 * @brief
 * Main function for handling CLI commands.
 *
 * @param=none
 * @returns 0
 *
 * @desc
 */
int32_t main(void)
{
    MgmtInfoType *info = &info_g;
    int32_t retVal;
    mqd_t mqd = -1;
    static uint32_t firstRun = 1;
    fd_set rfds;
    struct timeval tv;
    struct mq_attr attr;
    uint32_t activeSpiId[4UL];
    uint32_t count = 4UL;

    HOST_Log("\nVersion %s\n", SPIUTIL_IMAGE_VERSION);

    /* Initialize Managament API */
    retVal = HOST_SysInit(info);
    if (retVal != 0) {
        HOST_Log("Could not Initilaize management interface\n");
        goto done;
    }

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MGMT_CMD_STR_MAX_LEN-2;
    attr.mq_curmsgs = 0;

    mqd = mq_open(MGMT_MSGQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);
    if (mqd == -1) {
        perror("mq_open");
        goto done;
    }

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(mqd, &rfds);

#ifdef ENABLE_RECORD_NOTIFICATION
        /* poll every 100ms */
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
#else
        /* poll every sec */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
#endif

        if (firstRun) {
            /* Find active slaves */
            HIPC_ProbeSlaves();

            retVal = HIPC_GetActiveSlaves(&activeSpiId[0], &count);
            if (BCM_ERR_OK != retVal) {
                goto done;
            }

            if (count > 0) {
                retVal = HIPC_SetSlave(activeSpiId[0]);
                if (BCM_ERR_OK != retVal) {
                    HOST_Log("Probe: Failed to set slave ID:%u err:%d\n", __func__,
                        activeSpiId[0], retVal);
                } else {
                    HOST_Log("Probe: ID-%u is set\n", activeSpiId[0]);
                }
            } else {
                HOST_Log("Probe: No active slave ID found, set one explicitly\n");
            }

            /* Drain async messages here */
            Host_CheckAsyncMsg();
            retVal = HOST_EtherStackSync(info);
            if (BCM_ERR_OK != retVal) {
                /* HOST_Log("Error: Could not synchronise time\n"); */
                /* Continue processing, it is possible that the targets don't have a
                 * valid image in flash memory */
            } else {
                HOST_Log("Time synchronisation successful\n");
            }

            firstRun = 0;
        }
        /* Wait until we have an event */
        retVal = select(mqd + 1, &rfds, NULL, NULL, &tv);

        /* Drain async messages here */
        Host_CheckAsyncMsg();

        if (retVal == -1) {
            perror("select()");
        } else if (retVal) {
            if (FD_ISSET(mqd, &rfds)) {
                FD_CLR(mqd, &rfds);
                process_user_cmd(info, mqd);
            }
        }
    }

    HOST_SysDeinit(info);
done:
    if (mqd != -1) {
        mq_close(mqd);
        if (errno == -1) {
            perror("mq_close");
        }
    }
    return retVal;
}

int32_t IPC_QueueAsyncMsg(uint32_t currentSlave, uint32_t replyId, uint8_t *reply, uint32_t replyLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t notificationId;
    BCM_GroupIDType group;
    BCM_CompIDType comp;

    if ((RPC_CMD_ASYNC_MASK != (replyId & RPC_CMD_ASYNC_MASK))
        || (reply == NULL)) {
        HOST_Log("%s Invalid parameters replyId:0x%x reply:%p len:%u\n",
            __func__, replyId, reply, replyLen);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    group = (replyId & RPC_CMD_GROUP_MASK) >> RPC_CMD_GROUP_SHIFT;
    comp = (replyId & RPC_CMD_COMP_MASK) >> RPC_CMD_COMP_SHIFT;
    notificationId = (replyId & RPC_CMD_ID_MASK) >> RPC_CMD_ID_SHIFT;

    switch (group) {
    case BCM_GROUPID_SYS:
        retVal = HOST_SysNotificationHandler(currentSlave, comp, notificationId, reply,
                replyLen);
        break;
#ifdef ENABLE_HOST_COMMS_CMD_HANDLER
    case BCM_GROUPID_COMMS:
        retVal = HOST_COMMSNotificationHandler(currentSlave, comp, notificationId, reply,
                replyLen);
        break;
#endif
    case BCM_GROUPID_IMGL:
        retVal = HOST_ImglNotificationHandler(currentSlave, comp, notificationId, reply,
                replyLen);
        break;
    default:
        HOST_Log("SPI-Id %u Invalid ID:0x%x\n", currentSlave, replyId);
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

done:
    return retVal;
}

static int32_t Host_CheckAsyncMsg()
{
    uint32_t i, j;
    int32_t retVal;
    uint32_t OriginalSpiId;
    uint32_t replyId = 0, len = 0;
    RPC_ResponseType resp;
    uint32_t activeSpiId[4UL];
    uint32_t count = 4UL;

    /* store current spi id */
    retVal = HIPC_GetSlave(&OriginalSpiId);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    retVal = HIPC_GetActiveSlaves(&activeSpiId[0], &count);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    if (0UL == count) {
        HIPC_ProbeSlaves();

        count = 4UL;
        retVal = HIPC_GetActiveSlaves(&activeSpiId[0], &count);
        if (BCM_ERR_OK != retVal) {
            goto done;
        }
    }

    /* Get a maximum of one message from each active SPI Id. If there are
     * more, loop again */
    do {
        /* loop through valid Ids */
        for (i = 0; i < count; i++) {
            retVal = HIPC_SetSlave(activeSpiId[i]);
            if (BCM_ERR_OK != retVal) {
                continue;
            }

            /* check if there are any */
            retVal = HIPC_Recv(&replyId, (uint8_t *)&resp, sizeof(RPC_ResponseType), &len);
            if (retVal == BCM_ERR_OK) {
                if (RPC_CMD_ASYNC_MASK == (replyId & RPC_CMD_ASYNC_MASK)) {
                    /* process message */
                    retVal = IPC_QueueAsyncMsg(activeSpiId[i], replyId, (uint8_t *)&resp, len);
                    if (retVal != BCM_ERR_OK) {
                        HOST_Log("%s: Failed to Process Id:0x%x len:0x%x\n",
                                __func__, replyId, len);
                    }
                } else {
                    HOST_Log("%s Invalid reply ID: 0x%x\n", __func__, replyId);
                }
            } else {
                /* No messages with this ID, continue with the rest of Active Ids */
                count--;

                for (j = i; j < count; j++) {
                    activeSpiId[j] = activeSpiId[j + 1];
                }
                continue;
            }
        }
    } while (0UL != count);

    /* restore spi id */
    retVal = HIPC_SetSlave(OriginalSpiId);

done:
    return retVal;
}
