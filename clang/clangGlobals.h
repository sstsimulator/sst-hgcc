/**
Copyright 2009-2023 National Technology and Engineering Solutions of Sandia,
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S. Government
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly
owned subsidiary of Honeywell International, Inc., for the U.S. Department of
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2023, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
*/
/**
Copyright 2009-2023 National Technology and Engineering Solutions of Sandia,
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S. Government
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly
owned subsidiary of Honeywell International, Inc., for the U.S. Department of
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2023, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
*/

#pragma once

#include <map>
#include <list>
#include "clangHeaders.h"

namespace clang {
  class CompilerInstance;
  class ASTContext;
  class SourceManager;
  class LangOptions;
  class Sema;
}

namespace modes {

enum ModeMask {
  ENCAPSULATE = 1 << 0,
  MEMOIZE = 1 << 1,
  SKELETONIZE = 1 << 2,
  SHADOWIZE = 1 << 3,
  PUPPETIZE = 1 << 4
};

enum Mode {
  ENCAPSULATE_MODE = 0,
  MEMOIZE_MODE = 1,
  SKELETONIZE_MODE = 2,
  SHADOWIZE_MODE = 3,
  PUPPETIZE_MODE = 4,
  NUM_MODES
};

}

class SkeletonASTVisitor;
class FirstPassASTVisitor;
struct SSTPragma;
class SSTReplacePragma;
class SSTNullVariablePragma;

/**
 * @brief The ASTContextLists struct
 * This contains data structures that keep track of the enclosing context
 * of the currently being visited statement
 */
struct ASTContextLists {
  std::list<clang::FunctionDecl*> enclosingFunctionDecls;
  std::list<clang::CompoundStmt*> compoundStmtBlocks;
  clang::CompoundStmt::body_iterator currentStmtBlockBegin(){
    return compoundStmtBlocks.back()->body_begin();
  }

  clang::CompoundStmt::body_iterator findStmtBlockMatch(clang::Stmt* s){
    clang::CompoundStmt* block = compoundStmtBlocks.back();
    for (auto* iter = block->body_begin(); iter != block->body_end(); ++iter){
      if (*iter == s) return iter;
    }
    return block->body_end();
  }

  clang::CompoundStmt::body_iterator currentStmtBlockEnd(){
    return compoundStmtBlocks.back()->body_end();
  }
};

/**
 * @brief The ASTNodeMetadata struct
 * This contains data structures that keep track of specifing annotations or markings
 * on specific AST nodes that are required for correct mutations or rewrites later
 */
struct ASTNodeMetadata {
  std::map<clang::Decl*,SSTNullVariablePragma*> nullVariables;
  std::map<const clang::DeclContext*,SSTNullVariablePragma*> nullSafeFunctions;
  std::map<clang::Stmt*, std::string> computeMemoryOverrides;
  std::map<clang::IfStmt*,std::string> predicatedBlocks;
  std::string dependentScopeGlobal;
};

/**
 * @brief The ToolInfoRegistration struct
 * This contains data structures that keep track of things that must be registered
 * in a separate .cpp file and linked in. These are usually things like global variable
 * registrations with the simulator.
 */
struct ToolInfoRegistration {
  std::vector<std::pair<SSTPragma*, std::string>> globalCppFunctionsToWrite;
  std::set<std::string> extraInputFileParams;
};

struct PragmaConfig {
  bool makeNoChanges = false;

  PragmaConfig() = default;
};

struct PragmaDeletionSignal {
  bool stmtDeleted = false;
  bool declDeleted = false;
  clang::Stmt* deletedStmt = nullptr;
  clang::Decl* deletedDecl = nullptr;

  void signalStmtDelete(clang::Stmt* s) { stmtDeleted = true; deletedStmt = s; }
  void signalDeclDelete(clang::Decl* d) { declDeleted = true; deletedDecl = d; }
  bool hasSignal() const { return stmtDeleted || declDeleted; }
  void clear() { stmtDeleted = false; declDeleted = false; deletedStmt = nullptr; deletedDecl = nullptr; }
};

struct CompilerGlobals {

  // --- Immutable config (set once in setup(), read-only thereafter) ---
  static modes::Mode mode;
  static int modeMask;
  static clang::CompilerInstance* ci;
  static llvm::cl::OptionCategory ssthgCategoryOpt;

  static bool modeActive(int mask){
    return modeMask & mask;
  }

  static clang::CompilerInstance& CI() {
    return *ci;
  }
  static clang::SourceManager& SM() {
    return ci->getSourceManager();
  }
  static clang::ASTContext& ASTContext() {
    return ci->getASTContext();
  }
  static const clang::LangOptions& LangOpts() {
    return ci->getLangOpts();
  }

  static void setup(clang::CompilerInstance* ci);

  // --- Mutable traversal state (modified during AST passes) ---

  // Written by all rewrite passes (7 files); read by EndSourceFileAction for output.
  static clang::Rewriter rewriter;

  // Pass 1 writes (pragma handlers populate maps) -> Pass 2 reads (visitors query maps).
  // Cross-pass data channel for null variables, branch predictions, memory overrides.
  static ASTNodeMetadata astNodeMetadata;

  // Stack-like context tracking: pushed on scope entry, popped on exit via RAII PushGuard.
  static ASTContextLists astContextLists;

  // Per-statement flag toggled by SSTKeepPragma, checked/reset by PragmaActivateGuard.
  static PragmaConfig pragmaConfig;

  // Set by pragma activate() methods to signal node deletion; checked by PragmaActivateGuard.
  static PragmaDeletionSignal deletionSignal;

  // Accumulated output: pragmas register items, EndSourceFileAction reads them.
  static ToolInfoRegistration toolInfoRegistration;

  // Set once per pass to the active visitor instance.
  static union {
    SkeletonASTVisitor* skeleton;
    FirstPassASTVisitor* firstPass;
  } visitor;

  static void setVisitor(SkeletonASTVisitor& v){
    visitor.skeleton = &v;
  }

  static void setVisitor(FirstPassASTVisitor& v){
    visitor.firstPass = &v;
  }
};
