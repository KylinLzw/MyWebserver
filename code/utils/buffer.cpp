#include "buffer.h"

// 初始化缓冲区，读位置和写位置
Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

// 可以读的字节数
size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}

// 可以写的字节数
size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}

// 上一次读停止的位置
size_t Buffer::PrependableBytes() const {
    return readPos_;
}

// 当前读到的位置的指针
const char* Buffer::Peek() const {
    return BeginPtr_() + readPos_;
}

// 读取的长度
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos_ += len;
}

// 剩下的可以读的空间
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end );
    Retrieve(end - Peek());
}

// 清空缓冲区
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

// 把缓冲区数据转化为字符串返回
std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

// 写开始的位置
const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writePos_;
}

// 写开始的位置
char* Buffer::BeginWrite() {
    return BeginPtr_() + writePos_;
}

// 写数据后的位置
void Buffer::HasWritten(size_t len) {
    writePos_ += len;
} 

// 写入数据
void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}
void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

// 保证写缓冲区足够
void Buffer::EnsureWriteable(size_t len) {
    if(WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

// 读取文件
ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = WritableBytes();
    /* 分散读 readv（集中写：writev， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    }
    else {
        writePos_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

// 往文件中写
ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;
    return len;
}

// 开始指针
char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}
const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

// 缓冲区扩充
void Buffer::MakeSpace_(size_t len) {
    if(WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(writePos_ + len + 1);
    } 
    else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}