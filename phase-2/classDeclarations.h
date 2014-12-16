#include "array.h"
#include<iostream>
#include<string>
using namespace std;
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
};
class Constant : public Expr {
    public:
        void print(int indent) {
            Common::printSpaces(indent);
            cout << "<Constant>" << endl;
            Common::printSpaces(indent+2);
            cout << *constant << endl;
            Common::printSpaces(indent);
            cout << "</Constant>" << endl;
        }
};
class Statement {
    public:
        Array<Statement*> *block;
        Expr *location;
        string *op;
        Expr *expr;
        FieldDecl *fieldDecl;
        Expr *ifexpr;
        Array<Statement*> *ifblock, *elseblock;
        string *forInitIdentifier;
        Expr *forInitExpr, *forConditionalExpr;
        Array<Statement*> *forBlock;
        string *keyword;
        virtual void print(int indent) {
        }
};
class MethodCallStatement : public Statement {
    public:
       void print(int indent) {
            expr->print(indent);
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
};
class FieldDeclStatement : public Statement {
    public:
        void print (int indent) {
            fieldDecl->print(indent);
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
};
