#include "array.h"
#include<iostream>
#include<string>
#include <stdlib.h> 

#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

using namespace llvm;
using namespace std;

static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static map<std::string, AllocaInst*> NamedValues;
static map<std::string, Value*> GlobalNamedValues;
static map<std::string, int> ConstantNamedValues;
static Function *F;
static Value* RetVal;
static BasicBlock *BB;
static map<std::string, pair<Value*, Value*> > phiMap;

static void *Error(const char *Str) { 
    fprintf(stderr, "Error: %s\n", Str);
}

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
        const std::string &VarName, const std::string &varType) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
            TheFunction->getEntryBlock().begin());
    if (varType.compare("int")==0) {
        return TmpB.CreateAlloca(IntegerType::get(getGlobalContext(), 32), nullptr,
                VarName.c_str());
    } else {
        return TmpB.CreateAlloca(IntegerType::get(getGlobalContext(), 1), nullptr,
                VarName.c_str());
    }
}

static AllocaInst *CreateEntryBlockAllocaArray(Function *TheFunction,
        const std::string &VarName, const std::string &varType, const int varSize) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
            TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(ArrayType::get(IntegerType::get(TheModule->getContext(), 32), varSize), nullptr, VarName.c_str());
}


class Common {
    public:
        static void printSpaces(int indent) {
            for(int i=0; i<indent; i++) {
                cout << " ";
            }
        }
};
class Decl {
    public:
        virtual void print(int indent) {}

        virtual Value *Codegen() {
        }
};
class Program {
    public:
        Array<Decl*> *declarations;
        void print() {
            cout << "<Program>" << endl;

            cout << "  <DeclList>" << endl;
            for (int i=0;i<declarations->size();i++) {
                declarations->elementAt(i)->print(4);
            }
            cout << "  </DeclList>" << endl;

            cout << "</Program>" << endl;
        }

        virtual void Codegen() {
            // If we do following, we don't have to call Codegen() for FieldDecl and MethodDecl from parser. 
            for(int i=0; i<declarations->size(); ++i) {
                declarations->elementAt(i)->Codegen();
            }
        }
};
class Field {
    public: 
        string *fieldName;
        int fieldSize;
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Field>" << endl;

            Common::printSpaces(indent+2);
            cout << "<FieldName>" << endl;
            Common::printSpaces(indent+4);
            cout << *fieldName << endl;
            Common::printSpaces(indent+2);
            cout << "</FieldName>" << endl;

            if(fieldSize!=-1) {
                Common::printSpaces(indent+2);
                cout << "<FieldSize>" << endl;
                Common::printSpaces(indent+4);
                cout << fieldSize << endl;
                Common::printSpaces(indent+2);
                cout << "</FieldSize>" << endl;
            }

            Common::printSpaces(indent);
            cout << "</Field>" << endl;
        }

        virtual Value *Codegen() {
        }
};
class FieldDecl : public Decl {
    public: 
        string *fieldType;
        Array<Field*> *fieldList;
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<FieldDecl>" << endl;

            Common::printSpaces(indent+2);
            cout << "<FieldType>" << endl;
            Common::printSpaces(indent+4);
            cout << *fieldType << endl;
            Common::printSpaces(indent+2);
            cout << "</FieldType>" << endl;

            Common::printSpaces(indent+2);
            cout << "<FieldList>" << endl;
            for(int i=0; i<fieldList->size(); i++) {
                fieldList->elementAt(i)->print(indent+4);
            }
            Common::printSpaces(indent+2);
            cout << "</FieldList>" << endl;

