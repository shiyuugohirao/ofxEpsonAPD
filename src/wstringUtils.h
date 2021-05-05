// wstringUtils.h
// https://gist.github.com/shiyuugohirao/5fb074dd346f96945136537bc6e2e7b9
#pragma once
#include <stringapiset.h> 
#include <WinBase.h>
#include <system_error>
#include <vector>

static std::wstring to_wstring(std::string const& src)
{
    auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);
    std::vector<wchar_t> dest(dest_size, L'\0');
    if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size()) == 0) {
        throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
    }
    dest.resize(std::char_traits<wchar_t>::length(dest.data()));
    dest.shrink_to_fit();
    return std::wstring(dest.begin(), dest.end());
}

static std::string to_string(std::wstring const& src)
{
    auto const dest_size = ::WideCharToMultiByte(CP_ACP, 0U, src.data(), -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> dest(dest_size, '\0');
    if (::WideCharToMultiByte(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size(), nullptr, nullptr) == 0) {
        throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
    }
    dest.resize(std::char_traits<char>::length(dest.data()));
    dest.shrink_to_fit();
    return std::string(dest.begin(), dest.end());
}