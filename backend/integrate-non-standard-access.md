# Integrating Non-Standard MQTT Devices into Thingsly

## Problem Statement

1. **Incompatible MQTT Topics**: The MQTT device/gateway uses fixed publish and subscribe topics that do not match the Thingsly platform and cannot be modified. How can it be integrated?

2. **Incompatible JSON Payload Structure**: The MQTT device/gateway sends JSON payloads that differ from the platform's expected format. How can it be integrated into Thingsly?

## Solution 1: Using the MQTT Topic & Payload Converter

This tool resolves both topic and payload conversion issues. It is an external platform-level converter that includes a simplified MQTT broker and handles topic and payload conversion internally.

**Data Flow**: Device → MQTT Topic & Payload Converter → Thingsly GMQTT Broker → Thingsly Platform.

## Solution 2: Forwarding Data via Rules Engine

This method is suitable for resolving incompatible topic structures.

1. Create a new rule in the rules engine.
2. Drag an `mqtt in` node to subscribe to device data topics (note: use a privately deployed MQTT broker, not the Thingsly's built-in one).

   Data flow: Device → Private MQTT Broker → Rules Engine → Thingsly GMQTT Broker → Thingsly Platform.
3. Drag an `mqtt out` node and connect it to the `mqtt in` node.
4. Configure the `mqtt out` node by adding a new MQTT broker. Fill in the Thingsly topic and authentication method:

   * Use `username` for AccessToken-based auth.
   * Use `username` and `password` for MQTTBasic auth.
5. Reverse communication from platform to device follows the same configuration process.
6. You can also use additional function nodes to process payload structure and values.

## Solution 3: Use "Data Processing Script" in Device Parameter Settings

This solves issues with incompatible JSON payloads.

1. After device creation, go to parameter editing and select `Custom Protocol` under "Data Processing".
2. In the popup dialog, input "Uplink Parsing Script" and "Downlink Parsing Script".
3. For similar devices, reuse the saved parsing script.
4. The data processing script can also transform values transmitted by devices.

### Example

**Standard Payload Expected by the Platform:**

```json
{
  "TH180001": {
    "temp": 26.5,
    "hum": 45.5
  },
  "TH180002": {
    "temp": 26.5,
    "hum": 45.5
  }
}
```

**Non-Standard Payload from Device:**

```json
{
  "sensorDatas": {
    "TH180001": {
      "temp": 265,
      "hum": 455
    },
    "TH180002": {
      "temp": 265,
      "hum": 455
    }
  },
  "type": 2
}
```

**Steps to Handle Conversion:**

1. Identify differences: The device payload has an extra nested level and values need scaling.
2. Create a gateway device and register two sub-devices (set unique addresses for each).

**Sample Uplink Parsing Script:**

```javascript
// Debug Instructions: To test in a Node.js environment,
// save this file into a directory and run `node ThingsPanelScriptTestDemo.js` in that directory.

function encodeInp(msg, topic) {
    // Converts a device-specific message (in a custom format) into a JSON-formatted string.
    // This function is called when a device reports data to the IoT platform.

    // Parameters:
    // - topic: string representing the MQTT topic used by the device to report data.
    // - msg: byte[] (Uint8Array), must not be null or empty.

    // Returns:
    // - A stringified JSON object after transformation.

    // Conversion steps:
    // Example: convert byte array to string -> var msgString = String.fromCharCode.apply(null, msg);
    // Example: parse string to JSON -> var msgJson = JSON.parse(msgString);
    // Example: stringify JSON -> var msgString = JSON.stringify(msgJson);

    var msgString = String.fromCharCode.apply(null, msg);
    var jsonObj = JSON.parse(msgString);
    var newObj = jsonObj.sensorDatas;

    for (var key in newObj) {
        for (var k in newObj[key]) {
            newObj[key][k] = newObj[key][k] / 10; // Normalize the values
        }
    }

    msg = JSON.stringify(newObj); // Return the transformed message
    return msg;
}

// Convert a string to a byte array
const decodedMsg = new Uint8Array([...'{"sensorDatas":{"TH180001":{"temp":265,"hum":455},"TH180002":{"temp":26.5,"hum":45.5}},"type":2}'].map(c => c.charCodeAt(0)));
console.log(decodedMsg);

// Debugging logic
const topic = "gateway/attributes";
const encodedMsg = encodeInp(decodedMsg, topic);

// Output: {"TH180001":{"temp":26.5,"hum":45.5},"TH180002":{"temp":2.65,"hum":4.55}}
// NOTE: The final output string must not contain escaped characters (e.g. `\"`),
// or the platform will fail to parse the message.
console.log(encodedMsg);
```

1. Use online JS debugger like: [https://www.playcode.io](https://www.playcode.io)

2. Once validated, paste the function into the platform's script editor in the device parameters section.

3. Use similar logic for downlink parsing scripts.
