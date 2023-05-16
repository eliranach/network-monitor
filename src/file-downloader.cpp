#include <filesystem>
#include <string>
#include <network-monitor/file-downloader.h>
#include <cpr/cpr.h>


bool NetworkMonitor::SaveTextInFile(
    const std::string& content,
    const std::filesystem::path& destination
){

    std::ofstream out(destination);
    out << content;
    out.close();
    return true;
}


bool NetworkMonitor::DownloadFile(
    const std::string& fileUrl,
    const std::filesystem::path& destination,
    const std::filesystem::path& caCertFile
){
    bool ans {true};
    cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::TLSv1_2{});
    cpr::Response r = cpr::Get(cpr::Url{fileUrl}, sslOpts);
    ans = ans && r.status_code == 200 && r.header["content-type"] == "application/json";
    ans = ans && SaveTextInFile(r.text, destination);
    return ans;
}

