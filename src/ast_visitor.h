#ifndef C0C_AST_VISITOR_H
#define C0C_AST_VISITOR_H

#include "ast.h"

class ASTVisitor {
public:
  ASTVisitor(ASTNode *node) : m_top(node) {}
  virtual ~ASTVisitor() {}

  virtual bool Visit(ASTNode *node = nullptr);
  bool TraverseTranslationUnitDecl(TranslationUnitDecl *tud);

private:
  ASTNode *m_top;
};

#endif  // C0C_AST_VISITOR_H