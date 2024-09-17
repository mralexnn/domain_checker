#include <algorithm>
#include <iostream>
#include <cassert>
#include <set>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    // êîíñòðóêòîð äîëæåí ïîçâîëÿòü êîíñòðóèðîâàíèå èç string, ñ ñèãíàòóðîé îïðåäåëèòåñü ñàìè
    Domain(const string& name):name_(name) {
        reverse(name_.begin(), name_.end());
        name_ += ".";
    }

    string Name() {
        return name_;
    }

    string Name() const {
        return name_;
    }
    // ðàçðàáîòàéòå operator==
    //Думаю, что неконстантный оператор сравнения будет излишен, т.к. потенциальная возможность изменения внутри оператора
    //нарушает саму логику сравнения. В случае если оставить только константный оператор, и кто-либо попытается поменять через
    //него значнеие, то компилятор не позволит это сделать. При этом он таже будет работать для неконстантных объектов.
    bool operator==(const Domain& rhs)
    {
        return name_ == rhs.name_;
    }

    bool operator==(const Domain& rhs) const
    {
        return name_ == rhs.name_;
    }
    //То же самое что и со сравнением
    bool operator< (const Domain& rhs) 
    {
        return name_ < rhs.name_;
    }

    bool operator< (const Domain& rhs) const
    {
        return name_ < rhs.name_;
    }
    // ðàçðàáîòàéòå ìåòîä IsSubdomain, ïðèíèìàþùèé äðóãîé äîìåí è âîçâðàùàþùèé true, åñëè this åãî ïîääîìåí
    //Очень хороший метод по поиску подстроки, но пять же, я бы убрал неконстантный метод.
    bool IsSubDomain(const Domain& rhs) const {
        return !(rhs.name_.size() == 0) && name_.find(rhs.name_) == 0;
    }
    bool IsSubDomain(const Domain& rhs)  {
        return !(rhs.name_.size() == 0) && name_.find(rhs.name_) == 0;
    }
private:
    string name_;
};

class DomainChecker {
public:
    // êîíñòðóêòîð äîëæåí ïðèíèìàòü ñïèñîê çàïðåù¸ííûõ äîìåíîâ ÷åðåç ïàðó èòåðàòîðîâ
    template <typename T>
    DomainChecker(T begin, T end): forbidden_domains_(begin, end){
        sort(forbidden_domains_.begin(), forbidden_domains_.end());
        auto it = unique(forbidden_domains_.begin(), forbidden_domains_.end(), 
            [](const Domain& lhs, const Domain& rhs) {
            return rhs.IsSubDomain(lhs);
            });
        forbidden_domains_.erase(it, forbidden_domains_.end());
    }

    // ðàçðàáîòàéòå ìåòîä IsForbidden, âîçâðàùàþùèé true, åñëè äîìåí çàïðåù¸í
    bool IsForbidden(const Domain& domain) const {
        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain);
        return it != forbidden_domains_.begin() && domain.IsSubDomain(*prev(it));
    }

private:
    vector<Domain> forbidden_domains_;
};

// ðàçðàáîòàéòå ôóíêöèþ ReadDomains, ÷èòàþùóþ çàäàííîå êîëè÷åñòâî äîìåíîâ èç ñòàíäàðòíîãî âõîäà
template <typename Number>
vector<Domain> ReadDomains(istream& input, Number num) {
    vector<Domain> names;
    names.reserve(num);
    for (size_t i = 0; i < num; ++i) {
        string name;
        getline(input, name);
        names.push_back(name);
    }
    return names;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    istringstream(line) >> num;

    return num;
}

void TestDomain() {
    Domain domain1("domain1"s);
    Domain domain2("domain2"s);
    Domain domain3("domain1"s);
    Domain domain4("new.domain2"s);
    string reverse_domain = domain1.Name();
    string answer = { reverse_domain.rbegin(), reverse_domain.rend() };
    assert(answer == ".domain1"s);
    assert(!(domain1 == domain2));
    assert(domain1 == domain3);
    assert(!domain1.IsSubDomain(domain2));
    assert(domain1.IsSubDomain(domain3));
    assert(!domain2.IsSubDomain(domain4));
    assert(domain4.IsSubDomain(domain2));

    cout << "FUNCTION TestDomain" << " DONE"sv << endl;
}

void TestDomainChecker() {
    vector<string> names { "gdz.ru"s,
        "gdz.com"s,
        "gdz.ru"s};
    vector<Domain> domains;
    for (const auto& name : names) {
        domains.emplace_back(Domain(name));
    }
    DomainChecker checker(domains.begin(), domains.end());
    assert(checker.IsForbidden(Domain("gdz.ru"s)));
    assert(checker.IsForbidden(Domain("new.gdz.com"s)));
    assert(!checker.IsForbidden(Domain("gdz.pointers.ru"s)));

    cout << "FUNCTION TestDomainChecker" << " DONE"sv << endl;
}

void TestReadDomains() {
    istringstream input;
    input.str("gdz.ru\ngdz.com"s);
    auto results = ReadDomains(input, 2);
    vector<string> answers;
    for (const auto& result : results) {
        string reverse_domain = result.Name();
        answers.emplace_back(string(reverse_domain.rbegin(), reverse_domain.rend()));
    }
    assert(answers[0] == ".gdz.ru"s);
    assert(answers[1] == ".gdz.com"s);

    cout << "FUNCTION TestReadDomains" << " DONE"sv << endl;
}

void Tests() {
    TestDomain();
    TestDomainChecker();
    TestReadDomains();
}

int main() {
    Tests();
    const vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
