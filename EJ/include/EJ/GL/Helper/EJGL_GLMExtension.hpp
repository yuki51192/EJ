#ifndef EJGL_GLMEXTENSION_HPP
#define EJGL_GLMEXTENSION_HPP

//
// Tips:
//

#include "../Utils/EJGL_Config.hpp"
#include "../../Utils/EJ_Utils.hpp"

#include <string>
#include <iostream>
#include <glm/glm.hpp>

EJ_NAMESPACE_BEGIN

// vec
template <typename T, ::glm::length_t L, ::glm::qualifier Q>
_STD string toString(const ::glm::vec<L, T, Q>& v_) {
    _STD string s = "vec" + toString(L) + "(" + toString(v_[0]);
    for (int i = 1; i < L; ++i) {
        s += ", " + toString(v_[i]);
    }
    s += ")";
    return s;
}
template <typename T, ::glm::length_t L, ::glm::qualifier Q>
std::ostream& operator<<(std::ostream& os_, const ::glm::vec<L, T, Q>& v_) {
    os_ << toString(v_);
    return os_;
}

// quat
template <typename T, ::glm::qualifier Q>
_STD string toString(const ::glm::qua<T, Q>& q_) {
    _STD string s = "quat(" + toString(q_.x) + ", " + toString(q_.y) + ", " + toString(q_.z) + ", " + toString(q_.w) + ")";
    return s;
}
template <typename T, ::glm::qualifier Q>
std::ostream& operator<<(std::ostream& os_, const ::glm::qua<T, Q>& q_) {
    os_ << toString(q_);
    return os_;
}

template <typename T, ::glm::length_t C, ::glm::length_t R, ::glm::qualifier Q>
_STD string toString(const ::glm::mat<C, R, T, Q>& m_) {
    _STD string s = "mat" + toString(C) + toString(R) + "(\n";
    for (int c = 0; c < C; ++c) {
        for (int r = 0; r < R; ++r) {
            s += toString(m_[c][r]);
            if (c < C - 1 || r < R - 1)
                s += ", ";
        }
        if (c < C - 1)
            s += '\n';
    }
    s += ")";
    return s;
}
template <typename T, ::glm::length_t C, ::glm::length_t R, ::glm::qualifier Q>
std::ostream& operator<<(std::ostream& os_, const ::glm::mat<C, R, T, Q>& m_) {
    os_ << toString(m_);
    return os_;
}

EJ_NAMESPACE_END

#endif // EJGL_GLMEXTENSION_HPP