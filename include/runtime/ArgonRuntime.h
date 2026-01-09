#ifndef ARGON_RUNTIME_H
#define ARGON_RUNTIME_H

#include <cstdint>
#include <future>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <ostream>
#include <istream>
#include <string>
#include <cmath>
#include <stdfloat>

// ===== ARGONLANG TYPE SYSTEM =====
namespace ArgonLang {
namespace Runtime {
    // Standard integer types (8, 16, 32, 64 bits)
    #define I8 int8_t
    #define I16 int16_t
    #define I32 int32_t
    #define I64 int64_t
    #define U8 uint8_t
    #define U16 uint16_t
    #define U32 uint32_t
    #define U64 uint64_t
    #define F16 std::float16_t
    #define F32 std::float32_t
    #define F64 std::float64_t
    #define F128 std::float128_t

    #define STR std::string
    // Custom 128-bit integer class (using two 64-bit values for compatibility)
    class I128 {
    private:
        I64 high;  // High 64 bits (signed)
        U64 low;   // Low 64 bits (unsigned)

    public:
        // Constructors
        constexpr I128() : high(0), low(0) {}
        constexpr I128(I64 h, U64 l) : high(h), low(l) {}
        constexpr I128(I64 v) : high(v < 0 ? -1 : 0), low(static_cast<U64>(v)) {}
        constexpr I128(I32 v) : high(v < 0 ? -1 : 0), low(static_cast<U64>(static_cast<I64>(v))) {}
        I128(const std::string& str);

        // Conversion operators
        explicit operator int64_t() const;
        explicit operator int32_t() const { return static_cast<int32_t>(static_cast<int64_t>(*this)); }
        explicit operator double() const;

        // Arithmetic operators
        I128 operator+(const I128& other) const;
        I128 operator-(const I128& other) const;
        I128 operator*(const I128& other) const;
        I128 operator/(const I128& other) const;
        I128 operator%(const I128& other) const;
        I128 operator-() const;
        
        // Compound assignment
        I128& operator+=(const I128& other);
        I128& operator-=(const I128& other);
        I128& operator*=(const I128& other);
        I128& operator/=(const I128& other);
        I128& operator%=(const I128& other);
        
        // Increment/Decrement
        I128& operator++();
        I128 operator++(int);
        I128& operator--();
        I128 operator--(int);
        
        // Bitwise operators
        I128 operator&(const I128& other) const;
        I128 operator|(const I128& other) const;
        I128 operator^(const I128& other) const;
        I128 operator~() const;
        I128 operator<<(int shift) const;
        I128 operator>>(int shift) const;
        
        I128& operator&=(const I128& other);
        I128& operator|=(const I128& other);
        I128& operator^=(const I128& other);
        I128& operator<<=(int shift);
        I128& operator>>=(int shift);
        
        // Comparison operators
        bool operator==(const I128& other) const;
        bool operator!=(const I128& other) const;
        bool operator<(const I128& other) const;
        bool operator>(const I128& other) const;
        bool operator<=(const I128& other) const;
        bool operator>=(const I128& other) const;
        
        // Stream operators
        friend std::ostream& operator<<(std::ostream& os, const I128& val);
        friend std::istream& operator>>(std::istream& is, I128& val);
        
        // String conversion
        std::string to_string() const;
        
        // Helper methods
        bool is_negative() const { return high < 0; }
        bool is_zero() const { return high == 0 && low == 0; }
    };

    // Custom unsigned 128-bit integer class (using two 64-bit values for compatibility)
    class U128 {
        friend class I128;  // Allow i128 to access private members for conversions
        
    private:
        U64 high;  // High 64 bits
        U64 low;   // Low 64 bits

    public:
        // Constructors
        constexpr U128() : high(0), low(0) {}
        constexpr U128(U64 h, U64 l) : high(h), low(l) {}
        constexpr U128(U64 v) : high(0), low(v) {}
        constexpr U128(U32 v) : high(0), low(static_cast<U64>(v)) {}
        U128(const std::string& str);

