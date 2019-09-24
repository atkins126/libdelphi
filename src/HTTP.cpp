/*++

Library name:

  libdelphi

Module Name:

  Server.cpp

Notices:

  Delphi classes for C++

Author:

  Copyright (c) Prepodobny Alen

  mailto: alienufo@inbox.ru
  mailto: ufocomp@gmail.com

--*/

#include "delphi.hpp"
#include "delphi/HTTP.hpp"

extern "C++" {

namespace Delphi {

    namespace HTTP {

        #define StringArrayToStream(Stream, Buf) (Stream)->Write((Buf), sizeof((Buf)) - sizeof(TCHAR))

        namespace Mapping {

            struct CMapping {
                LPCTSTR Ext;
                LPCTSTR MimeType;
                bool IsText;
            } CMappings[] = {

                    { _T(".htm"),       _T("text/html"),                        true  },
                    { _T(".html"),      _T("text/html"),                        true  },
                    { _T(".css"),       _T("text/css"),                         true  },
                    { _T(".js"),        _T("text/javascript"),                  true  },
                    { _T(".php"),       _T("text/php"),                         true  },

                    { _T(".png"),       _T("image/png"),                        false },
                    { _T(".jpg"),       _T("image/jpeg"),                       false },
                    { _T(".jpeg"),      _T("image/jpeg"),                       false },
                    { _T(".gif"),       _T("image/gif"),                        false },
                    { _T(".tif"),       _T("image/tiff"),                       false },
                    { _T(".ico"),       _T("image/vnd.microsoft.icon"),         false },

                    { _T(".jpe"),       _T("image/jpeg"),                       false },
                    { _T(".jfif"),      _T("image/jpeg"),                       false },

                    { _T(".txt"),       _T("text/plain"),                       true  },
                    { _T(".md"),        _T("text/markdown"),                    true  },
                    { _T(".markdown"),  _T("text/markdown"),                    true  },
                    { _T(".cmd"),       _T("text/cmd"),                         true  },
                    { _T(".appcache"),  _T("text/cache-manifest"),              true  },

                    { _T(".svg"),       _T("image/svg+xml"),                    true  },
                    { _T(".svgz"),      _T("image/svg+xml"),                    true  },

                    { _T(".ttf"),       _T("application/x-font-ttf"),           false },
                    { _T(".otf"),       _T("application/x-font-opentype"),      false },
                    { _T(".woff"),      _T("application/x-font-woff"),          false },
                    { _T(".woff2"),     _T("application/x-font-woff2"),         false },
                    { _T(".eot"),       _T("application/vnd.ms-fontobject"),    false },
                    { _T(".sfnt"),      _T("application/font-sfnt"),            false },

                    { _T(".xml"),       _T("application/xml"),                  true  },
                    { _T(".json"),      _T("application/json"),                 true  },

                    { _T(".pdf"),       _T("application/pdf"),                  false },

                    { _T(".zip"),       _T("application/zip"),                  false },
                    { _T(".gz"),        _T("application/gzip"),                 false },
                    { _T(".tgz"),       _T("application/gzip"),                 false },

                    { _T(".tgz"),       _T("application/gzip"),                 false },
                    { _T(".torrent"),   _T("application/x-bittorrent"),         false },

                    { _T(".bin"),       _T("application/octet-stream"),         false },
                    { _T(".exe"),       _T("application/octet-stream"),         false },

                    {nullptr, nullptr, false} // Marks end of list.
            };

            LPCTSTR ExtToType(LPCTSTR Ext)
            {
                if (Assigned(Ext)) {
                    for (CMapping *m = CMappings; m->Ext; ++m) {
                        if (SameText(m->Ext, Ext)) {
                            return m->MimeType;
                        }
                    }
                }

                return _T("text/plain");
            }

            bool IsText(LPCTSTR MimeType)
            {
                if (Assigned(MimeType)) {
                    for (CMapping *m = CMappings; m->MimeType; ++m) {
                        if (SameText(m->MimeType, MimeType)) {
                            return m->IsText;
                        }
                    }
                }

                return false;
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHeaders --------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHeaders::~CHeaders() {
            Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::Put(int Index, const CHeader &Header) {
            m_pList.Insert(Index, Header);
        }
        //--------------------------------------------------------------------------------------------------------------

        CHeader &CHeaders::Get(int Index) {
            return m_pList.Items(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        const CHeader &CHeaders::Get(int Index) const {
            return m_pList.Items(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        const CString &CHeaders::GetValue(const CString &Name) const {
            int Index = IndexOfName(Name);
            if (Index != -1) {
                const CHeader& Header = Get(Index);
                return Header.Value;
            }
            return m_NullValue;
        }
        //--------------------------------------------------------------------------------------------------------------

        const CString &CHeaders::GetValue(LPCTSTR Name) const {
            int Index = IndexOfName(Name);
            if (Index != -1) {
                const CHeader& Header = Get(Index);
                return Header.Value;
            }
            return m_NullValue;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::Clear() {
            m_pList.Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::Insert(int Index, const CHeader &Header) {
            Put(Index, Header);
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::Add(const CHeader &Header) {
            int Result = GetCount();
            Insert(Result, Header);
            return Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::AddPair(LPCTSTR lpszName, LPCTSTR lpszValue) {
            int Index = Add(CHeader());
            Last().Name = lpszName;
            Last().Value = lpszValue;
            return Index;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::AddPair(LPCTSTR lpszName, const CString &Value) {
            int Index = Add(CHeader());
            Last().Name = lpszName;
            Last().Value = Value;
            return Index;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::AddPair(const CString &Name, const CString &Value) {
            int Index = Add(CHeader());
            Last().Name = Name;
            Last().Value = Value;
            return Index;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::Delete(int Index) {
            m_pList.Delete(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::GetCount() const {
            return m_pList.GetCount();
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::IndexOfName(const CString& Name) const {
            for (int I = 0; I < GetCount(); ++I) {
                const CHeader& Header = Get(I);
                if (Header.Name.Lower() == Name)
                    return I;
            }
            return -1;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CHeaders::IndexOfName(LPCTSTR Name) const {
            for (int I = 0; I < GetCount(); ++I) {
                const CHeader& Header = Get(I);
                if (Header.Name.Lower() == Name)
                    return I;
            }
            return -1;
        }
        //--------------------------------------------------------------------------------------------------------------

        CHeader &CHeaders::First() {
            return m_pList.First();
        }
        //--------------------------------------------------------------------------------------------------------------

        CHeader &CHeaders::Last() {
            return m_pList.Last();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::SetCount(int NewCount) {
            int LCount = GetCount();
            if (NewCount > LCount) {
                for (int I = LCount; I < NewCount; ++I)
                    Add(CHeader());
            } else {
                for (int I = LCount - 1; I >= NewCount; --I)
                    Delete(I);
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHeaders::Assign(const CHeaders &Headers) {
            Clear();
            for (int I = 0; I < Headers.GetCount(); ++I) {
                Add(Headers[I]);
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CFormData -------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CFormData::~CFormData() {
            Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::Clear() {
            m_pList.Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::Put(int Index, const CFormDataItem &Item) {
            m_pList.Insert(Index, Item);
        }
        //--------------------------------------------------------------------------------------------------------------

        CFormDataItem &CFormData::Get(int Index) {
            return m_pList.Items(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        const CFormDataItem &CFormData::Get(int Index) const {
            return m_pList.Items(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        int CFormData::GetCount() const {
            return m_pList.GetCount();
        }
        //--------------------------------------------------------------------------------------------------------------

        const CString &CFormData::GetData(const CString &Name) const {
            int Index = IndexOfName(Name);
            if (Index != -1) {
                const CFormDataItem& Item = Get(Index);
                return Item.Data;
            }
            return m_NullData;
        }
        //--------------------------------------------------------------------------------------------------------------

        const CString &CFormData::GetData(LPCTSTR Name) const {
            int Index = IndexOfName(Name);
            if (Index != -1) {
                const CFormDataItem& Item = Get(Index);
                return Item.Data;
            }
            return m_NullData;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CFormData::IndexOfName(const CString &Name) const {
            for (int I = 0; I < GetCount(); ++I) {
                const CFormDataItem& Item = Get(I);
                if (Item.Name.Lower() == Name)
                    return I;
            }
            return -1;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CFormData::IndexOfName(LPCTSTR Name) const {
            for (int I = 0; I < GetCount(); ++I) {
                const CFormDataItem& Item = Get(I);
                if (Item.Name.Lower() == Name)
                    return I;
            }
            return -1;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::Insert(int Index, const CFormDataItem &Item) {
            Put(Index, Item);
        }
        //--------------------------------------------------------------------------------------------------------------

        int CFormData::Add(const CFormDataItem &Item) {
            int Result = GetCount();
            Insert(Result, Item);
            return Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::Delete(int Index) {
            m_pList.Delete(Index);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::SetCount(int NewCount) {
            int LCount = GetCount();
            if (NewCount > LCount) {
                for (int I = LCount; I < NewCount; ++I)
                    Add(CFormDataItem());
            } else {
                for (int I = LCount - 1; I >= NewCount; --I)
                    Delete(I);
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        CFormDataItem &CFormData::First() {
            return m_pList.First();
        }
        //--------------------------------------------------------------------------------------------------------------

        CFormDataItem &CFormData::Last() {
            return m_pList.Last();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CFormData::Assign(const CFormData &Value) {
            Clear();
            for (int I = 0; I < Value.GetCount(); ++I) {
                Add(Value[I]);
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        namespace MiscStrings {
            const TCHAR http[] = _T("HTTP/1.1");
            const TCHAR question[] = _T("?");
            const TCHAR ampersand[] = _T("&");
            //const TCHAR dot[] = _T(".");
            const TCHAR space[] = _T(" ");
            const TCHAR separator[] = _T(": ");
            const TCHAR crlf[] = _T("\r\n");
        } // namespace MiscStrings

        //--------------------------------------------------------------------------------------------------------------

        //-- CRequest --------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        void http_request::ToBuffers(CMemoryStream *AStream) {

            Method.SaveToStream(AStream);
            StringArrayToStream(AStream, MiscStrings::space);

            Uri.SaveToStream(AStream);
            for (int i = 0; i < Params.Count(); ++i) {
                if (i == 0) {
                    StringArrayToStream(AStream, MiscStrings::question);
                } else {
                    StringArrayToStream(AStream, MiscStrings::ampersand);
                }
                Params[i].SaveToStream(AStream);
            }
            StringArrayToStream(AStream, MiscStrings::space);

            StringArrayToStream(AStream, MiscStrings::http);
            StringArrayToStream(AStream, MiscStrings::crlf);

            for (int i = 0; i < Headers.Count(); ++i) {
                CHeader &H = Headers[i];
                H.Name.SaveToStream(AStream);
                StringArrayToStream(AStream, MiscStrings::separator);
                H.Value.SaveToStream(AStream);
                StringArrayToStream(AStream, MiscStrings::crlf);
            }

            StringArrayToStream(AStream, MiscStrings::crlf);
            Content.SaveToStream(AStream);
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::Clear() {
            Method = "GET";
            Uri = "/";
            VMajor = 1;
            VMinor = 1;
            Params.Clear();
            Headers.Clear();
            Content.Clear();
            ContentLength = 0;
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::AddHeader(LPCTSTR lpszName, LPCTSTR lpszValue) {
            Headers.AddPair(lpszName, lpszValue);
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::AddHeader(LPCTSTR lpszName, const CString &Value) {
            Headers.AddPair(lpszName, Value);
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::AddHeader(const CString &Name, const CString &Value) {
            Headers.AddPair(Name, Value);
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::ToText() {
            CString Temp;
            TCHAR ch;
            if (!Content.IsEmpty()) {
                Temp = Content;
                Content.Clear();
                for (size_t i = 0; i < Temp.Size(); ++i) {
                    ch = Temp[i];
                    if (!IsCtl(ch) || (ch == '\t') || (ch == '\r') || (ch == '\n'))
                        Content.Append(ch);
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_request::ToJSON() {
            CString Temp;
            if (!Content.IsEmpty()) {
                Temp = Content;
                Content.Clear();
                for (size_t i = 0; i < Temp.Size(); ++i) {
                    switch (Temp[i]) {
                        case 8:
                            Content.Append(92);
                            Content.Append('b');
                            break;
                        case '\n':
                            Content.Append(92);
                            Content.Append('n');
                            break;
                        case 12:
                            Content.Append(92);
                            Content.Append('f');
                            break;
                        case '\r':
                            Content.Append(92);
                            Content.Append('r');
                            break;
                        case '\t':
                            Content.Append(92);
                            Content.Append('t');
                            break;
                        default:
                            Content.Append(Temp[i]);
                            break;
                    }
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        http_request *http_request::Prepare(http_request *ARequest, LPCTSTR AMethod, LPCTSTR AUri, LPCTSTR AContentType) {

            TCHAR szSize[_INT_T_LEN + 1] = {0};

            ARequest->Method = AMethod;
            ARequest->Uri = AUri;

            CString Host(ARequest->Host);
            if (!Host.IsEmpty()) {
                if (ARequest->Port > 0) {
                    Host << ":";
                    Host << (int) ARequest->Port;
                }

                ARequest->AddHeader(_T("Host"), Host);
            }

            ARequest->AddHeader(_T("User-Agent"), ARequest->UserAgent);

            if (!ARequest->Content.IsEmpty()) {
                ARequest->AddHeader(_T("Accept-Ranges"), _T("bytes"));

                if (AContentType == nullptr) {
                    switch (ARequest->ContentType) {
                        case content_type::html:
                            AContentType = _T("text/html");
                            break;
                        case content_type::json:
                            AContentType = _T("application/json");
                            ARequest->ToJSON();
                            break;
                        case content_type::xml:
                            AContentType = _T("application/xml");
                            ARequest->ToText();
                            break;
                        case content_type::text:
                            AContentType = _T("text/plain");
                            ARequest->ToText();
                            break;
                        case content_type::sbin:
                            AContentType = _T("application/octet-stream");
                            break;
                        default:
                            AContentType = _T("text/plain");
                            break;
                    }
                }

                ARequest->AddHeader(_T("Content-Type"), AContentType);
                ARequest->AddHeader(_T("Content-Length"), IntToStr((int) ARequest->Content.Size(), szSize, sizeof(szSize)));
            }

            if (ARequest->CloseConnection)
                ARequest->AddHeader(_T("Connection"), _T("close"));
            else
                ARequest->AddHeader(_T("Connection"), _T("keep-alive"));

            return ARequest;
        }
        //--------------------------------------------------------------------------------------------------------------

        http_request *http_request::Authorization(http_request *ARequest, LPCTSTR AMethod, LPCTSTR ALogin,
                LPCTSTR APassword) {

            CString LPassphrase, LAuthorization;

            LPassphrase = ALogin;
            LPassphrase << ":";
            LPassphrase << APassword;

            LAuthorization = AMethod;
            LAuthorization << " ";
            LAuthorization << base64_encode(LPassphrase);

            ARequest->AddHeader("Authorization", LAuthorization);

            return ARequest;
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CRequestParser --------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        int CRequestParser::Consume(CRequest *ARequest, CRequestContext& Context) {
            char AInput = *Context.Begin++;
            switch (Context.State) {
                case Request::method_start:
                    if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::method;
                        ARequest->Method.Append(AInput);
                        return -1;
                    }
                case Request::method:
                    if (AInput == ' ') {
                        Context.State = Request::uri;
                        return -1;
                    } else if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        ARequest->Method.Append(AInput);
                        return -1;
                    }
                case Request::uri_start:
                    if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::uri;
                        ARequest->Uri.Append(AInput);
                        return -1;
                    }
                case Request::uri:
                    if (AInput == ' ') {
                        Context.State = Request::http_version_h;
                        return -1;
                    } else if (AInput == '?') {
                        Context.State = Request::uri_param_start;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        ARequest->Uri.Append(AInput);
                        return -1;
                    }
                case Request::uri_param_start:
                    if (AInput == ' ') {
                        Context.State = Request::http_version_h;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::uri_param;
                        ARequest->Params.Add(AInput);
                        return -1;
                    }
                case Request::uri_param:
                    if (AInput == ' ') {
                        Context.State = Request::http_version_h;
                        return -1;
                    } else if (AInput == '&') {
                        Context.State = Request::uri_param_start;
                        return -1;
                    } else if (AInput == '%') {
                        Context.MimeIndex = 0;
                        ::SecureZeroMemory(Context.MIME, sizeof(Context.MIME));
                        Context.State = Request::uri_param_mime;
                        return -1;
                    } else if (AInput == '+') {
                        ARequest->Params.back().Append(' ');
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        ARequest->Params.back().Append(AInput);
                        return -1;
                    }
                case Request::http_version_h:
                    if (AInput == 'H') {
                        Context.State = Request::http_version_t_1;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_t_1:
                    if (AInput == 'T') {
                        Context.State = Request::http_version_t_2;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_t_2:
                    if (AInput == 'T') {
                        Context.State = Request::http_version_p;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_p:
                    if (AInput == 'P') {
                        Context.State = Request::http_version_slash;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_slash:
                    if (AInput == '/') {
                        ARequest->VMajor = 0;
                        ARequest->VMinor = 0;
                        Context.State = Request::http_version_major_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_major_start:
                    if (IsDigit(AInput)) {
                        ARequest->VMajor = ARequest->VMajor * 10 + AInput - '0';
                        Context.State = Request::http_version_major;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_major:
                    if (AInput == '.') {
                        Context.State = Request::http_version_minor_start;
                        return -1;
                    } else if (IsDigit(AInput)) {
                        ARequest->VMajor = ARequest->VMajor * 10 + AInput - '0';
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_minor_start:
                    if (IsDigit(AInput)) {
                        ARequest->VMinor = ARequest->VMinor * 10 + AInput - '0';
                        Context.State = Request::http_version_minor;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::http_version_minor:
                    if (AInput == '\r') {
                        Context.State = Request::expecting_newline_1;
                        return -1;
                    } else if (IsDigit(AInput)) {
                        ARequest->VMinor = ARequest->VMinor * 10 + AInput - '0';
                        return -1;
                    } else {
                        return false;
                    }
                case Request::expecting_newline_1:
                    if (AInput == '\n') {
                        Context.State = Request::header_line_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::header_line_start:
                    if (AInput == '\r') {
                        Context.State = Request::expecting_newline_3;
                        return -1;
                    } else if ((ARequest->Headers.Count() > 0) && (AInput == ' ' || AInput == '\t')) {
                        Context.State = Request::header_lws;
                        return -1;
                    } else if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        ARequest->Headers.Add(CHeader());
                        ARequest->Headers.Last().Name.Append(AInput);

                        Context.State = Request::header_name;
                        return -1;
                    }
                case Request::header_lws:
                    if (AInput == '\r') {
                        Context.State = Request::expecting_newline_2;
                        return -1;
                    } else if (AInput == ' ' || AInput == '\t') {
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::header_value;
                        ARequest->Headers.Last().Value.Append(AInput);
                        return -1;
                    }
                case Request::header_name:
                    if (AInput == ':') {
                        Context.State = Request::space_before_header_value;
                        return -1;
                    } else if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        ARequest->Headers.Last().Name.Append(AInput);
                        return -1;
                    }
                case Request::space_before_header_value:
                    if (AInput == ' ') {
                        Context.State = Request::header_value;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::header_value:
                    if (AInput == '\r') {
                        Context.State = Request::expecting_newline_2;
                        return -1;
                    } else if (AInput == ';') {
                        Context.State = Request::header_value_options_start;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        ARequest->Headers.Last().Value.Append(AInput);
                        return -1;
                    }
                case Request::header_value_options_start:
                    if ((AInput == ' ' || AInput == '\t')) {
                        Context.State = Request::header_value_options_start;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::header_value_options;
                        ARequest->Headers.Last().Options.Add(AInput);
                        return -1;
                    }
                case Request::header_value_options:
                    if (AInput == '\r') {
                        Context.State = Request::expecting_newline_2;
                        return -1;
                    } else if (AInput == ';') {
                        Context.State = Request::header_value_options_start;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        ARequest->Headers.Last().Options.back().Append(AInput);
                        return -1;
                    }
                case Request::expecting_newline_2:
                    if (AInput == '\n') {
                        Context.State = Request::header_line_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Request::expecting_newline_3:
                    if (AInput == '\n') {
                        Context.ContentLength = Context.End - Context.Begin;

                        if (ARequest->Headers.Count() > 0) {

                            const CString& contentLength = ARequest->Headers.Values(_T("content-length"));
                            if (!contentLength.IsEmpty()) {
                                ARequest->ContentLength = strtoul(contentLength.c_str(), nullptr, 0);
                            } else {
                                ARequest->ContentLength = Context.ContentLength;
                            }

                            const CString& ContentType = ARequest->Headers.Values(_T("content-type"));
                            if (ContentType == "application/x-www-form-urlencoded") {
                                Context.State = Request::form_data_start;
                                return -1;
                            }
                        } else {
                            ARequest->ContentLength = Context.ContentLength;
                        }

                        if (ARequest->ContentLength > 0) {
                            Context.State = Request::content;
                            return -1;
                        }

                        return true;
                    } else {
                        return false;
                    }
                case Request::content:
                    ARequest->Content.Append(AInput);

                    if (ARequest->Content.Size() < ARequest->ContentLength) {
                        return -1;
                    }

                    return true;
                case Request::form_data_start:
                    ARequest->Content.Append(AInput);

                    if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Request::form_data;
                        ARequest->FormData.Add(AInput);
                        return -1;
                    }
                case Request::form_data:
                    ARequest->Content.Append(AInput);

                    if (AInput == '\n') {
                        return true;
                    } else if (AInput == '\r') {
                        return -1;
                    } else if (AInput == '&') {
                        Context.State = Request::form_data_start;
                        return -1;
                    } else if (AInput == '+') {
                        ARequest->FormData.back().Append(' ');
                        return -1;
                    } else if (AInput == '%') {
                        Context.MimeIndex = 0;
                        ::SecureZeroMemory(Context.MIME, sizeof(Context.MIME));
                        Context.State = Request::form_mime;
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        ARequest->FormData.back().Append(AInput);

                        if (ARequest->Content.Size() < ARequest->ContentLength) {
                            return -1;
                        }
                    }

                    return true;
                case Request::uri_param_mime:
                    Context.MIME[Context.MimeIndex++] = AInput;
                    if (Context.MimeIndex == 2) {
                        ARequest->Params.back().Append((TCHAR) HexToDec(Context.MIME));
                        Context.State = Request::uri_param;
                    }
                    return -1;
                case Request::form_mime:
                    ARequest->Content.Append(AInput);
                    Context.MIME[Context.MimeIndex++] = AInput;
                    if (Context.MimeIndex == 2) {
                        ARequest->FormData.back().Append((TCHAR) HexToDec(Context.MIME));
                        Context.State = Request::form_data;
                    }
                    return -1;
                default:
                    return false;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        int CRequestParser::Parse(CRequest *ARequest, CRequestContext& Context) {
            Context.Result = -1;
            while ((Context.Result == -1) && (Context.Begin != Context.End)) {
                Context.Result = Consume(ARequest, Context);
            }
            return Context.Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        int CRequestParser::ParseFormData(CRequest *ARequest, CFormData& FormData) {
            const CString& Content = ARequest->Content;

            if (Content.IsEmpty())
                return 0;

            try {
                const CHeader& contentType = ARequest->Headers["content-type"];
                if (contentType.Value != "multipart/form-data")
                    return 0;

                const CString CRLF(MiscStrings::crlf);
                const CString boundary(CRLF + "--" + contentType.Options["boundary"]);

                CStringList Data;

                size_t DataBegin = boundary.Size();
                size_t DataEnd = Content.Find(boundary, DataBegin);

                while (DataEnd != CString::npos) {
                    Data.Add(Content.SubString(DataBegin, DataEnd - DataBegin));
                    DataBegin = DataEnd + boundary.Size() + CRLF.Size();
                    DataEnd = Content.Find(boundary, DataBegin);
                }

                CStringList& formData = ARequest->FormData;
                CRequest Request;

                for (int I = 0; I < Data.Count(); I++) {
                    CRequestContext Context = CRequestContext(Data[I].Data(), Data[I].Size());
                    Context.State = Request::CParserState::header_line_start;
                    const int Result = Parse(&Request, Context);
                    if (Result == 1) {
                        FormData.Add(CFormDataItem());
                        CFormDataItem& DataItem = FormData.Last();

                        const CHeader& contentDisposition = Request.Headers["content-disposition"];

                        DataItem.Name = contentDisposition.Options["name"];
                        DataItem.File = contentDisposition.Options["filename"];
                        DataItem.Data = Request.Content;

                        if (DataItem.Data.Find('\n') == CString::npos) {
                            formData.AddPair(DataItem.Name, DataItem.Data);
                        }
                    }
                    Request.Clear();
                }

                return FormData.Count();
            } catch (...) {
                return -1;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CRequestParser::IsChar(int c) {
            return c >= 0 && c <= 127;
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CRequestParser::IsCtl(int c) {
            return (c >= 0 && c <= 31) || (c == 127);
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CRequestParser::IsTSpecial(int c) {
            switch (c) {
                case '(':
                case ')':
                case '<':
                case '>':
                case '@':
                case ',':
                case ';':
                case ':':
                case '\\':
                case '"':
                case '/':
                case '[':
                case ']':
                case '?':
                case '=':
                case '{':
                case '}':
                case ' ':
                case '\t':
                    return true;
                default:
                    return false;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CRequestParser::IsDigit(int c) {
            return c >= '0' && c <= '9';
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CReply ----------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        static const CReply::status_type StatusArray[] = {
                CReply::ok,
                CReply::created,
                CReply::accepted,
                CReply::non_authoritative,
                CReply::no_content,
                CReply::multiple_choices,
                CReply::moved_permanently,
                CReply::moved_temporarily,
                CReply::not_modified,
                CReply::bad_request,
                CReply::unauthorized,
                CReply::forbidden,
                CReply::not_found,
                CReply::not_allowed,
                CReply::internal_server_error,
                CReply::not_implemented,
                CReply::bad_gateway,
                CReply::service_unavailable,
                CReply::gateway_timeout
        };
        //--------------------------------------------------------------------------------------------------------------

        namespace StatusStrings {

            const TCHAR ok[] = _T("HTTP/1.1 200 OK\r\n");
            const TCHAR created[] = _T("HTTP/1.1 201 Created\r\n");
            const TCHAR accepted[] = _T("HTTP/1.1 202 Accepted\r\n");
            const TCHAR non_authoritative[] = _T("HTTP/1.1 203 Non-Authoritative Information\r\n");
            const TCHAR no_content[] = _T("HTTP/1.1 204 No Content\r\n");
            const TCHAR multiple_choices[] = _T("HTTP/1.1 300 Multiple Choices\r\n");
            const TCHAR moved_permanently[] = _T("HTTP/1.1 301 Moved Permanently\r\n");
            const TCHAR moved_temporarily[] = _T("HTTP/1.1 302 Moved Temporarily\r\n");
            const TCHAR not_modified[] = _T("HTTP/1.1 304 Not Modified\r\n");
            const TCHAR bad_request[] = _T("HTTP/1.1 400 Bad Request\r\n");
            const TCHAR unauthorized[] = _T("HTTP/1.1 401 Unauthorized\r\n");
            const TCHAR forbidden[] = _T("HTTP/1.1 403 Forbidden\r\n");
            const TCHAR not_found[] = _T("HTTP/1.1 404 Not Found\r\n");
            const TCHAR not_allowed[] = _T("HTTP/1.1 405 Method Not Allowed\r\n");
            const TCHAR internal_server_error[] = _T("HTTP/1.1 500 Internal Server Error\r\n");
            const TCHAR not_implemented[] = _T("HTTP/1.1 501 Not Implemented\r\n");
            const TCHAR bad_gateway[] = _T("HTTP/1.1 502 Bad Gateway\r\n");
            const TCHAR service_unavailable[] = _T("HTTP/1.1 503 Service Unavailable\r\n");
            const TCHAR gateway_timeout[] = _T("HTTP/1.1 504 Gateway Timeout\r\n");

            size_t ToBuffer(CReply::status_type AStatus, CStream *AStream) {
                switch (AStatus) {
                    case CReply::ok:
                        return StringArrayToStream(AStream, ok);
                    case CReply::created:
                        return StringArrayToStream(AStream, created);
                    case CReply::accepted:
                        return StringArrayToStream(AStream, accepted);
                    case CReply::non_authoritative:
                        return StringArrayToStream(AStream, non_authoritative);
                    case CReply::no_content:
                        return StringArrayToStream(AStream, no_content);
                    case CReply::multiple_choices:
                        return StringArrayToStream(AStream, multiple_choices);
                    case CReply::moved_permanently:
                        return StringArrayToStream(AStream, moved_permanently);
                    case CReply::moved_temporarily:
                        return StringArrayToStream(AStream, moved_temporarily);
                    case CReply::not_modified:
                        return StringArrayToStream(AStream, not_modified);
                    case CReply::bad_request:
                        return StringArrayToStream(AStream, bad_request);
                    case CReply::unauthorized:
                        return StringArrayToStream(AStream, unauthorized);
                    case CReply::forbidden:
                        return StringArrayToStream(AStream, forbidden);
                    case CReply::not_found:
                        return StringArrayToStream(AStream, not_found);
                    case CReply::not_allowed:
                        return StringArrayToStream(AStream, not_allowed);
                    case CReply::internal_server_error:
                        return StringArrayToStream(AStream, internal_server_error);
                    case CReply::not_implemented:
                        return StringArrayToStream(AStream, not_implemented);
                    case CReply::bad_gateway:
                        return StringArrayToStream(AStream, bad_gateway);
                    case CReply::service_unavailable:
                        return StringArrayToStream(AStream, service_unavailable);
                    case CReply::gateway_timeout:
                        return StringArrayToStream(AStream, gateway_timeout);
                    default:
                        return StringArrayToStream(AStream, internal_server_error);
                }
            }
        } // namespace StatusStrings
        //--------------------------------------------------------------------------------------------------------------

        void CReply::ToBuffers(CMemoryStream *AStream) {

            StatusStrings::ToBuffer(Status, AStream);

            for (int i = 0; i < Headers.Count(); ++i) {
                CHeader &H = Headers[i];
                H.Name.SaveToStream(AStream);
                StringArrayToStream(AStream, MiscStrings::separator);
                H.Value.SaveToStream(AStream);
                StringArrayToStream(AStream, MiscStrings::crlf);
            }
            StringArrayToStream(AStream, MiscStrings::crlf);
            Content.SaveToStream(AStream);
        }
        //--------------------------------------------------------------------------------------------------------------

        #define CreateStockHtmlReplies(Code, Message) (                                 \
            "<html>\r\n"                                                                \
            "<head><title>" #Message "</title></head>\r\n"                              \
            "<body>\r\n"                                                                \
            "<center><h1>" #Code " " #Message "</h1></center>\r\n"                      \
            "<hr><center>" WWWServerName "</center>\r\n"                                \
            "</body>\r\n"                                                               \
            "</html>\r\n"                                                               \
        )                                                                               \
        //--------------------------------------------------------------------------------------------------------------

        #define CreateStockJsonReplies(Code, Message) (                                 \
            "{"                                                                         \
              "\"error\": {"                                                            \
                "\"code\":" #Code ","                                                   \
                "\"message\":\"" #Message "\""                                          \
              "}"                                                                       \
            "}"                                                                         \
        )                                                                               \
        //--------------------------------------------------------------------------------------------------------------

        #define CreateStockReplies(Code, Message) {                                     \
            _T(CreateStockHtmlReplies(Code, Message)),                                  \
            _T(CreateStockJsonReplies(Code, Message))                                   \
        }                                                                               \
        //--------------------------------------------------------------------------------------------------------------

        namespace StockReplies {

            LPCTSTR ok[]                    = CreateStockReplies(200, OK);
            LPCTSTR created[]               = CreateStockReplies(201, Created);
            LPCTSTR accepted[]              = CreateStockReplies(202, Accepted);
            LPCTSTR non_authoritative[]     = CreateStockReplies(202, Non - Authoritative Information);
            LPCTSTR no_content[]            = CreateStockReplies(204, No Content);
            LPCTSTR multiple_choices[]      = CreateStockReplies(300, Multiple Choices);
            LPCTSTR moved_permanently[]     = CreateStockReplies(301, Moved Permanently);
            LPCTSTR moved_temporarily[]     = CreateStockReplies(302, Moved Temporarily);
            LPCTSTR not_modified[]          = CreateStockReplies(304, Not Modified);
            LPCTSTR bad_request[]           = CreateStockReplies(400, Bad Request);
            LPCTSTR unauthorized[]          = CreateStockReplies(401, Unauthorized);
            LPCTSTR forbidden[]             = CreateStockReplies(403, Forbidden);
            LPCTSTR not_found[]             = CreateStockReplies(404, Not Found);
            LPCTSTR not_allowed[]           = CreateStockReplies(405, Method Not Allowed);
            LPCTSTR internal_server_error[] = CreateStockReplies(500, Internal Server Error);
            LPCTSTR not_implemented[]       = CreateStockReplies(501, Not Implemented);
            LPCTSTR bad_gateway[]           = CreateStockReplies(502, Bad Gateway);
            LPCTSTR service_unavailable[]   = CreateStockReplies(503, Service Unavailable);
            LPCTSTR gateway_timeout[]       = CreateStockReplies(504, Gateway Timeout);

            LPCTSTR ToString(CReply::status_type AStatus, CReply::content_type AMessage) {

                if (AMessage > CReply::content_type::json)
                    AMessage = CReply::content_type::html;

                switch (AStatus) {
                    case CReply::ok:
                        return ok[AMessage];
                    case CReply::created:
                        return created[AMessage];
                    case CReply::accepted:
                        return accepted[AMessage];
                    case CReply::non_authoritative:
                        return non_authoritative[AMessage];
                    case CReply::no_content:
                        return no_content[AMessage];
                    case CReply::multiple_choices:
                        return multiple_choices[AMessage];
                    case CReply::moved_permanently:
                        return moved_permanently[AMessage];
                    case CReply::moved_temporarily:
                        return moved_temporarily[AMessage];
                    case CReply::not_modified:
                        return not_modified[AMessage];
                    case CReply::bad_request:
                        return bad_request[AMessage];
                    case CReply::unauthorized:
                        return unauthorized[AMessage];
                    case CReply::forbidden:
                        return forbidden[AMessage];
                    case CReply::not_found:
                        return not_found[AMessage];
                    case CReply::not_allowed:
                        return not_allowed[AMessage];
                    case CReply::internal_server_error:
                        return internal_server_error[AMessage];
                    case CReply::not_implemented:
                        return not_implemented[AMessage];
                    case CReply::bad_gateway:
                        return bad_gateway[AMessage];
                    case CReply::service_unavailable:
                        return service_unavailable[AMessage];
                    case CReply::gateway_timeout:
                        return gateway_timeout[AMessage];
                    default:
                        return internal_server_error[AMessage];
                }
            }

        } // namespace stock_replies
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::Clear() {
            Status = status_type::internal_server_error;
            StatusString.Clear();
            StatusText.Clear();
            ContentType = content_type::html;
            CloseConnection = true;
            Headers.Clear();
            Content.Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::AddHeader(LPCTSTR lpszName, LPCTSTR lpszValue) {
            Headers.Add(CHeader());
            Headers.Last().Name = lpszName;
            Headers.Last().Value = lpszValue;
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::AddHeader(LPCTSTR lpszName, const CString &Value) {
            Headers.Add(CHeader());
            Headers.Last().Name = lpszName;
            Headers.Last().Value = Value;
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::AddHeader(const CString &Name, const CString &Value) {
            Headers.Add(CHeader());
            Headers.Last().Name = Name;
            Headers.Last().Value = Value;
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::ToText() {
            CString Temp;
            TCHAR ch;
            if (!Content.IsEmpty()) {
                Temp = Content;
                Content.Clear();
                for (size_t i = 0; i < Temp.Size(); ++i) {
                    ch = Temp[i];
                    if (!IsCtl(ch) || (ch == '\t') || (ch == '\r') || (ch == '\n'))
                        Content.Append(ch);
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::ToJSON() {
            CString Temp;
            if (!Content.IsEmpty()) {
                Temp = Content;
                Content.Clear();
                for (size_t i = 0; i < Temp.Size(); ++i) {
                    switch (Temp[i]) {
                        case 8:
                            Content.Append(92);
                            Content.Append('b');
                            break;
                        case '\n':
                            Content.Append(92);
                            Content.Append('n');
                            break;
                        case 12:
                            Content.Append(92);
                            Content.Append('f');
                            break;
                        case '\r':
                            Content.Append(92);
                            Content.Append('r');
                            break;
                        case '\t':
                            Content.Append(92);
                            Content.Append('t');
                            break;
                        default:
                            Content.Append(Temp[i]);
                            break;
                    }
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void http_reply::StringToStatus() {
            int I = StrToInt(StatusString.c_str());
            for (const auto S : StatusArray) {
                if (I == S) {
                    Status = S;
                    break;
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        http_reply *CReply::GetReply(http_reply *AReply, status_type AStatus, LPCTSTR AContentType) {

            TCHAR szTime[MAX_BUFFER_SIZE + 1] = {0};
            TCHAR szSize[_INT_T_LEN + 1] = {0};

            time_t wtime;
            struct tm *wtm;

            wtime = time(&wtime);
            wtm = gmtime(&wtime);

            AReply->Status = AStatus;

            AReply->AddHeader(_T("Server"), AReply->ServerName);

            if ((wtm != nullptr) && (strftime(szTime, sizeof(szTime), "%a, %d %b %Y %T %Z", wtm) != 0)) {
                AReply->AddHeader(_T("Date"), szTime);
            }

            switch (AStatus) {
                case not_allowed:
                case not_implemented:
                    AReply->AddHeader(_T("Allow"), AReply->AllowedMethods);
                    break;
                case unauthorized:
                    AReply->AddHeader(_T("WWW-Authenticate"), _T("Basic realm=\"Access to the staging site\", charset=\"UTF-8\""));
                    break;
                default:
                    break;
            }

            if (!AReply->Content.IsEmpty()) {
                AReply->AddHeader(_T("Accept-Ranges"), _T("bytes"));

                if (AContentType == nullptr) {
                    switch (AReply->ContentType) {
                        case content_type::html:
                            AContentType = _T("text/html");
                            break;
                        case content_type::json:
                            AContentType = _T("application/json");
                            AReply->ToJSON();
                            break;
                        case content_type::xml:
                            AContentType = _T("application/xml");
                            AReply->ToText();
                            break;
                        case content_type::text:
                            AContentType = _T("text/plain");
                            AReply->ToText();
                            break;
                        case content_type::sbin:
                            AContentType = _T("application/octet-stream");
                            break;
                        default:
                            AContentType = _T("text/plain");
                            break;
                    }
                }

                AReply->AddHeader(_T("Content-Type"), AContentType);
                AReply->AddHeader(_T("Content-Length"), IntToStr((int) AReply->Content.Size(), szSize, sizeof(szSize)));
            }

            if (AReply->CloseConnection)
                AReply->AddHeader(_T("Connection"), _T("close"));
            else
                AReply->AddHeader(_T("Connection"), _T("keep-alive"));

            return AReply;
        }
        //--------------------------------------------------------------------------------------------------------------

        http_reply *CReply::GetStockReply(http_reply *AReply, CReply::status_type AStatus) {
            AReply->Content = StockReplies::ToString(AStatus, AReply->ContentType);
            AReply = GetReply(AReply, AStatus);
            return AReply;
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CReplyParser ----------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        int CReplyParser::Consume(CReply *AReply, CReplyContext& Context) {
            char AInput = *Context.Begin++;
            switch (Context.State) {
                case Reply::http_version_h:
                    if (AInput == 'H') {
                        Context.State = Reply::http_version_t_1;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_t_1:
                    if (AInput == 'T') {
                        Context.State = Reply::http_version_t_2;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_t_2:
                    if (AInput == 'T') {
                        Context.State = Reply::http_version_p;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_p:
                    if (AInput == 'P') {
                        Context.State = Reply::http_version_slash;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_slash:
                    if (AInput == '/') {
                        AReply->VMajor = 0;
                        AReply->VMinor = 0;
                        Context.State = Reply::http_version_major_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_major_start:
                    if (IsDigit(AInput)) {
                        AReply->VMajor = AReply->VMajor * 10 + AInput - '0';
                        Context.State = Reply::http_version_major;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_major:
                    if (AInput == '.') {
                        Context.State = Reply::http_version_minor_start;
                        return -1;
                    } else if (IsDigit(AInput)) {
                        AReply->VMajor = AReply->VMajor * 10 + AInput - '0';
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_minor_start:
                    if (IsDigit(AInput)) {
                        AReply->VMinor = AReply->VMinor * 10 + AInput - '0';
                        Context.State = Reply::http_version_minor;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_version_minor:
                    if (AInput == ' ') {
                        Context.State = Reply::http_status_start;
                        return -1;
                    } else if (IsDigit(AInput)) {
                        AReply->VMinor = AReply->VMinor * 10 + AInput - '0';
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_status_start:
                    if (IsDigit(AInput)) {
                        AReply->StatusString.Append(AInput);
                        Context.State = Reply::http_status;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_status:
                    if (AInput == ' ') {
                        AReply->StringToStatus();
                        Context.State = Reply::http_status_text_start;
                        return -1;
                    } else if (IsDigit(AInput)) {
                        AReply->StatusString.Append(AInput);
                        Context.State = Reply::http_status;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_status_text_start:
                    if (IsChar(AInput)) {
                        AReply->StatusText.Append(AInput);
                        Context.State = Reply::http_status_text;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::http_status_text:
                    if (AInput == '\r') {
                        Context.State = Reply::expecting_newline_1;
                        return -1;
                    } else if (IsChar(AInput)) {
                        AReply->StatusText.Append(AInput);
                        Context.State = Reply::http_status_text;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::expecting_newline_1:
                    if (AInput == '\n') {
                        Context.State = Reply::header_line_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::header_line_start:
                    if (AInput == '\r') {
                        Context.State = Reply::expecting_newline_3;
                        return -1;
                    } else if ((AReply->Headers.Count() > 0) && (AInput == ' ' || AInput == '\t')) {
                        Context.State = Reply::header_lws;
                        return -1;
                    } else if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        AReply->Headers.Add(CHeader());
                        AReply->Headers.Last().Name.Append(AInput);

                        Context.State = Reply::header_name;
                        return -1;
                    }
                case Reply::header_lws:
                    if (AInput == '\r') {
                        Context.State = Reply::expecting_newline_2;
                        return -1;
                    } else if (AInput == ' ' || AInput == '\t') {
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Reply::header_value;
                        AReply->Headers.Last().Value.Append(AInput);
                        return -1;
                    }
                case Reply::header_name:
                    if (AInput == ':') {
                        Context.State = Reply::space_before_header_value;
                        return -1;
                    } else if (!IsChar(AInput) || IsCtl(AInput) || IsTSpecial(AInput)) {
                        return false;
                    } else {
                        AReply->Headers.Last().Name.Append(AInput);
                        return -1;
                    }
                case Reply::space_before_header_value:
                    if (AInput == ' ') {
                        Context.State = Reply::header_value;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::header_value:
                    if (AInput == '\r') {
                        Context.State = Reply::expecting_newline_2;
                        return -1;
                    } else if (AInput == ';') {
                        Context.State = Reply::header_value_options_start;
                        AReply->Headers.Last().Value.Append(AInput);
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        AReply->Headers.Last().Value.Append(AInput);
                        return -1;
                    }
                case Reply::header_value_options_start:
                    if ((AInput == ' ' || AInput == '\t')) {
                        Context.State = Reply::header_value_options_start;
                        AReply->Headers.Last().Value.Append(AInput);
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        Context.State = Reply::header_value_options;
                        AReply->Headers.Last().Value.Append(AInput);
                        AReply->Headers.Last().Options.Add(AInput);
                        return -1;
                    }
                case Reply::header_value_options:
                    if (AInput == '\r') {
                        Context.State = Reply::expecting_newline_2;
                        return -1;
                    } else if (AInput == ';') {
                        Context.State = Reply::header_value_options_start;
                        AReply->Headers.Last().Value.Append(AInput);
                        return -1;
                    } else if (IsCtl(AInput)) {
                        return false;
                    } else {
                        AReply->Headers.Last().Value.Append(AInput);
                        AReply->Headers.Last().Options.back().Append(AInput);
                        return -1;
                    }
                case Reply::expecting_newline_2:
                    if (AInput == '\n') {
                        Context.State = Reply::header_line_start;
                        return -1;
                    } else {
                        return false;
                    }
                case Reply::expecting_newline_3:
                    if (AInput == '\n') {
                        Context.ContentLength = Context.End - Context.Begin;

                        if (AReply->Headers.Count() > 0) {
                            const CString &contentLength = AReply->Headers.Values(_T("content-length"));
                            if (!contentLength.IsEmpty()) {
                                AReply->ContentLength = strtoul(contentLength.c_str(), nullptr, 0);
                            } else {
                                AReply->ContentLength = Context.ContentLength;
                            }
                        } else {
                            AReply->ContentLength = Context.ContentLength;
                        }

                        if (AReply->ContentLength > 0) {
                            Context.State = Reply::content;
                            return -1;
                        }

                        return true;
                    } else {
                        return false;
                    }
                case Reply::content:
                    AReply->Content.Append(AInput);
                    if (AReply->Content.Size() < AReply->ContentLength) {
                        return -1;
                    }
                    return true;

                default:
                    return false;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        int CReplyParser::Parse(CReply *AReply, CReplyContext& Context) {
            Context.Result = -1;
            while (Context.Result == -1 && Context.Begin != Context.End) {
                Context.Result = Consume(AReply, Context);
            }
            return Context.Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CReplyParser::IsChar(int c) {
            return c >= 0 && c <= 127;
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CReplyParser::IsCtl(int c) {
            return (c >= 0 && c <= 31) || (c == 127);
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CReplyParser::IsTSpecial(int c) {
            switch (c) {
                case '(':
                case ')':
                case '<':
                case '>':
                case '@':
                case ',':
                case ';':
                case ':':
                case '\\':
                case '"':
                case '/':
                case '[':
                case ']':
                case '?':
                case '=':
                case '{':
                case '}':
                case ' ':
                case '\t':
                    return true;
                default:
                    return false;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CReplyParser::IsDigit(int c) {
            return c >= '0' && c <= '9';
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPServerConnection -------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPServerConnection::CHTTPServerConnection(CPollSocketServer *AServer):
                CTCPServerConnection(AServer) {
            m_State = Request::method_start;
            m_CloseConnection = true;
            m_ConnectionStatus = csConnected;
            m_Request = nullptr;
            m_Reply = nullptr;
            m_OnReply = nullptr;
        }
        //--------------------------------------------------------------------------------------------------------------

        CHTTPServerConnection::~CHTTPServerConnection() {
            Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        CRequest *CHTTPServerConnection::GetRequest() {
            if (m_Request == nullptr)
                m_Request = new CRequest();
            return m_Request;
        }
        //--------------------------------------------------------------------------------------------------------------

        CReply *CHTTPServerConnection::GetReply() {
            if (m_Reply == nullptr) {
                m_Reply = new CReply;
                m_Reply->ServerName = Server()->ServerName();
                m_Reply->AllowedMethods = Server()->AllowedMethods();
            }
            return m_Reply;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::Clear() {
            m_State = Request::method_start;
            FreeAndNil(m_Request);
            FreeAndNil(m_Reply);
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPServerConnection::ParseInput() {
            bool Result = false;
            if (Connected()) {
                CMemoryStream LStream(ReadAsync());
                try {
                    Result = LStream.Size() > 0;
                    if (Result) {
                        InputBuffer()->Extract(LStream.Memory(), LStream.Size());

                        CRequestContext Context = CRequestContext((LPCTSTR) LStream.Memory(), LStream.Size(), m_State);
                        const int ParseResult = CRequestParser::Parse(GetRequest(), Context);

                        switch (ParseResult) {
                            case 0:
                                Tag(clock());
                                m_ConnectionStatus = csRequestError;
                                break;

                            case 1:
                                Tag(clock());
                                m_ConnectionStatus = csRequestOk;
                                DoRequest();
                                break;

                            default:
                                m_State = Context.State;
                                m_ConnectionStatus = csWaitRequest;
                                break;
                        }
                    }
                } catch (...) {
                    throw;
                }
            }

            return Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::SendStockReply(http_reply::status_type AStatus, bool ASendNow) {

            CloseConnection(true);

            GetReply()->CloseConnection = CloseConnection();

            CReply::GetStockReply(m_Reply, AStatus);

            SendReply(ASendNow);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::SendReply(http_reply::status_type AStatus, LPCTSTR AContentType, bool ASendNow) {

            CloseConnection(true);

            if (AStatus == CReply::ok && m_Request != nullptr) {
                const CString &Value = m_Request->Headers.Values(_T("connection"));
                if (!Value.IsEmpty()) {
                    if (Value == _T("keep-alive"))
                        CloseConnection(false);
                }
            }

            GetReply()->CloseConnection = CloseConnection();

            CReply::GetReply(m_Reply, AStatus, AContentType);

            SendReply(ASendNow);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::SendReply(bool ASendNow) {

            GetReply()->ToBuffers(OutputBuffer());

            m_ConnectionStatus = csReplyReady;

            DoReply();

            if (ASendNow) {
                WriteAsync();
                m_ConnectionStatus = csReplySent;
                Clear();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::DoRequest() {
            if (m_OnRequest != nullptr) {
                m_OnRequest(this);
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServerConnection::DoReply() {
            if (m_OnReply != nullptr) {
                m_OnReply(this);
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPClientConnection -------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPClientConnection::CHTTPClientConnection(CPollSocketClient *AClient): CTCPClientConnection(AClient) {
            m_State = Reply::http_version_h;
            m_CloseConnection = false;
            m_ConnectionStatus = csConnected;
            m_Request = nullptr;
            m_Reply = nullptr;
            m_OnReply = nullptr;
        }
        //--------------------------------------------------------------------------------------------------------------

        CHTTPClientConnection::~CHTTPClientConnection() {
            Clear();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClientConnection::Clear() {
            m_State = Reply::http_version_h;
            FreeAndNil(m_Request);
            FreeAndNil(m_Reply);
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPClientConnection::ParseInput() {
            bool Result = false;
            if (Connected()) {
                CMemoryStream LStream(ReadAsync());
                try {
                    Result = LStream.Size() > 0;
                    if (Result) {
                        InputBuffer()->Extract(LStream.Memory(), LStream.Size());

                        CReplyContext Context = CReplyContext((LPCTSTR) LStream.Memory(), LStream.Size(), m_State);
                        const int ParseResult = CReplyParser::Parse(GetReply(), Context);

                        switch (ParseResult) {
                            case 0:
                                Tag(clock());
                                m_ConnectionStatus = csReplyError;
                                break;

                            case 1:
                                Tag(clock());
                                m_ConnectionStatus = csReplyOk;
                                DoReply();
                                break;

                            default:
                                m_ConnectionStatus = csWaitReply;
                                break;
                        }
                    }
                } catch (...) {
                    throw;
                }
            }

            return Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        CRequest *CHTTPClientConnection::GetRequest() {
            if (m_Request == nullptr) {
                m_Request = new CRequest();
                m_Request->Host = Client()->Host();
                m_Request->Port = Client()->Port();
                m_Request->UserAgent = Client()->ClientName();
            }
            return m_Request;
        }
        //--------------------------------------------------------------------------------------------------------------

        CReply *CHTTPClientConnection::GetReply() {
            if (m_Reply == nullptr) {
                m_Reply = new CReply;
            }
            return m_Reply;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClientConnection::SendRequest(bool ASendNow) {

            GetRequest()->ToBuffers(OutputBuffer());

            m_ConnectionStatus = csRequestReady;

            DoRequest();

            if (ASendNow) {
                WriteAsync();
                m_ConnectionStatus = csRequestSent;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClientConnection::DoRequest() {
            if (m_OnRequest != nullptr) {
                m_OnRequest(this);
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClientConnection::DoReply() {
            if (m_OnReply != nullptr) {
                m_OnReply(this);
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPServer -----------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPServer::CHTTPServer(unsigned short AListen, LPCTSTR lpDocRoot): CAsyncServer() {
            DefaultPort(AListen);
            SetDocRoot(lpDocRoot);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::SetDocRoot(LPCTSTR AValue) {
            if (m_sDocRoot != AValue) {
                m_sDocRoot = AValue;
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPServer::URLDecode(const CString &In, CString &Out) {
            Out.Clear();
            for (size_t i = 0; i < In.size(); ++i) {
                if (In[i] == '%') {
                    if (i + 3 <= In.size()) {
                        int value = (int) HexToDec(In.substr(i + 1, 2).c_str());
                        Out += static_cast<char>(value);
                        i += 2;
                    } else {
                        return false;
                    }
                } else if (In[i] == '+') {
                    Out += ' ';
                } else {
                    Out += In[i];
                }
            }
            return true;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::DoTimeOut(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPServerConnection *> (AHandler->Binding());
            try {
                if (LConnection->ConnectionStatus() >= csRequestOk) {
                    if (LConnection->ConnectionStatus() == csRequestOk) {
                        LConnection->SendStockReply(CReply::gateway_timeout, true);
                    }
                    LConnection->Disconnect();
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                LConnection->Disconnect();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::DoAccept(CPollEventHandler *AHandler) {
            CIOHandlerSocket *LIOHandler = nullptr;
            CPollEventHandler *LEventHandler = nullptr;
            CHTTPServerConnection *LConnection = nullptr;

            try {
                LIOHandler = (CIOHandlerSocket *) IOHandler()->Accept(AHandler->Socket(), SOCK_NONBLOCK);

                if (Assigned(LIOHandler)) {
                    LConnection = new CHTTPServerConnection(this);

                    LConnection->OnDisconnected(std::bind(&CHTTPServer::DoDisconnected, this, _1));
                    LConnection->OnReply(std::bind(&CHTTPServer::DoReply, this, _1));

                    LConnection->IOHandler(LIOHandler);

                    LIOHandler->AfterAccept();

                    LEventHandler = m_EventHandlers->Add(LIOHandler->Binding()->Handle());
                    LEventHandler->Binding(LConnection);
                    LEventHandler->Start(etIO);

                    DoConnected(LConnection);
                } else {
                    throw ETCPServerError(_T("TCP Server Error..."));
                }
            } catch (Delphi::Exception::Exception &E) {
                delete LConnection;
                DoListenException(&E);
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::DoRead(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPServerConnection *> (AHandler->Binding());
            try {
                if (LConnection->ParseInput()) {
                    switch (LConnection->ConnectionStatus()) {
                        case csRequestError:
                            LConnection->SendStockReply(CReply::bad_request);
                            LConnection->Clear();
                            break;

                        case csRequestOk:
                            DoExecute(LConnection);
                            break;

                        default:
                            break;
                    }
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                LConnection->Disconnect();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::DoWrite(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPServerConnection *> (AHandler->Binding());
            try {
                if (LConnection->WriteAsync()) {
                    if (LConnection->ConnectionStatus() == csReplyReady) {

                        LConnection->ConnectionStatus(csReplySent);
                        LConnection->Clear();

                        if (LConnection->CloseConnection()) {
                            LConnection->Disconnect();
                        }
                    }
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                LConnection->Disconnect();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPServer::DoCommand(CTCPConnection *AConnection) {
            auto LConnection = dynamic_cast<CHTTPServerConnection *> (AConnection);
            auto LRequest = LConnection->Request();

            bool Result = CommandHandlers()->Count() > 0;

            if (Result) {
                DoBeforeCommandHandler(AConnection, LRequest->Method.c_str());
                try {
                    int i;
                    for (i = 0; i < CommandHandlers()->Count(); ++i) {
                        if (CommandHandlers()->Commands(i)->Enabled()) {
                            if (CommandHandlers()->Commands(i)->Check(LRequest->Method.c_str(),
                                                                      LRequest->Method.Size(), AConnection))
                                break;
                        }
                    }

                    if (i == CommandHandlers()->Count())
                        DoNoCommandHandler(LRequest->Method.c_str(), AConnection);
                } catch (Delphi::Exception::Exception &E) {
                    DoException(AConnection, &E);
                }
                DoAfterCommandHandler(AConnection);
            }

            return Result;
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPServer::DoExecute(CTCPConnection *AConnection) {
            if (m_OnExecute != nullptr) {
                return m_OnExecute(AConnection);
            }
            return DoCommand(AConnection);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPServer::DoReply(CObject *Sender) {
            DoAccessLog(dynamic_cast<CHTTPServerConnection *> (Sender));
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPClient -----------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPClient::CHTTPClient(): CAsyncClient() {

        }
        //--------------------------------------------------------------------------------------------------------------

        CHTTPClient::CHTTPClient(LPCTSTR AHost, unsigned short APort): CHTTPClient() {
            m_Host = AHost;
            m_Port = APort;
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClient::DoConnectStart(CIOHandlerSocket *AIOHandler, CPollEventHandler *AHandler) {
            auto LConnection = new CHTTPClientConnection(this);
            LConnection->IOHandler(AIOHandler);
            AHandler->Binding(LConnection, true);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClient::DoConnect(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPClientConnection *> (AHandler->Binding());

            if (LConnection == nullptr) {
                AHandler->Stop();
                return;
            }

            try {
                auto LIOHandler = (CIOHandlerSocket *) LConnection->IOHandler();

                if (LIOHandler->Binding()->CheckConnection()) {
                    LConnection->OnDisconnected(std::bind(&CHTTPClient::DoDisconnected, this, _1));
                    AHandler->Start(etIO);
                    DoConnected(LConnection);
                }
            } catch (Exception::Exception &E) {
                DoException(LConnection, &E);
                AHandler->Stop();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClient::DoRead(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPClientConnection *> (AHandler->Binding());
            try {
                if (LConnection->ParseInput()) {
                    switch (LConnection->ConnectionStatus()) {
                        case csReplyError:
                            LConnection->Clear();
                            break;

                        case csReplyOk:
                            DoExecute(LConnection);
                            LConnection->Clear();

                            if (LConnection->CloseConnection()) {
                                LConnection->Disconnect();
                            }

                            break;

                        default:
                            break;
                    }
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                LConnection->Disconnect();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPClient::DoWrite(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPClientConnection *> (AHandler->Binding());
            try {
                if (LConnection->WriteAsync()) {
                    if (LConnection->ConnectionStatus() == csRequestReady) {
                        LConnection->ConnectionStatus(csRequestSent);
                    }
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                LConnection->Disconnect();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        bool CHTTPClient::DoExecute(CTCPConnection *AConnection) {
            if (m_OnExecute != nullptr) {
                return m_OnExecute(AConnection);
            }
            return DoCommand(AConnection);
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPProxy ------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPProxy::CHTTPProxy(CHTTPServerConnection *AConnection, CHTTPProxyManager *AManager):
                CCollectionItem(AManager), CHTTPClient() {

            m_Request = nullptr;

            m_Connection = AConnection;
            m_ClientName = Server()->ServerName();

            SetPollStack(Server()->PollStack());
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPProxy::DoConnectStart(CIOHandlerSocket *AIOHandler, CPollEventHandler *AHandler) {
            auto LConnection = new CHTTPClientConnection(this);
            LConnection->IOHandler(AIOHandler);
            AHandler->Binding(LConnection, true);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPProxy::DoConnect(CPollEventHandler *AHandler) {
            auto LConnection = dynamic_cast<CHTTPClientConnection *> (AHandler->Binding());

            if (LConnection == nullptr) {
                AHandler->Stop();
                return;
            }

            try {
                auto LIOHandler = (CIOHandlerSocket *) LConnection->IOHandler();

                if (LIOHandler->Binding()->CheckConnection()) {
                    LConnection->OnDisconnected(std::bind(&CHTTPProxy::DoDisconnected, this, _1));
                    AHandler->Start(etIO);
                    DoConnected(LConnection);
                    DoRequest(LConnection);
                }
            } catch (Delphi::Exception::Exception &E) {
                DoException(LConnection, &E);
                AHandler->Stop();
            }
        }
        //--------------------------------------------------------------------------------------------------------------

        void CHTTPProxy::DoRequest(CHTTPClientConnection *AConnection) {
            auto LRequest = AConnection->Request();
            *LRequest = *m_Request;
            AConnection->SendRequest(true);
        }
        //--------------------------------------------------------------------------------------------------------------

        CRequest *CHTTPProxy::GetRequest() {
            if (m_Request == nullptr) {
                m_Request = new CRequest();
                m_Request->Host = Host();
                m_Request->Port = Port();
                m_Request->UserAgent = ClientName();
            }

            return m_Request;
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CHTTPProxyManager -----------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CHTTPProxy *CHTTPProxyManager::Add(CHTTPServerConnection *AConnection) {
            return new CHTTPProxy(AConnection, this);
        }
        //--------------------------------------------------------------------------------------------------------------

    }
}
}
