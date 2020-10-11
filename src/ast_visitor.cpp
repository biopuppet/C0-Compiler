#include "ast_visitor.h"
#include <iostream>

bool ASTVisitor::Visit(ASTNode *node)
{
  if (node == nullptr) {
    if (m_top != nullptr) {
      return Visit(m_top);
    }
    else {
      return false;
    }
  }
  // TODO
  return true;
}

bool ASTVisitor::TraverseTranslationUnitDecl(TranslationUnitDecl *tud)
{
  if (tud == nullptr) {
    return false;
  }
  for (auto extDecl : tud->ExtDecls()) {
    Visit(extDecl);
    // std::cout << extDecl << std::endl;
  }
  return false;
}
