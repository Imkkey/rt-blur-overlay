#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <vector>
#include <array>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "user32.lib")

using Microsoft::WRL::ComPtr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    const wchar_t* cls = L"BlurOverlay";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = cls;
    RegisterClass(&wc);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        cls, L"", WS_POPUP, 0, 0, width, height, nullptr, nullptr, hInst, nullptr);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ShowWindow(hwnd, SW_SHOW);

    // Init D3D11 device
    D3D_FEATURE_LEVEL fl;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION, &device, &fl, &context);

    // Duplication
    ComPtr<IDXGIFactory1> factory;
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
    ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);
    ComPtr<IDXGIOutput> output;
    adapter->EnumOutputs(0, &output);
    ComPtr<IDXGIOutput1> output1;
    output.As(&output1);
    ComPtr<IDXGIOutputDuplication> dupl;
    output1->DuplicateOutput(device.Get(), &dupl);

    // Ring buffer of 4 textures
    std::array<ComPtr<ID3D11Texture2D>, 4> ring;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    desc.SampleDesc.Count = 1;
    for (auto& tex : ring) device->CreateTexture2D(&desc, nullptr, &tex);
    ComPtr<ID3D11Texture2D> staging;
    desc.BindFlags = 0;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    device->CreateTexture2D(&desc, nullptr, &staging);

    // Load compute shader
    ComPtr<ID3DBlob> csBlob;
    D3DCompileFromFile(L"..\\src\\BlurCS.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", 0, 0, &csBlob, nullptr);
    ComPtr<ID3D11ComputeShader> cs;
    device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &cs);

    // Prev frame index
    UINT idx = 0;

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DXGI_OUTDUPL_FRAME_INFO fi;
        ComPtr<IDXGIResource> res;
        if (SUCCEEDED(dupl->AcquireNextFrame(0, &fi, &res))) {
            ComPtr<ID3D11Texture2D> frame;
            res.As(&frame);
            context->CopyResource(ring[idx].Get(), frame.Get());
            dupl->ReleaseFrame();
        }

        idx = (idx + 1) % ring.size();
        // Dispatch compute shader using ring buffer (not fully implemented)
        context->CSSetShader(cs.Get(), nullptr, 0);
        context->Dispatch((width + 7) / 8, (height + 7) / 8, 1);
    }
    return 0;
}