            Common::printSpaces(indent);
            cout << "</FieldDecl>" << endl;
        }

        //Note: FieldDecl can only have Global declarations, local declarations go to FieldDeclStatement and we do not generate code corresponding to it. 
        virtual Value* Codegen(){
            if(*fieldType=="int"){
                for(int i=0; i<fieldList->size(); ++i){
                    if(fieldList->elementAt(i)->fieldSize!=-1){
                        ArrayType* ArrayTy_0 = ArrayType::get(IntegerType::get(TheModule->getContext(), 32), fieldList->elementAt(i)->fieldSize);
                        GlobalVariable* gvar_array_a = new GlobalVariable(*TheModule,ArrayTy_0,false,GlobalValue::CommonLinkage,0,*(fieldList->elementAt(i)->fieldName));
                        gvar_array_a->setAlignment(16);
                        GlobalNamedValues[*(fieldList->elementAt(i)->fieldName)]=gvar_array_a;
                    } else{
                        GlobalVariable* gvar_ptr_abc = new GlobalVariable(*TheModule,IntegerType::get(getGlobalContext(),32),false,GlobalValue::CommonLinkage,nullptr,*(fieldList->elementAt(i)->fieldName));
                        gvar_ptr_abc->setAlignment(4);
                        GlobalNamedValues[*(fieldList->elementAt(i)->fieldName)]=gvar_ptr_abc;
                    } 
                }
            }
            if(*fieldType=="boolean"){
                for(int i=0; i<fieldList->size(); ++i){
                    /* Boolean is a type of Integer1 = Awesomeness!! :D */
                    if(fieldList->elementAt(i)->fieldSize!=-1){
                        ArrayType* ArrayTy_0 = ArrayType::get(IntegerType::get(TheModule->getContext(), 1), fieldList->elementAt(i)->fieldSize);
                        GlobalVariable* gvar_array_a = new GlobalVariable(*TheModule,ArrayTy_0,false,GlobalValue::CommonLinkage,0,*(fieldList->elementAt(i)->fieldName));
                        gvar_array_a->setAlignment(16);
                        GlobalNamedValues[*(fieldList->elementAt(i)->fieldName)]=gvar_array_a;
                    } else {
                        GlobalVariable* gvar_ptr_abc = new GlobalVariable(*TheModule,IntegerType::get(getGlobalContext(),1),false,GlobalValue::CommonLinkage,nullptr,*(fieldList->elementAt(i)->fieldName));
                        gvar_ptr_abc->setAlignment(1);
                        GlobalNamedValues[*(fieldList->elementAt(i)->fieldName)]=gvar_ptr_abc;
                    }
                }
            }
        }
};
class Expr {
    public:
        string *op;
        string *identifier;
        string *constant;
        Expr *expr1, *expr2;
        Array<Expr*> *exprList;
        virtual void print(int indent) {
        }

