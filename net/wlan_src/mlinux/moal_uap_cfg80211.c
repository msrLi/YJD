/** @file moal_uap_cfg80211.c
  *
  * @brief This file contains the functions for uAP CFG80211. 
  *
  * Copyright (C) 2011-2012, Marvell International Ltd. 
  * 
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */

#include "moal_cfg80211.h"

#ifdef	UAP_SUPPORT
#include "moal_uap_cfg80211.h"

/* these 3 function will be called in woal_cfg80211_wifi_direct_ops */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
int woal_cfg80211_add_beacon(struct wiphy *wiphy,
                             struct net_device *dev,
                             struct cfg80211_ap_settings *params);

int woal_cfg80211_set_beacon(struct wiphy *wiphy,
                             struct net_device *dev,
                             struct cfg80211_beacon_data *params);
#else
int woal_cfg80211_add_beacon(struct wiphy *wiphy,
                             struct net_device *dev,
                             struct beacon_parameters *params);

int woal_cfg80211_set_beacon(struct wiphy *wiphy,
                             struct net_device *dev,
                             struct beacon_parameters *params);
#endif

int woal_cfg80211_del_beacon(struct wiphy *wiphy, struct net_device *dev);

static int woal_uap_cfg80211_scan(struct wiphy *wiphy, struct net_device *dev,
                                  struct cfg80211_scan_request *request);

static int woal_uap_cfg80211_connect(struct wiphy *wiphy,
                                     struct net_device *dev,
                                     struct cfg80211_connect_params *sme);

static int woal_uap_cfg80211_disconnect(struct wiphy *wiphy,
                                        struct net_device *dev,
                                        t_u16 reason_code);

int woal_uap_cfg80211_get_station(struct wiphy *wiphy, struct net_device *dev,
                                  const u8 * mac, struct station_info *stainfo);

int woal_cfg80211_set_tx_power(struct wiphy *wiphy,
										struct net_device *dev,
                                      enum nl80211_tx_power_setting type,
                                      int dbm);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37) || defined(COMPAT_WIRELESS)
static const struct ieee80211_txrx_stypes
    ieee80211_uap_mgmt_stypes[NUM_NL80211_IFTYPES] = {
    /*[NL80211_IFTYPE_UNSPECIFIED] = */{
                              /*.tx =*/ 0x0000,
                              /*.rx =*/ 0x0000,
                              },
    /*[NL80211_IFTYPE_ADHOC] = */{
                              /*.tx =*/ 0x0000,
                              /*.rx =*/ 0x0000,
                              },
    /*[NL80211_IFTYPE_STATION] = */{
                                /*.tx = */BIT(IEEE80211_STYPE_ACTION >> 4),
                                /*.rx = */BIT(IEEE80211_STYPE_ACTION >> 4),
                                },
    /*[NL80211_IFTYPE_AP] = */{
                           /*.tx = */0xffff,
                           /*.rx = */BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
                           BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
                           BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
                           BIT(IEEE80211_STYPE_DISASSOC >> 4) |
                           BIT(IEEE80211_STYPE_AUTH >> 4) |
                           BIT(IEEE80211_STYPE_DEAUTH >> 4) |
                           BIT(IEEE80211_STYPE_ACTION >> 4),
                           },
    /*[NL80211_IFTYPE_AP_VLAN] = */{
                                /*.tx = */0x0000,
                                /*.rx = */0x0000,
                                },
    /*[NL80211_IFTYPE_WDS]= */{
                                /*.tx = */0x0000,
                                /*.rx = */0x0000,
                                },
	/*[NL80211_IFTYPE_MONITOR]= */{
                                /*.tx = */0x0000,
                                /*.rx = */0x0000,
                                },
	/*[NL80211_IFTYPE_MESH_POINT]= */{
                                /*.tx = */0x0000,
                                /*.rx = */0x0000,
                                },
    /*[NL80211_IFTYPE_P2P_CLIENT] = */{
                                   /*.tx = */BIT(IEEE80211_STYPE_ACTION >> 4) |
                                   BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
                                   /*.rx = */BIT(IEEE80211_STYPE_ACTION >> 4) |
                                   BIT(IEEE80211_STYPE_PROBE_REQ >> 4),
                                   },
    /*[NL80211_IFTYPE_P2P_GO] = */{
                               /*.tx = */BIT(IEEE80211_STYPE_ACTION >> 4) |
                               // BIT(IEEE80211_STYPE_PROBE_RESP >> 4) |
                               0,
                               /*.rx = */BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
                               BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
                               BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
                               BIT(IEEE80211_STYPE_DISASSOC >> 4) |
                               BIT(IEEE80211_STYPE_AUTH >> 4) |
                               BIT(IEEE80211_STYPE_DEAUTH >> 4) |
                               BIT(IEEE80211_STYPE_ACTION >> 4),
                               },
};

