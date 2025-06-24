# IoT Backend System Data Flow

## 1. Main Components

- **Client (Frontend/Device):**
  - Sends HTTP requests or connects via WebSocket to interact with the backend.
- **Router:**
  - Routes requests to the appropriate handler.
- **Middleware (CORS, Auth, Log, APIKey):**
  - Filters, authenticates, and logs requests before they reach the controller.
- **API/Controller:**
  - Handles requests, validates input, and calls service/logic.
- **Service/Logic:**
  - Processes business logic, normalizes data, and calls DAL.
- **DAL/Repository:**
  - Queries and writes data to the database.
- **Database/External Service:**
  - Stores data or connects to external services (PostgreSQL, Redis, MQTT, ...).
- **Cache (Redis):**
  - Temporarily stores data to speed up queries (sessions, frequently used data, locks, ...).
- **WebSocket Gateway:**
  - Handles WebSocket connections for real-time data, receives subscriptions from clients, and pushes real-time data.
- **MQTT Broker:**
  - Receives data from devices and forwards it to backend subscribers.

---

## 2. Main Data Flow (Solid Line)

### HTTP Request Flow

1. **Client** sends an HTTP request (e.g., GET /api/v1/device/list).
2. **Router** receives and routes the request.
3. **Middleware** checks CORS, authentication, and logs the request.
4. **Controller** receives the request, validates input, and calls the service.
5. **Service/Logic** processes business logic and normalizes data.
6. **DAL** queries the database.
7. **Database** returns data.

### Response Flow (Dashed Line)

- Data returns from the database, passing back through DAL, Service/Logic, Controller, Middleware, Router, and finally to the Client.

---

## 3. Auxiliary Flows

### Cache Flow

- **Service/Logic** and **DAL** can read/write cache to speed up access and reduce database load.

### WebSocket Flow

- **Client** connects to **WebSocket Gateway** for real-time data.
- **WebSocket Gateway** receives subscriptions and pushes real-time data from **Service/Logic** to the client.

---

## 4. Real-time Data Flow from Device to Backend

### 4.1. Data Flow Diagram

```
Device (Sensor) → MQTT Broker → Backend (MQTT Subscriber) → Service/Logic
    ├─> Save to Database
    ├─> Write to Cache (if needed)
    └─> Push real-time data via WebSocket Gateway → Client (Frontend)
```

### 4.2. Step-by-step Description

1. **Device (Sensor):**
   - Sends data (e.g., temperature, humidity, status, ...) to the broker via MQTT.
   - Data is published to a specific topic, e.g., `devices/telemetry/{device_id}`.
2. **MQTT Broker:**
   - Receives data from the device.
   - Acts as an intermediary, ensuring data is forwarded to backend subscribers.
3. **Backend Service (MQTT Subscriber):**
   - Subscribes to necessary topics on the broker.
   - Receives messages from the broker when new data arrives.
4. **Service/Logic:**
   - Processes, validates, and normalizes the received data.
   - May perform actions such as:
     - Saving data to the database (for history, analytics, ...).
     - Writing to cache for fast queries.
     - Checking alert conditions, triggering automations, etc.
   - If there are clients subscribed via WebSocket, pushes real-time data to them via the WebSocket Gateway.
5. **Client (Frontend):**
   - Receives and displays real-time data immediately (dashboard, alerts, ...).

### 4.3. Example

- A temperature sensor sends a new value to the topic `devices/telemetry/device-id-123`.
- The MQTT Broker receives the message and forwards it to the backend subscriber.
- The backend saves the data to the database, checks for any active dashboard clients, and pushes the data via WebSocket if needed.
- The frontend receives the new data and updates the UI in real-time.

---