        virtual Value* Codegen(){
        }
};
class UnaryExpr : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<UnaryExpr>" << endl;

            Common::printSpaces(indent+2);
            cout << "<UnaryOperator>" << endl; 
            Common::printSpaces(indent+4);
            cout << *op << endl;
            Common::printSpaces(indent+2);
            cout << "</UnaryOperator>" << endl; 

            expr1->print(indent+2);

            Common::printSpaces(indent);
            cout << "</UnaryExpr>" << endl;
        }

        virtual Value* Codegen() {
            //Minus and Not
            Value *R = expr1->Codegen();
            switch ((*op)[0]) {
                case '!': return Builder.CreateNot(R, "not"); break;
                case '-': return Builder.CreateNeg(R, "neg"); break;
            }
        }
};
class BinaryExpr : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<BinaryExpr>" << endl;

            expr1->print(indent+2);

            Common::printSpaces(indent+2);
            cout << "<Operator>" << endl; 
            Common::printSpaces(indent+4);
            cout << *op << endl;
            Common::printSpaces(indent+2);
            cout << "</Operator>" << endl; 

            expr2->print(indent+2);

            Common::printSpaces(indent);
            cout << "</BinaryExpr>" << endl;
        }

        virtual Value *Codegen(){
            Value *L = expr1->Codegen();
            Value *R = expr2->Codegen();
            if (L == nullptr || R == nullptr) return 0;
            switch ((*op)[0]) {
                case '+': return Builder.CreateAdd(L, R, "add"); break;
                case '-': return Builder.CreateSub(L, R, "sub"); break;
                case '*': return Builder.CreateMul(L, R, "mul"); break;
                case '/': return Builder.CreateUDiv(L, R, "div"); break;
                case '%': return Builder.CreateURem(L, R, "modulus"); break;
                case '<': 
                {
                    if((*op)[1]=='=') {
                        return Builder.CreateICmpULE(L, R, "less equal");
                    } else {
                        return Builder.CreateICmpULT(L, R, "less than");
                    }
                    break;
                }
                case '>': 
                {
                    if((*op)[1]=='=') {
                        return Builder.CreateICmpUGE(L, R, "greater equal");
                    } else {
                        return Builder.CreateICmpUGT(L, R, "greater than");
                    }
                    break;
                }
                case '|': return Builder.CreateOr(L, R, "or"); break;
                case '&': return Builder.CreateAnd(L, R, "and"); break;
                case '=': return Builder.CreateICmpEQ(L, R, "Double equals"); break;
                case '!': return Builder.CreateICmpNE(L, R, "Not equal"); break;
            }
        }
};
class Location : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Location>" << endl;

            Common::printSpaces(indent+2);
            cout << "<VariableName>" << endl;
            Common::printSpaces(indent+4);
            cout << *identifier << endl;
            Common::printSpaces(indent+2);
            cout << "</VariableName>" << endl;

            if(expr1) {
                Common::printSpaces(indent+2);
                cout << "<VariableAccessLocation>" << endl;
                expr1->print(indent+4);
                Common::printSpaces(indent+2);
                cout << "</VariableAccessLocation>" << endl;
            }

            Common::printSpaces(indent);
            cout << "</Location>" << endl;
        }

        virtual Value* Codegen(){
            if(expr1) {
                //array
                AllocaInst *V = NamedValues[*identifier];
                if (V == 0) {
                    Value *V2 = GlobalNamedValues[*identifier];
                    if (V2==0) {
                        Error("Unknown variable name");
                        return nullptr;
                    }
                    //global array
                    ConstantInt* const_zero = ConstantInt::get(TheModule->getContext(), APInt(64, StringRef("0"), 10));
                    int64_t indice_actual;
                    if(expr1->identifier) {
                        indice_actual = ConstantNamedValues[*expr1->identifier];
                    } else {
                        ConstantInt* CI = dyn_cast<ConstantInt>(expr1->Codegen());
                        indice_actual = CI->getSExtValue();
                    }
                    ConstantInt* const_actual = ConstantInt::get(TheModule->getContext(), APInt(64, indice_actual, 10));
                    std::vector<Constant*> indices;
                    indices.push_back(const_zero);
                    indices.push_back(const_actual);
                    Constant* global_ptr = dyn_cast<Constant>(V2);
                    Constant* element_ptr = ConstantExpr::getGetElementPtr(global_ptr, indices);
                    LoadInst* ret = new LoadInst(element_ptr, "", false, BB);
                    return ret;
                }
                //local array
                if(identifier) {
                    return Builder.CreateExtractValue(Builder.CreateLoad(V), ConstantNamedValues[*identifier]);
                }
                ConstantInt* CI = dyn_cast<ConstantInt>(expr1->Codegen());
                int64_t indice_actual = CI->getSExtValue();
                return Builder.CreateExtractValue(Builder.CreateLoad(V), indice_actual);
            } else {
                //variable
                AllocaInst *V = NamedValues[*identifier];
                if (V == 0) {
                    Value *V2 = GlobalNamedValues[*identifier];
                    if (V2==0) {
                        Error("Unknown variable name");
                        return nullptr;
                    }
                    //global variable
                    GlobalVariable *g_var = dyn_cast<GlobalVariable>(V2);
                    return g_var->getInitializer();
                }
                //local variable
                return Builder.CreateLoad(V, (*identifier));
            }
        }
};
class MethodCall : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<MethodCall>" << endl;            

            Common::printSpaces(indent+2);
            cout << "<MethodName>" << endl;            
            Common::printSpaces(indent+4);
            cout << *identifier << endl;
            Common::printSpaces(indent+2);
            cout << "</MethodName>" << endl;            

            Common::printSpaces(indent+2);
            cout << "<ParameterExpressions>" << endl;            
            if(exprList) {
                for (int i = 0; i < exprList->size(); i++) {
                    exprList->elementAt(i)->print(indent+4);
                }
            }
            Common::printSpaces(indent+2);
            cout << "</ParamenterExpressions>" << endl;            

            Common::printSpaces(indent);
            cout << "</MethodCall>" << endl;  
        }

        virtual Value* Codegen(){
            Function *CalleeF = TheModule->getFunction(*identifier);
            if (CalleeF == 0) {
                Error("Unknown function referenced");
                return nullptr;
            }

            if (CalleeF->arg_size() != exprList->size()) {
                Error("Incorrect # arguments passed");
                return nullptr;
            }

            std::vector<Value*> ArgsV;
            for (unsigned i = 0, e = exprList->size(); i != e; ++i) {
                ArgsV.push_back(exprList->elementAt(i)->Codegen());
                if (ArgsV.back() == 0) return 0;
            }

            return Builder.CreateCall(CalleeF, ArgsV, "functioncall");
        }
};
class Constants : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Constant>" << endl;
            Common::printSpaces(indent+2);
            cout << *constant << endl;
            Common::printSpaces(indent);
            cout << "</Constant>" << endl;
        }

        //Not Handling Strings and characters cause guess what, we don't have those datatypes in decaf -_-
        virtual Value* Codegen() {
            if (constant->compare("true")==0) {
                ConstantInt* const_int1 = ConstantInt::get(TheModule->getContext(), APInt(1, StringRef("1"), 10));
                return const_int1;
            } else if (constant->compare("false")==0) {
                ConstantInt* const_int1 = ConstantInt::get(TheModule->getContext(), APInt(1, StringRef("0"), 10));
                return const_int1;
            } else {
                ConstantInt* const_int32_2 = ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(*constant), 10));
                return const_int32_2;
            }
        }
};
class Statement {
    public:
        Array<Statement*> *block;
        Expr *location;
        string *op;
        Expr *expr = nullptr;
        FieldDecl *fieldDecl;
        Expr *ifexpr;
        Array<Statement*> *ifblock, *elseblock;
        string *forInitIdentifier;
        Expr *forInitExpr, *forConditionalExpr;
        Array<Statement*> *forBlock;
        string *keyword;
        virtual void print(int indent) {
        }
        virtual Value* Codegen() {
        }
};
class MethodCallStatement : public Statement {
    public:
        void print(int indent) {
            expr->print(indent);
        }
        Value* Codegen(){
            return expr->Codegen();
        }
};
class BlockStatement : public Statement {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Block>" << endl;
            for(int i=0; i<block->size(); i++) {
                block->elementAt(i)->print(indent+2);
            }
            Common::printSpaces(indent);
            cout << "</Block>" << endl;
        }
        virtual Value* Codegen(){
            static map<std::string, AllocaInst*> PreviousNamedValues = NamedValues;
            for(int i=0;i<block->size();++i){
                block->elementAt(i)->Codegen();
            }
            NamedValues = PreviousNamedValues;
        }
};
class FieldDeclStatement : public Statement {
    public:
        void print (int indent) {
            fieldDecl->print(indent);
        }
        virtual Value* Codegen(){
            for(int i = 0; i<fieldDecl->fieldList->size(); i++) {
                if (fieldDecl->fieldList->elementAt(i)->fieldSize == -1) {
                    AllocaInst *Alloca = CreateEntryBlockAlloca(F, *(fieldDecl->fieldList->elementAt(i)->fieldName), *(fieldDecl->fieldType));
                    if(fieldDecl->fieldType->compare("int")==0) {
                        Builder.CreateStore(ConstantInt::get(TheModule->getContext(), APInt(32, StringRef("0"), 10)), Alloca);
                    } else {
                        Builder.CreateStore(ConstantInt::get(TheModule->getContext(), APInt(1, StringRef("0"), 10)), Alloca);
                    }
                    NamedValues[*(fieldDecl->fieldList->elementAt(i)->fieldName)] = Alloca;
                    ConstantNamedValues[*(fieldDecl->fieldList->elementAt(i)->fieldName)] = 0;
                } else {
                    //Array
                    AllocaInst *Alloca = CreateEntryBlockAllocaArray(F, *(fieldDecl->fieldList->elementAt(i)->fieldName), *(fieldDecl->fieldType), fieldDecl->fieldList->elementAt(i)->fieldSize);
                    Builder.CreateStore(ConstantDataArray::get(TheModule->getContext(), llvm::ArrayRef<uint32_t>(vector<uint32_t>(fieldDecl->fieldList->elementAt(i)->fieldSize))), Alloca);
                    NamedValues[*(fieldDecl->fieldList->elementAt(i)->fieldName)] = Alloca;
                }
            }
        }
};
class IfStatement : public Statement {
    public:
        void print (int indent) {
            Common::printSpaces(indent);
            cout << "<If>" << endl;

            ifexpr->print(indent+2);

            Common::printSpaces(indent+2);
            cout << "<Block>" << endl;
            for(int i=0; i<ifblock->size(); i++) {
                ifblock->elementAt(i)->print(indent+4);
            }
            Common::printSpaces(indent+2);
            cout << "</Block>" << endl;

            Common::printSpaces(indent);
            cout << "</If>" << endl;

            if(elseblock) {
                Common::printSpaces(indent);
                cout << "<Else>" << endl;

                Common::printSpaces(indent+2);
                cout << "<Block>" << endl;
                for(int i=0; i<elseblock->size(); i++) {
                    elseblock->elementAt(i)->print(indent+4);
                }
                Common::printSpaces(indent+2);
                cout << "</Block>" << endl;

                Common::printSpaces(indent);
                cout << "</Else>" << endl;
            }
        }

