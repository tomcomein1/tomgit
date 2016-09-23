#include <iostream>
#include <string>

using namespace std;

class Client;

typedef void (Client::*ClientMemFun)(const string&, unsigned);

class Downloader {
public:
    void doDownloadJob(const string& url, Client* clientOwner, ClientMemFun pHandler) {
        // be busy doing sth. with downloading
        if ((clientOwner != NULL) && (pHandler != NULL)) {
            (clientOwner->*pHandler)(url, 0);
        }
    }
};

class Client {
public:
    void startDownload(const string& url) {
        cout << "start to download file " << url << endl;
        m_downloader.doDownloadJob(url, this, &Client::onDownloadComplete);
    }
    void onDownloadComplete(const string& url, unsigned ec) {
        cout << "file " << url << " finished downloading, ec = " << ec << endl;
    }
private:
    Downloader m_downloader;
};

int main() {
    Client c;
    c.startDownload("http://yy.com/music/spring.mp3");
}

