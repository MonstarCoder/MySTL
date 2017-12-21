#include "../string.h"

namespace mystl {

const size_t string::s_max; // 类内静态成员声明

// 构造，复制，析构相关
string::string(const string& str) {
    allocate_and_copy(str.start_, str.finish_);
}

string::string(string&& str) {
    start_ = str.start_;
    finish_ = str.finish_;
    end_of_storage_ = str.end_of_storage_;
    str.start_ = str.finish_ = str.end_of_storage_ = 0;
}

string::string(const string& str, size_type pos, size_type n) {
    n = equal_smax(n, str.size(), pos);
    allocate_and_copy(str.start_ + pos, str.start_ + pos + n);
}

string::string(const char* s) {
    allocate_and_copy(s, s + strlen(s));
}

string::string(const char* s, size_type n) {
    allocate_and_copy(s, s + n);
}

string::string(size_type n, char c) {
    allocate_and_fill(n, c);
}

string& string::operator=(const string& str) {
    swap(*this, str);
    return *this;
}

string& string::operator=(string&& str) {
    swap(*this, str);
    str.start_ = str.finish_ = str.end_of_storage_ = 0;
    return *this;
}

string& string::operator=(const char* s) {
    destroy(start_, finish_);
    deallocate();
    allocate_and_copy(s, s + strlen(s));
    return *this;
}

string& string::operator=(char c) {
    destroy(start_, finish_);
    deallocate();
    allocate_and_fill(1, c);
    return *this;

}

string::~string() {
    destroy(start_, finish_);
    data_allocator::deallocate(start_, end_of_storage_ - start_);
}

typename string::size_type
string::equal_smax(size_type n, size_type end, size_type start) {
    return (n == s_max ? end - start : n);
}

// 访问，操作元素相关
void string::resize(size_type n) {
    resize(n, value_type());
}

void string::resize(size_type n, char c) {
    if (n < size()) {
        destroy(start_ + n, finish_);
        finish_ = start_ + n;
    } else if (n > size() && n <= capacity()) {
        auto len = n - size();
        finish_ = std::uninitialized_fill(finish_, len, c);
    } else {
        size_type len_insert = n - size();
        size_type old_capacity = capacity();
        auto res = std::max(old_capacity, len_insert);
        auto new_capacity = old_capacity + res;
        iterator new_start_ = data_allocator::allocate(new_capacity);
        iterator new_finish_ = std::uninitialized_copy(start_, finish_, new_start_);
        new_finish_ = std::uninitialized_fill(new_finish_, len_insert, c);

        destroy(start_, finish_);
        deallocate();
        start_ = new_start_;
        finish_ = new_finish_;
        end_of_storage_ = start_ + n;
    }
}

void string::reserve(size_type) {
    if (n <= capacity()) return;
    iterator new_start_ = data_allocator::allocate(n);
    iterator new_finish_ = std::uninitialized_copy(start_, finish_, new_start_);
    destroy(start_, finish_);
    deallocate();
    start_ = new_start_;
    finish_ = new_finish_;
    end_of_storage_ = start_ + n;
}

string& string::insert(size_type pos, const char* s) {
    insert(start_ + pos, s, s + strlen(s));
    return *this;
}

string& string::insert(size_type pos, const char* s, size_type n) {
    insert(start_ + pos, s, s + n);
    return *this;
}

string& string::insert(size_type pos, size_type n, char c) {
    insert(start_ + pos, n, c);
    return *this;
}

string& strong::insert(size_type pos, const string& str) {
    insert(start_ + pos, str.start_, str.finish_);
    return *this;
}

string& string::insert(size_type pos, const string& str, size_type subpos,
                       size_type sublen) {
    sublen = equal_smax(sublen, str.size(), subpos);
    insert(start_ + pos, str.start_ + subpos, str.start_ + subpos + sublen);
    return *this;
}

typename string::iterator
string::insert_fill_aux(iterator p, size_type n, value_type c){
    size_type old_capacity = capacity();
    auto res = std::max(old_capacity, n);
    auto new_capacity = old_capacity + res;
    iterator new_start_ = data_allocator::allocate(new_capacity);
    iterator new_finish_ = std::uninitialized_copy(start_, p, new_start_);
    new_finish_ = std::uninitialized_fill(new_finish_, n, c);
    auto res = new_finish_;
    new_finish_ = std::uninitialized_copy(p, finish_, new_finish_);

    destroy(start_, finish_);
    deallocate();
    start_ = new_start_;
    finish_ = new_finish_;
    end_of_storage_ = start_ + new_capacity;
    return res;
}

iterator string::insert(iterator p, size_type n, char c) {
    auto len_left = capacity() - size();
    if (n <= len_left) {
        for (iterator it = finish_ - 1; it >= p; --ot)
            *(it + n) = *(it);
        std::uninitialized_fill(p, n, c);
        finish_ += n;
        return (p + n);
    } else {
        return insert_fill_aux(p, n, c);
    }
}

typename string::iterator string::insert(iterator p, char c) {
    return insert(p, 1, c);
}

string& string::append(const char* s) {
    insert(finish_, s);
    return *this;
}

string& string::append(const char* s, size_type n) {
    insert(finish_, s, n);
    return *this;
}

string& string::append(const string& str) {
    insert(finish_, str);
    return *this;
}

string& string::append(const string& str, size_type subpos, size_type sublen) {
    sublen = equal_smax(sublen, str.size(), subpos);
    insert(finish_, str, subpos, sublen);
    return *this;
}

string& string::append(size_type n, char c) {
    insert(finish_, n, c);
    return *this;
}

string& string::operator+= (const string& str) {
    insert(finish_, str);
    return *this;
}

string& string::operator+= ( const char* s ) {
    insert(finish_, s);
    return *this;
}

string& string::operator+= (char c) {
    insert(finish_, c);
    return *this;
}

typename string::iterator string::erase(iterator first, iterator last) {
    size_type len_move = finish_ - last;
    for (auto i = 0; i != len_move; ++i)
        *(first + i) = *(last + i);
    destroy(first + len_move, finish_);
    finish_ = first + len_move;
    return first;
}

string& string::erase(size_type pos, size_type len) {
    len = equal_smax(len, size(), pos);
    erase(start_ + pos, start_ + pos + len);
    return *this;
}

typename string;:iterator string::erase(iterator p) {
    return erase(p, p + 1);
}

string& string::replace(size_type pos, size_type len, const string& str) {
    return replace(start_ + pos, start_ + pos + len, str.start_, str.finish_);
}

string& string::replace(iterator it1, iterator it2, const string& str) {
    return replace(it1, it2, str.start_, str.finish_);
}

string& string::replace(size_type pos, size_type len, const string& str, size_type subpos,
                size_type sublen) {
    sublen = equal_smax(sublen, str.size(), subpos);
    return replace(start_ + pos, start_ + pos + len, str.start_ +
                   subpos, str.start_ + subpos + sublen);
}

string& string::replace(size_type pos, size_type len, const char* s) {
    return replace(start_ + pos, start_ + pos + len, s, s + strlen(s));
}

string& string::replace(iterator it1, iterator it2, const char* s) {
    return replace(it1, it2, s, s + strlen(s));
}

string& string::replace(size_type pos, size_type len, const char* s, size_type n) {
    return replace(start_ + pos, start_ + pos + len, s, s + n);
}

string& string::replace(iterator it1, iterator it2, const char* s, size_type n) {
    return replace(it1, it2, s, s + n);
}

string& string::replace(size_type pos, size_type len, size_type n, char c) {
    return replace(start_ + pos, start_ + pos + len, n, c);
}

string& string::replace(iterator it1, iterator it2, size_type n, char c) {
    autp ptr = erase(it1, it2);
    insert(ptr, n, c);
    return *this;
}

string& string::assign(const string& str) {
    assign(str.start_, str.finish_);
}

string& string::assign(const string& str, size_type subpos, size_type sublen) {
    assign(str.start_ + subpos, str.start+ + pos + sublen);
}

string& string::assign(const char* s) {
    assign(s, strlen(s));
}

string& string::assign(const char* s, size_type n) {
    assign(s, n);
}

string& stirng::assign(size_type n, char c) {
    autp ptr = erase(start_, finish_);
    insert(ptr, n, c);
    return *this;
}

int string::compare_aux(size_type pos, size_type len, const_iterator cit,
        size_type subpos, size_type sublen) const {
    size_type i, j;
    for (i = 0, j = 0; i != len && j != sublen; ++i, ++j) {
        if ((*this)[pos + i] < cit[subpos + j])
            return -1;
        else if ((*this)[pos + i] > cit[subpos + j])
            return 1;
    }
    if (i == len && j == sublen)
        return 0;
    else if (i == len)
        return -1;
    else
        return 1;
}

int string::compare(size_type pos, size_type len, const string& str,
        size_type subpos, size_type sublen) const {
    return compare_aux(pos, len, str.start_, subpos, sublen);
}

int string::compare(const string& str) const {
    return compare(0, size(), str, 0, str.size());
}

int string::compare(size_type pos, size_type len, const string& str) const {
    return compare(pos, len, str, 0, str.size());
}

int string::compare(const char* s) const {
    return compare(0, size(), s, strlen(s));
}

int string::compare(size_type pos, size_type len, const char* s) const {
    return compare(pos, len, s, strlen(s));
}

int string::compare(size_type pos, size_type len,
                    const char* s, size_type n) const {
    return compare_aux(pos, len, s, 0, n);
}

size_type string::find_aux(const cit, size_type pos,
                           size_type len, size_type cond) const {
    size_type i, j;
    for (i = pos; i != cond; ++i) {
        for (j = 0; j != len; ++j) {
            if (*(begin() + i + j) != cit[j])
                break;
        }
        if (j == len)
            return i;
    }
    return s_max;
}

size_type string::find(const string& str, size_type pos) const {
    size_type len = str.size();
    if (size() - pos < len)
        return s_max;
    return find_aux(str.cbegin(), pos, len, size());
}

size_type string::find(const char* s, size_type pos) const {
    return find(s, pos, strlen(s));
}

size_type find(const char* s, size_type pos, size_type n) const {
    size_type len = strlen(s);
    return find_aux(s, pos, n, size());
}

size_type find(char c, size_type pos) const {
    for (auto cit = cbegin() + pos; cit != cend(); ++cit) {
        if (*cit == c)
            return cit - cbegin();
    }
    return s_max;
}

/*
size_type find_first_of(const string& str, size_type pos = 0) const;
size_type find_first_of(const char* s, size_type pos = 0) const;
size_type find_first_of(const char* s, size_type pos, size_type n) const;
size_type find_first_of(char c, size_type pos = 0) const;
size_type find_last_of(const string& str, size_type pos) const;
size_type find_last_of(const char* s, size_type pos) const;
size_type find_last_of(const char* s, size_type pos, size_type n) const;
size_type find_last_of(char c, size_type pos) const;
size_type find_first_not_of(const string& str, size_type pos = 0) const;
size_type find_first_not_of(const char* s, size_type pos = 0) const;
size_type find_first_not_of(const char* s, size_type pos, size_type n) const;
size_type find_first_not_of(char c, size_type pos = 0) const;
size_type find_last_not_of(const string& str, size_type pos) const;
size_type find_last_not_of(const char* s, size_type pos) const;
size_type find_last_not_of(const char* s, size_type pos, size_type n) const;
size_type find_last_not_of(char c, size_type pos) const;
*/

std::ostream& operator<<(std::ostream& os, const string& str) {
    for (const auto ch : str)
        os << ch;
    return os;
}
std::istream& operator>>(std::istream& is, string& str) {
    char ch;
    typename string::size_type old_size = str.size(), index = 0;
    //TODO
}

/*
friend string operator+(const string& lhs, const string& rhs);
friend string operator+(const string& lhs, const char* rhs);
friend string operator+(const char* lhs, const string& rhs);
friend string operator+(const string& lhs, char rhs);
friend string operator+(char lhs, const string& rhs);
friend bool operator==(const string& lhs, const string& rhs);
friend bool operator==(const string& lhs, const char* rhs);
friend bool operator==(const char* lhs, const string& rhs);
friend bool operator!=(const string& lhs, const string& rhs);
friend bool operator!=(const string& lhs, const char* rhs);
friend bool operator!=(const char* lhs, const string& rhs);
friend bool operator<(const string& lhs, const string& rhs);
friend bool operator<(const string& lhs, const char* rhs);
friend bool operator<(const char* lhs, const string& rhs);
friend bool operator<=(const string& lhs, const string& rhs);
friend bool operator<=(const string& lhs, const char* rhs);
friend bool operator<=(const char* lhs, const string& rhs);
friend bool operator>(const string& lhs, const string& rhs);
friend bool operator>(const string& lhs, const char* rhs);
friend bool operator>(const char* lhs, const string& rhs);
friend bool operator>=(const string& lhs, const string& rhs);
friend bool operator>=(const string& lhs, const char* rhs);
friend bool operator>=(const char* lhs, const string& rhs);
*/

void swap(string& x, string& y) {
    x.swap(y);
}

std::istream& getline(std::istream& is, string& str, char delim) {

}

std::istream& getline(std::istream& is, string& str) {

}

}// namespace mystl
