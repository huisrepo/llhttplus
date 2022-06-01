#include <llhttp++/llhttp++.hpp>
#include <llhttp++/setting.hpp>
#include <iostream>

namespace llhttp
{
    class DefaultSetting : public llhttp::ParserSetting<DefaultSetting>
    {
    public:
        int _on_url(Parser* p, const char* at, size_t length)
        {
            p->request()->url = std::string_view(at, length);
            return 0;
        }

        int _on_status(Parser* p, const char* at, size_t length)
        {
            p->request()->status = std::string_view(at, length);
            return 0;
        }

        int _on_header_field(Parser* p, const char* at, size_t length)
        {
            p->request()->headers.push_back({
                    std::string_view(at, length), std::string_view()
                }
            );
            return 0;
        }

        int _on_header_value(Parser* p, const char* at, size_t length)
        {
            p->request()->headers.back().second = std::string_view(at, length);
            return 0;
        }

        int _on_body(Parser* p, const char* at, size_t length)
        {
            p->request()->body = std::string_view(at, length);
            return 0;
        }

        int _on_headers_complete(Parser* p)
        {
            p->request()->method = p->get_method();
            p->request()->version_major = p->get_http_major();
            p->request()->version_minor = p->get_http_minor();
            return 0;
        }

        int _on_message_complete(Parser* p)
        {
            std::cout << "complete" << std::endl;
            return 0;
        }
    };

    static DefaultSetting __default_setting;

    Parser::Parser()
    {
        __init(&__default_setting.low_layer_setting());
    }

    void Parser::__init(llhttp_settings_t* setting)
    {
        llhttp_init(
            &_low_layer_parser,
            HTTP_BOTH,
            setting
        );
        _setting = static_cast<void*>(setting);
        _low_layer_parser.data = this;
    }

    llhttp_type Parser::get_type()
    {
        return static_cast<llhttp_type>(_low_layer_parser.type);
    }

    uint8_t Parser::get_http_major()
    {
        return _low_layer_parser.http_major;
    }

    uint8_t Parser::get_http_minor()
    {
        return _low_layer_parser.http_minor;
    }

    llhttp_method Parser::get_method()
    {
        return static_cast<llhttp_method>(_low_layer_parser.method);
    }

    int Parser::get_status_code()
    {
        return _low_layer_parser.status_code;
    }

    uint8_t Parser::get_upgrade()
    {
        return _low_layer_parser.upgrade;
    }

    void Parser::reset()
    {
        return llhttp_reset(&_low_layer_parser);
    }

    llhttp_errno_t Parser::execute(Request* _request, const char *data, size_t len) noexcept
    {
        this->_request = _request;
        return llhttp_execute(&_low_layer_parser, data, len);
    }

    llhttp_errno_t Parser::execute(Request* _request, const std::string &data) noexcept
    {
        this->_request = _request;
        return llhttp_execute(&_low_layer_parser, data.c_str(), data.length());
    }

    llhttp_errno_t Parser::execute(Request* _request, std::string_view view) noexcept
    {
        this->_request = _request;
        return llhttp_execute(&_low_layer_parser, view.data(), view.length());
    }

    llhttp_errno_t Parser::finish()
    {
        return llhttp_finish(&_low_layer_parser);
    }

    int Parser::message_needs_eof()
    {
        return llhttp_message_needs_eof(&_low_layer_parser);
    }

    int Parser::should_keep_alive()
    {
        return llhttp_should_keep_alive(&_low_layer_parser);
    }

    void Parser::pause()
    {
        llhttp_pause(&_low_layer_parser);
    }

    /* Might be called to resume the execution after the pause in user's callback.
     * See `llhttp_execute()` above for details.
     *
     * Call this only if `llhttp_execute()` returns `HPE_PAUSED`.
     */
    void Parser::resume()
    {
        llhttp_resume(&_low_layer_parser);
    }

    /* Might be called to resume the execution after the pause in user's callback.
     * See `llhttp_execute()` above for details.
     *
     * Call this only if `llhttp_execute()` returns `HPE_PAUSED_UPGRADE`
     */
    void Parser::resume_after_upgrade()
    {
        llhttp_resume_after_upgrade(&_low_layer_parser);
    }

    llhttp_errno_t Parser::get_errno()
    {
        return llhttp_get_errno(&_low_layer_parser);
    }

    /* Returns the verbal explanation of the latest returned error.
     *
     * Note: User callback should set error reason when returning the error. See
     * `llhttp_set_error_reason()` for details.
     */
    const char* Parser::get_error_reason()
    {
        return llhttp_get_error_reason(&_low_layer_parser);
    }

    /* Assign verbal description to the returned error. Must be called in user
     * callbacks right before returning the errno.
     *
     * Note: `HPE_USER` error code might be useful in user callbacks.
     */
    void Parser::set_error_reason(const char *reason)
    {
        return llhttp_set_error_reason(&_low_layer_parser, reason);
    }

    /* Returns the pointer to the last parsed byte before the returned error. The
     * pointer is relative to the `data` argument of `llhttp_execute()`.
     *
     * Note: this method might be useful for counting the number of parsed bytes.
     */
    const char* Parser::get_error_pos()
    {
        return llhttp_get_error_pos(&_low_layer_parser);
    }

    /* Returns textual name of error code */
    const char* Parser::errno_name(llhttp_errno_t err)
    {
        return llhttp_errno_name(err);
    }

    /* Returns textual name of HTTP method */
    const char* Parser::method_name(llhttp_method_t method)
    {
        return llhttp_method_name(method);
    }

    void Parser::set_lenient_headers(int enabled)
    {
        return llhttp_set_lenient_headers(&_low_layer_parser, enabled);
    }

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
    void Parser::set_lenient_chunked_length(int enabled)
    {
        return llhttp_set_lenient_chunked_length(&_low_layer_parser, enabled);
    }

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
    void Parser::set_lenient_keep_alive(int enabled)
    {
        return llhttp_set_lenient_keep_alive(&_low_layer_parser, enabled);
    }

    void* Parser::setting()
    {
        return _setting;
    }

    Request* Parser::request()
    {
        return _request;
    }

    bool Parser::parse_done()
    {
        return _low_layer_parser.finish != HTTP_FINISH_UNSAFE;
    }
}