struct cfg80211_ops woal_cfg80211_uap_ops = {
	/*.suspend = 0,
	.resume = 0,
	.set_wakeup = 0,
	.add_virtual_intf = 0,
	.del_virtual_intf = 0,*/
    /*.change_virtual_intf = */woal_cfg80211_change_virtual_intf,
    /*.add_key = */woal_cfg80211_add_key,
    /*.get_key = 0,*/
    /*.del_key = */woal_cfg80211_del_key,
    /*.set_default_key = */woal_cfg80211_set_default_key,
    /*.set_default_mgmt_key = 0,*/
	/*.start_ap = */woal_cfg80211_add_beacon,
    /*.change_beacon = */woal_cfg80211_set_beacon,
    /*.stop_ap = */woal_cfg80211_del_beacon,
	/*.add_station = 0,
	.del_station = 0,
	.change_station = 0,*/
	/*.get_station = */woal_uap_cfg80211_get_station,
	/*.dump_station = *//*woal_cfg80211_dump_station*/0,
	/*.add_mpath = 0,
	.del_mpath = 0,
	.change_mpath = 0,
	.get_mpath = 0,
	.dump_mpath = 0,
	.get_mesh_config = 0,
	.update_mesh_config = 0,
	.join_mesh = 0,
	.leave_mesh = 0,
	.change_bss = 0,
	.set_txq_params = 0,
	.libertas_set_mesh_channel = 0,
	.set_monitor_channel = 0,*/
    /*.scan = */woal_uap_cfg80211_scan,
    /*.auth = 0,
	.assoc = 0,
	.deauth = 0,
	.disassoc = 0,*/
   /* .connect = */woal_uap_cfg80211_connect,
    /*.disconnect = */woal_uap_cfg80211_disconnect,
	/*.join_ibss = *//*woal_cfg80211_join_ibss*/0,
    /*.leave_ibss = *//*woal_cfg80211_leave_ibss*/0,
	/*.set_mcast_rate = 0,*/
    /*.set_wiphy_params = */woal_cfg80211_set_wiphy_params,
 #ifdef STA_SUPPORT
    /*.set_tx_power = */woal_cfg80211_set_tx_power,
 #else
	0,
 #endif
    /*.get_tx_power = 0,
    .set_wds_peer = 0,
    .rfkill_poll = 0,
    .set_bitrate_mask = 0,*/

	/*.mgmt_tx = */woal_cfg80211_mgmt_tx,
	/*.mgmt_tx_cancel_wait = */0,
    /*.remain_on_channel = */0,
    /*.cancel_remain_on_channel = */0,
    /*.set_power_mgmt = *//*woal_cfg80211_set_power_mgmt*/0,
    /*.set_cqm_rssi_config = *//*woal_cfg80211_set_cqm_rssi_config*/0,
    
    
    /*.mgmt_frame_register = */woal_cfg80211_mgmt_frame_register,
    /*.set_channel = */woal_cfg80211_set_channel,
};

#endif
/********************************************************
				Local Variables
********************************************************/

/********************************************************
				Global Variables
********************************************************/

/********************************************************
				Local Functions
********************************************************/

/********************************************************
				Global Functions
********************************************************/

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0) && !defined(COMPAT_WIRELESS)
/**
 * @brief Verify RSN IE
 *
 * @param rsn_ie          Pointer RSN IE
 * @param sys_config      Pointer to mlan_uap_bss_param structure
 *
 * @return                MTRUE/MFALSE
 */
static t_u8
woal_check_rsn_ie(IEEEtypes_Rsn_t * rsn_ie, mlan_uap_bss_param * sys_config)
{
    int left = 0;
    int count = 0;
    int i = 0;
    wpa_suite_auth_key_mgmt_t *key_mgmt = NULL;
    left = rsn_ie->len + 2;
    if (left < sizeof(IEEEtypes_Rsn_t))
        return MFALSE;
    sys_config->wpa_cfg.group_cipher = 0;
    sys_config->wpa_cfg.pairwise_cipher_wpa2 = 0;
    /* check the group cipher */
    switch (rsn_ie->group_cipher.type) {
    case WPA_CIPHER_TKIP:
        sys_config->wpa_cfg.group_cipher = CIPHER_TKIP;
        break;
    case WPA_CIPHER_AES_CCM:
        sys_config->wpa_cfg.group_cipher = CIPHER_AES_CCMP;
        break;
    default:
        break;
    }
    count = le16_to_cpu(rsn_ie->pairwise_cipher.count);
    for (i = 0; i < count; i++) {
        switch (rsn_ie->pairwise_cipher.list[i].type) {
        case WPA_CIPHER_TKIP:
            sys_config->wpa_cfg.pairwise_cipher_wpa2 |= CIPHER_TKIP;
            break;
        case WPA_CIPHER_AES_CCM:
            sys_config->wpa_cfg.pairwise_cipher_wpa2 |= CIPHER_AES_CCMP;
            break;
        default:
            break;
        }
    }
    left -= sizeof(IEEEtypes_Rsn_t) + (count - 1) * sizeof(wpa_suite);
    if (left < sizeof(wpa_suite_auth_key_mgmt_t))
        return MFALSE;
    key_mgmt =
        (wpa_suite_auth_key_mgmt_t *) ((u8 *) rsn_ie + sizeof(IEEEtypes_Rsn_t) +
                                       (count - 1) * sizeof(wpa_suite));
    count = le16_to_cpu(key_mgmt->count);
    if (left <
        (sizeof(wpa_suite_auth_key_mgmt_t) + (count - 1) * sizeof(wpa_suite)))
        return MFALSE;

    for (i = 0; i < count; i++) {
        switch (key_mgmt->list[i].type) {
        case RSN_AKM_8021X:
            sys_config->key_mgmt = KEY_MGMT_EAP;
            break;
        case RSN_AKM_PSK:
            sys_config->key_mgmt = KEY_MGMT_PSK;
            break;
        }
    }
    return MTRUE;
}

/**
 * @brief Verify WPA IE
 *
 * @param wpa_ie          Pointer WPA IE
 * @param sys_config      Pointer to mlan_uap_bss_param structure
 *
 * @return                MTRUE/MFALSE
 */
