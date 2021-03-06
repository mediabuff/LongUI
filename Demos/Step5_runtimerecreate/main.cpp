﻿#define LONGUI_WITH_DEFAULT_HEADER
#define _CRT_SECURE_NO_WARNINGS
#include "LongUI.h"
#include "../Step3_handleeventex/demo.h"
#include "LongUI/luiUiDConf.h"

#include <vector>

const wchar_t* const CPU_ADAPTER = L"Microsoft Basic Render Driver";

// <M><Text text="HW"/></M>
const char* const VIDEO_CARD_XML =
u8R"xml(<?xml version="1.0" encoding="utf-8"?>
<Window debugshow="true" textantimode="cleartype" size="512, 64" clearcolor="1,1,1,0.9" titlename="Choose Video Card">
    <HorizontalLayout>
        <ComboBox name="cbb_vc" align="left"  margin="4,4,4,4" borderwidth="1">
            <List linetemplate="Text">
                <ScrollBarA marginal="right"/>
            </List>
        </ComboBox>
        <Button borderwidth="1" weight="0.2" margin="4,4,4,4" name="btn_ok" text="OK!"/>
        <Button borderwidth="1" weight="0.4" margin="4,4,4,4" name="btn_re" text="Recreate!"/>
    </HorizontalLayout>
</Window>
)xml";

// longui::demo namespace
LONGUI_NAMESPACE_BEGIN namespace Demo {
    // config
    class MyConfig final : public CUIDefaultConfigure {
        // super class
        using Super = CUIDefaultConfigure;
    public:
        // ctor
        MyConfig() : Super(UIManager, "log.log") { }
        // add/release
        LONGUI_BASIC_INTERFACE_IMPL;
        // 获取控件模板
        //auto GetTemplateString() noexcept ->const char* override { return TEMPLATE_XML; }
        // return true, if use cpu rendering
        virtual auto GetConfigureFlag() noexcept ->ConfigureFlag override { return Flag_OutputDebugString; }
        // choose it
        virtual auto ChooseAdapter(const DXGI_ADAPTER_DESC1 adapters[], const size_t length) noexcept->size_t override;
        // adapters
        std::vector<DXGI_ADAPTER_DESC1>     adapter_vector;
    };
    // window to choose adapter
    class WindowChooseAdapter final : public UIViewport {
        // super class
        using Super = UIViewport;
        // clean up
        virtual void cleanup() noexcept override { this->before_deleted(); delete this; }
    public:
        // on close event, do not exit app directly
        virtual void OnClose() noexcept override {
            // exit on one window
            if (UIManager.GetSystemWindowCount() == 1) {
                UIManager.Exit();
            }
        };
        // ctor
        WindowChooseAdapter(XUIBaseWindow* window) : Super(window) {}
    public:
        // set config
        void SetMyConfig(MyConfig& config) noexcept;
    };
}
LONGUI_NAMESPACE_END

// Entry for App
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow) {
    // every windows desktop app should do this
    ::HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    // use OleInitialize to init ole and com
    if (SUCCEEDED(::OleInitialize(nullptr))) {
        // my config
        LongUI::Demo::MyConfig config;
        // init longui manager
        if (SUCCEEDED(UIManager.Initialize(&config))) {
            // my style
            UIManager << DL_Hint << L"Battle Control Online!" << LongUI::endl;
            // create main window, return nullptr for some error
            auto svc_window = UIManager.CreateUIWindow<LongUI::Demo::WindowChooseAdapter>(VIDEO_CARD_XML);
            // successed
            if (svc_window) {
                auto view = reinterpret_cast<LongUI::Demo::WindowChooseAdapter*>(svc_window->GetViewport());
                view->SetMyConfig(config);
            }
            // some error
            else {
                // TODO: error handle
            }
            // run this app
            UIManager.Run();
            // my style
            UIManager << DL_Hint << L"Battle Control Terminated!" << LongUI::endl;
            // cleanup longui
            UIManager.Uninitialize();
        }
        // cleanup ole and com
        ::OleUninitialize();
    }
    // exit
    return EXIT_SUCCESS;
}


