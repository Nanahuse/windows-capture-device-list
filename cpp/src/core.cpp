#include <string>
#include <vector>
#include <algorithm>

#include <windows.h>
#include <dshow.h>

#include "capture_device.h"
#include "resolution.h"
#include "scope_guard.hpp"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "oleaut32.lib")

std::string get_device_name(IMoniker *pMoniker)
{
    ScopeGuard scope_guard;

    IPropertyBag *pPropBag = nullptr;
    if (FAILED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag)))
    {
        return "Unknown";
    }
    scope_guard.add_release(pPropBag);

    VARIANT var;
    VariantInit(&var);
    if (FAILED(pPropBag->Read(L"FriendlyName", &var, 0)))
    {
        VariantClear(&var);
        return "Unknown";
    }

    wchar_t *wname = var.bstrVal;
    char cname[256];
    WideCharToMultiByte(CP_UTF8, 0, wname, -1, cname, sizeof(cname), nullptr, nullptr);
    std::string name = cname;

    VariantClear(&var);
    return name;
}

std::vector<Resolution> get_device_resolutions(IMoniker *pMoniker)
{
    ScopeGuard scope_guard;

    IBaseFilter *pFilter = nullptr;
    if (FAILED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pFilter)))
    {
        return {};
    }
    scope_guard.add_release(pFilter);

    IEnumPins *pEnumPins = nullptr;
    if (FAILED(pFilter->EnumPins(&pEnumPins)))
    {
        return {};
    }
    scope_guard.add_release(pEnumPins);

    std::vector<Resolution> resolutions;

    IPin *pPin = nullptr;
    while (pEnumPins->Next(1, &pPin, nullptr) == S_OK)
    {
        ScopeGuard loop_guard;

        loop_guard.add_release(pPin);

        IAMStreamConfig *pConfig = nullptr;
        if (FAILED(pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pConfig)))
        {
            continue;
        }
        loop_guard.add_release(pConfig);

        int count = 0, size = 0;
        if (FAILED(pConfig->GetNumberOfCapabilities(&count, &size)))
        {
            continue;
        }

        for (int i = 0; i < count; ++i)
        {
            AM_MEDIA_TYPE *pmt = nullptr;
            std::vector<BYTE> caps(size);

            if (FAILED(pConfig->GetStreamCaps(i, &pmt, caps.data())))
            {
                continue;
            }

            if (pmt->formattype == FORMAT_VideoInfo)
            {
                VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)pmt->pbFormat;
                int w = vih->bmiHeader.biWidth;
                int h = vih->bmiHeader.biHeight;
                Resolution res{w, h};
                if (std::none_of(resolutions.begin(), resolutions.end(), [&](const Resolution &r)
                                 { return r == res; }))
                {
                    resolutions.push_back(res);
                }
            }

            if (pmt->cbFormat != 0)
            {
                CoTaskMemFree((PVOID)pmt->pbFormat);
                pmt->pbFormat = nullptr;
            }
            if (pmt->pUnk != nullptr)
            {
                pmt->pUnk->Release();
                pmt->pUnk = nullptr;
            }
            CoTaskMemFree(pmt);
        }
    }

    return resolutions;
}

std::vector<CaptureDevice> list_devices()
{
    ScopeGuard scope_guard;

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        return {};
    }
    scope_guard.add(&CoUninitialize);

    ICreateDevEnum *pDevEnum = nullptr;
    if (FAILED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum))))
    {
        return {};
    }
    scope_guard.add_release(pDevEnum);

    IEnumMoniker *pEnum = nullptr;
    if (!(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0) == S_OK && pEnum))
    {
        return {};
    }
    scope_guard.add_release(pEnum);

    std::vector<CaptureDevice> devices;

    IMoniker *pMoniker = nullptr;
    int id = 0;
    while (pEnum->Next(1, &pMoniker, nullptr) == S_OK)
    {
        std::string name = get_device_name(pMoniker);
        std::vector<Resolution> resolutions = get_device_resolutions(pMoniker);
        devices.push_back(CaptureDevice{id++, name, resolutions});
        pMoniker->Release();
    }
    return devices;
}
