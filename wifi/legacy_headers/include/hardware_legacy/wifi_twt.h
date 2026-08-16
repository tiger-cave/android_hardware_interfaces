/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WIFI_HAL_TWT_H__
#define __WIFI_HAL_TWT_H__

#include "wifi_hal.h"

/*
 * Current framework TWT types.  Keep these declarations available even when
 * the corresponding wifi_hal_fn entries are moved to the end of the function
 * table to preserve the offsets used by ultra-legacy vendor HALs.
 */
typedef struct {
    u8 is_twt_requester_supported;
    u8 is_twt_responder_supported;
    u8 is_broadcast_twt_supported;
    u8 is_flexible_twt_supported;
    u32 min_wake_duration_micros;
    u32 max_wake_duration_micros;
    u64 min_wake_interval_micros;
    u64 max_wake_interval_micros;
} wifi_twt_capabilities;

typedef struct {
    s8 mlo_link_id;
    u32 min_wake_duration_micros;
    u32 max_wake_duration_micros;
    u64 min_wake_interval_micros;
    u64 max_wake_interval_micros;
} wifi_twt_request;

typedef enum {
    WIFI_TWT_NEGO_TYPE_INDIVIDUAL,
    WIFI_TWT_NEGO_TYPE_BROADCAST,
} wifi_twt_negotiation_type;

typedef struct {
    u32 session_id;
    s8 mlo_link_id;
    u32 wake_duration_micros;
    u64 wake_interval_micros;
    wifi_twt_negotiation_type negotiation_type;
    u8 is_trigger_enabled;
    u8 is_announced;
    u8 is_implicit;
    u8 is_protected;
    u8 is_updatable;
    u8 is_suspendable;
    u8 is_responder_pm_mode_enabled;
} wifi_twt_session;

typedef struct {
    u32 avg_pkt_num_tx;
    u32 avg_pkt_num_rx;
    u32 avg_tx_pkt_size;
    u32 avg_rx_pkt_size;
    u32 avg_eosp_dur_us;
    u32 eosp_count;
} wifi_twt_session_stats;

typedef enum {
    WIFI_TWT_ERROR_CODE_FAILURE_UNKNOWN,
    WIFI_TWT_ERROR_CODE_ALREADY_RESUMED,
    WIFI_TWT_ERROR_CODE_ALREADY_SUSPENDED,
    WIFI_TWT_ERROR_CODE_INVALID_PARAMS,
    WIFI_TWT_ERROR_CODE_MAX_SESSION_REACHED,
    WIFI_TWT_ERROR_CODE_NOT_AVAILABLE,
    WIFI_TWT_ERROR_CODE_NOT_SUPPORTED,
    WIFI_TWT_ERROR_CODE_PEER_NOT_SUPPORTED,
    WIFI_TWT_ERROR_CODE_PEER_REJECTED,
    WIFI_TWT_ERROR_CODE_TIMEOUT,
} wifi_twt_error_code;

typedef enum {
    WIFI_TWT_TEARDOWN_REASON_CODE_UNKNOWN,
    WIFI_TWT_TEARDOWN_REASON_CODE_LOCALLY_REQUESTED,
    WIFI_TWT_TEARDOWN_REASON_CODE_INTERNALLY_INITIATED,
    WIFI_TWT_TEARDOWN_REASON_CODE_PEER_INITIATED,
} wifi_twt_teardown_reason_code;

typedef struct {
    void (*on_twt_failure)(wifi_request_id id, wifi_twt_error_code error_code);
    void (*on_twt_session_create)(wifi_request_id id, wifi_twt_session session);
    void (*on_twt_session_update)(wifi_request_id id, wifi_twt_session session);
    void (*on_twt_session_teardown)(wifi_request_id id, int session_id,
                                    wifi_twt_teardown_reason_code reason);
    void (*on_twt_session_stats)(wifi_request_id id, int session_id,
                                 wifi_twt_session_stats stats);
    void (*on_twt_session_suspend)(wifi_request_id id, int session_id);
    void (*on_twt_session_resume)(wifi_request_id id, int session_id);
} wifi_twt_events;

typedef struct {
    u8 requester_supported; // 0 for not supporting requester
    u8 responder_supported; // 0 for not supporting responder
    u8 broadcast_twt_supported; // 0 for not supporting broadcast TWT
    u8 flexibile_twt_supported; // 0 for not supporting flexible TWT
} TwtCapability;

typedef struct {
    TwtCapability device_capability;
    TwtCapability peer_capability;
} TwtCapabilitySet;

// For all optional fields below, if no value specify -1
typedef struct {
    u8 config_id;        // An unique ID for an individual TWT request
    u8 negotiation_type; // 0 for individual TWT, 1 for broadcast TWT
    u8 trigger_type;     // 0 for non-triggered TWT, 1 for triggered TWT
    s32 wake_dur_us;     // Proposed wake duration in us
    s32 wake_int_us;     // Average wake interval in us
    s32 wake_int_min_us; // Min wake interval in us. Optional.
    s32 wake_int_max_us; // Max wake interval in us. Optional.
    s32 wake_dur_min_us; // Min wake duration in us. Optional.
    s32 wake_dur_max_us; // Max wake duration in us. Optional.
    s32 avg_pkt_size;    // Average bytes of each packet to send in each wake
                         // duration. Optional.
    s32 avg_pkt_num;     // Average number of packets to send in each wake
                         // duration. Optional.
    s32 wake_time_off_us; // First wake duration time offset in us. Optional.
} TwtSetupRequest;