        virtual Value* Codegen(){
            if(ifexpr){
                Value *ifexp = ifexpr->Codegen();
                
                bool flag=true;
                if(ifexp==0){
                    flag=false;
                }
                if(ifexp->getType() == IntegerType::get(TheModule->getContext(), 32)) {
                    ifexp = Builder.CreateICmpNE(ifexp, 
                            ConstantInt::get(TheModule->getContext(),APInt(32,0,10)),
                            "ifcond");
                } else {
                    ifexp = Builder.CreateICmpNE(ifexp, 
                            ConstantInt::get(TheModule->getContext(),APInt(1,0,10)),
                            "ifcond");
                }

                // Create blocks for the then and else cases. 
                BasicBlock *ThenBB = BasicBlock::Create(TheModule->getContext(), "then", F);
                BasicBlock *ElseBB = BasicBlock::Create(TheModule->getContext(), "else");
                BasicBlock *MergeBB = BasicBlock::Create(TheModule->getContext(), "ifcont");

                Builder.CreateCondBr(ifexp, ThenBB, ElseBB);

                // Emit then value.
                Builder.SetInsertPoint(ThenBB);

                BB = ThenBB;
                
                phiMap.clear();
                 
                map<std::string, AllocaInst*> PreviousNamedValues = NamedValues;
                vector <Value*> ifblockV;
                for(int i=0; i<ifblock->size(); i++) {
                    ifblockV.push_back(ifblock->elementAt(i)->Codegen());
                }
                NamedValues = PreviousNamedValues;

                Builder.CreateBr(MergeBB);
                ThenBB = Builder.GetInsertBlock();

                // Emit else block.
                F->getBasicBlockList().push_back(ElseBB);
                Builder.SetInsertPoint(ElseBB);

                BB = ElseBB;
                
                PreviousNamedValues = NamedValues;
                vector<Value*> elseblockV; 
                if(elseblock) {
                    for(int i=0; i<elseblock->size(); i++) {
                        elseblockV.push_back(elseblock->elementAt(i)->Codegen());
                    }
                }
                NamedValues = PreviousNamedValues;

                Builder.CreateBr(MergeBB);
                ElseBB = Builder.GetInsertBlock();

                // Emit merge block.
                F->getBasicBlockList().push_back(MergeBB);
                Builder.SetInsertPoint(MergeBB);

                BB = MergeBB;
                
                for(map<string, pair<Value*, Value*> >::iterator it = phiMap.begin(); it!= phiMap.end(); it++) {
                    if(it->second.first!=nullptr && it->second.second!=nullptr) {
                        PHINode *PN = Builder.CreatePHI(it->second.first->getType(), 2,it->first);

                        PN->addIncoming(it->second.first, ThenBB);
                        PN->addIncoming(it->second.second, ElseBB);
                    }
                }
            }
        }
};
class ForStatement : public Statement {
    public:
        void print (int indent) {
            Common::printSpaces(indent);
            cout << "<For>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ForInitIdentifier>" << endl;
            Common::printSpaces(indent+4);
            cout << *forInitIdentifier << endl;
            Common::printSpaces(indent+2);
            cout << "</ForInitIdentifier>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ForInitExpr>" << endl;
            forInitExpr->print(indent+4);
            Common::printSpaces(indent+2);
            cout << "</ForInitExpr>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ForConditionalExpr>" << endl;
            forConditionalExpr->print(indent+4);
            Common::printSpaces(indent+2);
            cout << "</ForCondtionalExpr>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ForBlock>" << endl;
            for(int i=0; i<forBlock->size(); i++) {
                forBlock->elementAt(i)->print(indent+4);
            }
            Common::printSpaces(indent+2);
            cout << "</ForBlock>" << endl;

            Common::printSpaces(indent);
            cout << "<For>" << endl;
        }