static t_u8
woal_check_wpa_ie(IEEEtypes_Wpa_t * wpa_ie, mlan_uap_bss_param * sys_config)
{
    int left = 0;
    int count = 0;
    int i = 0;
    wpa_suite_auth_key_mgmt_t *key_mgmt = NULL;
    left = wpa_ie->len + 2;
    if (left < sizeof(IEEEtypes_Wpa_t))
        return MFALSE;
    sys_config->wpa_cfg.group_cipher = 0;
    sys_config->wpa_cfg.pairwise_cipher_wpa = 0;
    switch (wpa_ie->group_cipher.type) {
    case WPA_CIPHER_TKIP:
        sys_config->wpa_cfg.group_cipher = CIPHER_TKIP;
        break;
    case WPA_CIPHER_AES_CCM:
        sys_config->wpa_cfg.group_cipher = CIPHER_AES_CCMP;
        break;
    default:
        break;
    }
    count = le16_to_cpu(wpa_ie->pairwise_cipher.count);
    for (i = 0; i < count; i++) {
        switch (wpa_ie->pairwise_cipher.list[i].type) {
        case WPA_CIPHER_TKIP:
            sys_config->wpa_cfg.pairwise_cipher_wpa |= CIPHER_TKIP;
            break;
        case WPA_CIPHER_AES_CCM:
            sys_config->wpa_cfg.pairwise_cipher_wpa |= CIPHER_AES_CCMP;
            break;
        default:
            break;
        }
    }
    left -= sizeof(IEEEtypes_Wpa_t) + (count - 1) * sizeof(wpa_suite);
    if (left < sizeof(wpa_suite_auth_key_mgmt_t))
        return MFALSE;
    key_mgmt =
        (wpa_suite_auth_key_mgmt_t *) ((u8 *) wpa_ie + sizeof(IEEEtypes_Wpa_t) +
                                       (count - 1) * sizeof(wpa_suite));
    count = le16_to_cpu(key_mgmt->count);
    if (left <
        (sizeof(wpa_suite_auth_key_mgmt_t) + (count - 1) * sizeof(wpa_suite)))
        return MFALSE;
    for (i = 0; i < count; i++) {
        switch (key_mgmt->list[i].type) {
        case RSN_AKM_8021X:
            sys_config->key_mgmt = KEY_MGMT_EAP;
            break;
        case RSN_AKM_PSK:
            sys_config->key_mgmt = KEY_MGMT_PSK;
            break;
        }
    }
    return MTRUE;
}

/**
 * @brief Find RSN/WPA IES
 *
 * @param ie              Pointer IE buffer
 * @param sys_config      Pointer to mlan_uap_bss_param structure
 *
 * @return                MTRUE/MFALSE
 */
static t_u8
woal_find_wpa_ies(t_u8 * ie, int len, mlan_uap_bss_param * sys_config)
{
    int bytes_left = len;
    t_u8 *pcurrent_ptr = ie;
    t_u16 total_ie_len;
    t_u8 element_len;
    t_u8 wpa2 = 0;
    t_u8 wpa = 0;
    t_u8 ret = MFALSE;
    IEEEtypes_ElementId_e element_id;
    IEEEtypes_VendorSpecific_t *pvendor_ie;
    const t_u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };

    while (bytes_left >= 2) {
        element_id = (IEEEtypes_ElementId_e) (*((t_u8 *) pcurrent_ptr));
        element_len = *((t_u8 *) pcurrent_ptr + 1);
        total_ie_len = element_len + sizeof(IEEEtypes_Header_t);
        if (bytes_left < total_ie_len) {
            PRINTM(MERROR, "InterpretIE: Error in processing IE, "
                   "bytes left < IE length\n");
            bytes_left = 0;
            continue;
        }
        switch (element_id) {
        case RSN_IE:
            wpa2 =
                woal_check_rsn_ie((IEEEtypes_Rsn_t *) pcurrent_ptr, sys_config);
            break;
        case VENDOR_SPECIFIC_221:
            pvendor_ie = (IEEEtypes_VendorSpecific_t *) pcurrent_ptr;
            if (!memcmp(pvendor_ie->vend_hdr.oui, wpa_oui, sizeof(wpa_oui)))
                wpa =
                    woal_check_wpa_ie((IEEEtypes_Wpa_t *) pcurrent_ptr,
                                      sys_config);
            break;
        default:
            break;
        }
        pcurrent_ptr += element_len + 2;
        /* Need to account for IE ID and IE Len */
        bytes_left -= (element_len + 2);
    }
    if (wpa && wpa2) {
        sys_config->protocol = PROTOCOL_WPA | PROTOCOL_WPA2;
        ret = MTRUE;
    } else if (wpa2) {
        sys_config->protocol = PROTOCOL_WPA2;
        ret = MTRUE;
    } else if (wpa) {
        sys_config->protocol = PROTOCOL_WPA;
        ret = MTRUE;
    }
    return ret;
}
#endif

/** secondary channel is below */
#define SECOND_CHANNEL_BELOW    0x30
/** secondary channel is above */
#define SECOND_CHANNEL_ABOVE    0x10
/** no secondary channel */
#define SECONDARY_CHANNEL_NONE     0x00

/**
 * @brief Get second channel offset 
 *
 * @param chan 			  channel num
 * @return                second channel offset
 */
