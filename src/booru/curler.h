#ifndef _CURLER_H_
#define _CURLER_H_

#include <curl/curl.h>
#include <glibmm.h>
#include <giomm.h>
#include <atomic>
#include <chrono>

namespace AhoViewer
{
    namespace Booru
    {
        using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

        class Curler
        {
            friend class ImageFetcher;

            using SignalWriteType = sigc::signal<void, const unsigned char*, size_t>;
        public:
            Curler(const std::string &url = "", CURLSH *share = nullptr);
            ~Curler();

            void set_url(const std::string &url);
            void set_no_body(const bool n = true) const;
            void set_follow_location(const bool n = true) const;
            void set_referer(const std::string &url) const;
            void set_http_auth(const std::string &u, const std::string &p) const;
            void set_cookie_jar(const std::string &path) const;
            void set_cookie_file(const std::string &path) const;
            void set_post_fields(const std::string &fields) const;
            void set_share_handle(CURLSH *s) const;

            std::string escape(const std::string &str) const;
            bool perform();

            void clear() { m_Buffer.clear(); }
            void save_file(const std::string &path) const;

            void get_progress(double &current, double &total);

            bool is_active() const { return m_Active; }
            std::string get_url() const { return m_Url; }

            unsigned char* get_data() { return m_Buffer.data(); }
            size_t get_data_size() const { return m_Buffer.size(); }

            std::string get_error() const  { return curl_easy_strerror(m_Response); }
            CURLcode get_response() const { return m_Response; }
            // HTTP reponse code
            long get_response_code() const;
            time_point_t get_start_time() const { return m_StartTime; }

            void cancel() { m_Cancel->cancel(); }
            bool is_cancelled() const { return m_Cancel->is_cancelled(); }

            SignalWriteType signal_write() const { return m_SignalWrite; }
            Glib::Dispatcher& signal_progress() { return m_SignalProgress; }
            Glib::Dispatcher& signal_finished() { return m_SignalFinished; }
        private:
            static size_t write_cb(const unsigned char *ptr, size_t size, size_t nmemb, void *userp);
            static int progress_cb(void *userp, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

            static const char *UserAgent;

            CURL *m_EasyHandle;
            CURLcode m_Response;
            std::string m_Url;
            std::vector<unsigned char> m_Buffer;

            std::atomic<bool> m_Active;
            std::atomic<double> m_DownloadTotal,
                                m_DownloadCurrent;
            time_point_t m_StartTime;

            Glib::RefPtr<Gio::Cancellable> m_Cancel;

            SignalWriteType m_SignalWrite;
            Glib::Dispatcher m_SignalProgress,
                             m_SignalFinished;
        };
    }
}

#endif /* _CURLER_H_ */
