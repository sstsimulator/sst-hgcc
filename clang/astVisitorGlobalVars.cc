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
#include "astVisitor.h"
#include <unusedvariablemacro.h>

using namespace clang;

static const Type*
getBaseType(VarDecl* D){
  auto ty = D->getType().getTypePtr();
  while (ty->isPointerType() && !(ty->isFunctionPointerType() || ty->isFunctionProtoType())){
    ty = ty->getPointeeType().getTypePtr();
  }
  return ty;
}

static const Type*
unravelArrayPointers(const Type* ty, int& num_pointers){
  num_pointers = 0;
  while(ty->isPointerType()  && !(ty->isFunctionPointerType() || ty->isFunctionProtoType())){
    ty = ty->getPointeeType().getTypePtr();
    ++num_pointers;
  }

  while(ty->isConstantArrayType() || ty->isArrayType()){
    QualType elty = ty->getAsArrayTypeUnsafe()->getElementType();
    ty = elty.getTypePtr();
    ++num_pointers;
  }

  return ty;
}


static const Type*
getPointerBaseType(VarDecl* D){
  auto ty = D->getType().getTypePtr();
  while (ty->isPointerType()){
    ty = ty->getPointeeType().getTypePtr();
  }
  return ty;
}

static std::string
getFxnTypedef(clang::SourceLocation loc, const Type* ty, const std::string& name){
  std::string typeName = GetAsString(ty);
  //this is horrible... but the only way I know
  auto pos = typeName.find(')');
  if (pos == std::string::npos){
    internalError(loc, "failed typedef on " + typeName);
  }
  std::string tdefName = "typedef " + typeName.substr(0, pos) + name +typeName.substr(pos);
  return tdefName;
}

static void
scopeString(SourceLocation loc, DeclContext* ctx,
            std::list<std::string>& scopes){
  while (ctx->getDeclKind() != Decl::TranslationUnit){
    switch (ctx->getDeclKind()){
      case Decl::Namespace: {
        NamespaceDecl* nsDecl = cast<NamespaceDecl>(ctx);
        ctx = nsDecl->getDeclContext();
        scopes.push_front(nsDecl->getNameAsString());
      }
      break;
      case Decl::CXXRecord: {
        CXXRecordDecl* clsDecl = cast<CXXRecordDecl>(ctx);
        ctx = clsDecl->getDeclContext();
        scopes.push_front(clsDecl->getNameAsString());
      }
      break;
    default:
      errorAbort(loc, "bad context type in scope string");
      break;
    }
  }
}