        // Conversion operators
        explicit operator uint64_t() const;
        explicit operator uint32_t() const { return static_cast<uint32_t>(static_cast<uint64_t>(*this)); }
        explicit operator double() const;

        // Arithmetic operators
        U128 operator+(const U128& other) const;
        U128 operator-(const U128& other) const;
        U128 operator*(const U128& other) const;
        U128 operator/(const U128& other) const;
        U128 operator%(const U128& other) const;
        
        // Compound assignment
        U128& operator+=(const U128& other);
        U128& operator-=(const U128& other);
        U128& operator*=(const U128& other);
        U128& operator/=(const U128& other);
        U128& operator%=(const U128& other);
        
        // Increment/Decrement
        U128& operator++();
        U128 operator++(int);
        U128& operator--();
        U128 operator--(int);
        
        // Bitwise operators
        U128 operator&(const U128& other) const;
        U128 operator|(const U128& other) const;
        U128 operator^(const U128& other) const;
        U128 operator~() const;
        U128 operator<<(int shift) const;
        U128 operator>>(int shift) const;
        
        U128& operator&=(const U128& other);
        U128& operator|=(const U128& other);
        U128& operator^=(const U128& other);
        U128& operator<<=(int shift);
        U128& operator>>=(int shift);
        
        // Comparison operators
        bool operator==(const U128& other) const;
        bool operator!=(const U128& other) const;
        bool operator<(const U128& other) const;
        bool operator>(const U128& other) const;
        bool operator<=(const U128& other) const;
        bool operator>=(const U128& other) const;
        
        // Stream operators
        friend std::ostream& operator<<(std::ostream& os, const U128& val);
        friend std::istream& operator>>(std::istream& is, U128& val);
        
        // String conversion
        std::string to_string() const;
        
        // Helper methods
        bool is_zero() const { return high == 0 && low == 0; }
    };

    // Forward declarations
    class ScopeManager;
    
    // ArgonFuture - wrapper around std::future with scope tracking
    template<typename T>
    class ArgonFuture {
    private:
        std::future<T> future_;
        std::shared_ptr<ScopeManager> scope_manager_;
        
    public:
        ArgonFuture(std::future<T>&& future, std::shared_ptr<ScopeManager> scope_manager)
            : future_(std::move(future)), scope_manager_(std::move(scope_manager)) {}
        
        // Move constructor
        ArgonFuture(ArgonFuture&& other) noexcept 
            : future_(std::move(other.future_)), scope_manager_(std::move(other.scope_manager_)) {}
        
        // Move assignment
        ArgonFuture& operator=(ArgonFuture&& other) noexcept {
            if (this != &other) {
                future_ = std::move(other.future_);
                scope_manager_ = std::move(other.scope_manager_);
            }
            return *this;
        }
        
        // Delete copy operations
        ArgonFuture(const ArgonFuture&) = delete;
        ArgonFuture& operator=(const ArgonFuture&) = delete;
        
        // Await operation - blocks until result is ready
        T get() {
            return future_.get();
        }
        
        // Check if result is ready
        bool is_ready() const {
            return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }
        
        // Wait with timeout
        template<typename Rep, typename Period>
        std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) {
            return future_.wait_for(timeout_duration);
        }
    };
    
    // ScopeManager - tracks futures in a scope and waits for completion
    class ScopeManager {
    private:
        std::vector<std::function<void()>> pending_futures_;
        bool is_destroyed_ = false;
        
    public:
        ScopeManager() = default;
        
        // Destructor waits for all pending futures
        ~ScopeManager() {
            wait_all();
            is_destroyed_ = true;
        }
        
        // Register a future to be waited on at scope exit
        template<typename T>
        void register_future(const std::future<T>& future) {
            if (!is_destroyed_) {
                pending_futures_.push_back([&future]() {
                    if (future.valid()) {
                        future.wait();
                    }
                });
            }
        }
        
        // Wait for all registered futures
        void wait_all() {
            for (auto& waiter : pending_futures_) {
                waiter();
            }
            pending_futures_.clear();
        }
        