static t_u8
woal_get_second_channel_offset(int chan)
{
    t_u8 chan2Offset = SECONDARY_CHANNEL_NONE;

    switch (chan) {
    case 36:
    case 44:
    case 52:
    case 60:
    case 100:
    case 108:
    case 116:
    case 124:
    case 132:
    case 149:
    case 157:
        chan2Offset = SECOND_CHANNEL_ABOVE;
        break;
    case 40:
    case 48:
    case 56:
    case 64:
    case 104:
    case 112:
    case 120:
    case 128:
    case 136:
    case 153:
    case 161:
        chan2Offset = SECOND_CHANNEL_BELOW;
        break;
    case 140:
    case 165:
        /* Special Case: 20Mhz-only Channel */
        chan2Offset = SECONDARY_CHANNEL_NONE;
        break;
    }
    return chan2Offset;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
/**
 * @brief initialize AP or GO bss config
 *
 * @param priv            A pointer to moal private structure
 * @param params          A pointer to cfg80211_ap_settings structure 
 * @return                0 -- success, otherwise fail
 */
static int
woal_cfg80211_beacon_config(moal_private * priv,
                            struct cfg80211_ap_settings *params)
#else
/**
 * @brief initialize AP or GO bss config
 *
 * @param priv            A pointer to moal private structure
 * @param params          A pointer to beacon_parameters structure 
 * @return                0 -- success, otherwise fail
 */
static int
woal_cfg80211_beacon_config(moal_private * priv,
                            struct beacon_parameters *params)
#endif
{
    int ret = 0;
    mlan_uap_bss_param sys_config;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0) || defined(COMPAT_WIRELESS)
    int i = 0;
#else
    const t_u8 *ssid_ie = NULL;
    struct ieee80211_mgmt *head = NULL;
    t_u16 capab_info = 0;
#endif
    t_u8 Rates_BG[13] =
        { 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24, 0x30, 0x48,
        0x60, 0x6c, 0
    };
    t_u8 Rates_A[9] = { 0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c, 0 };
    t_u8 chan2Offset = 0;
#ifdef WIFI_DIRECT_SUPPORT
    t_u8 Rates_WFD[9] = { 0x8c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6c, 0 };
#endif

    ENTER();

    if (params == NULL) {
        ret = -EFAULT;
        goto done;
    }

    if (priv->bss_type != MLAN_BSS_TYPE_UAP
#ifdef WIFI_DIRECT_SUPPORT
        && priv->bss_type != MLAN_BSS_TYPE_WIFIDIRECT
#endif
        ) {
        ret = -EFAULT;
        goto done;
    }

    /* Initialize the uap bss values which are uploaded from firmware */
    if (MLAN_STATUS_SUCCESS != woal_set_get_sys_config(priv,
                                                       MLAN_ACT_GET,
                                                       MOAL_IOCTL_WAIT,
                                                       &sys_config)) {
        PRINTM(MERROR, "Error getting AP confiruration\n");
        ret = -EFAULT;
        goto done;
    }

    /* Setting the default values */
    sys_config.channel = 6;
    sys_config.preamble_type = 0;

    if (priv->bss_type == MLAN_BSS_TYPE_UAP) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
        if (params->beacon_interval)
            sys_config.beacon_period = params->beacon_interval;
#else
        if (params->interval)
            sys_config.beacon_period = params->interval;
#endif
        if (params->dtim_period)
            sys_config.dtim_period = params->dtim_period;
    }
    if (priv->channel) {
        memset(sys_config.rates, 0, sizeof(sys_config.rates));
        sys_config.channel = priv->channel;
        if (priv->channel <= MAX_BG_CHANNEL) {
            sys_config.band_cfg = BAND_CONFIG_2G;
#ifdef WIFI_DIRECT_SUPPORT
            if (priv->bss_type == MLAN_BSS_TYPE_WIFIDIRECT)
                memcpy(sys_config.rates, Rates_WFD, sizeof(Rates_WFD));
            else
#endif
                memcpy(sys_config.rates, Rates_BG, sizeof(Rates_BG));
        } else {
            sys_config.band_cfg = BAND_CONFIG_5G;
            chan2Offset = woal_get_second_channel_offset(priv->channel);
            if (chan2Offset) {
                sys_config.band_cfg |= chan2Offset;
                sys_config.ht_cap_info = 0x117e;
                sys_config.ampdu_param = 3;
            }
#ifdef WIFI_DIRECT_SUPPORT
            if (priv->bss_type == MLAN_BSS_TYPE_WIFIDIRECT)
                memcpy(sys_config.rates, Rates_WFD, sizeof(Rates_WFD));
            else
#endif
                memcpy(sys_config.rates, Rates_A, sizeof(Rates_A));
        }
    }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0) || defined(COMPAT_WIRELESS)
    if (!params->ssid || !params->ssid_len) {
        ret = -EINVAL;
        goto done;
    }
    memcpy(sys_config.ssid.ssid, params->ssid,
           MIN(MLAN_MAX_SSID_LENGTH, params->ssid_len));
    sys_config.ssid.ssid_len = MIN(MLAN_MAX_SSID_LENGTH, params->ssid_len);
    if (params->hidden_ssid)
        sys_config.bcast_ssid_ctl = 0;
    else
        sys_config.bcast_ssid_ctl = 1;
    if (params->auth_type == NL80211_AUTHTYPE_SHARED_KEY)
        sys_config.auth_mode = MLAN_AUTH_MODE_SHARED;
    else
        sys_config.auth_mode = MLAN_AUTH_MODE_OPEN;

    for (i = 0; i < params->crypto.n_akm_suites; i++) {
        switch (params->crypto.akm_suites[i]) {
        case WLAN_AKM_SUITE_8021X:
            sys_config.key_mgmt = KEY_MGMT_EAP;
            if ((params->crypto.wpa_versions & NL80211_WPA_VERSION_1) &&
                (params->crypto.wpa_versions & NL80211_WPA_VERSION_2))
                sys_config.protocol = PROTOCOL_WPA | PROTOCOL_WPA2;
            else if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
                sys_config.protocol = PROTOCOL_WPA2;
            else if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
                sys_config.protocol = PROTOCOL_WPA;
            break;
        case WLAN_AKM_SUITE_PSK:
            sys_config.key_mgmt = KEY_MGMT_PSK;
            if ((params->crypto.wpa_versions & NL80211_WPA_VERSION_1) &&
                (params->crypto.wpa_versions & NL80211_WPA_VERSION_2))
                sys_config.protocol = PROTOCOL_WPA | PROTOCOL_WPA2;
            else if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
                sys_config.protocol = PROTOCOL_WPA2;
            else if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
                sys_config.protocol = PROTOCOL_WPA;
            break;
        }
    }
    sys_config.wpa_cfg.pairwise_cipher_wpa = 0;
    sys_config.wpa_cfg.pairwise_cipher_wpa2 = 0;
    for (i = 0; i < params->crypto.n_ciphers_pairwise; i++) {
        switch (params->crypto.ciphers_pairwise[i]) {
        case WLAN_CIPHER_SUITE_WEP40:
        case WLAN_CIPHER_SUITE_WEP104:
            break;
        case WLAN_CIPHER_SUITE_TKIP:
            if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
                sys_config.wpa_cfg.pairwise_cipher_wpa |= CIPHER_TKIP;
            if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
                sys_config.wpa_cfg.pairwise_cipher_wpa2 |= CIPHER_TKIP;
            break;
        case WLAN_CIPHER_SUITE_CCMP:
            if (params->crypto.wpa_versions & NL80211_WPA_VERSION_1)
                sys_config.wpa_cfg.pairwise_cipher_wpa |= CIPHER_AES_CCMP;
            if (params->crypto.wpa_versions & NL80211_WPA_VERSION_2)
                sys_config.wpa_cfg.pairwise_cipher_wpa2 |= CIPHER_AES_CCMP;
            break;
        }
    }
    switch (params->crypto.cipher_group) {
    case WLAN_CIPHER_SUITE_WEP40:
    case WLAN_CIPHER_SUITE_WEP104:
        if ((priv->cipher == WLAN_CIPHER_SUITE_WEP40) ||
            (priv->cipher == WLAN_CIPHER_SUITE_WEP104)) {
            sys_config.protocol = PROTOCOL_STATIC_WEP;
            sys_config.key_mgmt = KEY_MGMT_NONE;
            sys_config.wpa_cfg.length = 0;
            sys_config.wep_cfg.key0.key_index = priv->key_index;
            sys_config.wep_cfg.key0.is_default = 1;
            sys_config.wep_cfg.key0.length = priv->key_len;
            memcpy(sys_config.wep_cfg.key0.key, priv->key_material,
                   priv->key_len);
        }
        break;
    case WLAN_CIPHER_SUITE_TKIP:
        sys_config.wpa_cfg.group_cipher = CIPHER_TKIP;
        break;
    case WLAN_CIPHER_SUITE_CCMP:
        sys_config.wpa_cfg.group_cipher = CIPHER_AES_CCMP;
        break;
    }
