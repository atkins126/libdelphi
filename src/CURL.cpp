/*++

Library name:

  libdelphi

Module Name:

  CURL.cpp

Notices:

  Delphi classes for C++

Author:

  Copyright (c) Prepodobny Alen

  mailto: alienufo@inbox.ru
  mailto: ufocomp@gmail.com

--*/

#include "delphi.hpp"
#include "delphi/CURL.hpp"

extern "C++" {

namespace Delphi {

    namespace cURL {

        static unsigned long GURLCount = 0;

        //--------------------------------------------------------------------------------------------------------------

        //-- CCurlComponent --------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CCurlComponent::CCurlComponent() {
            if (GURLCount == 0) {
                curl_global_init(CURL_GLOBAL_DEFAULT);
            }
            GURLCount++;
        }
        //--------------------------------------------------------------------------------------------------------------

        CCurlComponent::~CCurlComponent() {
            GURLCount--;
            if (GURLCount == 0) {
                curl_global_cleanup();
            }
        }

        //--------------------------------------------------------------------------------------------------------------

        //-- CCurlApi --------------------------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------------------------------------

        CCurlApi::CCurlApi() {
            m_curl = nullptr;
            m_Headers.NameValueSeparator(": ");

            Init();
        }
        //--------------------------------------------------------------------------------------------------------------

        CCurlApi::~CCurlApi() {
            Cleanup();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CCurlApi::Init() {
            m_curl = curl_easy_init();
        }
        //--------------------------------------------------------------------------------------------------------------

        void CCurlApi::Cleanup() {
            if (m_curl != nullptr)
                curl_easy_cleanup(m_curl);
        }
        //--------------------------------------------------------------------------------------------------------------

        size_t CCurlApi::CallBack(void *content, size_t size, size_t nmemb, CString *Buffer) {
            size_t buffer_size = nmemb * size;
            Buffer->Append((LPCTSTR) content, buffer_size);
            return buffer_size;
        }
        //--------------------------------------------------------------------------------------------------------------

        size_t CCurlApi::HeaderCallBack(char *buffer, size_t size, size_t nitems, CStringList *Headers) {
            size_t buffer_size = nitems * size;
            Headers->Add(CString((LPCTSTR) buffer, buffer_size));
            return buffer_size;
        }
        //--------------------------------------------------------------------------------------------------------------

        CString CCurlApi::GetErrorMessage(CURLcode code) {
            return curl_easy_strerror(code);
        }
        //--------------------------------------------------------------------------------------------------------------

        void CCurlApi::Reset() const {
            m_Headers.Clear();
            m_Result.Clear();
            curl_easy_reset(m_curl);
        }
        //--------------------------------------------------------------------------------------------------------------

        CURLcode CCurlApi::Get(const CLocation &URL, const CStringList &Headers) const {
            return Send(URL, "GET", CString(), Headers);
        }
        //--------------------------------------------------------------------------------------------------------------

        CURLcode CCurlApi::Post(const CLocation &URL, const CString &Content, const CStringList &Headers) const {
            return Send(URL, "POST", Content, Headers);
        }
        //--------------------------------------------------------------------------------------------------------------

        CURLcode CCurlApi::Send(const CLocation &URL, const CString &Method, const CString &Content, const CStringList &Headers) const {
            CURLcode code = CURLE_SEND_ERROR;

            if (m_curl) {
                Reset();

                curl_easy_setopt(m_curl, CURLOPT_URL, URL.href().c_str());
                curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, CCurlApi::HeaderCallBack);
                curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, &m_Headers);
                curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CCurlApi::CallBack);
                curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_Result);
                curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, FALSE);
#ifdef _DEBUG
                curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif

                if (Headers.Count() > 0) {
                    struct curl_slist *chunk = nullptr;
                    for ( int i = 0; i < Headers.Count(); i++ ) {
                        chunk = curl_slist_append(chunk, Headers[i].c_str());
                    }

                    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, chunk);
                }

                if (Method == "GET") {

                    curl_easy_setopt(m_curl, CURLOPT_HTTPGET, TRUE);

                } else if (Method == "POST" || Method == "PUT" || Method == "DELETE") {

                    if (Method == "PUT" || Method == "DELETE") {
                        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, Method.c_str() );
                    } else {
                        curl_easy_setopt(m_curl, CURLOPT_POST, TRUE);

                        if (!Content.IsEmpty()) {
                            curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, Content.c_str());
                        }
                    }
                }

                code = curl_easy_perform(m_curl);

                if (code == CURLE_OK) {
                    CurlInfo();
                }
            }

            return code;
        }
        //--------------------------------------------------------------------------------------------------------------

    }
}
}