        // Get shared pointer to this scope manager
        std::shared_ptr<ScopeManager> get_shared() {
            return std::shared_ptr<ScopeManager>(this, [](ScopeManager*){});
        }
    };
    
    // Thread-local scope manager stack
    extern thread_local std::vector<std::shared_ptr<ScopeManager>> scope_stack;
    
    // Scope guard for automatic scope management
    class ScopeGuard {
    private:
        std::shared_ptr<ScopeManager> scope_manager_;
        
    public:
        ScopeGuard() {
            scope_manager_ = std::make_shared<ScopeManager>();
            scope_stack.push_back(scope_manager_);
        }
        
        ~ScopeGuard() {
            if (!scope_stack.empty()) {
                scope_stack.pop_back();
            }
        }
        
        std::shared_ptr<ScopeManager> get_scope_manager() {
            return scope_manager_;
        }
    };
    
    // Parallel execution functions
    template<typename F>
    auto par(F&& func) -> ArgonFuture<decltype(func())> {
        auto scope_manager = scope_stack.empty() ? nullptr : scope_stack.back();
        
        auto future = std::async(std::launch::async, std::forward<F>(func));
        
        if (scope_manager) {
            scope_manager->register_future(future);
        }
        
        return ArgonFuture<decltype(func())>(std::move(future), scope_manager);
    }
    
    // Parallel execution with configuration (placeholder for now)
    template<typename Config, typename F>
    auto par(Config&& config, F&& func) -> ArgonFuture<decltype(func())> {
        // TODO: Use config for timeout, retries, etc.
        return par(std::forward<F>(func));
    }
    
    // Await function (alternative to .get())
    template<typename T>
    T await(ArgonFuture<T>&& future) {
        return future.get();
    }
    
    // Functional programming utilities to reduce code bloat
    // (Implementations moved to ArgonRuntime.cpp)
    
    // Filter utility: container | predicate
    template<typename Container, typename Predicate>
    std::decay_t<Container> filter(Container&& container, Predicate&& predicate);
    
    // Map utility: container & transform
    template<typename Container, typename Transform>
    std::decay_t<Container> map(Container&& container, Transform&& transform);
    
    // Reduce utility: container ^ reducer
    template<typename Container, typename Reducer>
    typename std::decay_t<Container>::value_type reduce(Container&& container, Reducer&& reducer);
    
    // Map-pipe utility: container ||> transform (in-place transformation)
    template<typename Container, typename Transform>
    Container& map_pipe(Container& container, Transform&& transform);
    
    // Pattern matching utilities to reduce code bloat
    // (Implementations moved to ArgonRuntime.cpp)
    
    // Match utility: replaces verbose match expression generation
    template<typename T, typename... Cases>
    auto match(T&& value, Cases&&... cases);
    
    // Range pattern matching (template for generic types)
    template<typename T>
    bool match_range(T value, T start, T end, bool inclusive = false);
    
    // Non-template overloads for common types (implemented in .cpp)
    bool match_range_int(int value, int start, int end, bool inclusive = false);
    bool match_range_float(float value, float start, float end, bool inclusive = false);
    bool match_range_double(double value, double start, double end, bool inclusive = false);
    
    // Wildcard pattern (always matches)
    template<typename T>
    bool match_wildcard(T&& value);
    
    // Value pattern matching
    template<typename T, typename U>
    bool match_value(T&& value, U&& pattern);
    
    // Destructuring utilities to reduce code bloat
    // (Implementations moved to ArgonRuntime.cpp)
    
    // Array destructuring: extracts elements by index
    template<typename Container>
    typename std::decay_t<Container>::value_type destructure_array_element(Container&& container, size_t index);
    
    // Array rest destructuring: extracts remaining elements as vector
    template<typename Container>
    std::vector<typename std::decay_t<Container>::value_type> destructure_array_rest(Container&& container, size_t start_index);
    
