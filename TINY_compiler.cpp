#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

///handling negative numbers (in inc value) as subtraction operation (0-x) {option 1}


/*

{
assignment 3 program by shehab ahmed mohamed 20170129 cs_is_2 , copy the comment into input.txt
note : handling negative numbers (in inc value) as subtraction operation (0-x) [option 1]
}

{test 1 , output = 36}
sum:=0;
for i from 1 to n inc 2
startfor
sum:=sum+i;
if i=11 then break end
endfor;
write sum
;
{test 2 , output = 1 , 2 , 3}
for j from 1 to 4 inc 1
startfor
write j
endfor
;
{test 3 , output = 7 , 6 , 5 , 4}
for k from 7 to 3 inc 0-1
startfor
write k
endfor
;
{test 4 , output = 1 , 2}
for l from 1 to 4 inc 1
startfor
write l;
if l=2 then break end
endfor
;
{test 5 , output = 7 , 6}
for m from 7 to 3 inc 0-1
startfor
write m;
if m=6 then break end
endfor
;
{test 6 , output = no output because to expr == from expr }
for n from 1 to 1 inc 0
startfor
write n
endfor
;
{test 7 , output = 6144}
x := 1;
for o from 16 to 1 inc 0-4
startfor
x := x * o
endfor;
write x
;
{test 8 , output = 11 , 14 , 17}
for p from 5-4 to 110/11 inc 1+2
startfor
write p + 10
endfor
;
{test 9 , output = 1 , 2 , 10 , 20 , 30}
for q from 1 to 5 inc 1
startfor
if q=3 then
write 10;
if q=3 then
write 20;
if q=3 then
 write 30;
 break
end
end
end
;write q
endfor
;
{test 10 , output = 11 , 12 , 13 , 21 , 22 , 23 , 31 , 32 , 33 }
for r from 1 to 4 inc 1
startfor
for s from 1 to 4 inc 1
startfor
write (r*10)+s
endfor
endfor

*/


bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};


struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};


#define MAX_TOKEN_LEN 40

enum TokenType{
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
                FOR, FROM, TO , INC, STARTFOR, ENDFOR, BREAK, ///added new token types
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, TIMES, DIVIDE, POWER,
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, NUM,
                ENDFILE, ERROR
              };

const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "For", "From", "To" , "Inc", "Startfor", "Endfor","Break", ///added string of new token types
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "Num",
                "EndFile", "Error"
            };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];

    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

const Token reserved_words[]=
{
    Token(IF, "if"),
    Token(THEN, "then"),
    Token(ELSE, "else"),
    Token(END, "end"),
    Token(REPEAT, "repeat"),
    Token(UNTIL, "until"),
    Token(READ, "read"),
    Token(WRITE, "write"),
    ///added new reserved words
    Token(FOR, "for"),
    Token(FROM, "from"),
    Token(TO, "to"),
    Token(INC, "inc"),
    Token(STARTFOR, "startfor"),
    Token(ENDFOR, "endfor"),
    Token(BREAK, "break")


};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
    Token(MINUS, "-"),
    Token(TIMES, "*"),
    Token(DIVIDE, "/"),
    Token(POWER, "^"),
    Token(SEMI_COLON, ";"),
    Token(LEFT_PAREN, "("),
    Token(RIGHT_PAREN, ")"),
    Token(LEFT_BRACE, "{"),
    Token(RIGHT_BRACE, "}")
};
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));}
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_');}

void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {
        int j=1;
        while(IsDigit(s[j])) j++;

        ptoken->type=NUM;
        Copy(ptoken->str, s, j);
    }
    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        while(IsLetterOrUnderscore(s[j])) j++;

        ptoken->type=ID;
        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }

    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}


// program -> stmtseq
// stmtseq -> stmt { ; stmt }
/// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt | forstmt | breakstmt //added the new forstmt and breakstmt in the program
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
///<forstmt> -> for <identifier> from <mathexpr> to <mathexpr> inc <mathexpr> startfor <stmtseq> endfor //adding the forstmt function
///<breakstmt> -> break //adding the breakstmt function
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }    left associative
// term -> factor { (*|/) factor }    left associative
// factor -> newexpr { ^ newexpr }    right associative
// newexpr -> ( mathexpr ) | number | identifier