SkeletonASTVisitor::GlobalVariableReplacement
SkeletonASTVisitor::setupGlobalReplacement(VarDecl *D, const std::string& namePrefix,
                                           bool useAccessor, bool isFxnStatic, bool needFullNs)
{
  bool threadLocal = isThreadLocal(D);
  if (threadLocal){
    errorAbort(D, "thread local variables not yet allowed");
  }

  std::string uniqueName = namePrefix + D->getNameAsString();

  //if a pointer type, the subtype of the pointer
  const Type* ptrSubTy = nullptr;
  if (D->getType()->isPointerType()){
    ptrSubTy = getBaseType(D);
  }

  GlobalVariableReplacement var(uniqueName, useAccessor, isFxnStatic, needFullNs, threadLocal);
  var.anonRecord = checkAnonStruct(D);
  var.arrayInfo = checkArray(D);
  if (var.arrayInfo) {
    var.retType = var.arrayInfo->retType;
    var.typeStr = var.arrayInfo->fqTypedefName;
    delayedInsertAfter(D, var.arrayInfo->typedefDeclString + ";");
  } else if (var.anonRecord){
    var.retType = var.anonRecord->retType;
    SourceLocation openBrace = var.anonRecord->decl->getBraceRange().getBegin();
    CompilerGlobals::rewriter.InsertText(openBrace, "  " + var.anonRecord->typeName, false, false);
    var.typeStr = var.anonRecord->structType + " " + var.anonRecord->typeName;
  } else if (D->getType()->isBooleanType()) {
    var.retType = "bool*";
    var.typeStr = "bool";
  } else if (isCxx() && D->getType()->isStructureOrClassType()) {
    //it's effing obnoxious that I will have to fix this at some point
    //the problem is that Clang in its stupidity doesn't print this type with correct namespacing
    //this produces obnoxious incomplete type or unknown type errors later
    //rather than directly calling D->getType()....
    //this is going to break at some point
    var.typeStr = getCleanName(GetAsString(D->getType()));
    var.retType = var.typeStr + "*";
  } else if (D->getType().getTypePtr()->isFunctionPointerType()){
    const Type* baseType = getPointerBaseType(D);
    bool isTypeDef = isa<TypedefType>(D->getType().getTypePtr()) ||
                     isa<TypedefType>(baseType);
    if (isTypeDef){
      var.typeStr = GetAsString(D->getType());
      var.retType = var.typeStr + "*";
    } else {
      std::string typedefName = D->getNameAsString() + "_sst_hg_fxn_typedef";
      std::string typedefDecl = getFxnTypedef(getStart(D), D->getType().getTypePtr(), typedefName);
      delayedInsertAfter(D, typedefDecl + ";");
      var.typeStr = typedefName;
      var.retType = typedefName + "*";
    }
  } else if (ptrSubTy && (ptrSubTy->isFunctionPointerType() || ptrSubTy->isFunctionProtoType())){
    bool isTypeDef = isa<TypedefType>(ptrSubTy);
    std::string typeStr = GetAsString(D->getType());
    if (isTypeDef){
      var.typeStr = typeStr;
      var.retType = var.typeStr + "*";
    } else {
      std::string typedefName = D->getNameAsString() + "_sst_hg_fxn_typedef";
      std::string typedefDecl = getFxnTypedef(getStart(D), ptrSubTy, typedefName);
      int ptrDepth = std::count(typeStr.begin(), typeStr.end(), '*') - 1;
      var.typeStr = typedefName;
      for (int i=0; i < ptrDepth; i++){
        var.typeStr += "*";
      }
      var.retType = var.typeStr + "*";
      delayedInsertAfter(D, typedefDecl + ";");
    }
  } else if (ptrSubTy && (ptrSubTy->isConstantArrayType() || ptrSubTy->isArrayType())) {
    //the awful case of something declared like char (*)[256]
    //which is a pointer to char[256] types
    var.typeStr = GetAsString(D->getType());

    int num_pointers;
    const Type* elTy = unravelArrayPointers(D->getType().getTypePtr(), num_pointers);
    var.retType = GetAsString(elTy);
    for (int p=0; p < (num_pointers+1); ++p){
      var.retType += "*";
    }
  } else {
    var.typeStr = GetAsString(D->getType());
    if (D->getType()->isEnumeralType()){
      if (var.typeStr.find("anonymous") != std::string::npos){
        //weird case of anonymous enum like enum { X, Y, Z } dim
        //we have no type we can actually refer to, but that's okay
        //just make it an int
        var.typeStr = "int";
      }
    }
    var.retType = var.typeStr + "*";
  }

  return var;
}