    // Struct field destructuring: extracts field by name (using member pointer)
    template<typename Struct, typename Member>
    auto destructure_struct_field(Struct&& obj, Member Struct::*field);
    
    // Compound destructuring: handles [first], rest = arr patterns
    template<typename Container>
    struct CompoundDestructure {
        Container& source;
        
        explicit CompoundDestructure(Container& src);
        
        // Extract first N elements
        template<size_t N>
        auto take_first();
        
        // Extract last N elements  
        template<size_t N>
        auto take_last();
        
        // Extract middle elements (skip first N, skip last M)
        auto take_middle(size_t skip_first, size_t skip_last = 0);
    };
    
    // Helper function to create compound destructure
    template<typename Container>
    CompoundDestructure<std::decay_t<Container>> compound_destructure(Container& container);

    // ===== EXTERN TEMPLATE DECLARATIONS FOR COMMON TYPES =====
    // These reduce compilation bloat by preventing implicit instantiation
    
    // Functional operators for std::vector<int>
    extern template std::vector<int> filter(std::vector<int>&& container, std::function<bool(int)>&& predicate);
    extern template std::vector<int> filter(const std::vector<int>& container, std::function<bool(int)>&& predicate);
    extern template std::vector<int> map(std::vector<int>&& container, std::function<int(int)>&& transform);
    extern template std::vector<int> map(const std::vector<int>& container, std::function<int(int)>&& transform);
    extern template int reduce(std::vector<int>&& container, std::function<int(int, int)>&& reducer);
    extern template int reduce(const std::vector<int>& container, std::function<int(int, int)>&& reducer);
    extern template std::vector<int>& map_pipe(std::vector<int>& container, std::function<int(int)>&& transform);

    // Functional operators for std::vector<float>
    extern template std::vector<float> filter(std::vector<float>&& container, std::function<bool(float)>&& predicate);
    extern template std::vector<float> filter(const std::vector<float>& container, std::function<bool(float)>&& predicate);
    extern template std::vector<float> map(std::vector<float>&& container, std::function<float(float)>&& transform);
    extern template std::vector<float> map(const std::vector<float>& container, std::function<float(float)>&& transform);
    extern template float reduce(std::vector<float>&& container, std::function<float(float, float)>&& reducer);
    extern template float reduce(const std::vector<float>& container, std::function<float(float, float)>&& reducer);
    extern template std::vector<float>& map_pipe(std::vector<float>& container, std::function<float(float)>&& transform);

    // Destructuring for std::vector<int>
    extern template int destructure_array_element(std::vector<int>&& container, size_t index);
    extern template int destructure_array_element(const std::vector<int>& container, size_t index);
    extern template std::vector<int> destructure_array_rest(std::vector<int>&& container, size_t start_index);
    extern template std::vector<int> destructure_array_rest(const std::vector<int>& container, size_t start_index);
    extern template CompoundDestructure<std::vector<int>> compound_destructure(std::vector<int>& container);

    // Destructuring for std::vector<float>
    extern template float destructure_array_element(std::vector<float>&& container, size_t index);
    extern template float destructure_array_element(const std::vector<float>& container, size_t index);
    extern template std::vector<float> destructure_array_rest(std::vector<float>&& container, size_t start_index);
    extern template std::vector<float> destructure_array_rest(const std::vector<float>& container, size_t start_index);
    extern template CompoundDestructure<std::vector<float>> compound_destructure(std::vector<float>& container);

    // Pattern matching for common types
    extern template bool match_wildcard(int&&);
    extern template bool match_wildcard(const int&);
    extern template bool match_wildcard(float&&);
    extern template bool match_wildcard(const float&);
    extern template bool match_wildcard(double&&);
    extern template bool match_wildcard(const double&);
    
    extern template bool match_value(int&& value, int&& pattern);
    extern template bool match_value(const int& value, const int& pattern);
    extern template bool match_value(float&& value, float&& pattern);
    extern template bool match_value(const float& value, const float& pattern);
    extern template bool match_value(double&& value, double&& pattern);
    extern template bool match_value(const double& value, const double& pattern);