        virtual Value* Codegen(){

                BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", F);

                Builder.CreateBr(LoopBB);

                Builder.SetInsertPoint(LoopBB);
                
                BB = LoopBB;
               
                Value* StartVal = forInitExpr->Codegen();
                
                Builder.CreateStore(StartVal, NamedValues[*forInitIdentifier]);
                ConstantInt *CI = dyn_cast<ConstantInt>(StartVal); 
                ConstantNamedValues[*forInitIdentifier] = CI->getSExtValue(); 

                if(forBlock){
                    for(int i=0; i<forBlock->size(); ++i){
                        forBlock->elementAt(i)->Codegen();
                    }
                } else{
                    return 0;
                }

                //We don't have step value, it is by default 1 in decaf. 
                
                Value* EndCond;
                if(forConditionalExpr){
                    EndCond = forConditionalExpr->Codegen();
                } else{
                    return 0;
                }

                Value *CurVar = Builder.CreateLoad(NamedValues[*forInitIdentifier], *forInitIdentifier);
                Value *NextVar = Builder.CreateAdd(CurVar, ConstantInt::get(TheModule->getContext(),APInt(32,1,10)), "nextvar");
                Builder.CreateStore(NextVar, NamedValues[*forInitIdentifier]);
                ConstantNamedValues[*forInitIdentifier]+=1; 
                
                if(EndCond->getType()->isIntegerTy(1)==true) {
                    EndCond = Builder.CreateICmpNE(EndCond,ConstantInt::get(getGlobalContext(), APInt(1, 1, 10)),"loopcond");
                } else {
                    EndCond = Builder.CreateICmpNE(EndCond,ConstantInt::get(getGlobalContext(), APInt(32, 1, 10)),"loopcond");
                }

                BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", F);
                
                Builder.CreateCondBr(EndCond, LoopBB, AfterBB);

                Builder.SetInsertPoint(AfterBB);

                BB = AfterBB;

                // for expr always returns 0.0.
                return ConstantInt::get(getGlobalContext(),APInt(32,0,10));
        }
};
class KeywordStatement : public Statement {
    public:
        void print(int indent) {
            if(expr) {
                Common::printSpaces(indent);
                cout << "<Return>" << endl;
                Common::printSpaces(indent+2);
                cout << "<ReturnExpr>" << endl;
                expr->print(indent+4);
                Common::printSpaces(indent+2);
                cout << "</ReturnExpr>" << endl;
                Common::printSpaces(indent);
                cout << "<Return>" << endl;
            } else {
                Common::printSpaces(indent);
                cout << "<" << *keyword << "/>" <<endl;
            }
        }
        virtual Value* Codegen(){
            if(expr){ // return
                RetVal = expr->Codegen();
                Builder.CreateRet(RetVal);
                return RetVal;
            } else{
                if(keyword->compare("Return")==0) { 
                    RetVal = nullptr;
                    Builder.CreateRet(RetVal);
                    return RetVal;
                }
            } 
        }
};
class Param {
    public:
        string *paramType, *paramName;
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Param>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ParamType>" << endl;
            Common::printSpaces(indent+4);
            cout << *paramType << endl;
            Common::printSpaces(indent+2);
            cout << "</ParamType>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ParamName>" << endl;
            Common::printSpaces(indent+4);
            cout << *paramName << endl;
            Common::printSpaces(indent+2);
            cout << "</ParamName>" << endl;