void
SkeletonASTVisitor::registerGlobalReplacement(VarDecl* D, GlobalVariableReplacement* repl)
{
  //std::string scopeUniqueVarName = scopePrefix + D->getNameAsString();
  setActiveGlobalScopedName(repl->scopeUniqueVarName);
  scopedNames_[mainDecl(D)] = repl->scopeUniqueVarName;

  if (isGlobalDefinition(D, repl)){
    currentNs_->replVars.emplace(std::piecewise_construct,
          std::forward_as_tuple(repl->scopeUniqueVarName),
          std::forward_as_tuple(isCxx(), repl->isFxnStatic, repl->threadLocal));
  }

  SourceLocation declEnd = getEndLoc(getEnd(D));
  if (declEnd.isInvalid()){
    D->dump();
    errorAbort(D, "unable to locate end of variable declaration");
  }

  const Decl* md = mainDecl(D);
  if (repl->useAccessor){
    globals_.insert({md, GlobalReplacement("", "_getter()", true)});
  } else {
    std::string standinName = "sst_hg_" + repl->scopeUniqueVarName;
    if (repl->needFullNamespace){
      //we could have two variables named x in different namespaces
      //this just makes sure their global var replacements are called different things
      std::string uniqueNsPrefix = currentNs_->nsPrefix();
      for (int i=0; i < uniqueNsPrefix.size(); ++i){
        char& next = uniqueNsPrefix.at(i);
        if (next == ':') next = '_';
      }
      standinName = uniqueNsPrefix + standinName;
    }

    /**
     * we create two ways to use the global variable replacement
     * assume we have a global int a used in the expression
     * int sum = a + 3
     * at the beginning of the function we can create
     * void* sst_hg_global_data = get_sst_hg_global_data();
     * int* sst_hg_a = (int*)(sst_hg_global_data + __offset_a);
     * then we can replace every use of a with (*sst_hg_a), e.g.
     * int sum = (*sst_hg_a) + 3;
     * if a is used many times in a function, we essentially "cache" the replacement
     * every use of 'a' is replaced with 'sst_hg_a'
     *
     * in some scenarios, it's not possible to create the variable sst_hg_a
     * and we need to use the global variable directly inline
     * now we have to replace every use of 'a' with
     * *((int*)(get_sst_hg_global_data() + __offset_a))
     *
     */

    std::stringstream cachedUseSstr;
    std::stringstream inlineUseSstr;
    cachedUseSstr << repl->retType << " " << standinName;
    inlineUseSstr << "(*((" << repl->retType << ")(";

    cachedUseSstr << "=(" << repl->retType << ")";
    if (repl->threadLocal){
      cachedUseSstr << "(sst_hg_tls_data + ";
      inlineUseSstr << "get_sst_hg_tls_data() + ";
    } else {
      cachedUseSstr << "(sst_hg_global_data + ";
      inlineUseSstr << "get_sst_hg_global_data() + ";
    }

    if (repl->needFullNamespace){
      cachedUseSstr << currentNs_->nsPrefix();
      inlineUseSstr << currentNs_->nsPrefix();
    }
    cachedUseSstr << repl->classScope;
    inlineUseSstr << repl->classScope;

    cachedUseSstr << "__offset_" << repl->scopeUniqueVarName << ")";
    inlineUseSstr << "__offset_" << repl->scopeUniqueVarName << ")))";

    GlobalStandin& newStandin = globalStandins_[md];
    newStandin.replText = cachedUseSstr.str();
    newStandin.threadLocal = repl->threadLocal;
    newStandin.fxnStatic = repl->isFxnStatic;

    std::stringstream replSstr;
    replSstr << "(*" << standinName << ")";
    globals_.insert({md, GlobalReplacement(replSstr.str(), inlineUseSstr.str(), false)});
  }
}

bool
SkeletonASTVisitor::setupClassStaticVarDecl(VarDecl* D)
{
  SourceLocation declEnd = getEndLoc(getEnd(D));
  bool threadLocal = isThreadLocal(D);
  //later on we will have to actually handle its definition in another file
  if (D->getDeclContext()->isDependentContext()){
    dependentStaticMembers_[D->getNameAsString()] = D;
    //don't privatize this anymore - it breaks all sorts of things like decltype
    //we used to privatize variables to force compiler errors

    //create an accessor method for the variable
    //use decltype for return... this can create weirdness

    GlobalVariableReplacement repl = setupGlobalReplacement(D, "",
                              false, /*uses of the variable don't need any extra scope prefixes*/
                              true, /*this should be accessed through the special getter method above*/
                              false /*don't need full ns added*/);
    std::stringstream os;
    os << "static " << repl.typeStr << "& "
        << D->getNameAsString() << "_getter(){ "
        << " char* data = " << (threadLocal ? "get_sst_hg_tls_data(); " : "get_sst_hg_global_data(); ")
        << " void* offsetPtr = data + __offset_" << D->getNameAsString() << ";"
        << "return *((" << repl.retType << ")(offsetPtr));"
        << "}";
    //everywhere the variable A->x or A.x is used will be replaced with the function above to be
    //A->x_getter() or A.x_getter()
    CompilerGlobals::rewriter.InsertText(declEnd, os.str());

    registerGlobalReplacement(D, &repl);
  } else {
    std::stringstream varname_scope_sstr; varname_scope_sstr << "_";
    std::stringstream cls_scope_sstr;
    for (CXXRecordDecl* decl : classContexts_){
      varname_scope_sstr << "_" << decl->getNameAsString();
      cls_scope_sstr << decl->getNameAsString() << "::";
    }

    GlobalVariableReplacement repl = setupGlobalReplacement(D, "", false, false, true);
    repl.classScope = cls_scope_sstr.str();
    registerGlobalReplacement(D, &repl);
  }

  std::string offsetDecl = " public: static int __offset_" + D->getNameAsString() + ";";
  delayedInsertAfter(D, offsetDecl);
  return true;
}

 bool