// ChooseAdapter
auto LongUI::Demo::MyConfig::ChooseAdapter(const DXGI_ADAPTER_DESC1 adapters[], const size_t length) noexcept -> size_t {
    // first call
    if (adapter_vector.empty()) {
        try {
            adapter_vector.assign(adapters, adapters + length);
        }
        catch (const std::exception& exp) {
            auto info = exp.what();
            assert(!info);
        }
        // CPU renderer first
        for (size_t i = 0; i < length; ++i) {
            if (!std::wcscmp(CPU_ADAPTER, adapters[i].Description))
                return i;
        }
    }
    // stored infomation
    else {
        for (size_t i = 0; i < length; ++i) {
            if (!std::wcscmp(adapter_vector.back().Description, adapters[i].Description))
                return i;
        }
    }
    return length;
}

// reauire UIList control
#include <Control/UIComboBox.h>

// set MyConfig
void LongUI::Demo::WindowChooseAdapter::SetMyConfig(MyConfig& config) noexcept {
    // find list that named "lst_vc"
    auto combo = longui_cast<UIComboBox*>(m_pWindow->FindControl("cbb_vc"));
    // if found
    if (combo) {
        // add infomation to list via 'adapter_vector'
        for (const auto& desc : config.adapter_vector) {
            combo->PushItem(desc.Description);
        }
        // set select-index to 0
        combo->SetSelectedIndex(0);
        // find button that named "btn_ok"
        auto btn = m_pWindow->FindControl("btn_ok");
        // if found
        if (btn) {
            // I wanna capture 'adapter_vector'
            auto& vec = config.adapter_vector;
            // add event callback
            btn->AddEventCall([&vec, combo, this](UIControl*) noexcept ->bool {
                // if selectED
                auto sel = combo->GetSelectedIndex();
                if (sel < vec.size()) {
                    try {
                        vec.push_back(vec[sel]);
                    }
                    catch (...) {
                        ::MessageBoxW(m_pWindow->GetHwnd(), L"ERROR", L"ERROR", MB_ICONERROR);
                    }
                }
                else {
                    ::MessageBoxW(m_pWindow->GetHwnd(), L"You may choose one adapter", L"HINT", MB_OK);
                }
                // recreate
                UIManager.RecreateResources();
                // create main window
                UIManager.CreateUIWindow<LongUI::Demo::MainViewport>(DEMO_XML)->ShowWindow(SW_SHOW);
                // close this window 
                m_pWindow->Close();
                return true;
            }, SubEvent::Event_ItemClicked);
        }
    }
    // find button that named "btn_re"
    auto btn = m_pWindow->FindControl("btn_re");
    // if found
    if (btn) {
        // add event callback
        btn->AddEventCall([](UIControl*) noexcept ->bool {
            // recreate
            UIManager.RecreateResources();
            return true;
        }, SubEvent::Event_ItemClicked);
    }
    // show the window
    m_pWindow->ShowWindow(SW_SHOW);
}

/*

            <ListLine name="lin1">
                <Text text="1" name="listline1-1"/>
                <Text text="伍湖"/>
            </ListLine>
            <ListLine name="lin2">
                <Text text="2" name="listline2-1"/>
                <Text text="亖死"/>
            </ListLine>
            <ListLine name="lin3">
                <Text text="3" name="listline3-1"/>
                <Text text="三妻"/>
            </ListLine>
            <ListLine name="lin4">
                <Text text="5" name="listline4-1"/>
                <Text text="二连"/>
            </ListLine>
            <ListLine name="lin5">
                <Text text="4" name="listline5-1"/>
                <Text text="一瞬"/>
            </ListLine>
            <ListLine>
                <Text text="4"/>
                <Text text="一瞬"/>
            </ListLine>
            <ListLine>
                <Text text="4"/>
                <Text text="一瞬"/>
            </ListLine>
            <ListLine>
                <Text text="4"/>
                <Text text="一瞬"/>
            </ListLine>
            <ListLine>
                <Text text="4"/>
                <Text text="一瞬"/>
            </ListLine>
*/