#else
    /* Since in Android ICS 4.0.1's wpa_supplicant, there is no way to set ssid
       when GO (AP) starts up, so get it from beacon head parameter TODO: right
       now use hard code 24 -- ieee80211 header lenth, 12 -- fixed element
       length for beacon */
#define BEACON_IE_OFFSET	36
    /* Find SSID in head SSID IE id: 0, right now use hard code */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    ssid_ie = woal_parse_ie_tlv(params->beacon.head + BEACON_IE_OFFSET,
                                params->beacon.head_len - BEACON_IE_OFFSET, 0);
#else
    ssid_ie = woal_parse_ie_tlv(params->head + BEACON_IE_OFFSET,
                                params->head_len - BEACON_IE_OFFSET, 0);
#endif
    if (!ssid_ie) {
        PRINTM(MERROR, "No ssid IE found.\n");
        ret = -EFAULT;
        goto done;
    }
    if (*(ssid_ie + 1) > 32) {
        PRINTM(MERROR, "ssid len error: %d\n", *(ssid_ie + 1));
        ret = -EFAULT;
        goto done;
    }
    memcpy(sys_config.ssid.ssid, ssid_ie + 2, *(ssid_ie + 1));
    sys_config.ssid.ssid_len = *(ssid_ie + 1);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    head = (struct ieee80211_mgmt *) params->beacon.head;
#else
    head = (struct ieee80211_mgmt *) params->head;
#endif
    capab_info = le16_to_cpu(head->u.beacon.capab_info);
    PRINTM(MIOCTL, "capab_info=0x%x\n", head->u.beacon.capab_info);
    sys_config.auth_mode = MLAN_AUTH_MODE_OPEN;
        /** For ICS, we don't support OPEN mode */
    if ((priv->cipher == WLAN_CIPHER_SUITE_WEP40) ||
        (priv->cipher == WLAN_CIPHER_SUITE_WEP104)) {
        sys_config.protocol = PROTOCOL_STATIC_WEP;
        sys_config.key_mgmt = KEY_MGMT_NONE;
        sys_config.wpa_cfg.length = 0;
        sys_config.wep_cfg.key0.key_index = priv->key_index;
        sys_config.wep_cfg.key0.is_default = 1;
        sys_config.wep_cfg.key0.length = priv->key_len;
        memcpy(sys_config.wep_cfg.key0.key, priv->key_material, priv->key_len);
    } else {
                /** Get cipher and key_mgmt from RSN/WPA IE */
        if (capab_info & WLAN_CAPABILITY_PRIVACY) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
            if (MFALSE ==
                woal_find_wpa_ies(params->beacon.tail, params->beacon.tail_len,
                                  &sys_config))
#else
            if (MFALSE ==
                woal_find_wpa_ies(params->tail, params->tail_len, &sys_config))
#endif
            {
                /* hard code setting to wpa2-psk */
                sys_config.protocol = PROTOCOL_WPA2;
                sys_config.key_mgmt = KEY_MGMT_PSK;
                sys_config.wpa_cfg.pairwise_cipher_wpa2 = CIPHER_AES_CCMP;
                sys_config.wpa_cfg.group_cipher = CIPHER_AES_CCMP;
            }
        }
    }
#endif /* COMPAT_WIRELESS */
    /* If the security mode is configured as WEP or WPA-PSK, it will disable
       11n automatically, and if configured as open(off) or wpa2-psk, it will
       automatically enable 11n */
    if ((sys_config.protocol == PROTOCOL_STATIC_WEP) ||
        (sys_config.protocol == PROTOCOL_WPA))
        woal_uap_set_11n_status(&sys_config, MLAN_ACT_DISABLE);
    else
        woal_uap_set_11n_status(&sys_config, MLAN_ACT_ENABLE);
    if (MLAN_STATUS_SUCCESS != woal_set_get_sys_config(priv,
                                                       MLAN_ACT_SET,
                                                       MOAL_IOCTL_WAIT,
                                                       &sys_config)) {
        ret = -EFAULT;
        goto done;
    }
  done:
    LEAVE();
    return ret;
}

