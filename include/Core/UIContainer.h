﻿#pragma once
/**
* Copyright (c) 2014-2015 dustpg   mailto:dustpg@gmail.com
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

// LongUI namespace
namespace LongUI {
#if 1
    // base container control class -- 基本容器类
    class UIContainer : public UIMarginalable {
        // 父类申明
        using Super = UIMarginalable;
#else
    // base container control class
    class UIContainer : public UIControl {
        // 父类申明
        using Super = UIControl;
#endif
    public:
        // render this
        virtual void Render(RenderType) const noexcept override;
        // update this
        virtual void Update() noexcept override;
        // do event
        virtual bool DoEvent(const LongUI::EventArgument&) noexcept override;
        // recreate this
        virtual auto Recreate() noexcept->HRESULT override;
        // find control by mouse point
        virtual auto FindControl(const D2D1_POINT_2F& pt) noexcept->UIControl* override;
    public:
        // refresh layout
        //virtual void RefreshLayout() noexcept {};
        // push back
        virtual void PushBack(UIControl* child) noexcept = 0;
        // just remove 
        virtual void RemoveJust(UIControl* child) noexcept = 0;
        // remove and clean child
        void RemoveClean(UIControl* child) noexcept { this->RemoveJust(child); child->Cleanup(); }
    public:
        // ctor
        UIContainer(pugi::xml_node node) noexcept;
        // before update
        void BeforeUpdateContainer() noexcept;
    public:
        // get length/count of children
        auto GetLength() const noexcept { return m_cChildrenCount; }
        // get length/count of children
        auto GetCount() const noexcept { return m_cChildrenCount; }
    public: 
        // get content width - zoomed
        auto GetContentWidthZoomed() const noexcept { return m_2fContentSize.width / m_2fZoom.width; }
        // get content height - zoomed
        auto GetContentHeightZoomed() const noexcept { return m_2fContentSize.height / m_2fZoom.height; }
        // get view.width - zoomed
        auto GetViewWidthZoomed() const noexcept { return this->view_size.width / m_2fZoom.width; }
        // get view.height - zoomed
        auto GetViewHeightZoomed() const noexcept { return this->view_size.height / m_2fZoom.height; }
        // get offset.x - zoomed
        auto GetOffsetXZoomed() const noexcept { return m_2fOffset.x / m_2fZoom.width; }
        // get offset.y - zoomed
        auto GetOffsetYZoomed() const noexcept { return m_2fOffset.y / m_2fZoom.height; }
        // set offset.x -zoomed
        void SetOffsetXZoomed(float value) noexcept { return this->SetOffsetX(value * m_2fZoom.width); }
        // set offset.y - zoomed
        void SetOffsetYZoomed(float value) noexcept { return this->SetOffsetY(value * m_2fZoom.height); }
        // set offset -zoomed
        auto SetOffsetZoomed(int xy, float value) noexcept { return xy ? SetOffsetYZoomed(value) : SetOffsetXZoomed(value); }
    public:
        // get content width - without zooming
        auto GetContentWidth() const noexcept { return m_2fContentSize.width ; }
        // get content height - without zooming
        auto GetContentHeight() const noexcept { return m_2fContentSize.height ; }
        // get view.width - without zooming
        auto GetViewWidth() const noexcept { return this->view_size.width; }
        // get view.height - without zooming
        auto GetViewHeight() const noexcept { return this->view_size.height; }
        // get offset.x - without zooming
        auto GetOffsetX() const noexcept { return m_2fOffset.x ; }
        // get offset.y - without zooming
        auto GetOffsetY() const noexcept { return m_2fOffset.y; }
        // set offset.x - without zooming
        void SetOffsetX(float value) noexcept;
        // set offset.y - without zooming
        void SetOffsetY(float value) noexcept;
        // set offset - without zooming
        auto SetOffset(int xy, float value) noexcept { return xy ? SetOffsetY(value) : SetOffsetX(value); }
        // get zoom in x
        auto GetZoomX() const noexcept { return m_2fZoom.width; }
        // get zoom in y
        auto GetZoomY() const noexcept { return m_2fZoom.height; }
        // get zoom 
        auto GetZoom(int xy) const noexcept { return xy ? this->GetZoomY() : this->GetZoomX(); }
    private:
    protected:
        // dtor
        ~UIContainer() noexcept;
        // refresh marginal controls
        void refresh_marginal_controls() noexcept;
        // do render for child
        static void child_do_render(const UIControl* ctrl) noexcept;
        // after insert
        void after_insert(UIControl* child) noexcept;
    public:
        // marginal controls
        UIMarginalable* const   marginal_control[UIMarginalable::MARGINAL_CONTROL_SIZE];
    protected:
        // count of children, just make "GetLength/Count" to faster
        size_t                  m_cChildrenCount = 0;
        // orginal margin
        D2D1_RECT_F             m_orgMargin = D2D1::RectF();
        // template size
        D2D1_SIZE_F             m_2fTemplateSize = D2D1::SizeF(0.f, 0.f);
        // offset position
        D2D1_POINT_2F           m_2fOffset = D2D1::Point2F();
        // size of content
        D2D1_SIZE_F             m_2fContentSize = D2D1::SizeF(0.f, 0.f);
        // zoom size
        D2D1_SIZE_F             m_2fZoom = D2D1::SizeF(1.f, 1.f);
        // assert if exist marginal control
    public:
#ifdef _DEBUG
        void AssertMarginalControl() const noexcept {
            if (!(this->flags & Flag_Container_ExistMarginalControl)) {
                for (auto ctrl : this->marginal_control) {
                    assert(!ctrl && "exist marginal control");
                }
            }
        }
#else
        void AssertMarginalControl() const noexcept {};
#endif
#ifdef LongUIDebugEvent
    protected:
        // debug infomation
        virtual bool debug_do_event(const LongUI::DebugEventInformation&) const noexcept override;
    };
    // 重载?特例化 GetIID
    template<> LongUIInline const IID& GetIID<LongUI::UIContainer>() {
        // {30523BF0-4170-4F2F-9FF6-7946A2B8BEEB}
        static const GUID IID_LongUI_UIContainer = { 
            0x30523bf0, 0x4170, 0x4f2f,{ 0x9f, 0xf6, 0x79, 0x46, 0xa2, 0xb8, 0xbe, 0xeb }
        };
        return IID_LongUI_UIContainer;
    }
#else
    };
#endif
    // XXX: top level?
    LongUIInline auto UIControl::IsTopLevel() const noexcept { return static_cast<UIControl*>(this->parent) == this; }
}