SkeletonASTVisitor::isGlobalDefinition(VarDecl* D, GlobalVariableReplacement* var)
{
  if (D->hasExternalStorage()){
    return false;
  }
  if (var->arrayInfo && var->arrayInfo->implicitSize && D->isThisDeclarationADefinition() == VarDecl::TentativeDefinition){
    //a weird c-style forward declaration
    //like int x[]; which defines the symbol but doesn't actually give the size
    return false;
  }
  return true;
}

bool
SkeletonASTVisitor::setupCGlobalVar(VarDecl* D, const std::string& scopePrefix)
{
  Expr* init = D->hasInit() ? getUnderlyingExpr(D->getInit()) : nullptr;

  std::stringstream newVarSstr;
  GlobalVariableReplacement var = setupGlobalReplacement(D, scopePrefix, false, false, true);
  newVarSstr << "extern int __offset_" << var.scopeUniqueVarName << "; ";
  if (isGlobalDefinition(D, &var)){
    std::string initFxnName = "sst_hg_init_" + var.scopeUniqueVarName;
    //add an init function for it
    newVarSstr << "void " << initFxnName << "(void* ptr){";

    //recreate the global variable temporarily in this function,
    //but with globals refs correctly replaced
    if (init){
      newVarSstr << var.typeStr << " initer = "
          << printWithGlobalsReplaced(init) << "; ";
      newVarSstr << "memcpy(ptr, (void*) &initer, sizeof(initer));";
    }
    newVarSstr << " } ";
    newVarSstr << "int __sizeof_" << var.scopeUniqueVarName << " = sizeof(" << var.typeStr << "); ";
  }
  registerGlobalReplacement(D, &var);
  delayedInsertAfter(D, newVarSstr.str());

  return true;
}

