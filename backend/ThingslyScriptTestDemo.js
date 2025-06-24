// Debug Instructions: To test in a Node.js environment,
// save this file into a directory and run `node ThingslyScriptTestDemo.js` in that directory.

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
// console.log(decodedMsg);

// Debugging logic
const topic = "gateway/attributes";
const encodedMsg = encodeInp(decodedMsg, topic);

// Output: {"TH180001":{"temp":26.5,"hum":45.5},"TH180002":{"temp":2.65,"hum":4.55}}
// NOTE: The final output string must not contain escaped characters (e.g. `\"`),
// or the platform will fail to parse the message.
console.log(encodedMsg);