            Common::printSpaces(indent);
            cout << "</Param>" << endl;
        }
        Value* Codegen() {
        }
};
class MethodDecl : public Decl {
    public:
        string *methodType, *methodName;
        Array<Param*> *paramList;
        Array<Statement*> *block;
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<MethodDecl>" << endl;

            Common::printSpaces(indent+2);
            cout << "<MethodType>" << endl;
            Common::printSpaces(indent+4);
            cout << *methodType << endl;
            Common::printSpaces(indent+2);
            cout << "</MethodType>" << endl;

            Common::printSpaces(indent+2);
            cout << "<MethodName>" << endl;
            Common::printSpaces(indent+4);
            cout << *methodName << endl;
            Common::printSpaces(indent+2);
            cout << "</MethodName>" << endl;

            Common::printSpaces(indent+2);
            cout << "<ParamList>" << endl;
            for(int i=0; i<paramList->size(); i++) {
                paramList->elementAt(i)->print(indent+4);
            }
            Common::printSpaces(indent+2);
            cout << "</ParamList>" << endl;

            Common::printSpaces(indent+2);
            cout << "<Block>" << endl;
            for(int i=0; i<block->size(); i++) {
                block->elementAt(i)->print(indent+4);
            }
            Common::printSpaces(indent+2);
            cout << "</Block>" << endl;

