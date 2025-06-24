# MQTT Broker Topic Converter

A simplified MQTT broker focused on **topic and message conversion**, without features like message persistence, will messages, or advanced functionality, enables seamless communication between devices and IoT platforms with different topic structures by transforming topics and payloads **inside the broker**.

## 📘 Background

[MQTT](https://mqtt.org/) (Message Queuing Telemetry Transport) is a lightweight, open-source, publish-subscribe messaging protocol standardized as [ISO 20922](https://www.iso.org/standard/69466.html). It is designed for remote communication over TCP/IP where low bandwidth and small code footprint are critical.

Different IoT platforms and devices often use incompatible MQTT topic structures, leading to integration challenges. To solve this, an **external topic converter** can be used to translate between device and platform topics and payloads.

## ✅ Benefits of an External Converter

- **Flexibility**: Easily swap or upgrade the converter without depending on a specific platform.
- **Performance**: Offloading conversion logic reduces processing overhead on the IoT platform, especially under high message throughput.
- **Maintainability**: Upgrade or patch the converter independently of the platform.
- **Scalability**: Deploy different or custom converters for various devices or systems.
- **Simplified Platform Architecture**: Delegating message transformation externally reduces complexity within the core platform.
- **Adaptability**: Overcome limitations of built-in platform transformation capabilities.
- **Easy Integration**: Works as a standalone service alongside existing infrastructure.
- **Multi-Platform Compatibility**: Use the same converter for multiple platforms or services.
- **Custom Logic**: Tailor transformation logic to business-specific needs.

## ⚠️ Drawbacks of Traditional External Converters

Traditional converters often use an intermediary broker and a conversion client (e.g., Node-RED or rule engines). Potential downsides include:

- **Deployment Complexity**: Requires deploying and managing an additional broker and conversion logic.
- **Increased Latency**: Extra network hops add delay (Device → Converter → Broker → Platform).
- **Maintenance Overhead**: More components mean higher operational burden.
- **Online Status Detection**: Intermediate brokers may lack device connection state awareness.

## 🔁 Internal Broker-Based Converter

Implements an MQTT broker that **natively performs topic and payload conversion**. It avoids the downsides of client-based converters while preserving their flexibility.

### 🔧 Features

- Full MQTT 3.1.1 support: connect, keep-alive, publish, subscribe, disconnect.
- Internal client auto-subscribes to IoT platform topics based on conversion rules.
- Automatically transforms topics/payloads for messages published by devices/platforms.
- Supports device connection status tracking (online/offline).
- Shared conversion rules for devices of the same type.

## ⚙️ Configuration

Create a `config.yaml` file to define topic transformation rules.

```yaml
# Rewrite rules for publishing topics (device → platform)
pub_topic_rewriting:
  - from: "/sys/+/+/thing/event/consumption/post"
    to: "device/attributes"

  - from: "device/+/event"
    to: "device/event"

# Rewrite rules for subscription topics (device ← platform)
sub_topic_rewriting:
  - from: "/+/+/user/get"
    to: "device/attributes/{username}"

  - from: "device/sub/+/event"
    to: "device/event/{username}"
```

Use `{}` placeholders for dynamic topic segments.

---

## 🧩 Payload Conversion

Payload transformation logic is defined in [`payload_hook.go`](payload_hook.go). Use a `switch` statement to match conversion rules defined in `config.yaml`, and write custom transformation logic for each case.

Example:

```go
switch fromTopic {
case "/sys/+/+/thing/event/consumption/post":
    // Transform JSON structure here
}
```