bool
SkeletonASTVisitor::setupCppGlobalVar(VarDecl* D, const std::string& scopePrefix)
{
  Expr* init = D->hasInit() ? getUnderlyingExpr(D->getInit()) : nullptr;
  Stmt* cppCtorArgs = nullptr;
  RecordDecl* rd = D->getType().getTypePtr()->getAsCXXRecordDecl();
  std::stringstream newVarSstr;
  GlobalVariableReplacement repl = setupGlobalReplacement(D, scopePrefix, false, false, true);
  bool isDef = isGlobalDefinition(D, &repl);
  if (isDef){
    if (rd){
      cppCtorArgs = getCtor(D);
    } else if (init) {
      cppCtorArgs = init;
    }
  }

  newVarSstr << "extern int __offset_" << repl.scopeUniqueVarName << "; ";
  if (isDef){
    newVarSstr << "int __sizeof_" << repl.scopeUniqueVarName << " = sizeof(" << repl.typeStr << ");";
    auto pos = repl.typeStr.find("struct ");
    if (pos != std::string::npos){
      repl.typeStr = eraseAllStructQualifiers(repl.typeStr);
    }

    newVarSstr << "static std::function<void(void*)> sst_hg_init_" << repl.scopeUniqueVarName
               << " = [](void* ptr){"
                  " new (ptr) " << repl.typeStr;
    if (cppCtorArgs){
      switch (cppCtorArgs->getStmtClass()){
      case Stmt::InitListExprClass:
        newVarSstr << printWithGlobalsReplaced(cppCtorArgs);
        break;
      default:
        newVarSstr << "{" << printWithGlobalsReplaced(cppCtorArgs) << "}";
        break;
      }
    }
    newVarSstr << "; };";

    if (D->getStorageClass() == SC_Static){
      newVarSstr << "static ";
    }
    newVarSstr << "SST::Hg::CppGlobalRegisterGuard "
               << D->getNameAsString() << "_sst_hg_ctor("
               << "__offset_" << repl.scopeUniqueVarName
               << ", __sizeof_" << repl.scopeUniqueVarName
               << ", " << std::boolalpha << repl.threadLocal
               << ", \"" << D->getNameAsString() << "\""
               << ", std::move(sst_hg_init_" << repl.scopeUniqueVarName << "));";
  }
  registerGlobalReplacement(D, &repl);
  delayedInsertAfter(D, newVarSstr.str());
  return true;
}

bool
SkeletonASTVisitor::setupFunctionStaticCpp(VarDecl* D, const std::string& scopePrefix)
{
  if (insideTemplateFxn()){
    internalError(D, "static function variables in template functions not yet supported");
  }

  FunctionDecl* outerFxn = CompilerGlobals::astContextLists.enclosingFunctionDecls.front();
  SourceLocation fxnStart = getStart(outerFxn);

  GlobalVariableReplacement var = setupGlobalReplacement(D, scopePrefix, false, true, false);
  std::string replText = "int __sizeof_" + var.scopeUniqueVarName + " = sizeof(void*); "
      " extern int __offset_" + var.scopeUniqueVarName + "; "
      " extern \"C\" void sst_hg_init_" + var.scopeUniqueVarName + "(void* ptr){ "
      "   void** ptrptr = (void**) ptr; "
      "   *ptrptr = nullptr; "
      "}";

  CompilerGlobals::rewriter.InsertText(fxnStart, replText, false);

  std::string initializer;
  if (var.arrayInfo){
    //create a temp that is the original object initialzed
    //the *x = expr syntax is not valid for certain initializations
    initializer = "sst_hg_" + var.scopeUniqueVarName + "= ("
        + var.arrayInfo->typedefName + "*) new char[sizeof(" + var.typeStr + ")];";
    if (D->hasInit()){
      initializer += var.arrayInfo->typedefName + " initer_" + var.scopeUniqueVarName
                    + "=" + printWithGlobalsReplaced(D->getInit()) + ";";
      //then memcopy from it into the original
      initializer += " memcpy(sst_hg_" + var.scopeUniqueVarName + ", initer_" + var.scopeUniqueVarName
                  + ", sizeof(" + var.arrayInfo->typedefName + "));";
    }
  } else {
    std::string ctor;
    if (D->hasInit()){
      ctor = printWithGlobalsReplaced(D->getInit());
      switch(D->getInit()->getStmtClass()){
       case Stmt::InitListExprClass:
       case Stmt::CXXConstructExprClass:
         break;
       default:
        ctor = "{ " + ctor + " }";
        break;
      }
    } else {
      ctor = "{}";
    }
    initializer = "sst_hg_" + var.scopeUniqueVarName + "= new " + var.typeStr + ctor + ";";
  }



  std::string initText =
   "void** ptr_sst_hg_" + var.scopeUniqueVarName
    + " = ((void**)(sst_hg_global_data + __offset_" + var.scopeUniqueVarName + "));"
    + var.typeStr + "* sst_hg_" + var.scopeUniqueVarName + " = (" + var.typeStr
    + "*)(*ptr_sst_hg_" + var.scopeUniqueVarName + "); "
   "if (sst_hg_" + var.scopeUniqueVarName + " == nullptr){ "
    + initializer +
   "  *ptr_sst_hg_" + var.scopeUniqueVarName + " = sst_hg_" + var.scopeUniqueVarName + "; "
   "}";
  delayedInsertAfter(D, initText);
  registerGlobalReplacement(D, &var);
  //make sure this is included for the function
  globalsTouched_.back().insert(mainDecl(D));
  return true;
}