            Common::printSpaces(indent);
            cout << "</MethodDecl>" << endl;
        }
        //Function declaration and Function overloading not allowed in decaf
        virtual Value *Codegen(){
            vector<Type*> Parameters; // create vector of N LLVM types
            for (int i = 0; i < paramList->size(); i++) {
                if ((*paramList->elementAt(i)->paramType).compare("int")==0) {
                    Parameters.push_back(IntegerType::get(getGlobalContext(), 32));
                } else {
                    //boolean
                    Parameters.push_back(IntegerType::get(getGlobalContext(), 1));
                }
            }

            FunctionType *FT;
            if(methodType->compare("int")==0) {
                FT = FunctionType::get(IntegerType::get(getGlobalContext(), 32), Parameters, false); 
            } else if (methodType->compare("boolean")==0) {
                FT = FunctionType::get(IntegerType::get(getGlobalContext(), 1), Parameters, false); 
            } else {
                FT = FunctionType::get(Type::getVoidTy(TheModule->getContext()), Parameters, false); 
            }
            F = Function::Create(FT, Function::ExternalLinkage, *methodName, TheModule);  

            //if method already exists, llvm renames it, so we can use the following check
            if (F->getName() != *methodName) {
                Error("Redefinition of function");
                return 0;
            }

            BB = BasicBlock::Create(getGlobalContext(), "entry", F);
            Builder.SetInsertPoint(BB);

            //Achievement: Scoping
            map<std::string, AllocaInst*> PreviousNamedValues = NamedValues;
            Function::arg_iterator AI = F->arg_begin();
            for (unsigned Idx = 0, e = paramList->size(); Idx < e; Idx++, AI++) {    
                // Create an alloca for this variable.
                AllocaInst *Alloca = CreateEntryBlockAlloca(F, *(paramList->elementAt(Idx)->paramName), *(paramList->elementAt(Idx)->paramType));

                // Store the initial value into the alloca.
                Builder.CreateStore(AI, Alloca);

                // Add arguments to variable symbol table.
                NamedValues[*paramList->elementAt(Idx)->paramName] = Alloca;
                ConstantNamedValues[*paramList->elementAt(Idx)->paramName] = 0;
            }

            //Achievement: checks if return statement matches return type
            int i=0;
            for(i=0; i<block->size(); i++) {
                block->elementAt(i)->Codegen();
                if(block->elementAt(i)) {
                    if(block->elementAt(i)->keyword) {
                        if(block->elementAt(i)->keyword->compare("Return")==0) {
                            if(RetVal) {
                                if(RetVal->getType() != FT->getReturnType()) {
                                    Error("Return type doesn't match function prototype");
                                }
                            } else {
                                if(FT->getReturnType() != Type::getVoidTy(TheModule->getContext())) {
                                    Error("Return type doesn't match function prototype");
                                }
                            }
                        }
                    }
                }
            }

            
            verifyFunction(*F);
            NamedValues = PreviousNamedValues;
            return F;
        }
};
class AssignmentStatement : public Statement {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<AssignmentStatement>" << endl;

            location->print(indent+2);

            Common::printSpaces(indent+2);
            cout << "<Operator>" << endl;
            Common::printSpaces(indent+4);
            cout << *op << endl;
            Common::printSpaces(indent+2);
            cout << "</Operator>" << endl;

            expr->print(indent+2);

