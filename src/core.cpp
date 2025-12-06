#include <string>
#include <vector>

#include <windows.h>
#include <dshow.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "oleaut32.lib")

namespace py = pybind11;

struct Resolution
{
    int width;
    int height;
};

struct CaptureDevice
{
    int id;
    std::string name;
    std::vector<Resolution> resolutions;
};

std::string get_device_name(IMoniker *pMoniker)
{
    std::string name = "Unknown";
    IPropertyBag *pPropBag = nullptr;
    HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
    if (FAILED(hr))
    {
        return name;
    }

    VARIANT var;
    VariantInit(&var);
    hr = pPropBag->Read(L"FriendlyName", &var, 0);
    if (FAILED(hr))
    {
        VariantClear(&var);
        pPropBag->Release();
        return name;
    }

    wchar_t *wname = var.bstrVal;
    char cname[256];
    WideCharToMultiByte(CP_UTF8, 0, wname, -1, cname, sizeof(cname), nullptr, nullptr);
    name = cname;

    VariantClear(&var);
    pPropBag->Release();
}

std::vector<Resolution> get_device_resolutions(IMoniker *pMoniker)
{
    std::vector<Resolution> resolutions;
    IBaseFilter *pFilter = nullptr;
    HRESULT hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pFilter);
    if (SUCCEEDED(hr))
    {
        IEnumPins *pEnumPins = nullptr;
        hr = pFilter->EnumPins(&pEnumPins);
        if (SUCCEEDED(hr))
        {
            IPin *pPin = nullptr;
            while (pEnumPins->Next(1, &pPin, nullptr) == S_OK)
            {
                IAMStreamConfig *pConfig = nullptr;
                hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pConfig);
                if (SUCCEEDED(hr))
                {
                    int count = 0, size = 0;
                    hr = pConfig->GetNumberOfCapabilities(&count, &size);
                    if (SUCCEEDED(hr))
                    {
                        for (int i = 0; i < count; ++i)
                        {
                            AM_MEDIA_TYPE *pmt = nullptr;
                            std::vector<BYTE> caps(size);
                            hr = pConfig->GetStreamCaps(i, &pmt, caps.data());
                            if (SUCCEEDED(hr) && pmt->formattype == FORMAT_VideoInfo)
                            {
                                VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)pmt->pbFormat;
                                int w = vih->bmiHeader.biWidth;
                                int h = vih->bmiHeader.biHeight;
                                Resolution res{w, h};
                                // 重複排除
                                bool exists = false;
                                for (const auto &r : resolutions)
                                {
                                    if (r.width == res.width && r.height == res.height)
                                    {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (!exists)
                                {
                                    resolutions.push_back(res);
                                }
                            }
                            if (pmt)
                            {
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
                    }
                    pConfig->Release();
                }
                pPin->Release();
            }
            pEnumPins->Release();
        }
        pFilter->Release();
    }
    return resolutions;
}

std::vector<CaptureDevice> list_devices()
{
    std::vector<CaptureDevice> devices;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return devices;
    }

    ICreateDevEnum *pDevEnum = nullptr;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
    if (FAILED(hr))
    {
        CoUninitialize();
        return devices;
    }

    IEnumMoniker *pEnum = nullptr;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_OK && pEnum)
    {
        IMoniker *pMoniker = nullptr;
        int id = 0;
        while (pEnum->Next(1, &pMoniker, nullptr) == S_OK)
        {
            std::string name = get_device_name(pMoniker);
            std::vector<Resolution> resolutions = get_device_resolutions(pMoniker);
            devices.push_back(CaptureDevice{id++, name, resolutions});
            pMoniker->Release();
        }
        pEnum->Release();
    }
    pDevEnum->Release();
    CoUninitialize();
    return devices;
}

PYBIND11_MODULE(core, m)
{
    m.doc() = "Windows Capture Device List Module";

    py::class_<Resolution>(m, "Resolution")
        .def(py::init<int, int>())
        .def_readonly("width", &Resolution::width)
        .def_readonly("height", &Resolution::height);

    py::class_<CaptureDevice>(m, "CaptureDevice")
        .def(py::init<int, std::string, std::vector<Resolution>>())
        .def_readonly("id", &CaptureDevice::id)
        .def_readonly("name", &CaptureDevice::name)
        .def_readonly("resolutions", &CaptureDevice::resolutions);

    m.def("list_devices", &list_devices, "List video capture devices");
}