extern void cfg80211_scan_done(struct cfg80211_scan_request *request, bool aborted);
static int
woal_uap_cfg80211_scan(struct wiphy *wiphy, struct net_device *dev,
                       struct cfg80211_scan_request *request)
{
    ENTER();

    cfg80211_scan_done(request, TRUE);

    LEAVE();
    return 0;
}

static int
woal_uap_cfg80211_connect(struct wiphy *wiphy, struct net_device *dev,
                          struct cfg80211_connect_params *sme)
{
    ENTER();
    LEAVE();
    return 0;
}

static int
woal_uap_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *dev,
                             t_u16 reason_code)
{
    ENTER();
    LEAVE();
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
/**
 * @brief initialize AP or GO parameters

 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 * @param params          A pointer to cfg80211_ap_settings structure 
 * @return                0 -- success, otherwise fail
 */
int
woal_cfg80211_add_beacon(struct wiphy *wiphy,
                         struct net_device *dev,
                         struct cfg80211_ap_settings *params)
#else
/**
 * @brief initialize AP or GO parameters

 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 * @param params          A pointer to beacon_parameters structure 
 * @return                0 -- success, otherwise fail
 */
int
woal_cfg80211_add_beacon(struct wiphy *wiphy,
                         struct net_device *dev,
                         struct beacon_parameters *params)
#endif
{
    moal_private *priv = (moal_private *) woal_get_netdev_priv(dev);
    int ret = 0;

    ENTER();

    PRINTM(MIOCTL, "add beacon\n");
#ifdef STA_CFG80211
        /*** cancel pending scan */
    woal_cancel_scan(priv, MOAL_IOCTL_WAIT);
#endif
    if (params != NULL) {
        /* bss config */
        if (MLAN_STATUS_SUCCESS != woal_cfg80211_beacon_config(priv, params)) {
            ret = -EFAULT;
            goto done;
        }

        /* set mgmt frame ies */
        if (MLAN_STATUS_SUCCESS != woal_cfg80211_mgmt_frame_ie(priv,
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0) && !defined(COMPAT_WIRELESS)
                                                               params->tail,
                                                               params->tail_len,
                                                               NULL, 0, NULL, 0,
                                                               NULL, 0,
                                                               MGMT_MASK_BEACON
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
                                                               params->beacon.
                                                               tail,
                                                               params->beacon.
                                                               tail_len,
                                                               params->beacon.
                                                               proberesp_ies,
                                                               params->beacon.
                                                               proberesp_ies_len,
                                                               params->beacon.
                                                               assocresp_ies,
                                                               params->beacon.
                                                               assocresp_ies_len,
#else
                                                               params->tail,
                                                               params->tail_len,
                                                               params->
                                                               proberesp_ies,
                                                               params->
                                                               proberesp_ies_len,
                                                               params->
                                                               assocresp_ies,
                                                               params->
                                                               assocresp_ies_len,
#endif
                                                               NULL, 0,
                                                               MGMT_MASK_BEACON
                                                               |
                                                               MGMT_MASK_PROBE_RESP
                                                               |
                                                               MGMT_MASK_ASSOC_RESP
#endif
            )) {
            ret = -EFAULT;
            goto done;
        }
    }

    /* if the bss is stopped, then start it */
    if (priv->bss_started == MFALSE) {
        if (MLAN_STATUS_SUCCESS !=
            woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_START)) {
            ret = -EFAULT;
            goto done;
        }
    }

  done:
    LEAVE();
    return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
/**
 * @brief set AP or GO parameter
 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 * @param params          A pointer to cfg80211_beacon_data structure 
 * @return                0 -- success, otherwise fail
 */
int
woal_cfg80211_set_beacon(struct wiphy *wiphy,
                         struct net_device *dev,
                         struct cfg80211_beacon_data *params)
#else
/**
 * @brief set AP or GO parameter
 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 * @param params          A pointer to beacon_parameters structure 
 * @return                0 -- success, otherwise fail
 */
int
woal_cfg80211_set_beacon(struct wiphy *wiphy,
                         struct net_device *dev,
                         struct beacon_parameters *params)
#endif
{
    moal_private *priv = (moal_private *) woal_get_netdev_priv(dev);
    int ret = 0;

    ENTER();

    PRINTM(MIOCTL, "set beacon\n");
    if (params != NULL) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0) && !defined(COMPAT_WIRELESS)
        if (params->tail && params->tail_len) {
            if (MLAN_STATUS_SUCCESS !=
                woal_cfg80211_mgmt_frame_ie(priv,
                                            params->tail, params->tail_len,
                                            NULL, 0, NULL, 0, NULL, 0,
                                            MGMT_MASK_BEACON)) {
                ret = -EFAULT;
                goto done;
            }
        }
#else
        if (params->beacon_ies && params->beacon_ies_len) {
            if (MLAN_STATUS_SUCCESS !=
                woal_cfg80211_mgmt_frame_ie(priv, params->tail,
                                            params->tail_len, NULL, 0, NULL, 0,
                                            NULL, 0, MGMT_MASK_BEACON)) {
                ret = -EFAULT;
                goto done;
            }
        }

        if (params->proberesp_ies && params->proberesp_ies_len) {
            if (MLAN_STATUS_SUCCESS !=
                woal_cfg80211_mgmt_frame_ie(priv, NULL, 0,
                                            params->proberesp_ies,
                                            params->proberesp_ies_len, NULL, 0,
                                            NULL, 0, MGMT_MASK_PROBE_RESP)) {
                ret = -EFAULT;
                goto done;
            }
        }

        if (params->assocresp_ies && params->assocresp_ies_len) {
            if (MLAN_STATUS_SUCCESS !=
                woal_cfg80211_mgmt_frame_ie(priv, NULL, 0, NULL, 0,
                                            params->assocresp_ies,
                                            params->assocresp_ies_len, NULL, 0,
                                            MGMT_MASK_ASSOC_RESP)) {
                ret = -EFAULT;
                goto done;
            }
        }
