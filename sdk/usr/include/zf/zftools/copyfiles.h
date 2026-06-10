#ifndef COPYFILES_H
#define COPYFILES_H

#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>

namespace zftools {
enum class FileSystemError
{
    OK,
    IOError,
    Canceled,
    Unknown
};
class PrjPath {
public:
    PrjPath(std::string prjName) : projectName(prjName), copied(false) {}
    std::string projectName;
    std::string source = "";
    std::string destination = "";
    bool exists = false;
    bool copied = false;
};

class CopyFiles {
public:
    static CopyFiles& Instance() {
        static CopyFiles instance;
        return instance;
    }
    
    void Start(std::vector<PrjPath> paths) {
        std::lock_guard<std::mutex> lock(threadMutex);
        if (running) return;
        if (copyThread.joinable()) {
            copyThread.join();
        }
        error = FileSystemError::OK;
        running = true;
        cancelFlag = false;
        projectPaths = std::move(paths);
        totalCopied = 0;
        computeTotalSize();
        copyThread = std::thread(&CopyFiles::copyBatch, this);
    }

    void Cancel() {
        error = FileSystemError::Canceled;
        Stop();
    }
    
    void Stop() {
        std::lock_guard<std::mutex> lock(threadMutex);
        if (!running) return;
        cancelFlag = true;
        if (copyThread.joinable()) copyThread.join();
        running = false;
        copyThread = std::thread();
    }
    
    bool isRunning() {
        std::lock_guard<std::mutex> lock(threadMutex);
        return running;
    }
    
    int getProgress() {
        if (cancelFlag || (error != FileSystemError::OK)) return -1;
        if (totalCopied > totalSize) 
        {
            totalCopied = static_cast<unsigned long>(totalSize);
            error = FileSystemError::Unknown;
        }        
        return totalSize == 0 ? 100 : static_cast<int>((100.0 * totalCopied) / totalSize);
    }

    FileSystemError getErrorCode() {
        std::lock_guard<std::mutex> lock(threadMutex);
        return error;
    }

    std::string getErrorMessage() {
        return errorMessage.empty() ? "Copy process failed" : errorMessage;
    }
    
    std::string getProgressInformation() {
        if (cancelFlag) return getErrorMessage();
        auto elapsedTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
        double speed = elapsedTime > 0 ? (totalCopied / (1024.0 * 1024.0)) / elapsedTime : 0;
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << speed;
        return stream.str() + " MB/s";
    }
    
private:
    CopyFiles() : cancelFlag(false), running(false), totalCopied(0), totalSize(0), overwrite(true), errorMessage("") {}
    ~CopyFiles() { Stop(); }
    CopyFiles(const CopyFiles&) = delete;
    CopyFiles& operator=(const CopyFiles&) = delete;
    
    // error codes during copy
    FileSystemError error = FileSystemError::OK;

    void copyBatch() {
        startTime = std::chrono::steady_clock::now();
        for (auto& path : projectPaths) {
            if (cancelFlag) {
                running = false;
                error = FileSystemError::Canceled;
                return;
            }
            if (path.source.empty() || path.destination.empty() || (path.exists && !overwrite)) {
                path.copied = false;
                continue;
            }
            std::filesystem::path src = path.source;
            std::filesystem::path dest = path.destination;
            try
            {
                if (!std::filesystem::exists(dest.parent_path())) {
                    std::filesystem::create_directories(dest.parent_path());
                }
                bool success = false;
                if (std::filesystem::is_directory(src)) success = copyDirectory(src, dest);
                else success = copyFile(src, dest);
                path.exists = success || std::filesystem::exists(dest);
                path.copied = true;
            }
            catch (std::exception& e)
            {
                errorMessage = "Create directory error: " + std::string(e.what());
                error = FileSystemError::IOError;
                cancelFlag = true;
            }
        }
        running = false;
    }
    
    bool copyFile(const std::filesystem::path& src, const std::filesystem::path& dest, std::size_t bufferSize = 4 * 1024 * 1024) {
        try {
            if (!std::filesystem::exists(src) || !std::filesystem::is_regular_file(src)) return false;
            if (std::filesystem::exists(dest) && !overwrite) return true;
            std::ifstream inFile(src, std::ios::binary);
            std::ofstream outFile(dest, std::ios::binary);
            if (!inFile || !outFile) return false;
            std::vector<char> buffer(bufferSize);
            while (!cancelFlag) {
                inFile.read(buffer.data(), buffer.size());
                std::streamsize bytesRead = inFile.gcount();
                if (bytesRead <= 0) break;
                outFile.write(buffer.data(), bytesRead);
                totalCopied += bytesRead;
            }
            return !cancelFlag;
        } catch (const std::exception& e) {
            errorMessage = "File copy error: " + std::string(e.what());
            error = FileSystemError::IOError;
            cancelFlag = true;
            return false;
        }
    }
    
    bool copyDirectory(const std::filesystem::path& src, const std::filesystem::path& dest) {
        try {
            if (!std::filesystem::exists(src) || !std::filesystem::is_directory(src)) return false;
            std::filesystem::create_directories(dest);
            for (const auto& entry : std::filesystem::recursive_directory_iterator(src)) {
                if (cancelFlag) return false;
                auto destPath = dest / std::filesystem::relative(entry.path(), src);
                if (entry.is_directory()) std::filesystem::create_directories(destPath);
                else if (entry.is_regular_file()) {
                    if (!copyFile(entry.path(), destPath)) return false;
                }
            }
            return !cancelFlag;
        } catch (const std::exception& e) {
            errorMessage = "Directory copy error: " + std::string(e.what());
            error = FileSystemError::IOError;
            cancelFlag = true;
            return false;
        }
    }
    
    void computeTotalSize() {
        totalSize = 0;
        for (const auto& path : projectPaths) {
            std::filesystem::path src = path.source;
            if (std::filesystem::is_directory(src)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(src)) {
                    if (entry.is_regular_file()) totalSize += std::filesystem::file_size(entry.path());
                }
            } else if (std::filesystem::is_regular_file(src)) {
                totalSize += std::filesystem::file_size(src);
            }
        }
    }
    
    std::atomic<bool> cancelFlag;
    std::atomic<bool> running;
    std::atomic<std::size_t> totalCopied, totalSize;
    std::chrono::steady_clock::time_point startTime;
    bool overwrite;
    mutable std::string errorMessage;
    std::thread copyThread;
    std::mutex threadMutex;
    std::vector<PrjPath> projectPaths;
};
}  // namespace zftools

#endif // COPYFILES_H
