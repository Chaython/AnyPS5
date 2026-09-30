#include <io/FileWriter.hpp>
#include <domain/Types.hpp>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace Io {

namespace {

std::string ErrorSuffix(const int error) {
    if (error == 0) return {};
    return " (" + std::error_code(error, std::generic_category()).message() +
           "; errno=" + std::to_string(error) + ")";
}

void RemovePartialRegularFile(const std::string& path) {
    std::error_code error;
    if (std::filesystem::is_regular_file(path, error))
        std::filesystem::remove(path, error);
}

void CheckFreeSpace(const std::string& path, const std::uintmax_t newSize) {
    const std::filesystem::path destination(path);
    std::error_code error;
    auto parent = destination.parent_path();
    if (parent.empty())
        parent = std::filesystem::current_path(error);
    if (error || parent.empty()) return;

    std::uintmax_t reusable = 0;
    error.clear();
    if (std::filesystem::is_regular_file(destination, error)) {
        error.clear();
        reusable = std::filesystem::file_size(destination, error);
        if (error) reusable = 0;
    }

    error.clear();
    const auto space = std::filesystem::space(parent, error);
    if (error || space.available == static_cast<std::uintmax_t>(-1)) return;

    const auto requiredAdditional = newSize > reusable ? newSize - reusable : 0;
    if (requiredAdditional > space.available) {
        throw Domain::RelinkerException(
            "Insufficient free space for output file: " + path +
            " (need " + std::to_string(requiredAdditional) +
            " additional bytes; " + std::to_string(space.available) + " available)");
    }
}

template<typename TWriter>
void WriteChecked(const std::string& path, const std::uintmax_t expectedSize, TWriter writer, const bool binary) {
    CheckFreeSpace(path, expectedSize);

    errno = 0;
    std::ofstream file(path, (binary ? std::ios::binary : std::ios::openmode{}) | std::ios::trunc);
    if (!file) {
        const int error = errno;
        throw Domain::RelinkerException("Cannot open output file: " + path + ErrorSuffix(error));
    }

    errno = 0;
    writer(file);
    if (!file) {
        const int error = errno;
        file.close();
        RemovePartialRegularFile(path);
        throw Domain::RelinkerException("Failed to write file: " + path + ErrorSuffix(error));
    }

    errno = 0;
    file.flush();
    if (!file) {
        const int error = errno;
        file.close();
        RemovePartialRegularFile(path);
        throw Domain::RelinkerException("Failed to flush file: " + path + ErrorSuffix(error));
    }

    errno = 0;
    file.close();
    if (!file) {
        const int error = errno;
        RemovePartialRegularFile(path);
        throw Domain::RelinkerException("Failed to close file: " + path + ErrorSuffix(error));
    }
}

}

void FileWriter::Write(const std::string& path, const std::vector<std::uint8_t>& data) {
    WriteChecked(path, data.size(), [&](std::ofstream& file) {
        file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    }, true);
}

void FileWriter::Write(const std::string& path, const std::string& content) {
    WriteChecked(path, content.size(), [&](std::ofstream& file) {
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
    }, false);
}

}