bool
SkeletonASTVisitor::setupFunctionStaticC(VarDecl* D, const std::string& scopePrefix)
{
  FunctionDecl* outerFxn = CompilerGlobals::astContextLists.enclosingFunctionDecls.front();
  SourceLocation fxnStart = getStart(outerFxn);

  GlobalVariableReplacement var = setupGlobalReplacement(D, scopePrefix, false, true, false);
  std::string replText = "int __sizeof_" + var.scopeUniqueVarName + " = sizeof(void*); "
      " extern int __offset_" + var.scopeUniqueVarName + "; "
      " void sst_hg_init_" + var.scopeUniqueVarName + "(void* ptr){ "
      "   void** ptrptr = (void**) ptr; "
      "   *ptrptr = 0; "
      "}";
  CompilerGlobals::rewriter.InsertText(fxnStart, replText, false);

  std::string initText =
   "void** ptr_sst_hg_" + var.scopeUniqueVarName
    + " = ((void**)(sst_hg_global_data + __offset_" + var.scopeUniqueVarName + "));"
    + var.typeStr + "* sst_hg_" + var.scopeUniqueVarName + " = (" + var.typeStr
    + "*)(*ptr_sst_hg_" + var.scopeUniqueVarName + "); "
   "if (sst_hg_" + var.scopeUniqueVarName + " == 0){ "
   "  sst_hg_" + var.scopeUniqueVarName + " = (" + var.typeStr + "*) malloc(sizeof(" + var.typeStr + ")); "
   "  *ptr_sst_hg_" + var.scopeUniqueVarName + " = sstgh_" + var.scopeUniqueVarName + "; }";
  if (var.arrayInfo && D->hasInit()){
    //create a temp that is the original object initialzed
    //the *x = expr syntax is not valid for certain initializations
    initText += var.arrayInfo->typedefName + " initer_" + var.scopeUniqueVarName
                  + "=" + printWithGlobalsReplaced(D->getInit()) + ";";
    //then memcopy from it into the original
    initText += " memcpy(*ptr_sst_hg_" + var.scopeUniqueVarName + ", initer_" + var.scopeUniqueVarName
                + ", sizeof(" + var.arrayInfo->typedefName + "));";
  } else if (D->hasInit()){
    initText += "*sst_hg_" + var.scopeUniqueVarName + " = ("
        + var.typeStr + ") " + printWithGlobalsReplaced(D->getInit()) + ";";
  }

  delayedInsertAfter(D, initText);
  registerGlobalReplacement(D, &var);
  //make sure this is included for the function
  globalsTouched_.back().insert(mainDecl(D));
  return true;
}

bool
SkeletonASTVisitor::checkDeclStaticClassVar(VarDecl *D)
{
  if (classContexts_.size() > 1){
    errorAbort(D, "cannot handle static variables in inner classes");
  }

  if (!D->hasInit()){
    setupClassStaticVarDecl(D);
  } //else this must be a const integer if inited in the header file
  //we don't have to "deglobalize" this

  return false;
}

bool
SkeletonASTVisitor::checkStaticFileVar(VarDecl* D)
{
  std::string prefix = currentNs_->filePrefix(getStart(D));
  if (isCxx()){
    return setupCppGlobalVar(D, prefix);
  } else {
    return setupCGlobalVar(D, prefix);
  }
}

bool
SkeletonASTVisitor::checkGlobalVar(VarDecl* D)
{
  if (isCxx()){
    return setupCppGlobalVar(D, "");
  } else {
    return setupCGlobalVar(D, "");
  }
}