typedef enum {
    TWT_SETUP_SUCCESS = 0, // TWT setup is accepted.
    TWT_SETUP_REJECT = 1,  // TWT setup is rejected by AP.
    TWT_SETUP_TIMEOUT = 2, // TWT setup response from AP times out.
    TWT_SETUP_IE = 3,      // AP sent TWT Setup IE parsing failure.
    TWT_SETUP_PARAMS = 4,  // AP sent TWT Setup IE Parameters invalid.
    TWT_SETUP_ERROR = 255, // Generic error
} TwtSetupReasonCode;

typedef struct {
    u8 config_id; // An unique ID for an individual TWT request
    u8 status;    // 0 for success, non-zero for failure
    TwtSetupReasonCode reason_code;
    u8 negotiation_type; // 0 for individual TWT, 1 for broadcast TWT
    u8 trigger_type;     // 0 for non-triggered TWT, 1 for triggered TWT
    s32 wake_dur_us;     // Proposed wake duration in us
    s32 wake_int_us;     // Average wake interval in us
    s32 wake_time_off_us; // First wake duration time offset in us.
} TwtSetupResponse;

typedef struct {
    u8 config_id;        // An unique ID for an individual TWT request
    u8 all_twt;          // 0 for individual setp request, 1 for all TWT
    u8 negotiation_type; // 0 for individual TWT, 1 for broadcast TWT
} TwtTeardownRequest;

typedef enum {
    TWT_TD_RC_HOST = 0,  // Teardown triggered by Host
    TWT_TD_RC_PEER = 1,  // Peer initiated teardown
    TWT_TD_RC_MCHAN = 2, // Teardown due to MCHAN Active
    TWT_TD_RC_MCNX = 3,  // Teardown due to MultiConnection
    TWT_TD_RC_CSA = 4,   // Teardown due to CSA
    TWT_TD_RC_BTCX = 5,  // Teardown due to BT Coex
    TWT_TD_RC_SETUP_FAIL = 6, // Setup fails midway. Teardown all connections
    TWT_TD_RC_SCHED = 7,   // Teardown by TWT Scheduler
    TWT_TD_RC_ERROR = 255, // Generic error cases
} TwtTeardownReason;

typedef struct {
    u8 config_id; // An unique ID for an individual TWT request
    u8 all_twt;   // 0 for individual setp request, 1 for all TWT
    u8 status;    // 0 for success, non-zero for failure
    TwtTeardownReason reason;
} TwtTeardownCompletion;

typedef struct {
    u8 config_id;       // An unique ID for an individual TWT request
    u8 all_twt;         // 0 for individual setup request, 1 for all TWT
    s32 resume_time_us; // If -1, TWT is suspended for indefinite time.
                        // Otherwise, TWT is suspended for resume_time_us
} TwtInfoFrameRequest;

typedef enum {
    TWT_INFO_RC_HOST  = 0, // Host initiated TWT Info frame */
    TWT_INFO_RC_PEER  = 1, // Peer initiated TWT Info frame
    TWT_INFO_RC_ERROR = 2, // Generic error conditions */
} TwtInfoFrameReason;

// TWT Info frame triggered externally.
// Device should not send TwtInfoFrameReceived to Host for internally
// triggered TWT Info frame during SCAN, MCHAN operations.
typedef struct {
    u8 config_id; // An unique ID for an individual TWT request
    u8 all_twt;   // 0 for individual setup request, 1 for all TWT
    u8 status;    // 0 for success, non-zero for failure
    TwtInfoFrameReason reason;
    u8 twt_resumed; // 1 - TWT resumed, 0 - TWT suspended
} TwtInfoFrameReceived;

typedef struct {
    u8 config_id;
    u32 avg_pkt_num_tx; // Average number of Tx packets in each wake duration.
    u32 avg_pkt_num_rx; // Average number of Rx packets in each wake duration.
    u32 avg_tx_pkt_size; // Average bytes per Rx packet in each wake duration.
    u32 avg_rx_pkt_size; // Average bytes per Rx packet in each wake duration.
    u32 avg_eosp_dur_us; // Average duration of early terminated SP
    u32 eosp_count;  // Count of early terminations
    u32 num_sp; // Count of service period (SP), also known as wake duration.
} TwtStats;

// Asynchronous notification from the device.
// For example, TWT was torn down by the device and later when the device is
// ready, it can send this async notification.
// This can be expandable in future.
typedef enum {
   TWT_NOTIF_ALLOW_TWT  = 1, // Device ready to process TWT Setup request
} TwtNotification;

typedef struct {
    TwtNotification notification;
} TwtDeviceNotify;

// Callbacks for various TWT responses and events
typedef struct {
    // Callback for TWT setup response
    void (*EventTwtSetupResponse)(TwtSetupResponse *event);
    // Callback for TWT teardown completion
    void (*EventTwtTeardownCompletion)(TwtTeardownCompletion* event);
    // Callback for TWT info frame received event
    void (*EventTwtInfoFrameReceived)(TwtInfoFrameReceived* event);
    // Callback for TWT notification from the device
    void (*EventTwtDeviceNotify)(TwtDeviceNotify* event);
} TwtCallbackHandler;

#endif /* __WIFI_HAL_TWT_H__ */
