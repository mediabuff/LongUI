﻿#pragma once
/**
* Copyright (c) 2014-2016 dustpg   mailto:dustpg@gmail.com
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

#include "UIContainer.h"

// LongUI namespace
namespace LongUI {
    // Buld-In container control class
    class UIContainerBuiltIn : public UIContainer {
        // super class
        using Super = UIContainer;
    protected:
        // something must do before deleted
        void before_deleted() noexcept;
        // ctor
        UIContainerBuiltIn(UIContainer* cp) noexcept : Super(cp) {}
        // ctor
        void initialize(pugi::xml_node node) noexcept { Super::initialize(node); }
        // init without xml-node
        void initialize() noexcept { return Super::initialize(); }
        // dtor
        ~UIContainerBuiltIn() noexcept {}
    public:
        // itr 迭代器
        class Iterator {
        public:
            // operator =
            auto& operator=(const Iterator& itr) noexcept { m_pControl = itr.m_pControl; return *this; }
            // ctor
            Iterator(UIControl* c) noexcept : m_pControl(c) { }
            // ctor cpy
            Iterator(const Iterator& itr) noexcept : m_pControl(itr.m_pControl) { }
            // ctor mov
            Iterator(Iterator&& itr) noexcept : m_pControl(itr.m_pControl) { itr.m_pControl = nullptr; }
            //  ++itr
            auto operator++() noexcept { assert(m_pControl); m_pControl = m_pControl->next; return *this; }
            // itr++
            auto operator++(int) const noexcept { assert(m_pControl); Iterator itr(m_pControl); return ++itr; }
            //  --itr
            auto operator--() noexcept { assert(m_pControl); m_pControl = m_pControl->prev; return *this; }
            // itr--
            auto operator--(int) const noexcept { assert(m_pControl); Iterator itr(m_pControl); return --itr; }
            // operator ==
            auto operator==(const UIControl* c) const noexcept { return m_pControl == c; }
            // operator ==
            auto operator==(const Iterator& itr) const noexcept { return m_pControl == itr.m_pControl; }
            // operator !=
            auto operator!=(const UIControl* c) const noexcept { return m_pControl != c; }
            // operator !=
            auto operator!=(const Iterator& itr) const noexcept { return m_pControl != itr.m_pControl; }
            // pointer
            auto Ptr() const noexcept { return m_pControl; }
            // operator ->
            auto operator->() const noexcept { return m_pControl; }
            // operator *
            auto operator*() const noexcept { return m_pControl; }
        private:
            // control pointer
            UIControl*          m_pControl;
        };
    public:
        // render this
        virtual void Render() const noexcept override;
        // update this
        virtual void Update() noexcept override;
        // do event
        virtual bool DoEvent(const LongUI::EventArgument& arg) noexcept override;
        // do mouse event
        //virtual bool DoMouseEvent(const LongUI::MouseEventArgument& arg) noexcept override;
        // recreate this
        virtual auto Recreate() noexcept ->HRESULT override;
        // find child control by mouse point
        virtual auto FindChild(const D2D1_POINT_2F& pt) noexcept ->UIControl* override final;
    public:
        // push back
        virtual void Push(UIControl* child) noexcept override final;
        // remove child
        virtual void Remove(UIControl* child) noexcept override final;
    public:
        // get index of ctrl in [0, length), return OOR value if not found
        auto GetIndexOf(UIControl*) const noexcept ->uint32_t;
        // get control at index
        auto GetAt(uint32_t i) const noexcept ->UIControl*;
        // swap child
        void SwapChild(Iterator itr1, Iterator itr2) noexcept;
        // move child to
        void MoveChildTo(UIControl* ctrl, Iterator itr) noexcept { assert(ctrl&& "bad argument"); this->RemoveJust(ctrl); this->insert_only(itr, ctrl); }
        // move child to
        void MoveChildTo(UIControl* ctrl, UIControl* pos) noexcept { assert(ctrl&& "bad argument"); this->RemoveJust(ctrl); this->insert_only(Iterator(pos), ctrl); }
        // insert child, UIContainerBuiltIn support insert
        void Insert(Iterator itr, UIControl* ctrl) noexcept { this->insert_only(itr, ctrl); this->after_insert(ctrl);}
        // just remove child, : remove from list and set prev/next to null
        void RemoveJust(Iterator itr) noexcept { this->RemoveJust(*itr); }
    protected:
        // insert only
        void insert_only(Iterator itr, UIControl* ctrl) noexcept;
        // render chain -> background
        void render_chain_background() const noexcept { return Super::render_chain_background(); }
        // render chain -> mainground
        void render_chain_main() const noexcept;
        // render chain -> foreground
        void render_chain_foreground() const noexcept { return Super::render_chain_foreground(); }
    public: // for C++ 11
        // begin 
        auto begin() const noexcept { return Iterator(m_pHead); };
        // end
        auto end() const noexcept { return Iterator(nullptr); };
#ifdef _DEBUG
    private:
        // debug for_each
        virtual void debug_for_each(const CUIFunction<void(UIControl*)>& call) noexcept override {
            for (auto ctrl : (*this)) { call(ctrl); }
        }
#endif
    protected:
        // head of list
        UIControl*              m_pHead = nullptr;
        // tail of list
        UIControl*              m_pTail = nullptr;
#ifdef LongUIDebugEvent
    protected:
        // debug infomation
        virtual bool debug_do_event(const LongUI::DebugEventInformation&) const noexcept override;
    };
    // 重载?特例化 GetIID
    template<> const IID& GetIID<LongUI::UIContainerBuiltIn>() noexcept;
#else
    };
#endif
    // make built-in iterator
    inline auto MakeIteratorBI(UIControl* ctrl) noexcept { 
        return UIContainerBuiltIn::Iterator(ctrl); 
    }
}