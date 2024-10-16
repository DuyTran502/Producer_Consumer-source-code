#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Semaphore {
public:
	Semaphore(int initial) : count(initial) {}

	void Down() {
		std::unique_lock<std::mutex> lock(mtx);
		while (count <= 0) {
			cv.wait(lock); // Chờ cho đến khi có chỗ
		}
		count--; // Giảm số lượng
	}

	void Up() {
		std::unique_lock<std::mutex> lock(mtx);
		count++; // Tăng số lượng
		cv.notify_one(); // Đánh thức một tiến trình
	}

private:
	int count;
	std::mutex mtx;
	std::condition_variable cv;
};

Semaphore Full(0);   // Khởi gán Full bằng 0
Semaphore Empty(3);  // Khởi gán Empty bằng 3
Semaphore Mutex(1);  // Khởi gán Mutex bằng 1

std::queue<int> buffer; // Buffer lưu trữ dữ liệu
const int BUFFER_SIZE = 3; // Kích thước của buffer

void Producer() {
	int data;
	while (true) {
		data = rand() % 100; // Tạo dữ liệu
		Empty.Down(); // Kiểm tra xem buffer còn chỗ trống
		Mutex.Down(); // Xác lập quyền truy xuất Buffer
		buffer.push(data); // Đặt dữ liệu vào Buffer
		std::cout << "Producer da them du lieu: " << data << std::endl;
		Mutex.Up(); // Kết thúc truy xuất Buffer
		Full.Up(); // Đã có 1 phần tử dữ liệu trong Buffer
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Giả lập thời gian sản xuất
	}
}

void Consumer() {
	int data;
	while (true) {
		Full.Down(); // Còn phần tử dữ liệu trong Buffer?
		Mutex.Down(); // Xác lập quyền truy xuất Buffer
		data = buffer.front(); // Nhận dữ liệu từ Buffer
		buffer.pop(); // Xóa dữ liệu khỏi Buffer
		std::cout << "Consumer da nhan du lieu: " << data << std::endl;
		Mutex.Up(); // Kết thúc truy xuất Buffer
		Empty.Up(); // Đã lấy 1 phần tử dữ liệu trong Buffer
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Giả lập thời gian tiêu thụ
	}
}

int main() {
	std::thread producerThread(Producer);
	std::thread consumerThread(Consumer);

	producerThread.join(); // Chờ producer kết thúc
	consumerThread.join(); // Chờ consumer kết thúc

	return 0;
}