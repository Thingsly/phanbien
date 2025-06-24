# Luồng Hoạt Động Hệ Thống Thingsly IoT Platform

## 1. Tổng Quan Hệ Thống

Thingsly là một nền tảng hỗ trợ ứng dụng IoT mã nguồn mở, nhẹ và dựa trên các thành phần, cung cấp giải pháp toàn diện cho việc quản lý thiết bị IoT, thu thập và xử lý dữ liệu.

## 2. Các Thành Phần Chính

### 2.1 Dịch Vụ Backend
- **Máy Chủ API**: Các endpoint RESTful API cho ứng dụng client
- **Broker MQTT**: Xử lý giao tiếp thiết bị và dữ liệu thời gian thực
- **Cơ Sở Dữ Liệu**: PostgreSQL với TimescaleDB cho dữ liệu chuỗi thời gian
- **Bộ Nhớ Đệm**: Redis để tối ưu hiệu suất
- **Kiểm Soát Truy Cập**: Casbin để quản lý quyền

### 2.2 Các Module Chính
- Quản Lý Thiết Bị
- Thu Thập & Xử Lý Dữ Liệu
- Quản Lý Người Dùng & Truy Cập
- Công Cụ Quy Tắc
- Bảng Điều Khiển & Hiển Thị

## 3. Luồng Hệ Thống

### 3.1 Luồng Đăng Ký & Quản Lý Thiết Bị
1. Thiết bị kết nối đến broker MQTT với thông tin xác thực duy nhất
2. Hệ thống xác thực thiết bị bằng xác thực MQTT
3. Thiết bị gửi tin nhắn đăng ký với metadata
4. Backend tạo/cập nhật bản ghi thiết bị trong cơ sở dữ liệu
5. Hệ thống phân bổ thiết bị vào các nhóm/người thuê phù hợp
6. Thiết bị nhận cấu hình và bắt đầu hoạt động bình thường

### 3.2 Luồng Thu Thập Dữ Liệu
1. Thiết bị xuất bản dữ liệu telemetry đến các topic MQTT
2. Broker MQTT nhận và chuyển tiếp tin nhắn
3. Dịch vụ backend xử lý dữ liệu đến:
   - Xác thực định dạng và nội dung dữ liệu
   - Áp dụng các biến đổi dữ liệu nếu cần
   - Lưu trữ trong TimescaleDB cho dữ liệu chuỗi thời gian
   - Cập nhật bộ nhớ đệm Redis cho truy cập thời gian thực
4. Kích hoạt các quy tắc/cảnh báo đã cấu hình
5. Chuyển tiếp dữ liệu đến các client đã đăng ký

### 3.3 Luồng Truy Cập Người Dùng
1. Người dùng xác thực thông qua các endpoint API
2. Hệ thống xác thực thông tin đăng nhập và tạo token JWT
3. Các yêu cầu người dùng được xác thực bằng JWT
4. Casbin kiểm tra quyền cho các tài nguyên được yêu cầu
5. Nếu được phép, yêu cầu được xử lý và phản hồi được trả về

### 3.4 Luồng Công Cụ Quy Tắc
1. Hệ thống giám sát dữ liệu thiết bị đến
2. Đánh giá dữ liệu theo các quy tắc đã cấu hình
3. Khi điều kiện được đáp ứng:
   - Thực thi các hành động đã định nghĩa
   - Gửi thông báo
   - Cập nhật trạng thái thiết bị
   - Kích hoạt các quy trình

### 3.5 Luồng Truy Vấn Dữ Liệu
1. Client yêu cầu dữ liệu thông qua API
2. Hệ thống kiểm tra quyền người dùng
3. Truy vấn các nguồn dữ liệu phù hợp:
   - Redis cho dữ liệu thời gian thực
   - TimescaleDB cho dữ liệu lịch sử
4. Áp dụng bộ lọc và tổng hợp
5. Trả về phản hồi đã định dạng cho client

## 4. Kiến Trúc Luồng Dữ Liệu

### 4.1 Luồng Dữ Liệu Đến
```
Thiết Bị -> Broker MQTT -> Bộ Xử Lý Tin Nhắn -> Cơ Sở Dữ Liệu/Bộ Nhớ Đệm
```

### 4.2 Luồng Dữ Liệu Đi
```
Yêu Cầu Client -> Cổng API -> Xác Thực -> Logic Nghiệp Vụ -> Phản Hồi
```

### 4.3 Luồng Dữ Liệu Thời Gian Thực
```
Thiết Bị -> Broker MQTT -> Bộ Xử Lý Thời Gian Thực -> WebSocket -> Client
```

## 5. Luồng Bảo Mật

### 5.1 Luồng Xác Thực
1. Người dùng/Thiết bị cung cấp thông tin đăng nhập
2. Hệ thống xác thực với thông tin đăng nhập đã lưu
3. Tạo và trả về token JWT
4. Token được sử dụng cho các yêu cầu tiếp theo

### 5.2 Luồng Phân Quyền
1. Nhận yêu cầu với token JWT
2. Hệ thống xác thực token
3. Casbin kiểm tra quyền tài nguyên
4. Xử lý yêu cầu nếu được phép

## 6. Luồng Xử Lý Lỗi

1. Hệ thống phát hiện điều kiện lỗi
2. Ghi log lỗi với mức độ nghiêm trọng phù hợp
3. Tạo phản hồi lỗi với mã
4. Thông báo cho hệ thống giám sát nếu nghiêm trọng
5. Trả về lỗi đã định dạng cho client

## 7. Điểm Tích Hợp Hệ Thống

### 7.1 Hệ Thống Bên Ngoài
- Hệ Thống Quản Lý Thiết Bị
- Phân Tích Bên Thứ Ba
- Dịch Vụ Đám Mây
- Ứng Dụng Tùy Chỉnh

### 7.2 Hệ Thống Nội Bộ
- Hệ Thống Cơ Sở Dữ Liệu
- Hệ Thống Bộ Nhớ Đệm
- Hàng Đợi Tin Nhắn
- Lưu Trữ Tệp

## 8. Luồng Giám Sát & Bảo Trì

1. Các thành phần hệ thống báo cáo trạng thái hoạt động
2. Hệ thống giám sát thu thập số liệu
3. Tạo cảnh báo cho các vấn đề
4. Lên lịch các tác vụ bảo trì
5. Triển khai cập nhật hệ thống

## 9. Luồng Sao Lưu & Khôi Phục

1. Sao lưu cơ sở dữ liệu định kỳ
2. Sao lưu cấu hình
3. Chụp ảnh trạng thái hệ thống
4. Quy trình khôi phục
5. Quy trình khôi phục dữ liệu
