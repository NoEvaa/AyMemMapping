/**
 * MIT License
 *
 * Copyright (c) 2024 NoEvaa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
/**
 * aytestm.hpp - the lightest modern C++ single-header unit testing framework
 * The library's page: https://github.com/NoEvaa/AyTest-Mini
 *
 * Configuration micros
 * - AYTESTM_CONFIG_MAIN
 *     Generate a general `main` function.
 * - AYTESTM_DISABLE_TEST_MACRO
 *     Testing macro will be disabled.
 * - AYTESTM_DISABLE_ANSI_COLOR
 *     Output without ansi color code.
 */

#include <cassert>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <functional>
#include <source_location>
#include <iostream>
#include <iomanip>

#define AYTESTM_VERSION "1.0.1"

#if !defined(AYTESTM_DISABLE_TEST_MACRO)
#define TEST_CASE(case_name, ...)       AYTTM_TEST_CASE(case_name, __VA_ARGS__)
#define SECTION(...)                    AYTTM_SECTION(__VA_ARGS__)
#define CHECK(...)                      AYTTM_CHECK(__VA_ARGS__)
#define REQUIRE(...)                    AYTTM_REQUIRE(__VA_ARGS__)
#define CHECK_THROWS(...)               AYTTM_CHECK_THROWS(__VA_ARGS__)
#define REQUIRE_THROWS(...)             AYTTM_REQUIRE_THROWS(__VA_ARGS__)
#define CHECK_NOTHROW(...)              AYTTM_CHECK_NOTHROW(__VA_ARGS__)
#define REQUIRE_NOTHROW(...)            AYTTM_REQUIRE_NOTHROW(__VA_ARGS__)
#define CHECK_THROWS_AS(ex_type, ...)   AYTTM_CHECK_THROWS_AS(ex_type, __VA_ARGS__)
#define REQUIRE_THROWS_AS(ex_type, ...) AYTTM_REQUIRE_THROWS_AS(ex_type, __VA_ARGS__)
#endif

#define AYTTM_STR(...)   #__VA_ARGS__
#define AYTTM_CAT(a, b)  a##b
#define AYTTM_CAT1(a, b) AYTTM_CAT(a, b)

#define AYTTM_TEST_CASE(case_name, ...)                                                            \
    AYTTM_TEST_CASE_IMPL(AYTTM_CAT1(TestCase, __COUNTER__), case_name, __VA_ARGS__)
#define AYTTM_SECTION(...) AYTTM_SECTION_IMPL(__VA_ARGS__)
#define AYTTM_CHECK(...)   AYTTM_EXPR_1(BOOL, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), CHECK,)
#define AYTTM_REQUIRE(...) AYTTM_EXPR_1(BOOL, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), REQUIRE,)
#define AYTTM_CHECK_THROWS(...)                                                                    \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), CHECK, THROWS,)
#define AYTTM_REQUIRE_THROWS(...)                                                                  \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), REQUIRE, THROWS,)
#define AYTTM_CHECK_NOTHROW(...)                                                                   \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), CHECK, NOTHROW,)
#define AYTTM_REQUIRE_NOTHROW(...)                                                                 \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), REQUIRE, NOTHROW,)
#define AYTTM_CHECK_THROWS_AS(_ex, ...)                                                            \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), CHECK, THROWS_AS, _ex)
#define AYTTM_REQUIRE_THROWS_AS(_ex, ...)                                                          \
    AYTTM_EXPR_2(VOID, (__VA_ARGS__), AYTTM_STR(__VA_ARGS__), REQUIRE, THROWS_AS, _ex)

#define AYTTM_BUILTIN(_n) AYTTM_CAT(__A_Y_T_E_S_T_M__builtin__, _n)
#define AYTTM_SRC_LOC     aytest_mini::SrcLoc::current()
#define AYTTM_TEST_CASE_IMPL(class_name, case_name, ...)                                           \
    namespace {                                                                                    \
    class AYTTM_BUILTIN(class_name) : public aytest_mini::TestCase {                               \
    public:                                                                                        \
        void AYTTM_BUILTIN(runImpl)() override;                                                    \
    };                                                                                             \
    static int AYTTM_BUILTIN(AYTTM_CAT(s_i_, class_name)) =                                        \
        aytest_mini::initTestCase<AYTTM_BUILTIN(class_name)>(case_name, AYTTM_SRC_LOC);            \
    }                                                                                              \
    inline void AYTTM_BUILTIN(class_name)::AYTTM_BUILTIN(runImpl)()
