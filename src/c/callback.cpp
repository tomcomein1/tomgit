#include <iostream>
#include <string>

using namespace std;

//封装
typedef void (*DownloadHandler)(const string& url, unsigned ec);

//回调函数
void doDownloadJob(const string& url, DownloadHandler pHandler) {
    // be busy doing sth. with downloading
    if (pHandler != NULL) {
        pHandler(url, 0);
    }
}

void onDownloadComplete(const string& url, unsigned ec) {
    cout << "file " << url << " finished downloading, ec = " << ec << endl;
}

int main() {
    doDownloadJob("http://yy.com/music/spring.mp3", onDownloadComplete);
}

