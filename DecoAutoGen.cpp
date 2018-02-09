#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace clang;

std::ofstream *ofs;

class InterfaceGenerator
        : public RecursiveASTVisitor<InterfaceGenerator> {
public:
    explicit InterfaceGenerator(ASTContext *Context) : Context(Context) {}

    bool VisitCXXRecordDecl(CXXRecordDecl *recordDecl) {
        recordDecl->dump();

        if(recordDecl->isClass()){
            (*ofs)<<"class ";
        } else if(recordDecl->isStruct()){
            (*ofs)<<"struct ";
        }

        (*ofs)<< recordDecl->getNameAsString()<<"Interface {\n";
        (*ofs)<< "public:\n";

        /* create FooInterface */
        FullSourceLoc FullLocation = Context->getFullLoc(recordDecl->getLocStart());
        if (FullLocation.isValid())
            llvm::outs() << "Found recordDecl at "
                         << FullLocation.getSpellingLineNumber() << ":"
                         << FullLocation.getSpellingColumnNumber() << "\n";

        llvm::outs() << "\n\n";

        // // Iterate through methods.
        for (CXXRecordDecl::method_iterator I = recordDecl->method_begin(), E = recordDecl->method_end(); I != E; ++I) {
            const CXXMethodDecl *methodDecl = *I;

            if(isa<CXXConstructorDecl>(methodDecl)
               || isa<CXXDestructorDecl>(methodDecl)
               || methodDecl->isOverloadedOperator())
                continue;
            
            (*ofs)<<"\tvirtual";

            if (methodDecl->isVolatile()){
                (*ofs)<<"\tvolatile";
            }

            (*ofs)<<"\t"<<methodDecl->getReturnType().getAsString();

            (*ofs)<<"\t"<<(methodDecl->getNameAsString())<<"(";

            for(CXXMethodDecl::param_const_iterator PI = methodDecl->param_begin(), PE = methodDecl->param_end(); PI != PE;){
                QualType qt = (*PI)->getType();

                (*ofs)<< qt.getAsString()<<" ";
                (*ofs)<< (*PI)->getNameAsString();

                ++PI;
                if(PI != PE)
                    (*ofs) <<", ";
            }

            (*ofs)<<") = 0; \n";
        }

        (*ofs)<<"};\n\n";
        ofs->flush();

        // The return value indicates whether we want the visitation to proceed.
        // Return false to stop the traversal of the AST.
        return true;
    }

private:
    ASTContext *Context;
};

class DecoAutoGenConsumer : public clang::ASTConsumer {
public:
    explicit DecoAutoGenConsumer(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    InterfaceGenerator Visitor;
};

class DecoAutoGenFrontendAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new DecoAutoGenConsumer(&Compiler.getASTContext()));
    }
};


int main(int argc, char **argv) {
    if (argc > 1) {
        std::string filename(argv[1]);
        std::ifstream inFile;
        inFile.open(filename);

        std::stringstream strStream;
        strStream << inFile.rdbuf();//read the file
        std::string str = strStream.str();//str holds the content of the file

        ofs = new std::ofstream(filename+".deco.cpp", std::ofstream::out);

        clang::tooling::runToolOnCode(new DecoAutoGenFrontendAction, str);

        ofs->close();
    }
}