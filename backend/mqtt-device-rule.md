# Direct Device MQTT Data Interaction Specification

## Overview

This specification outlines the technical requirements for directly connected devices using the MQTT protocol to interact with the Thingsly IoT platform. It defines the data exchange formats and communication protocols between devices and the platform.

## Core Concepts

Four Core Data Types
Understanding the following four core concepts is essential:

* **🔄 Telemetry** – Real-time data reported by devices, typically time-series measurements

  * Example: Periodic temperature readings from a sensor
* **📋 Attributes** – Static or infrequently changing device properties

  * Example: Device IP address, MAC address, or firmware version
* **🎯 Events** – Specific incidents or state changes detected by the device

  * Example: Motion detection or device startup
* **⚡ Commands** – Control instructions sent from the platform to the device

  * Example: Turning on a light or rebooting the device

## Key Parameters

### 📍 `message_id` (Message Identifier)

* **Purpose**: Uniquely identifies each message to avoid duplication
* **Recommendation**: Use the last 7 digits of the millisecond timestamp
* **Requirement**: Must remain unique within a short period

### 🏷️ `device_number` (Device Number)

* **Purpose**: Uniquely identifies a device within the system
* **Requirement**: Must be globally unique and non-repetitive

### 🎭 `method` (Method Identifier)

* **Definition**: String used to specify command or event types
* **Command Examples**: `"SetTemperature"`, `"TurnOnLight"`, `"RebootDevice"`
* **Event Examples**: `"TemperatureExceeded"`, `"MotionDetected"`, `"BatteryLow"`

### ⚙️ `params` (Parameter Object)

* **Purpose**: Contains detailed information related to the method
* **Command Scenario**: Parameters required to execute a command
* **Event Scenario**: Data describing the event

## MQTT Authentication Rules

Authentication Requirements

### 🔐 Uniqueness Requirements

* **Username + Password** combination must be globally unique
* **ClientID** must be globally unique

### 🔄 Consistency Requirements

* Devices must always connect with the same authentication information
* Maintain consistency in ClientID, Username, and Password

## MQTT Topic Specification

Flexible Implementation

**Note**: Devices are not required to implement all listed MQTT topics. Select topics based on actual device functionality and application scenarios. Only implement necessary topics to optimize performance and resource usage.


### Device Publish Topics

| Topic                                          | Description                | Data Type  | Required |
| ---------------------------------------------- | -------------------------- | ---------- | -------- |
| `devices/telemetry`                            | Report telemetry data      | Telemetry  | Optional |
| `devices/attributes/{message_id}`              | Report attribute status    | Attributes | Optional |
| `devices/event/{message_id}`                   | Report event information   | Events     | Optional |
| `devices/command/response/{message_id}`        | Command execution response | Response   | Optional |
| `devices/attributes/set/response/{message_id}` | Attribute setting response | Response   | Optional |

### Device Subscribe Topics

Topic Notation

**Note**: `+` denotes a wildcard for `message_id`, `{device_number}` represents a specific device identifier

| Topic                                           | Description                             | Data Type             | Required |
| ----------------------------------------------- | --------------------------------------- | --------------------- | -------- |
| `devices/telemetry/control/{device_number}`     | Receive telemetry control commands      | Control               | Optional |
| `devices/attributes/set/{device_number}/+`      | Receive attribute setting commands      | Attribute Setting     | Optional |
| `devices/attributes/get/{device_number}`        | Receive attribute query requests        | Attribute Query       | Optional |
| `devices/command/{device_number}/+`             | Receive command execution requests      | Commands              | Optional |
| `devices/attributes/response/{device_number}/+` | Receive attribute response confirmation | Response Confirmation | Optional |
| `devices/event/response/{device_number}/+`      | Receive event response confirmation     | Response Confirmation | Optional |

## Data Interaction Formats

### 🔄 Telemetry Data Reporting

**Topic:** `devices/telemetry`

```json title="Telemetry Data Example"
{
  "temperature": 28.5,
  "switch": true
}
```

### 📋 Attribute Data Reporting

**Topic:** `devices/attributes/{message_id}`

```json title="Attribute Data Example"
{
  "ip": "127.0.0.1",
  "mac": "xxxxxxxxxx",
  "port": 1883
}
```

### 🎯 Event Data Reporting

**Topic:** `devices/event/{message_id}`

```json title="Event Data Example"
{
  "method": "FindAnimal",
  "params": {
    "count": 2,
    "animalType": "cat"
  }
}
```

### ⚡ Receiving Control Commands

**Topic:** `devices/telemetry/control/{device_number}`

```json title="Control Command Example"
{
  "temperature": 28.5,
  "light": 2000,
  "switch": true
}
```

### 📋 Receiving Attribute Setting

**Topic:** `devices/attributes/set/{device_number}/+`

```json title="Attribute Setting Example"
{
  "ip": "127.0.0.1",
  "mac": "xxxxxxxxxx",
  "port": 1883
}
```

### 🔍 Receiving Attribute Query Requests

**Topic:** `devices/attributes/get/{device_number}`

**Query All:**

```json title="Query All Attributes"
{
  "keys": []
}
```

**Query Specific Keys:**

```json title="Query Specific Attributes"
{
  "keys": ["temp", "hum"]
}
```

### ⚡ Receiving Command Execution Requests

**Topic:** `devices/command/{device_number}/+`

```json title="Command Execution Example"
{
  "method": "ReSet",
  "params": {
    "switch": 1,
    "light": "close"
  }
}
```

## Response Format Specification

### Response Parameter Descriptions

| Parameter | Required | Type   | Description                                |
| --------- | -------- | ------ | ------------------------------------------ |
| `result`  | ✅        | number | Execution result: `0`-success, `1`-failure |
| `errcode` | ❌        | string | Error code (provided on failure)           |
| `message` | ✅        | string | Message content                            |
| `ts`      | ❌        | number | Timestamp (in seconds)                     |
| `method`  | ❌        | string | Method name for events and commands        |

### Response Format Examples

**Success Response:**

```json title="Success"
{
  "result": 0,
  "message": "success",
  "ts": 1609143039
}
```

**Failure Response:**

```json title="Failure"
{
  "result": 1,
  "errcode": "INVALID_PARAM",
  "message": "Parameter validation failed",
  "ts": 1609143039,
  "method": "ReSet"
}
```

**Success Response with Method:**

```json title="Command Success"
{
  "result": 0,
  "message": "Command executed successfully",
  "ts": 1609143039,
  "method": "ReSet"
}
```

## Best Practices

Development Recommendations

1. **Message ID Management** – Use timestamp suffixes to ensure short-term uniqueness
2. **Selective Implementation** – Implement only necessary topics based on device requirements
3. **Authentication Consistency** – Reuse the same credentials for stable connections
4. **Error Handling** – Implement complete error responses for diagnostics
5. **Data Validation** – Ensure valid JSON structure before publishing
6. **Connection Keep-Alive** – Use MQTT keep-alive mechanisms
7. **Reconnection Logic** – Implement auto-reconnect on network failures

Important Notes

* Ensure device numbers are globally unique
* Periodically report telemetry to avoid data loss
* Send confirmation responses after executing commands
* Validate attribute parameters before applying settings
* Maintain consistent authentication information to avoid connection issues
* Subscribe only to necessary topics to reduce network traffic
