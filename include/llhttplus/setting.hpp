#pragma once

#include "llhttp.h"

/**
 * To use this Class, You should inherit server::http::Parser(CRTP template class).
 * Then you may be need to implement the interface as below in your subclass.
 *
 * int _on_message_begin();
 * int _on_url(const char* at, size_t length);
 * int _on_status(const char* at, size_t length);
 * int _on_header_field(const char* at, size_t length);
 * int _on_header_value(const char* at, size_t length);
 * int _on_headers_complete();
 * int _on_body(const char* at, size_t length);
 * int _on_message_complete();
 * int _on_chunk_header();
 * int _on_chunk_complete();
 * int _on_url_complete();
 * int _on_status_complete();
 * int _on_header_field_complete();
 * int _on_header_value_complete();
 */

#define _CB_FUNCTION_DECLARE(name)                       \
    static int __##name(llhttp_t *lparser)               \
    {                                                    \
        auto *parser = (Parser *)lparser->data;          \
        auto *self = (ParserSetting *)parser->setting(); \
        return self->name(parser);                       \
    }

#define _DATA_CB_FUNCTION_DECLARE(name)                                   \
    static int __##name(llhttp_t *lparser, const char *at, size_t length) \
    {                                                                     \
        auto *parser = (Parser *)lparser->data;                           \
        auto *self = (ParserSetting *)parser->setting();                  \
        return self->name(parser, at, length);                            \
    }

#define _HAS_MEMBER_FUNCTION(name, ...)                              \
    template <class T>                                               \
    class has_##name                                                 \
    {                                                                \
    private:                                                         \
        template <typename U, int (U::*)(__VA_ARGS__) = &U::name>    \
        static constexpr bool check(U *) { return true; }            \
        static constexpr bool check(...) { return false; }           \
                                                                     \
    public:                                                          \
        static constexpr bool value = check(static_cast<T *>(0));    \
    };

#define INVOKE_CB(name)         return self->name(parser);
#define INVOKE_DATA_CB(name)    return self->name(parser, at, length);

#define GET_CONTEXT                                 \
    auto *parser = (Parser *)lparser->data;         \
    auto *self = (ParserSetting *)parser->setting();\

#define STATIC_CB_DEFINE(name)                      \
    GET_CONTEXT                                     \
    INVOKE_CB(name);

#define STATIC_DATA_CB_DEFINE(name)                 \
    GET_CONTEXT                                     \
    INVOKE_DATA_CB(name);

namespace llhttp
{
    class Parser;

    template <class SubClass>
    class ParserSetting
    {
    public:
        ParserSetting()
        {
            llhttp_settings_init(&_low_layer_setting);
            __bind_low_layer_setting();
        }

        // INTERFACES
        int on_url(Parser* p, const char *at, size_t length) 
        { 
            return static_cast<SubClass *>(this)->_on_url(p, at, length); 
        }

        int on_status(Parser* p, const char *at, size_t length) 
        { 
            return static_cast<SubClass *>(this)->_on_status(p, at, length); 
        }

        int on_header_field(Parser* p, const char *at, size_t length) 
        { 
            return static_cast<SubClass *>(this)->_on_header_field(p, at, length); 
        }

        int on_header_value(Parser* p, const char *at, size_t length)
        {
            return static_cast<SubClass *>(this)->_on_header_value(p, at, length); 
        }

        int on_headers_complete(Parser* p)
        {
            return static_cast<SubClass*>(this)->_on_headers_complete(p);
        }

        int on_body(Parser* p, const char *at, size_t length)
        {
            return static_cast<SubClass *>(this)->_on_body(p, at, length); 
        }

        int on_message_begin(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_message_begin(p); 
        }

        /* Possible return values 0, -1, `HPE_PAUSED` */
        int on_message_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_message_complete(p); 
        }

