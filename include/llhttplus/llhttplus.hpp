#pragma once

#ifndef _LLHTTP_HPP_
#define _LLHTTP_HPP_
#include "llhttp.h"
#include <string>
#include <vector>
#include <string_view>
#include <memory>

namespace llhttplus
{
    using Header = std::pair<std::string_view, std::string_view>;

    struct Request
    {
        llhttp_method_t     method;
        std::string_view    url;
        uint8_t             version_major;
        uint8_t             version_minor;
        std::vector<Header> headers;
        std::string_view    status;
        std::string_view    body;
    };

    class Parser
    {
    public:
        template<class T> friend class ParserSetting;

        Parser();

        template<class Setting>
        Parser(ParserSetting<Setting>* setting)
        {
            __init(&setting->low_layer_setting());
        }

    public:
        llhttp_type get_type();

        uint8_t get_http_major();

        uint8_t get_http_minor();

        llhttp_method get_method();

        int get_status_code();

        uint8_t get_upgrade();

        /*
         * Reset an already initialized parser back to the start state, preserving the
         * existing parser type, callback settings, user data, and lenient flags.
         */
        void reset();

        /* Parse full or partial request/response, invoking user callbacks along the
         * way.
         *
         * If any of `llhttp_data_cb` returns errno not equal to `HPE_OK` - the parsing
         * interrupts, and such errno is returned from `llhttp_execute()`. If
         * `HPE_PAUSED` was used as a errno, the execution can be resumed with
         * `llhttp_resume()` call.
         *
         * In a special case of CONNECT/Upgrade request/response `HPE_PAUSED_UPGRADE`
         * is returned after fully parsing the request/response. If the user wishes to
         * continue parsing, they need to invoke `llhttp_resume_after_upgrade()`.
         *
         * NOTE: if this function ever returns a non-pause type error, it will continue
         * to return the same error upon each successive call up until `llhttp_init()`
         * is called.
         */
        llhttp_errno_t execute(Request* _request, const char *data, size_t len) noexcept;
        llhttp_errno_t execute(Request* _request, const std::string &data) noexcept;
        llhttp_errno_t execute(Request* _request, std::string_view view) noexcept;

        llhttp_errno_t finish();

        /* Returns `1` if the incoming message is parsed until the last byte, and has
         * to be completed by calling `llhttp_finish()` on EOF
         */
        int message_needs_eof();

        /* Returns `1` if there might be any other messages following the last that was
         * successfully parsed.
         */
        int should_keep_alive();

        /* Make further calls of `llhttp_execute()` return `HPE_PAUSED` and set
         * appropriate error reason.
         *
         * Important: do not call this from user callbacks! User callbacks must return
         * `HPE_PAUSED` if pausing is required.
         */
        void pause();

        /* Might be called to resume the execution after the pause in user's callback.
         * See `llhttp_execute()` above for details.
         *
         * Call this only if `llhttp_execute()` returns `HPE_PAUSED`.
         */
        void resume();

        /* Might be called to resume the execution after the pause in user's callback.
         * See `llhttp_execute()` above for details.
         *
         * Call this only if `llhttp_execute()` returns `HPE_PAUSED_UPGRADE`
         */
        void resume_after_upgrade();

        /* Returns the latest return error */
        llhttp_errno_t get_errno();

        /* Returns the verbal explanation of the latest returned error.
         *
         * Note: User callback should set error reason when returning the error. See
         * `llhttp_set_error_reason()` for details.
         */
        const char *get_error_reason();

        /* Assign verbal description to the returned error. Must be called in user
         * callbacks right before returning the errno.
         *
         * Note: `HPE_USER` error code might be useful in user callbacks.
         */
        void set_error_reason(const char *reason);

        /* Returns the pointer to the last parsed byte before the returned error. The
         * pointer is relative to the `data` argument of `llhttp_execute()`.
         *
         * Note: this method might be useful for counting the number of parsed bytes.
         */
        const char *get_error_pos();

        /* Returns textual name of error code */
        static const char *errno_name(llhttp_errno_t err);

        /* Returns textual name of HTTP method */
        static const char *method_name(llhttp_method_t method);

        /* Enables/disables lenient header value parsing (disabled by default).
         *
         * Lenient parsing disables header value token checks, extending llhttp's
         * protocol support to highly non-compliant clients/server. No
         * `HPE_INVALID_HEADER_TOKEN` will be raised for incorrect header values when
         * lenient parsing is "on".
         *
         * **(USE AT YOUR OWN RISK)**
         */
        void set_lenient_headers(int enabled);

        /* Enables/disables lenient handling of conflicting `Transfer-Encoding` and
         * `Content-Length` headers (disabled by default).
         *
         * Normally `llhttp` would error when `Transfer-Encoding` is present in
         * conjunction with `Content-Length`. This error is important to prevent HTTP
         * request smuggling, but may be less desirable for small number of cases
         * involving legacy servers.
         *
         * **(USE AT YOUR OWN RISK)**
         */
        void set_lenient_chunked_length(int enabled);

        /* Enables/disables lenient handling of `Connection: close` and HTTP/1.0
         * requests responses.
         *
         * Normally `llhttp` would error on (in strict mode) or discard (in loose mode)
         * the HTTP request/response after the request/response with `Connection: close`
         * and `Content-Length`. This is important to prevent cache poisoning attacks,
         * but might interact badly with outdated and insecure clients. With this flag
         * the extra request/response will be parsed normally.
         *
         * **(USE AT YOUR OWN RISK)**
         */
        void set_lenient_keep_alive(int enabled);

        void *setting();

        Request* request();

        bool parse_done();

    protected:
        void __init(llhttp_settings_t*);

        llhttp_t _low_layer_parser;
        Request* _request;
        void *_setting;
    };
}

#endif
