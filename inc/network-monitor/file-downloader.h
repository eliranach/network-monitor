#include <filesystem>
#include <string>
#include <json/json.h>

namespace NetworkMonitor {

/*! \brief Download a file from a remote HTTPS URL.
 *
 *  \param destination The full path and filename of the output file. The path
 *                     to the file must exist.
 *  \param caCertFile  The path to a cacert.pem file to perform certificate
 *                     verification in an HTTPS connection.
 */
bool DownloadFile(
    const std::string& fileUrl,
    const std::filesystem::path& destination,
    const std::filesystem::path& caCertFile = {}
);



/*! \brief Save string content into a file.
 *
 *  \param content the string to be saved
 *  \param destination The full path and filename of the output file. 
 */
bool SaveTextInFile(
    const std::string& content,
    const std::filesystem::path& destination
);


/*! \brief Parse a local file into a JSON object.
 *
 *  \param source The path to the JSON file to load and parse.
 */
Json::Value ParseJsonFile(
    const std::filesystem::path& source
);


} // namespace NetworkMonitor