    // Try/Result type for error handling (Rust-style Result)
    template<typename T, typename E = std::string>
    class Try {
    private:
        std::variant<T, E> data_;
        bool is_ok_;
        
    public:
        // Constructors
        static Try Ok(T&& value) {
            Try result;
            result.data_ = std::move(value);
            result.is_ok_ = true;
            return result;
        }
        
        static Try Ok(const T& value) {
            Try result;
            result.data_ = value;
            result.is_ok_ = true;
            return result;
        }
        
        static Try Err(E&& error) {
            Try result;
            result.data_ = std::move(error);
            result.is_ok_ = false;
            return result;
        }
        
        static Try Err(const E& error) {
            Try result;
            result.data_ = error;
            result.is_ok_ = false;
            return result;
        }
        
        // Check if result is Ok or Err
        bool is_ok() const { return is_ok_; }
        bool is_err() const { return !is_ok_; }
        
        // Get value (throws if Err)
        T& unwrap() {
            if (!is_ok_) {
                throw std::runtime_error("Called unwrap() on an Err value");
            }
            return std::get<T>(data_);
        }
        
        const T& unwrap() const {
            if (!is_ok_) {
                throw std::runtime_error("Called unwrap() on an Err value");
            }
            return std::get<T>(data_);
        }
        
        // Get error (throws if Ok)
        E& unwrap_err() {
            if (is_ok_) {
                throw std::runtime_error("Called unwrap_err() on an Ok value");
            }
            return std::get<E>(data_);
        }
        
        const E& unwrap_err() const {
            if (is_ok_) {
                throw std::runtime_error("Called unwrap_err() on an Ok value");
            }
            return std::get<E>(data_);
        }
        
        // Get value or default
        T unwrap_or(T&& default_value) const {
            if (is_ok_) {
                return std::get<T>(data_);
            }
            return std::move(default_value);
        }
        
        // Map operation
        template<typename F>
        auto map(F&& func) -> Try<std::invoke_result_t<F, T>, E> {
            if (is_ok_) {
                return Try<std::invoke_result_t<F, T>, E>::Ok(func(std::get<T>(data_)));
            } else {
                return Try<std::invoke_result_t<F, T>, E>::Err(std::get<E>(data_));
            }
        }
        
        // Map error operation
        template<typename F>
        auto map_err(F&& func) -> Try<T, std::invoke_result_t<F, E>> {
            if (is_ok_) {
                return Try<T, std::invoke_result_t<F, E>>::Ok(std::get<T>(data_));
            } else {
                return Try<T, std::invoke_result_t<F, E>>::Err(func(std::get<E>(data_)));
            }
        }
        
        // And then operation (flatMap)
        template<typename F>
        auto and_then(F&& func) -> std::invoke_result_t<F, T> {
            if (is_ok_) {
                return func(std::get<T>(data_));
            } else {
                using ReturnType = std::invoke_result_t<F, T>;
                return ReturnType::Err(std::get<E>(data_));
            }
        }
        
        // Or else operation
        template<typename F>
        Try or_else(F&& func) {
            if (is_ok_) {
                return *this;
            } else {
                return func(std::get<E>(data_));
            }
        }
        
    private:
        Try() : is_ok_(false) {}
    };
    
    // Helper function to wrap an expression in a Try block
    template<typename F>
    auto try_catch(F&& func) -> Try<std::invoke_result_t<F>, std::string> {
        try {
            return Try<std::invoke_result_t<F>, std::string>::Ok(func());
        } catch (const std::exception& e) {
            return Try<std::invoke_result_t<F>, std::string>::Err(std::string(e.what()));
        } catch (...) {
            return Try<std::invoke_result_t<F>, std::string>::Err(std::string("Unknown error"));
        }
    }

    // Helper macros for code generation
    #define ARGON_SCOPE_BEGIN() ArgonLang::Runtime::ScopeGuard __scope_guard;
    
} // namespace Runtime
} // namespace ArgonLang

#endif // ARGON_RUNTIME_H