#endif
    }

  done:
    LEAVE();
    return ret;
}

/**
 * @brief reset AP or GO parameters
 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
int
woal_cfg80211_del_beacon(struct wiphy *wiphy, struct net_device *dev)
{
    moal_private *priv = (moal_private *) woal_get_netdev_priv(dev);
    int ret = 0;

    ENTER();

    PRINTM(MIOCTL, "del beacon\n");
    /* if the bss is still running, then stop it */
    if (priv->bss_started == MTRUE) {
        if (MLAN_STATUS_SUCCESS !=
            woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_STOP)) {
            ret = -EFAULT;
            goto done;
        }
        if (MLAN_STATUS_SUCCESS !=
            woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_RESET)) {
            ret = -EFAULT;
            goto done;
        }
    }
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0) && !defined(COMPAT_WIRELESS)
    /* clear BEACON WPS/P2P IE */
    if (MLAN_STATUS_SUCCESS !=
        woal_cfg80211_mgmt_frame_ie(priv, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                                    MGMT_MASK_BEACON_WPS_P2P)) {
        ret = -EFAULT;
        goto done;
    }
#endif
    /* clear mgmt frame ies */
    if (MLAN_STATUS_SUCCESS !=
        woal_cfg80211_mgmt_frame_ie(priv, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                                    MGMT_MASK_BEACON | MGMT_MASK_PROBE_RESP |
                                    MGMT_MASK_ASSOC_RESP)) {
        ret = -EFAULT;
        goto done;
    }

    priv->cipher = 0;
    priv->key_len = 0;
  done:
    LEAVE();
    return ret;
}

/**
 * @brief Get station info
 *
 * @param wiphy           A pointer to wiphy structure
 * @param dev             A pointer to net_device structure
 * @param mac			  A pointer to station mac address
 * @param stainfo		  A pointer to station_info structure
 *
 * @return                0 -- success, otherwise fail
 */
int
woal_uap_cfg80211_get_station(struct wiphy *wiphy, struct net_device *dev,
                              const u8 * mac, struct station_info *stainfo)
{
    moal_private *priv = (moal_private *) woal_get_netdev_priv(dev);
    int ret = -EFAULT;
    int i = 0;
    mlan_ds_get_info *info = NULL;
    mlan_ioctl_req *ioctl_req = NULL;

    ENTER();
    if (priv->media_connected == MFALSE) {
        PRINTM(MINFO, "cfg80211: Media not connected!\n");
        LEAVE();
        return -ENOENT;
    }

    /* Allocate an IOCTL request buffer */
    ioctl_req =
        (mlan_ioctl_req *) woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_get_info));
    if (ioctl_req == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    info = (mlan_ds_get_info *) ioctl_req->pbuf;
    info->sub_command = MLAN_OID_UAP_STA_LIST;
    ioctl_req->req_id = MLAN_IOCTL_GET_INFO;
    ioctl_req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS !=
        woal_request_ioctl(priv, ioctl_req, MOAL_IOCTL_WAIT)) {
        goto done;
    }
    for (i = 0; i < info->param.sta_list.sta_count; i++) {
        if (!memcmp(info->param.sta_list.info[i].mac_address, mac, ETH_ALEN)) {
            PRINTM(MIOCTL,
                   "Get station: %02x:%02x:%02x:%02x:%02x:%02x RSSI=%d\n",
                   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                   (int) info->param.sta_list.info[i].rssi);
            stainfo->filled = STATION_INFO_INACTIVE_TIME | STATION_INFO_SIGNAL;
            stainfo->inactive_time = 0;
            stainfo->signal = info->param.sta_list.info[i].rssi;
            ret = 0;
            break;
        }
    }
  done:
    if (ioctl_req)
        kfree(ioctl_req);
    LEAVE();
    return ret;
}