            Common::printSpaces(indent);
            cout << "</AssignmentStatement>" << endl;
        }

        virtual Value* Codegen(){
            Value *RHS;
            switch((*op)[0]) {
                case '=': RHS = expr->Codegen(); break;
                case '+': RHS = Builder.CreateAdd(location->Codegen(), expr->Codegen(), "add"); break;
                case '-': RHS = Builder.CreateSub(location->Codegen(), expr->Codegen(), "sub"); break;
                case '*': RHS = Builder.CreateMul(location->Codegen(), expr->Codegen(), "mul"); break;
                case '/': RHS = Builder.CreateUDiv(location->Codegen(), expr->Codegen(), "div"); break;
            }
            if(location->expr1) {
                //array access
                Value *V = NamedValues[*location->identifier];
                if(V==0) {
                    Value *V = GlobalNamedValues[*location->identifier];
                    if(V==0) {
                        Error("Unknown Variable Name");
                        return nullptr;
                    }
                    //global array
                    int rhs_int;
                    if(expr->identifier) {
                        rhs_int=ConstantNamedValues[*expr->identifier];
                    } else {
                        ConstantInt *rhs = dyn_cast<ConstantInt>(RHS);
                        rhs_int = rhs->getSExtValue();
                    }

                    ConstantInt *CI;
                    if(RHS->getType()==IntegerType::get(getGlobalContext(), 32)) {
                        CI = ConstantInt::get(TheModule->getContext(), APInt(32, rhs_int, 10));
                    } else {
                        CI = ConstantInt::get(TheModule->getContext(), APInt(1, rhs_int, 10));
                    }
                    std::vector<Constant*> indices;
                    ConstantInt* const_zero = ConstantInt::get(TheModule->getContext(), APInt(64, StringRef("0"), 10));
                    indices.push_back(const_zero);
                    int indice_actual;
                    if(location->expr1->identifier) {
                        indice_actual=ConstantNamedValues[*location->expr1->identifier];
                    } else {
                        ConstantInt* indice_CI = dyn_cast<ConstantInt>(location->expr1->Codegen());
                        indice_actual = indice_CI->getSExtValue();
                    }
                    ConstantInt* const_actual = ConstantInt::get(TheModule->getContext(), APInt(64, indice_actual, 10));
                    indices.push_back(const_actual);
                    Constant* elem_ptr = ConstantExpr::getGetElementPtr(dyn_cast<Constant>(V), indices);
                    StoreInst* SI = new StoreInst(CI, elem_ptr, false, BB);
                    SI->setAlignment(4);

                    return RHS;
                }
                //local array
                if(location->expr1->identifier) {
                    Builder.CreateInsertValue(Builder.CreateLoad(V), RHS, ConstantNamedValues[*location->expr1->identifier]);
                    return RHS;
                }
                ConstantInt* CI = dyn_cast<ConstantInt>(location->expr1->Codegen());
                int64_t indice_actual = CI->getSExtValue();
                Builder.CreateInsertValue(Builder.CreateLoad(V), RHS, indice_actual);
                return RHS;
            } else {
                //variable access
                Value *V = NamedValues[*location->identifier];
                if(V==0) {
                    Value *V = GlobalNamedValues[*location->identifier];
                    if(V==0) {
                        Error("Unknown Variable Name");
                        return nullptr;
                    }
                    //global variable
                    GlobalVariable *g_var = dyn_cast<GlobalVariable>(V);
                    g_var->setInitializer(dyn_cast<Constant>(RHS));
                    if(phiMap.find(*location->identifier)==phiMap.end()) {
                        phiMap.insert(make_pair(*location->identifier, make_pair(RHS, nullptr)));
                    } else {
                        if(phiMap[*location->identifier].second==nullptr) {
                            phiMap[*location->identifier].second=RHS;
                        } else {
                            phiMap[*location->identifier].second=nullptr;
                            phiMap[*location->identifier].first=nullptr;
                        }
                    }
                    return RHS;
                }
                //local variable
                Builder.CreateStore(RHS, V);
                if(phiMap.find(*location->identifier)==phiMap.end()) {
                    phiMap.insert(make_pair(*location->identifier, make_pair(RHS, nullptr)));
                } else {
                    if(phiMap[*location->identifier].second==nullptr) {
                        phiMap[*location->identifier].second=RHS;
                    } else {
                        phiMap[*location->identifier].second=nullptr;
                        phiMap[*location->identifier].first=nullptr;
                    }
                }
                return RHS;
            }
        }
};