bool
SkeletonASTVisitor::checkStaticFxnVar(VarDecl *D)
{
  FunctionDecl* outerFxn = CompilerGlobals::astContextLists.enclosingFunctionDecls.front();
  std::stringstream prefix_sstr;
  prefix_sstr << "_" << outerFxn->getNameAsString();

  auto& staticVars = staticFxnVarCounts_[outerFxn];
  int& cnt = staticVars[D->getNameAsString()];
  //due to scoping, we might have several static fxn vars
  //all with the same name
  if (cnt != 0){
    prefix_sstr << "_" << cnt;
  }
  ++cnt;

  std::string scope_prefix = currentNs_->filePrefix(getStart(D)) + prefix_sstr.str();

  if (isCxx()){
    return setupFunctionStaticCpp(D, scope_prefix);
  } else {
    return setupFunctionStaticC(D, scope_prefix);
  }
}

bool
SkeletonASTVisitor::checkInstanceStaticClassVar(VarDecl *D)
{
  auto iter = globals_.find(mainDecl(D));
  if (iter == globals_.end()){
    //hmm, this must be const or something
    //because the main decl didn't end up in the globals
    return true;
  }

  //okay - this sucks - I have no idea how this variable is being declared
  //it could be ns::A::x = 10 for a variable in namespace ns, class A
  //it could namespace ns { A::x = 10 }
  //we must have the variable declarations in the full namespace - we can't cheat
  DeclContext* lexicalContext = D->getLexicalDeclContext();
  DeclContext* semanticContext = D->getDeclContext();
  if (!isa<CXXRecordDecl>(semanticContext)){
    std::string error = "variable " + D->getNameAsString() + " does not have class semantic context";
    errorAbort(D, error);
  }
  CXXRecordDecl* parentCls = cast<CXXRecordDecl>(semanticContext);
  std::list<std::string> lex;
  scopeString(getStart(D), lexicalContext, lex);
  std::list<std::string> sem;
  scopeString(getStart(D), semanticContext, sem);

  //match the format from checkDeclStaticClassVar

  std::stringstream os;
  //throw away the class name in the list of scopes
  sem.pop_back();
  auto semBegin = sem.begin();

  std::advance(semBegin, lex.size());
  for (auto iter = semBegin; iter != sem.end(); ++iter){ //I must init/declare vars in the most enclosing namespace
    os << "namespace " << *iter << " {";
  }

  std::string clsName = parentCls->getNameAsString();
  std::string tagName = "UniqueTag" + clsName + D->getNameAsString();
  os << "struct " << tagName << "{}; ";
  if (parentCls->isDependentContext()){
    std::stringstream tmplSstr; //the name A<T,U>
    tmplSstr << clsName << "<";
    int numLists = D->getNumTemplateParameterLists();
    if (numLists > 1){
      internalError(getStart(D), "cannot handle nested template declarations");
    }
    TemplateParameterList* theList = D->getTemplateParameterList(0);
    getTemplatePrefixString(os, theList);
    getTemplateParamsString(tmplSstr, theList);
    clsName = tmplSstr.str();
  }

  GlobalVariableReplacement repl = setupGlobalReplacement(D, "", false, false, true);
  os << "int " << clsName
     << "::__offset_" << D->getNameAsString()
     << " = SST::Hg::inplaceCppGlobal<"
     << tagName
     << "," << repl.typeStr
     << "," << std::boolalpha << isThreadLocal(D)
     << ">(" << "\"" << D->getNameAsString() << "\""
     << ",[](void* ptr){ "
     << "    new (ptr) " << repl.typeStr;

  if (D->hasInit()){
    switch (D->getInit()->getStmtClass()){
    case Stmt::InitListExprClass:
      os << printWithGlobalsReplaced(D->getInit());
      break;
    default:
      os << "{" << printWithGlobalsReplaced(D->getInit()) << "}";
      break;
    }
  } else {
    os << "{}; ";
  }
  os << "; });";

  for (auto iter = semBegin; iter != sem.end(); ++iter){
    os << "}"; //close namespaces
  }

  delayedInsertAfter(D, os.str());

  return true;
}
