#include <utilities.h>

namespace veng
{

  bool streq(gsl::czstring left, gsl::czstring right)
  {
    return std::strcmp(left, right) == 0;
  }

  std::vector<std::uint8_t> ReadFile(std::filesystem::path shader_path)
  {
    if (!std::filesystem::exists(shader_path))
    {
      SPDLOG_ERROR("The file for {} does not exist" , shader_path.string());
      return NULL_STRUCT;
    };

    // Check if regular file
    if (!std::filesystem::is_regular_file(shader_path))
    {
      SPDLOG_ERROR("The path {} is not a regular file", shader_path.string());
      return NULL_STRUCT;
    }

    // Create new File Stream in binary mode
    std::ifstream file(shader_path, std::ios::binary);

    // Check if file is opened properly
    if (!file.is_open())
    {
      SPDLOG_ERROR("Failed to open file: {}", shader_path.string());
      std::exit(EXIT_FAILURE);
    }

    // Create read buffer
    std::uint32_t file_size = std::filesystem::file_size(shader_path);
    std::vector<std::uint8_t> buffer(file_size);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);

    // Return Buffer
    return buffer;
  }
}  // namespace veng
