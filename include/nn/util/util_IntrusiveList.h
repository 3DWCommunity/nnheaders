#pragma once

namespace nn {
namespace util {
namespace detail {
class IntrusiveListImplementation;
};

class IntrusiveListNode {
public:
    IntrusiveListNode() {
        m_Prev = this;
        m_Next = this;
    }

    IntrusiveListNode* m_Prev;
    IntrusiveListNode* m_Next;
};
};  // namespace util
};  // namespace nn