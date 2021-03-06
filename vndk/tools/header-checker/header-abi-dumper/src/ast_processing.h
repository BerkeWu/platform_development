// Copyright (C) 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AST_PROCESSING_H_
#define AST_PROCESSING_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#include "proto/abi_dump.pb.h"
#pragma clang diagnostic pop

#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Mangle.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/PPCallbacks.h>

class HeaderASTVisitor
    : public clang::RecursiveASTVisitor<HeaderASTVisitor> {
 public:
  HeaderASTVisitor(abi_dump::TranslationUnit *tu_ptr,
                   clang::MangleContext *mangle_contextp,
                   const clang::ASTContext *ast_contextp,
                   const clang::CompilerInstance *compiler_instance_p,
                   const std::string &current_file_name);

  bool VisitCXXRecordDecl(const clang::CXXRecordDecl *decl);

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);

 private:
  bool SetupFunction(abi_dump::FunctionDecl *methodp,
                     const clang::FunctionDecl *decl,
                     const std::string &source_file);

  bool SetupClassFields(abi_dump::RecordDecl *classp,
                        const clang::CXXRecordDecl *decl,
                        const std::string &source_file);

 bool SetupClassBases(abi_dump::RecordDecl *classp,
                      const clang::CXXRecordDecl *decl);

  std::string GetDeclSourceFile(const clang::NamedDecl *decl);

  std::string AccessToString(const clang::AccessSpecifier sp);

  std::string GetMangledNameDecl(const clang::NamedDecl *decl);

 private:
  abi_dump::TranslationUnit *tu_ptr_;
  clang::MangleContext *mangle_contextp_;
  const clang::ASTContext *ast_contextp_;
  const clang::CompilerInstance *cip_;
  const std::string current_file_name_;
};

class HeaderASTConsumer : public clang::ASTConsumer {
 public:
  HeaderASTConsumer(const std::string &file_name,
                    clang::CompilerInstance *compiler_instancep,
                    const std::string &out_dump_name);

  void HandleTranslationUnit(clang::ASTContext &ctx) override;

  void HandleVTable(clang::CXXRecordDecl *crd) override;

 private:
  std::string file_name_;
  clang::CompilerInstance *cip_;
  std::string out_dump_name_;
};

class HeaderASTPPCallbacks : public clang::PPCallbacks {
 public:
  void MacroDefined(const clang::Token &macro_name_tok,
                    const clang::MacroDirective *) override;
};

#endif  // AST_PROCESSING_H_
