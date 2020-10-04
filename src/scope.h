#ifndef C0C_SCOPE_H
#define C0C_SCOPE_H

#include "ast.h"

#include <map>
#include <string>
#include <vector>

struct Token;

enum ScopeType {
    S_FILE = 0x01,
    S_PROTO,
    S_BLOCK,
    S_VOID_FUNC = 0x10,
    S_INT_FUNC = 0x20,
    S_CHAR_FUNC = 0x40,
    S_FUNC = 0xf0,  // abstract, just for checking
};

class Scope
{
    using IdentifierMap = std::map<std::string, Identifier *>;

public:
    Scope(Scope *parent, enum ScopeType type)
        : m_parent(parent), m_type(type), m_retflag(0)
    {
    }
    ~Scope() {}

    enum ScopeType Type() const
    {
        return m_type;
    }
    Scope *parent() const
    {
        return m_parent;
    }
    void SetRetFlag(int flag)
    {
        m_retflag = flag;
    }
    int RetFlag() const
    {
        return m_retflag;
    }
    bool IsFuncScope() const
    {
        return m_type & ScopeType::S_FUNC;
    }
    bool IsGlobalScope() const
    {
        return m_type & ScopeType::S_FILE;
    }

    Identifier *Find(const std::string &name);
    Identifier *FindInCurScope(const std::string &name);

    void Insert(const std::string &name, Identifier *iden);

    bool operator==(const Scope &other) const
    {
        return m_type == other.m_type;
    }
    IdentifierMap::iterator begin()
    {
        return m_identifier_map.begin();
    }
    IdentifierMap::iterator end()
    {
        return m_identifier_map.end();
    }
    size_t size() const
    {
        return m_identifier_map.size();
    }

    void Peek();

    const Scope &operator=(const Scope &other);
    Scope(const Scope &scope);

private:
    IdentifierMap m_identifier_map;
    Scope *m_parent;
    enum ScopeType m_type;
    int m_retflag;
};

#endif  // !C0C_SCOPE_H