enum NodeKind{
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE, FOR_NODE, BREAK_NODE, ///added FOR node
                OPER_NODE, NUM_NODE, ID_NODE
             };

const char* NodeKindStr[]=
            {
                "If", "Repeat", "Assign", "Read", "Write", "For", "Break", ///added string of FOR node
                "Oper", "Num", "ID"
            };

enum ExprDataType {VOID, INTEGER, BOOLEAN};


const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Boolean"
            };

#define MAX_CHILDREN 4 ///increase children size

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling;

    NodeKind node_kind;

    union{TokenType oper; int num; char* id;};
    ExprDataType expr_data_type;

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo* pci, ParseInfo* ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if(ppi->next_token.type!=expected_token_type) throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]); fflush(pci->debug_file.file);
}

TreeNode* Expr(CompilerInfo*, ParseInfo*);

// newexpr -> ( mathexpr ) | number | identifier
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    if(ppi->next_token.type==NUM)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;
        char* num_str=ppi->next_token.str;
        tree->num=0; while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==ID)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=Expr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    throw 0;
    return 0;
}

// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=Factor(pci, ppi);

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Term(pci, ppi);

    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Term(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

    if(ppi->next_token.type==EQUAL || ppi->next_token.type==LESS_THAN)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

TreeNode* StmtSeq(CompilerInfo*, ParseInfo*);


///<breakstmt> -> break //adding the breakstmt function
TreeNode* BreakStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start BreakStmt"); ///print in debug file

    TreeNode* tree=new TreeNode; ///new node
    tree->node_kind=BREAK_NODE; ///break node
    tree->line_num=pci->in_file.cur_line_num; ///line number

    Match(pci, ppi, BREAK); ///break

    pci->debug_file.Out("End BreakStmt"); ///print in debug file
    return tree;
}


///<forstmt> -> for <identifier> from <mathexpr> to <mathexpr> inc <mathexpr> startfor <stmtseq> endfor ///adding the forstmt function
TreeNode* ForStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ForStmt"); ///print in debug file

    TreeNode* tree=new TreeNode; ///new node
    tree->node_kind=FOR_NODE; ///for node
    tree->line_num=pci->in_file.cur_line_num; ///line number

    Match(pci, ppi, FOR); ///for
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str); ///<identifier>
    Match(pci, ppi, ID); ///id
    Match(pci, ppi, FROM); ///from
    tree->child[0]=MathExpr(pci, ppi);///<mathexpr>
    Match(pci, ppi, TO); ///to
    tree->child[1]=MathExpr(pci, ppi);///<mathexpr>
    Match(pci, ppi, INC); ///inc
    tree->child[2]=MathExpr(pci, ppi);///<mathexpr>
    Match(pci, ppi, STARTFOR); ///startfor
    tree->child[3]=StmtSeq(pci, ppi);///<stmtseq>
    Match(pci, ppi, ENDFOR); ///endfor

    pci->debug_file.Out("End ForStmt"); ///print in debug file
    return tree;
}


// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=WRITE_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=READ_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, READ);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AssignStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=ASSIGN_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN); tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End AssignStmt");
    return tree;
}

// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT); tree->child[0]=StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL); tree->child[1]=Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=IF_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, IF); tree->child[0]=Expr(pci, ppi);
    Match(pci, ppi, THEN); tree->child[1]=StmtSeq(pci, ppi);
    if(ppi->next_token.type==ELSE) {Match(pci, ppi, ELSE); tree->child[2]=StmtSeq(pci, ppi);}
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}

/// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt | forstmt | breakstmt //added the new forstmt and breakstmt in the program
TreeNode* Stmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Stmt");

    TreeNode* tree=0;
    if(ppi->next_token.type==IF) tree=IfStmt(pci, ppi);
    else if(ppi->next_token.type==REPEAT) tree=RepeatStmt(pci, ppi);
    else if(ppi->next_token.type==ID) tree=AssignStmt(pci, ppi);
    else if(ppi->next_token.type==READ) tree=ReadStmt(pci, ppi);
    else if(ppi->next_token.type==WRITE) tree=WriteStmt(pci, ppi);
    else if(ppi->next_token.type==FOR) tree=ForStmt(pci, ppi); ///calling forstmt
    else if(ppi->next_token.type==BREAK) tree=BreakStmt(pci, ppi); ///calling breakstmt

    else throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode* first_tree=Stmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    while(ppi->next_token.type!=ENDFILE && ppi->next_token.type!=END &&
          ppi->next_token.type!=ELSE && ppi->next_token.type!=UNTIL && ppi->next_token.type!=ENDFOR) ///add the ENDFOR to continue
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode* next_tree=Stmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