#define AYTTM_SECTION_IMPL(...)                                                                    \
    if (auto AYTTM_BUILTIN(sec) = this->AYTTM_BUILTIN(enterSection)(); !!AYTTM_BUILTIN(sec))
#define AYTTM_EXPR_1(expr_mode, expr, expr_msg, eval, eval_args)                                   \
    AYTTM_EXPR_IMPL(expr_mode, expr, expr_msg, AYTTM_EVAL(eval, eval_args), AYTTM_EVAL_NONE)
#define AYTTM_EXPR_2(expr_mode, expr, expr_msg, handler, eval, eval_args)                          \
    AYTTM_EXPR_IMPL(expr_mode, expr, expr_msg, AYTTM_EVAL(eval, eval_args), AYTTM_EVAL(handler))
#define AYTTM_EXPR_IMPL(expr_mode, expr, expr_msg, eval, handler, ...)                             \
    this->AYTTM_BUILTIN(invokeExpr)(                                                               \
        aytest_mini::TestExpr(AYTTM_EXPRINFO(expr_mode, expr_msg, expr))                           \
            .bindEval(aytest_mini::EvalInfo(eval))                                                 \
            .bindHandler(aytest_mini::EvalInfo(handler)),                                          \
        AYTTM_SRC_LOC)
#define AYTTM_EXPRINFO(_mode, ...) AYTTM_CAT1(AYTTM_CAT(AYTTM_EXPRINFO_, _mode)(__VA_ARGS__),)
#define AYTTM_EXPRINFO_BOOL(msg, ...)                                                              \
    aytest_mini::ExprInfo([&]() { return static_cast<bool>(__VA_ARGS__); }, msg)
#define AYTTM_EXPRINFO_VOID(msg, ...)                                                              \
    aytest_mini::ExprInfo([&]() { static_cast<void>(__VA_ARGS__); return true; }, msg)
#define AYTTM_EVAL_NONE           nullptr, ""
#define AYTTM_EVAL(_mode, ...)    AYTTM_CAT(AYTTM_EVAL_, _mode)(__VA_ARGS__), AYTTM_STR(_mode)
#define AYTTM_EVAL_CHECK(...)     nullptr
#define AYTTM_EVAL_REQUIRE(...)   aytest_mini::handleRequire
#define AYTTM_EVAL_THROWS(...)    aytest_mini::evalThrow
#define AYTTM_EVAL_NOTHROW(...)   aytest_mini::evalNoThrow
#define AYTTM_EVAL_THROWS_AS(...) aytest_mini::evalThrowAs<__VA_ARGS__>

