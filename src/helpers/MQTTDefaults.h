#pragma once

#ifdef WITH_MQTT_BRIDGE

#include <string.h>
#include <MeshCore.h>
#include "CommonCLI.h"
#include "MQTTPacketFilter.h"
#include "MQTTPresets.h"

// Compile-time defaults for fresh /mqtt.json (override via platformio build_flags).
// Example:
//   -D MQTT_DEFAULT_SLOT1_PRESET='"coloradomesh"'
//   -D MQTT_DEFAULT_IATA='"DEN"'
//   -D MQTT_DEFAULT_TIMEZONE='"America/Denver"'
//   -D MQTT_DEFAULT_TIMEZONE_OFFSET=-8

#ifndef MQTT_DEFAULT_SLOT1_PRESET
#define MQTT_DEFAULT_SLOT1_PRESET "coloradomesh"
#endif
#ifndef MQTT_DEFAULT_SLOT2_PRESET
#define MQTT_DEFAULT_SLOT2_PRESET "meshmapper"
#endif
#ifndef MQTT_DEFAULT_SLOT3_PRESET
#define MQTT_DEFAULT_SLOT3_PRESET "none"
#endif
#ifndef MQTT_DEFAULT_SLOT4_PRESET
#define MQTT_DEFAULT_SLOT4_PRESET "none"
#endif
#ifndef MQTT_DEFAULT_SLOT5_PRESET
#define MQTT_DEFAULT_SLOT5_PRESET "none"
#endif
#ifndef MQTT_DEFAULT_SLOT6_PRESET
#define MQTT_DEFAULT_SLOT6_PRESET "none"
#endif

#ifndef MQTT_DEFAULT_IATA
#define MQTT_DEFAULT_IATA ""
#endif

#ifndef MQTT_DEFAULT_TIMEZONE
#define MQTT_DEFAULT_TIMEZONE "America/Denver"
#endif

#ifndef MQTT_DEFAULT_TIMEZONE_OFFSET
#define MQTT_DEFAULT_TIMEZONE_OFFSET -8
#endif

static inline void mqttDefaultSlotPreset(char* dest, size_t dest_size, const char* preset) {
  const char* resolved = MQTT_PRESET_NONE;
  if (preset && preset[0] != '\0') {
    if (strcmp(preset, MQTT_PRESET_NONE) == 0 ||
        strcmp(preset, MQTT_PRESET_CUSTOM) == 0 ||
        findMQTTPreset(preset) != nullptr) {
      resolved = preset;
    } else {
      MESH_DEBUG_PRINTLN("MQTT: invalid default preset '%s', using none", preset);
    }
  }
  strncpy(dest, resolved, dest_size - 1);
  dest[dest_size - 1] = '\0';
}