// program -> stmtseq
TreeNode* Parse(CompilerInfo* pci)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    TreeNode* syntax_tree=StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==NUM_NODE) printf("[%d]", node->num);
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

    if(node->expr_data_type!=VOID) printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode* node)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        if(node->id) delete[] node->id;

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) DestroyTree(node->child[i]);
    if(node->sibling) DestroyTree(node->sibling);

    delete node;
}

const int SYMBOL_HASH_SIZE=10007;

struct LineLocation
{
    int line_num;
    LineLocation* next;
};

struct VariableInfo
{
    char* name;
    int memloc;
    LineLocation* head_line;
    LineLocation* tail_line;
    VariableInfo* next_var;
};

struct SymbolTable
{
    int num_vars;
    VariableInfo* var_info[SYMBOL_HASH_SIZE];

    SymbolTable() {num_vars=0; int i; for(i=0;i<SYMBOL_HASH_SIZE;i++) var_info[i]=0;}

    int Hash(const char* name)
    {
        int i, len=strlen(name);
        int hash_val=11;
        for(i=0;i<len;i++) hash_val=(hash_val*17+(int)name[i])%SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo* Find(const char* name)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        return 0;
    }

    void Insert(const char* name, int line_num)
    {
        LineLocation* lineloc=new LineLocation;
        lineloc->line_num=line_num;
        lineloc->next=0;

        int h=Hash(name);
        VariableInfo* prev=0;
        VariableInfo* cur=var_info[h];

        while(cur)
        {
            if(Equals(name, cur->name))
            {
                cur->tail_line->next=lineloc;
                cur->tail_line=lineloc;
                return;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->memloc=num_vars++;
        AllocateAndCopy(&vi->name, name);

        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;
    }

    void Print()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                printf("[Var=%s][Mem=%d]", curv->name, curv->memloc);
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl=curl->next;
                }
                printf("\n");
                curv=curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    LineLocation* pl=curl;
                    curl=curl->next;
                    delete pl;
                }
                VariableInfo* p=curv;
                curv=curv->next_var;
                delete p;
            }
            var_info[i]=0;
        }
    }
};

void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE || node->node_kind==FOR_NODE)///added for table node
        symbol_table->Insert(node->id, node->line_num);

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);

    if(node->node_kind==OPER_NODE)
    {
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        else node->expr_data_type=INTEGER;
    }
    else if(node->node_kind==ID_NODE || node->node_kind==NUM_NODE) node->expr_data_type=INTEGER;

    if(node->node_kind==OPER_NODE)
    {
        if(node->child[0]->expr_data_type!=INTEGER || node->child[1]->expr_data_type!=INTEGER)
            printf("ERROR Operator applied to non-integers\n");
    }
    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN) printf("ERROR If test must be BOOLEAN\n");
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN) printf("ERROR Repeat test must be BOOLEAN\n");
    if(node->node_kind==WRITE_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Write works only for INTEGER\n");
    if(node->node_kind==ASSIGN_NODE && node->child[0]->expr_data_type!=INTEGER) printf("ERROR Assign works only for INTEGER\n");
    if(node->node_kind==FOR_NODE && node->child[0]->expr_data_type!=INTEGER
    && node->child[1]->expr_data_type!=INTEGER && node->child[2]->expr_data_type!=INTEGER) printf("ERROR Fpr test must be INTEGER\n");///added error message

    if(node->sibling) Analyze(node->sibling, symbol_table);
}

int Power(int a, int b)
{
    if(a==0) return 0;
    if(b==0) return 1;
    if(b>=1) return a*Power(a, b-1);
    return 0;
}