namespace aytest_mini {
namespace detail {
constexpr char const * kStrEmpty = "";
constexpr char const * kStrTab   = "    ";

constexpr char const * kStrUnexpectedEx = "Thrown unexpected exception:";
constexpr char const * kStrNoExThrown   = "No exception thrown.";

// divider: length 10
constexpr char const * kStrDivider       = "----------";
constexpr char const * kStrWavyDivider   = "~~~~~~~~~~";
constexpr char const * kStrDoubleDivider = "==========";
}

typedef struct test_exception {
    test_exception() = default; 
    explicit test_exception(std::string const & msg) : m_msg(msg) {}
    std::string const & what() const noexcept {
        return m_msg;
    }
private:
    std::string m_msg;
} TestException;
typedef struct test_output : TestException {
    explicit test_output(std::string const & msg) : TestException(msg) {}
} TestOutput;
typedef struct test_termination : TestException {
    test_termination() = default;
    explicit test_termination(std::string const & msg) : TestException(msg) {}
} TestTermination;

using SrcLoc = std::source_location;
namespace detail {
template <typename T>
T const & exception_cast(auto const & ex) {
    return static_cast<T const &>(static_cast<TestException const &>(ex));
}
inline void rethrowException() {
    auto p_e = std::current_exception();
    if (p_e) {
        std::rethrow_exception(p_e);
    }
}
inline std::string getExceptionInfo() {
    try {
        rethrowException();
    } catch (std::exception const & e) {
        return e.what();
    } catch (test_exception const & e) {
        return e.what();
    } catch (...) {
        return "Unknown exception.";
    }
    return "";
}

class Locksmith {
public:
    void reset() {
        m_num = m_key = m_keyhole = 0;
    }
    bool nextKey() {
        m_keyhole = 0;
        return ++m_key < m_num;
    }
    bool unlocking() {
        bool b_ret = (m_key == m_keyhole++);
        if (m_keyhole >= m_num) {
            m_num = m_keyhole;
        }
        return b_ret;
    }
private:
    std::size_t m_num     = 0;
    std::size_t m_key     = 0;
    std::size_t m_keyhole = 0;
};

template <typename Func>
class FuncInfo {
public:
    FuncInfo() = default;
    explicit FuncInfo(Func _fn, std::string_view str_info = std::string_view{ kStrEmpty })
        : m_fn(_fn), m_str_info(str_info) {}
    operator bool() const {
        return bool(m_fn);
    }
    bool operator()(auto... args) const {
        if (m_fn) {
            return m_fn(args...);
        }
        assert(0);
        return false;
    }
    std::string_view const & info() const {
        return m_str_info;
    }
private:
    Func             m_fn;
    std::string_view m_str_info;
};
}
using ExprInfo = detail::FuncInfo<std::function<bool(void)>>;
using EvalInfo = detail::FuncInfo<std::function<bool(ExprInfo const &)>>;

class TestExpr {
public:
    explicit TestExpr(ExprInfo const & expr_info) : m_expr(expr_info) {}

    TestExpr & bindEval(EvalInfo const & eval_info) {
        m_eval = eval_info;
        return *this;
    }
    TestExpr & bindHandler(EvalInfo const & handler_info) {
        m_handler = handler_info;
        return *this;
    }

    bool run() const;
    friend std::ostream & operator<<(std::ostream &, TestExpr const &);

private:
    ExprInfo m_expr;
    EvalInfo m_eval;
    EvalInfo m_handler;
};

struct TestCount {
    std::size_t total_  = 0;
    std::size_t passed_ = 0;
    std::size_t failed_ = 0;

    TestCount & operator+=(TestCount const & rhs) {
        total_  += rhs.total_;
        passed_ += rhs.passed_;
        failed_ += rhs.failed_;
        return *this;
    }
    void countOne(bool b_pass) {
        ++total_;
        if (b_pass) {
            ++passed_;
        } else {
            ++failed_;
        }
    }
};

class TestCase {
public:
    virtual ~TestCase() = default;

    void AYTTM_BUILTIN(setName)(char const * case_name) {
        m_name = case_name;
    }
    void AYTTM_BUILTIN(setSrcLoc)(SrcLoc const & src_loc) {
        m_src_loc = src_loc;
    }
    TestCount const & AYTTM_BUILTIN(getTestCount)() {
        return m_cnt;
    }

    virtual void AYTTM_BUILTIN(runImpl)() = 0;