        /*
         * When on_chunk_header is called, the current chunk length is stored
         * in parser->content_length.
         * Possible return values 0, -1, `HPE_PAUSED`
         */
        int on_chunk_header(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_chunk_header(p); 
        }
        int on_chunk_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_chunk_complete(p); 
        }

        /* Information-only callbacks, return value is ignored */
        int on_url_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_url_complete(p); 
        }
        int on_status_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_status_complete(p); 
        }
        int on_header_field_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_header_field_complete(p); 
        }
        int on_header_value_complete(Parser* p)
        {
            return static_cast<SubClass *>(this)->_on_header_value_complete(p); 
        }

        llhttp_settings_t& low_layer_setting()
        {
            return _low_layer_setting;
        }

    private:
        _HAS_MEMBER_FUNCTION(_on_url,           Parser *, const char*, size_t)
        _HAS_MEMBER_FUNCTION(_on_status,        Parser*, const char*, size_t)
        _HAS_MEMBER_FUNCTION(_on_header_field,  Parser*, const char*, size_t)
        _HAS_MEMBER_FUNCTION(_on_header_value,  Parser*, const char*, size_t)
        _HAS_MEMBER_FUNCTION(_on_body,          Parser*, const char*, size_t)

        static int __on_url(llhttp_t *lparser, const char *at, size_t length)
        {
            STATIC_DATA_CB_DEFINE(on_url)
        }

        static int __on_status(llhttp_t *lparser, const char *at, size_t length)
        {
            STATIC_DATA_CB_DEFINE(on_status)
        }

        static int __on_header_field(llhttp_t *lparser, const char *at, size_t length)
        {
            STATIC_DATA_CB_DEFINE(on_header_field)
        }

        static int __on_header_value(llhttp_t *lparser, const char *at, size_t length)
        {
            STATIC_DATA_CB_DEFINE(on_header_value)
        }

        static int __on_body(llhttp_t *lparser, const char *at, size_t length)
        {
            STATIC_DATA_CB_DEFINE(on_body)
        }

        _HAS_MEMBER_FUNCTION(_on_message_begin,         Parser*)
        _HAS_MEMBER_FUNCTION(_on_headers_complete,      Parser*)
        _HAS_MEMBER_FUNCTION(_on_message_complete,      Parser*)
        _HAS_MEMBER_FUNCTION(_on_chunk_header,          Parser*)
        _HAS_MEMBER_FUNCTION(_on_chunk_complete,        Parser*)
        _HAS_MEMBER_FUNCTION(_on_url_complete,          Parser*)
        _HAS_MEMBER_FUNCTION(_on_status_complete,       Parser*)
        _HAS_MEMBER_FUNCTION(_on_header_field_complete, Parser*)
        _HAS_MEMBER_FUNCTION(_on_header_value_complete, Parser*)

        static int __on_message_begin(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_message_begin)
        }

        /*
         * Possible return values:
         * 0  - Proceed normally
         * 1  - Assume that request/response has no body, and proceed to parsing the
         *      next message
         * 2  - Assume absence of body (as above) and make `llhttp_execute()` return
         *      `HPE_PAUSED_UPGRADE`
         * -1 - Error
         * `HPE_PAUSED`
         */
        static int __on_headers_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_headers_complete)
        }

        /* Possible return values 0, -1, `HPE_PAUSED` */
        static int __on_message_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_message_complete)
        }

        /*
         * When on_chunk_header is called, the current chunk length is stored
         * in parser->content_length.
         * Possible return values 0, -1, `HPE_PAUSED`
         */
        static int __on_chunk_header(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_chunk_header)
        }
        static int __on_chunk_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_chunk_complete)
        }

        /* Information-only callbacks, return value is ignored */
        static int __on_url_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_url_complete)
        }
        static int __on_status_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_status_complete)
        }
        static int __on_header_field_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_header_field_complete)
        }
        static int __on_header_value_complete(llhttp_t *lparser)
        {
            STATIC_CB_DEFINE(on_header_value_complete)
        }

#define _BIND_CB(name)                          \
    if constexpr (has__##name<SubClass>::value) \
    _low_layer_setting.name = ParserSetting::__##name

        void __bind_low_layer_setting()
        {
            _BIND_CB(on_message_begin);
            _BIND_CB(on_url);
            _BIND_CB(on_status);
            _BIND_CB(on_header_field);
            _BIND_CB(on_header_value);
            _BIND_CB(on_headers_complete);
            _BIND_CB(on_body);
            _BIND_CB(on_message_complete);
            _BIND_CB(on_chunk_header);
            _BIND_CB(on_chunk_complete);
            _BIND_CB(on_url_complete);
            _BIND_CB(on_status_complete);
            _BIND_CB(on_header_field_complete);
            _BIND_CB(on_header_value_complete);
        }

        llhttp_settings_t _low_layer_setting;
    };
}