int Evaluate(TreeNode* node, SymbolTable* symbol_table, int* variables)
{
    if(node->node_kind==NUM_NODE) return node->num;
    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id)->memloc];

    int a=Evaluate(node->child[0], symbol_table, variables);
    int b=Evaluate(node->child[1], symbol_table, variables);

    if(node->oper==EQUAL) return a==b;
    if(node->oper==LESS_THAN) return a<b;
    if(node->oper==PLUS) return a+b;
    if(node->oper==MINUS) return a-b;
    if(node->oper==TIMES) return a*b;
    if(node->oper==DIVIDE) return a/b;
    if(node->oper==POWER) return Power(a,b);
    throw 0;
    return 0;
}

bool RunProgram(TreeNode* node, SymbolTable* symbol_table, int* variables) ///change the return type to handle the break part
{
    bool flag = true ; ///flag to control the break statement

    if(node->node_kind==IF_NODE)
    {
        int cond=Evaluate(node->child[0], symbol_table, variables);
        if(cond)
        {
            if(RunProgram(node->child[1], symbol_table, variables)==false) flag=false; ///if stmt returned false because of break return the flag
        }

        else
         if(node->child[2])
         {
            if(RunProgram(node->child[2], symbol_table, variables)==false) flag=false; ///if stmt returned false because of break return the flag
         }

    }
    if(node->node_kind==ASSIGN_NODE)
    {
        int v=Evaluate(node->child[0], symbol_table, variables);
        variables[symbol_table->Find(node->id)->memloc]=v;
    }
    if(node->node_kind==READ_NODE)
    {
        printf("Enter %s: ", node->id);
        scanf("%d", &variables[symbol_table->Find(node->id)->memloc]);
    }
    if(node->node_kind==WRITE_NODE)
    {
        int v=Evaluate(node->child[0], symbol_table, variables);
        printf("Val: %d\n", v);
    }
    if(node->node_kind==REPEAT_NODE)
    {
        do
        {
           RunProgram(node->child[0], symbol_table, variables);
        }
        while(!Evaluate(node->child[1], symbol_table, variables));
    }

    ///describe how for node will work
    if(node->node_kind==FOR_NODE)
    {
        ///assign id with from expr
        variables[symbol_table->Find(node->id)->memloc]=Evaluate(node->child[0], symbol_table, variables);

        ///handling negative numbers (in inc value) as subtraction operation (0-x) {option 1}
        ///if to is bigger then from
        if(Evaluate(node->child[1], symbol_table, variables) > Evaluate(node->child[0], symbol_table, variables))
        {
            ///while the id < to expr
            while(variables[symbol_table->Find(node->id)->memloc] < Evaluate(node->child[1], symbol_table, variables))
            {
                ///run the stmtseq
                if(RunProgram(node->child[3], symbol_table, variables)==false) break; ///break if returned was false

                ///increase id by inc
                variables[symbol_table->Find(node->id)->memloc]+=Evaluate(node->child[2], symbol_table, variables);
            }
        }


        ///if from is bigger then to
        else
        {
            ///while the id > to expr
            while(variables[symbol_table->Find(node->id)->memloc] > Evaluate(node->child[1], symbol_table, variables))
            {
                ///run the stmtseq
                if(RunProgram(node->child[3], symbol_table, variables)==false) break; ///break if returned was false

                ///increase id by inc
                variables[symbol_table->Find(node->id)->memloc]+=Evaluate(node->child[2], symbol_table, variables);
            }

        }

    }

    ///describe how break node will work
    if(node->node_kind==BREAK_NODE) return false ; ///return false if break happened


    if(!flag) return false ; ///don't continue to sibling

    if(node->sibling)
        return RunProgram(node->sibling, symbol_table, variables); ///recursion
    else
        return true ; ///return true
}

void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    int* variables=new int[symbol_table->num_vars];
    for(i=0;i<symbol_table->num_vars;i++) variables[i]=0;
    RunProgram(syntax_tree, symbol_table, variables);
    delete[] variables;
}


void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=Parse(pci);


    SymbolTable symbol_table;
    Analyze(syntax_tree, &symbol_table);


    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n"); fflush(NULL);

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}


void StartScanner(CompilerInfo* pci)
{
    Token token;

    while(true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]); fflush(NULL);
        if(token.type==ENDFILE || token.type==ERROR) break;
    }
}



int main()
{


    printf("Start main()\n"); fflush(NULL);


    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    StartCompiler(&compiler_info);

    printf("End main()\n"); fflush(NULL);


    return 0;
}