    bool AYTTM_BUILTIN(run)();
    bool AYTTM_BUILTIN(invokeExpr)(TestExpr const &, SrcLoc const &);
    std::shared_ptr<bool> AYTTM_BUILTIN(enterSection)();
    friend std::ostream & operator<<(std::ostream &, TestCase const &);

private:
    void recordResult(bool);
    bool nextSection();

private:
    std::string_view    m_name;
    SrcLoc              m_src_loc;
    TestCount           m_cnt;
    detail::Locksmith   m_section_lock;
    std::weak_ptr<bool> m_section_flag;
};
using TestCases = std::vector<std::shared_ptr<TestCase>>;

class TestGroup {
public:
    bool run();
    void appendCase(std::shared_ptr<TestCase> p_tc) {
        assert(p_tc);
        m_cases.push_back(p_tc);
    }
private:
    TestCases m_cases;
};

struct TestConfig {
    std::function<std::ostream &(void)> fn_get_ostream_;
    std::ostream & getOStream() const {
        return fn_get_ostream_ ? fn_get_ostream_() : std::cout;
    }
    std::function<void(int, char**)> fn_parse_args_;
    void parseArgs(int argc, char** argv) {
        if (fn_parse_args_) {
            fn_parse_args_(argc, argv);
        }
    }
};

class TestContext {
public:
    static int run() {
        return getGroup().run() ? 0 : 1;
    }
    static TestConfig & getConfig() {
        static TestConfig s_config;
        return s_config;
    }
    static TestGroup & getGroup() {
        static TestGroup s_group;
        return s_group;
    }
};

template <typename T>
int initTestCase(char const * p_name, SrcLoc const & src_loc) {
    auto p_tcase = std::make_shared<T>();
    p_tcase->AYTTM_BUILTIN(setName)(p_name);
    p_tcase->AYTTM_BUILTIN(setSrcLoc)(src_loc);
    TestContext::getGroup().appendCase(std::static_pointer_cast<TestCase>(p_tcase));
    return 0;
}

namespace detail {
enum class FontColor {
    kDefault = 39,
    kRed     = 31,
    kGreen   = 32,
    kCase    = 36,
    kExpr    = 33,
};
inline std::ostream & operator<<(std::ostream & ost, FontColor c) {
#ifndef AYTESTM_DISABLE_ANSI_COLOR
    ost << "\033[" << static_cast<int>(c) << 'm';
#endif
    return ost;
}

inline std::string getExMsg(char const * desc) {
    std::stringstream ss;
    ss << desc << '\n'
       << kStrTab << getExceptionInfo();
    return ss.str();
}
inline std::ostream & outputToStream(std::ostream & ost, SrcLoc const & src_loc) {
    return ost << src_loc.file_name() << "(" << src_loc.line() << ")";
}
inline void outputFailedMsg(TestExpr const & expr,
    SrcLoc const & expr_loc, std::string const & msg) {
    auto & ost = TestContext::getConfig().getOStream();
    outputToStream(ost, expr_loc) << ':'
        << FontColor::kRed << " FAILED:"
        << FontColor::kDefault << '\n';
    ost << FontColor::kExpr << kStrTab << expr
        << FontColor::kDefault << '\n';
    if (!msg.empty()) {
        ost << msg << '\n';
    }
    ost << std::endl;
}
template <std::size_t N = 8>
std::ostream & outputToStreamRepeat(std::ostream & ost, char const * s) {
    for (std::size_t i = 0; i < N; ++i) {
        ost << s;
    }
    return ost;
}
}
inline std::ostream & operator<<(std::ostream & ost, TestCount const & cnt) {
    if (!cnt.total_) {
        return ost << "- None -";
    }
    ost << std::setw(5) << cnt.total_;
    if (cnt.passed_) {
        ost << " |" << detail::FontColor::kGreen
            << std::setw(5) << cnt.passed_ << " passed"
            << detail::FontColor::kDefault;
    }
    if (cnt.failed_) {
        ost << " |" << detail::FontColor::kRed
            << std::setw(5) << cnt.failed_ << " failed"
            << detail::FontColor::kDefault;
    }
    return ost;
}
inline std::ostream & operator<<(std::ostream & ost, TestExpr const & te) {
    bool b_exist_handler = !!te.m_handler.info().size();
    bool b_exist_eval    = !!te.m_eval.info().size();
    if (b_exist_handler) {
        ost << te.m_handler.info();
        if (b_exist_eval) {
            ost << '_';
        }
    }
    if (b_exist_eval) {
        ost << te.m_eval.info();
    }
    if (b_exist_handler || b_exist_eval) {
        ost << "( " << te.m_expr.info() << " )";
    } else {
        ost << te.m_expr.info();
    }
    return ost;
}
inline std::ostream & operator<<(std::ostream & ost, TestCase const & tc) {
    detail::outputToStream(ost, tc.m_src_loc) << ":\n";
    ost << "TEST CASE: " << detail::FontColor::kCase << tc.m_name;
    return ost << detail::FontColor::kDefault;
}

inline bool handleRequire(ExprInfo const & expr) {
    bool b_res = false;
    try {
        b_res = expr();
    } catch (TestException const & e) {
        throw detail::exception_cast<TestTermination>(e);
    } catch (...) {
        throw TestTermination{ detail::getExMsg(detail::kStrUnexpectedEx) };
    }
    if (b_res) {
        return true;
    }
    throw TestTermination{};
}
inline bool evalNoThrow(ExprInfo const & expr) {
    try {
        expr();
    } catch (...) {
        throw TestOutput{ detail::getExMsg(detail::kStrUnexpectedEx) };
    }
    return true;
}
inline bool evalThrow(ExprInfo const & expr) {
    try {
        expr();
    } catch (...) {
        return true;
    }
    throw TestOutput{ detail::kStrNoExThrown };
}
template <typename ExTy>
bool evalThrowAs(ExprInfo const & expr) {
    try {
        expr();
    } catch (ExTy const &) {
        return true;
    } catch (...) {
        throw TestOutput{ detail::getExMsg(detail::kStrUnexpectedEx) };
    }
    throw TestOutput{ detail::kStrNoExThrown };
}

inline bool TestExpr::run() const {
    if (!m_expr) {
        throw TestException{"Empty test expression."};
    }
    auto eval_helper = [this]() {
        return this->m_eval ? this->m_eval(this->m_expr) : this->m_expr();
    };
    return m_handler ? m_handler(ExprInfo(eval_helper)) : eval_helper();
}

inline bool TestCase::AYTTM_BUILTIN(run)() {
    assert(m_section_flag.expired());
    m_section_lock.reset();
    do {
        try {
            AYTTM_BUILTIN(runImpl)();
        } catch (TestTermination const &) {}
    } while(nextSection());
    return !m_cnt.failed_;
}

inline bool TestCase::AYTTM_BUILTIN(invokeExpr)(
    TestExpr const & expr, SrcLoc const & expr_loc) {
    try {
        if (expr.run()) {
            recordResult(true);
            return true;
        } else {
            recordResult(false);
            detail::outputFailedMsg(expr, expr_loc, "");
        }
    } catch (TestTermination const & e) {
        recordResult(false);
        detail::outputFailedMsg(expr, expr_loc, e.what());
        throw TestTermination{};
    } catch (TestOutput const & e) {
        recordResult(false);
        detail::outputFailedMsg(expr, expr_loc, e.what());
    } catch (...) {
        recordResult(false);
        detail::outputFailedMsg(expr, expr_loc, detail::getExMsg(detail::kStrUnexpectedEx));
    }
    return false;
}

inline std::shared_ptr<bool> TestCase::AYTTM_BUILTIN(enterSection)() {
    if (!m_section_flag.expired()) {
        // prevent nested sections
        return nullptr;
    }
    if (!m_section_lock.unlocking()) {
        return nullptr;
    }
    auto p_ret = std::make_shared<bool>(true);
    m_section_flag = p_ret;
    return p_ret;
}

inline void TestCase::recordResult(bool b_pass) {
    if (!b_pass && !m_cnt.failed_) {
        auto & ost = TestContext::getConfig().getOStream();
        detail::outputToStreamRepeat<>(ost, detail::kStrDivider) << '\n';
        ost << *this << "\n\n";
        detail::outputToStreamRepeat<>(ost, detail::kStrWavyDivider) << '\n';
        ost << std::endl;
    }
    m_cnt.countOne(b_pass);
}

inline bool TestCase::nextSection() {
    return m_section_lock.nextKey();
}

inline bool TestGroup::run() {
    TestCount case_res;
    TestCount expr_res;
    for (auto & p_tcase : m_cases) {
        bool b_res = p_tcase->AYTTM_BUILTIN(run)();
        expr_res += p_tcase->AYTTM_BUILTIN(getTestCount)();
        case_res.countOne(b_res);
    }
    auto & ost = TestContext::getConfig().getOStream();
    detail::outputToStreamRepeat<>(ost, detail::kStrDoubleDivider) << '\n';
    ost << "test cases: " << case_res << '\n'
        << "assertions: " << expr_res << '\n'
        << std::endl;
    return case_res.failed_ == 0;
}
}
#ifdef AYTESTM_CONFIG_MAIN
int main(int argc, char** argv) {
    using namespace aytest_mini;
    TestContext::getConfig().parseArgs(argc, argv);
    TestContext::getConfig().getOStream()
        << "AyTest-Mini v" << AYTESTM_VERSION
        << '\n' << std::endl;
    return TestContext::run();
}
#endif