/**
 * @brief Initialize the uAP wiphy
 *
 * @param priv            A pointer to moal_private structure
 * @param wait_option     Wait option
 *
 * @return                MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
woal_cfg80211_uap_init_wiphy(moal_private * priv, t_u8 wait_option)
{
    struct wiphy *wiphy;
    mlan_uap_bss_param ap_cfg;
    mlan_ioctl_req *req = NULL;
    mlan_ds_11n_cfg *cfg_11n = NULL;
    t_u32 hw_dev_cap;

    ENTER();

    if (priv->wdev)
        wiphy = priv->wdev->wiphy;
    else {
        //PRINTM(MERROR, "Invalid parameter when init wiphy.\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    if (MLAN_STATUS_SUCCESS != woal_set_get_sys_config(priv,
                                                       MLAN_ACT_GET,
                                                       wait_option, &ap_cfg)) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    /* Get 11n tx parameters from MLAN */
    req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_11n_cfg));
    if (req == NULL) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    cfg_11n = (mlan_ds_11n_cfg *) req->pbuf;
    cfg_11n->sub_command = MLAN_OID_11N_HTCAP_CFG;
    req->req_id = MLAN_IOCTL_11N_CFG;
    req->action = MLAN_ACT_GET;
    cfg_11n->param.htcap_cfg.hw_cap_req = MTRUE;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, wait_option)) {
        kfree(req);
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    hw_dev_cap = cfg_11n->param.htcap_cfg.htcap;

    /* Get supported MCS sets */
    memset(req->pbuf, 0, sizeof(mlan_ds_11n_cfg));
    cfg_11n->sub_command = MLAN_OID_11N_CFG_SUPPORTED_MCS_SET;
    req->req_id = MLAN_IOCTL_11N_CFG;
    req->action = MLAN_ACT_GET;

    if (MLAN_STATUS_SUCCESS != woal_request_ioctl(priv, req, wait_option)) {
        kfree(req);
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    /* Initialize parameters for 2GHz and 5GHz bands */
    woal_cfg80211_setup_ht_cap(&wiphy->bands[IEEE80211_BAND_2GHZ]->ht_cap,
                               hw_dev_cap, cfg_11n->param.supported_mcs_set);
    /* For 2.4G band only card, this shouldn't be set */
    if (wiphy->bands[IEEE80211_BAND_5GHZ])
        woal_cfg80211_setup_ht_cap(&wiphy->bands[IEEE80211_BAND_5GHZ]->ht_cap,
                                   hw_dev_cap,
                                   cfg_11n->param.supported_mcs_set);
    if (req)
        kfree(req);

    /* Set retry limit count to wiphy */
    //wiphy->retry_long = (t_u8) ap_cfg.retry_limit;
    //wiphy->retry_short = (t_u8) ap_cfg.retry_limit;
    //wiphy->max_scan_ie_len = MAX_IE_SIZE;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37) || defined(COMPAT_WIRELESS)
    wiphy->mgmt_stypes = (struct ieee80211_txrx_stypes*)ieee80211_uap_mgmt_stypes;
#endif
    /* Set RTS threshold to wiphy */
    //wiphy->rts_threshold = (t_u32) ap_cfg.rts_threshold;

    /* Set fragment threshold to wiphy */
    //wiphy->frag_threshold = (t_u32) ap_cfg.frag_threshold;

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 * @brief Register the device with cfg80211
 *
 * @param dev       A pointer to net_device structure
 * @param bss_type  BSS type
 *
 * @return          MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */

mlan_status
woal_register_uap_cfg80211(struct net_device * dev, t_u8 bss_type)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    moal_private *priv = (moal_private *) netdev_priv(dev);
    //void *wdev_priv = NULL;
    struct wireless_dev *wdev = NULL;
    mlan_fw_info fw_info;

    ENTER();

    /* Allocate wireless device */
    wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
    if (!wdev) {
        PRINTM(MERROR, "Could not allocate wireless device\n");
        ret = MLAN_STATUS_FAILURE;
        goto err_wdev;
    }

    /* Allocate wiphy */
	wdev->wiphy = mem_calloc(sizeof(struct wiphy), 1);
	dev->cfg80211_ops = &woal_cfg80211_uap_ops;
    //wdev->wiphy = wiphy_new(&woal_cfg80211_sta_ops, sizeof(moal_private *));
    if (!wdev->wiphy) {
        PRINTM(MERROR, "Could not allocate wiphy device\n");
        ret = MLAN_STATUS_FAILURE;
        goto err_wdev;
    }
    if (bss_type == MLAN_BSS_TYPE_UAP) {
       // dev_set_name(&wdev->wiphy->dev, dev->name);
        wdev->iftype = NL80211_IFTYPE_AP;
        wdev->wiphy->interface_modes =
            MBIT(NL80211_IFTYPE_AP) | MBIT(NL80211_IFTYPE_STATION) | 0;
        wdev->wiphy->max_scan_ssids = 10;
    }

    /* Make this wiphy known to this driver only */
    //wdev->wiphy->privid = mrvl_wiphy_privid;

    /* Supported bands */
    wdev->wiphy->bands[IEEE80211_BAND_2GHZ] = &cfg80211_band_2ghz;
    if (MLAN_STATUS_SUCCESS ==
        woal_request_get_fw_info(priv, MOAL_CMD_WAIT, &fw_info)) {
        if (fw_info.fw_bands & BAND_A)
            wdev->wiphy->bands[IEEE80211_BAND_5GHZ] = &cfg80211_band_5ghz;
    }

    /* Initialize cipher suits */
    wdev->wiphy->cipher_suites = cfg80211_cipher_suites;
    wdev->wiphy->n_cipher_suites = ARRAY_SIZE(cfg80211_cipher_suites, u32);

    wdev->wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

    /* We are using custom domains */
    //wdev->wiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;

    //wdev->wiphy->reg_notifier = NULL;   // TODO: woal_cfg80211_reg_notifier;

    /* Set moal_private pointer in wiphy_priv */
    //wdev_priv = wiphy_priv(wdev->wiphy);

    //*(unsigned long *) wdev_priv = (unsigned long) priv;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39) || defined(COMPAT_WIRELESS)
    //set_wiphy_dev(wdev->wiphy, (struct device *) priv->phandle->hotplug_device);
#endif

    //if (wiphy_register(wdev->wiphy) < 0) {
    //    PRINTM(MERROR, "Wiphy device registration failed!\n");
    //    ret = MLAN_STATUS_FAILURE;
    //    goto err_wdev;
    //}
	wdev->wiphy->priv = priv;
    //dev_net_set(dev, wiphy_net(wdev->wiphy));
    dev->ieee80211_ptr = wdev;
    //SET_NETDEV_DEV(dev, wiphy_dev(wdev->wiphy));

    if (ret != MLAN_STATUS_SUCCESS) {
        PRINTM(MERROR, "Wiphy device registration failed!\n");
    } else {
        PRINTM(MINFO, "Successfully registered wiphy device\n");
        LEAVE();
        return ret;
    }

    wiphy_unregister(wdev->wiphy);
  err_wdev:
    dev->ieee80211_ptr = NULL;
    if (wdev && wdev->wiphy)
        wiphy_free(wdev->wiphy);
    kfree(wdev);
    LEAVE();
    return ret;
}
#endif