static inline void applyMQTTDefaults(MQTTPrefs* prefs) {
  memset(prefs, 0, sizeof(MQTTPrefs));
  prefs->mqtt_status_enabled = 1;
  prefs->mqtt_packets_enabled = 1;
  prefs->mqtt_raw_enabled = 0;
  prefs->mqtt_tx_enabled = 2;
  prefs->mqtt_rx_enabled = 1;
  prefs->mqtt_status_interval = 300000;
  prefs->wifi_power_save = 1;

  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[0], sizeof(prefs->mqtt_slot_preset[0]),
                        MQTT_DEFAULT_SLOT1_PRESET);
  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[1], sizeof(prefs->mqtt_slot_preset[1]),
                        MQTT_DEFAULT_SLOT2_PRESET);
  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[2], sizeof(prefs->mqtt_slot_preset[2]),
                        MQTT_DEFAULT_SLOT3_PRESET);
  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[3], sizeof(prefs->mqtt_slot_preset[3]),
                        MQTT_DEFAULT_SLOT4_PRESET);
  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[4], sizeof(prefs->mqtt_slot_preset[4]),
                        MQTT_DEFAULT_SLOT5_PRESET);
  mqttDefaultSlotPreset(prefs->mqtt_slot_preset[5], sizeof(prefs->mqtt_slot_preset[5]),
                        MQTT_DEFAULT_SLOT6_PRESET);
  #ifdef CORDER_FARM_CORE_PROFILE
  // Corder Farm MQTT destinations.
  // Custom TLS/WSS slots use the firmware's embedded CA bundle.
  // Slot 1 - MichMesh
  strncpy(prefs->mqtt_slot_preset[0], "custom",
          sizeof(prefs->mqtt_slot_preset[0]) - 1);
  strncpy(prefs->mqtt_slot_host[0], "mqtts://mqtt.michmesh.net:8883",
          sizeof(prefs->mqtt_slot_host[0]) - 1);
  prefs->mqtt_slot_port[0] = 8883;
  strncpy(prefs->mqtt_slot_username[0], "meshdev",
          sizeof(prefs->mqtt_slot_username[0]) - 1);
  strncpy(prefs->mqtt_slot_password[0], "large4cats",
          sizeof(prefs->mqtt_slot_password[0]) - 1);

  // Slot 2 - WestMich
  strncpy(prefs->mqtt_slot_preset[1], "custom",
          sizeof(prefs->mqtt_slot_preset[1]) - 1);
  strncpy(prefs->mqtt_slot_host[1], "wss://mqtt.westmichmesh.com:443",
          sizeof(prefs->mqtt_slot_host[1]) - 1);
  prefs->mqtt_slot_port[1] = 443;

  // Slot 3 - Timmins
  strncpy(prefs->mqtt_slot_preset[2], "custom",
          sizeof(prefs->mqtt_slot_preset[2]) - 1);
  strncpy(prefs->mqtt_slot_host[2], "mqtt://corescope.timmins.net:1883",
          sizeof(prefs->mqtt_slot_host[2]) - 1);
  prefs->mqtt_slot_port[2] = 1883;

  // Slot 4 - Thumb
  strncpy(prefs->mqtt_slot_preset[3], "custom",
          sizeof(prefs->mqtt_slot_preset[3]) - 1);
  strncpy(prefs->mqtt_slot_host[3], "mqtt://cs.tarratt.net:1883",
          sizeof(prefs->mqtt_slot_host[3]) - 1);
  prefs->mqtt_slot_port[3] = 1883;

  // Slot 5 - MeshMapper built-in preset
  strncpy(prefs->mqtt_slot_preset[4], "meshmapper",
          sizeof(prefs->mqtt_slot_preset[4]) - 1);

  // Slot 6 intentionally left available
  strncpy(prefs->mqtt_slot_preset[5], "none",
          sizeof(prefs->mqtt_slot_preset[5]) - 1);
#endif

  for (int i = 0; i < MQTT_PREFS_SLOT_COUNT; ++i) {
    prefs->mqtt_slot_packet_filter[i] = MQTTPacketFilter::kAllPacketTypes;
  }

  if (MQTT_DEFAULT_IATA[0] != '\0') {
    strncpy(prefs->mqtt_iata, MQTT_DEFAULT_IATA, sizeof(prefs->mqtt_iata) - 1);
    prefs->mqtt_iata[sizeof(prefs->mqtt_iata) - 1] = '\0';
  }

  if (MQTT_DEFAULT_TIMEZONE[0] != '\0') {
    strncpy(prefs->timezone_string, MQTT_DEFAULT_TIMEZONE, sizeof(prefs->timezone_string) - 1);
    prefs->timezone_string[sizeof(prefs->timezone_string) - 1] = '\0';
  }
  prefs->timezone_offset = MQTT_DEFAULT_TIMEZONE_OFFSET;

  // Observer non-MQTT defaults (moved out of NodePrefs/MyMesh ctor in Phase 2).
  strncpy(prefs->snmp_community, "public", sizeof(prefs->snmp_community) - 1);
  prefs->radio_watchdog_minutes = 5;
  prefs->alert_wifi_minutes = 30;
  prefs->alert_mqtt_minutes = 240;
  prefs->alert_min_interval_min = 60;

   // Neighbor publication.
#ifdef CORDER_FARM_CORE_PROFILE
  prefs->mqtt_neighbors_enabled = 1;
#else
  prefs->mqtt_neighbors_enabled = 0;
#endif
  prefs->mqtt_neighbors_interval = MQTT_NEIGHBORS_DEFAULT_INTERVAL_MS;
  prefs->display_timeout_secs = DISPLAY_TIMEOUT_DEFAULT_SECS;
  prefs->display_flip = 0;
}

#endif // WITH_MQTT_BRIDGE
