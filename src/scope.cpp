#include "scope.h"
#include "ast.h"

#include <cassert>
#include <iostream>

Identifier *Scope::Find(const std::string &name)
{
  auto ident = m_identifier_map.find(name);
  if (ident != m_identifier_map.end())
    return ident->second;
  if (m_type == S_FILE || m_parent == nullptr)
    return 0;
  return m_parent->Find(name);
}

Identifier *Scope::FindInCurScope(const std::string &name)
{
  auto ident = m_identifier_map.find(name);
  if (ident == m_identifier_map.end()) {
    // debug("Scope::FindInCurScope: %s not found\n", name.c_str());
    return 0;
  }
  return ident->second;
}

void Scope::Insert(const std::string &name, Identifier *ident)
{
  assert(ident != nullptr);
  m_identifier_map[name] = ident;
  // std::cout << "name:" << name << " ident:" << ident << std::endl;
}

void Scope::Peek()
{
#ifndef NDEBUG
  std::cout << "scope: " << this << std::endl;

  auto iter = m_identifier_map.begin();
  for (; iter != m_identifier_map.end(); ++iter) {
    auto name = iter->first;
    auto ident = iter->second;
    if (ident->ToTypeName()) {
      std::cout << name << "\t[type:\t" << ident->Type()->Str() << "]"
                << std::endl;
    }
    else {
      std::cout << name << "\t[object:\t" << ident->Type()->Str() << "]"
                << std::endl;
    }
  }
  std::cout << std::endl;
#endif
}
