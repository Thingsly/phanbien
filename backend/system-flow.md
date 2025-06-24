# Thingsly IoT Platform System Flow

## 1. System Overview

Thingsly is a lightweight, component-based open-source IoT application support platform that provides a complete solution for IoT device management, data collection, and processing.

## 2. Core Components

### 2.1 Backend Services
- **API Server**: RESTful API endpoints for client applications
- **MQTT Broker**: Handles device communication and real-time data
- **Database**: PostgreSQL with TimescaleDB for time-series data
- **Cache**: Redis for performance optimization
- **Access Control**: Casbin for permission management

### 2.2 Main Modules
- Device Management
- Data Collection & Processing
- User & Access Management
- Rule Engine
- Dashboard & Visualization

## 3. System Flow

### 3.1 Device Registration & Management Flow
1. Device connects to MQTT broker with unique credentials
2. System authenticates device using MQTT authentication
3. Device sends registration message with metadata
4. Backend creates/updates device record in database
5. System assigns device to appropriate groups/tenants
6. Device receives configuration and starts normal operation

### 3.2 Data Collection Flow
1. Device publishes telemetry data to MQTT topics
2. MQTT broker receives and forwards messages
3. Backend services process incoming data:
   - Validate data format and content
   - Apply data transformations if needed
   - Store in TimescaleDB for time-series data
   - Update Redis cache for real-time access
4. Trigger any configured rules/alerts
5. Forward data to subscribed clients

### 3.3 User Access Flow
1. User authenticates through API endpoints
2. System validates credentials and generates JWT token
3. User requests are authenticated using JWT
4. Casbin checks permissions for requested resources
5. If authorized, request is processed and response returned

### 3.4 Rule Engine Flow
1. System monitors incoming device data
2. Evaluates data against configured rules
3. When conditions are met:
   - Executes defined actions
   - Sends notifications
   - Updates device states
   - Triggers workflows

### 3.5 Data Query Flow
1. Client requests data through API
2. System checks user permissions
3. Queries appropriate data sources:
   - Redis for real-time data
   - TimescaleDB for historical data
4. Applies filters and aggregations
5. Returns formatted response to client

## 4. Data Flow Architecture

### 4.1 Inbound Data Flow
```
Device -> MQTT Broker -> Message Processor -> Database/Cache
```

### 4.2 Outbound Data Flow
```
Client Request -> API Gateway -> Authentication -> Business Logic -> Response
```

### 4.3 Real-time Data Flow
```
Device -> MQTT Broker -> Real-time Processor -> WebSocket -> Client
```

## 5. Security Flow

### 5.1 Authentication Flow
1. User/Device provides credentials
2. System validates against stored credentials
3. Generates and returns JWT token
4. Token used for subsequent requests

### 5.2 Authorization Flow
1. Request received with JWT token
2. System validates token
3. Casbin checks resource permissions
4. Request processed if authorized

## 6. Error Handling Flow

1. System detects error condition
2. Logs error with appropriate severity
3. Generates error response with code
4. Notifies monitoring system if critical
5. Returns formatted error to client

## 7. System Integration Points

### 7.1 External Systems
- Device Management Systems
- Third-party Analytics
- Cloud Services
- Custom Applications

### 7.2 Internal Systems
- Database Systems
- Cache Systems
- Message Queues
- File Storage

## 8. Monitoring & Maintenance Flow

1. System components report health status
2. Monitoring system collects metrics
3. Alerts generated for issues
4. Maintenance tasks scheduled
5. System updates deployed

## 9. Backup & Recovery Flow

1. Regular database backups
2. Configuration backups
3. System state snapshots
4. Recovery procedures
5. Data restoration process
