#include <cstdlib>
#include <stdio.h>
#include <string.h>

enum TokenType { IDENTIFIER = 34512, CONSTANT = 87232, SPECIAL = 29742 } ;
typedef char * CharPtr ;
struct Column {
  int column ;                  // 此token(的字首)在本line有出現於此column
  Column * next;                // 下一個此token(在本line)有出現的column
} ; // Column
typedef Column * ColumnPtr ;
struct Line {                   // struct Line 記錄了4種資訊
  int line ;                    // 此token放在哪一 line
  ColumnPtr firstAppearAt ;     // 指到此token第一次出現在此line的哪一column
  ColumnPtr lastAppearAt ;      // 指到此token最後出現在此line的哪一column
  Line * next;                  // 同一 token 下一次出現在哪一 line
} ;
typedef Line * LinePtr ;
struct Token {                 // Struct token 紀錄了4種資訊
  CharPtr tokenStr;            // tokenStr 放你切下來的token
  TokenType type;              // type 紀錄此token是屬於哪一個case
  LinePtr firstAppearOn;       // 此 token 第一次出現在哪一 line
  LinePtr lastAppearOn;        // 此 token 最後出現在哪一 line
  Token * next;                // 指向下一個token
} ; // struct Token
typedef Token * TokenPtr;
typedef char Str100[ 100 ];     

// 全域變數
int gLine = 1 ,gColumn = 0, gIdentifier = 0, gConstant = 0, gSpecial = 0;
bool gFindEND_OF_FILE = false;
char gNextChar = 0;
TokenPtr gFront = NULL, gRear = NULL;  // 分別指向Token串列的頭跟尾

// Function宣告
bool IsNum( char ch );                                                                //  80
bool IsBlank( char ch );                                                              //  84
bool IsLetter ( char ch );                                                            //  88
bool FindTheStringAndPrintCoordinate( Str100 s );                                     //  92
void Sort();                                                                          // 102
void PrintAll();                                                                      // 120
void PrintMenu();                                                                     // 129
void CreateToken();                                                                   // 138
void GetNextChar();                                                                   // 237
void SkipLongComment();                                                               // 242
void SkipShortComment();                                                              // 253
void Reaction( int command );                                                         // 257
void Plus_gKindOfType( TokenType type );                                              // 286
void FindAllTokenInThisLine( int line );                                              // 291
void PrintCoordinate( LinePtr FirstLine );                                            // 302
void PutTokenIntoLinkList( TokenPtr ready_TokenPtr );                                 // 315
CharPtr CreateJustEnoughString( Str100 s );                                           // 356
LinePtr CreateLinePtr( int line, ColumnPtr ready_ColumnPtr );                         // 361
TokenPtr MakeUpAllUnitToBeAToken( CharPtr s, TokenType type, LinePtr ready_LinePtr ); // 368
ColumnPtr CreateColumnPtr( int column );                                              // 376

//main()
int main() {
  GetNextChar();                                 // 開頭 讀第一個字
  while ( IsBlank(gNextChar) ) GetNextChar();    // 開頭 去空格
  while ( !gFindEND_OF_FILE ) {                  // 一直切Token 直到 END_OF_FILE
    CreateToken();
    if ( !gFindEND_OF_FILE ) while ( IsBlank(gNextChar) ) GetNextChar();
  }
  PrintAll();
  printf("\n");
  PrintMenu();
  int command;
  scanf( "%d", &command );
  while ( command != 5 ) {
    Reaction(command);
    PrintMenu();
    scanf( "%d", &command );
  }
  printf( "byebye" );
}

// 以下是Function
bool IsNum( char ch ) {
  if ( '0' <= ch && ch <= '9' ) return true;
  else return false;
}
bool IsBlank( char ch ) {
  if ( ch == 9 || ch == '\n' || ch == 32 ) return true; // 9 = TAB, 10 = ENTER, 32 = SPACE (by ASCII)
  else return false;
}
bool IsLetter ( char ch ) {
  if ( ch == '_' || ( 'a' <= ch && ch <= 'z' ) || ( 'A' <= ch && ch <= 'Z' ) ) return true;
  else return false;
}
bool FindTheStringAndPrintCoordinate( Str100 s ) {
  bool find = false; 
  TokenPtr t = gFront;
  while ( t != NULL && !find ) {
    if ( !strcmp(t->tokenStr, s) ) find = true;
    else t = t->next;
  }
  if ( find ) PrintCoordinate(t->firstAppearOn);
  return find;
}
void Sort() {
  TokenPtr LoopOut = gFront, LoopIn;
  CharPtr t1;
  TokenType t2;
  LinePtr t3;
  while ( LoopOut->next != NULL ) {
   LoopIn = LoopOut->next;
   while ( LoopIn != NULL ) {
     if ( strcmp(LoopOut->tokenStr, LoopIn->tokenStr) > 0 ) {
       t1 = LoopOut->tokenStr, LoopOut->tokenStr = LoopIn->tokenStr, LoopIn->tokenStr = t1;
       t2 = LoopOut->type, LoopOut->type = LoopIn->type, LoopIn->type = t2;
       t3 = LoopOut->firstAppearOn, LoopOut->firstAppearOn = LoopIn->firstAppearOn, LoopIn->firstAppearOn = t3;
     }  
     LoopIn = LoopIn->next; 
   }
   LoopOut = LoopOut->next;
  }
}
void PrintAll() {
  Sort();
  TokenPtr walk = gFront;
  while ( walk != NULL ) {
    printf( "%s ", walk->tokenStr );
    PrintCoordinate(walk->firstAppearOn);  
    walk = walk->next;
  }
}
void PrintMenu() {
  printf( "請輸入指令 : \n" );
  printf( "1.總共有多少種 token\n" );
  printf( "2.三種case各有多少 token\n" );
  printf( "3.印出指定 token 的位置 (line number,column number) (要排序)\n" );
  printf( "4.印出所指定的那一 line 出現哪些 token (要排序)\n" );
  printf( "5.結束\n" );
  printf( "> " );
}
void CreateToken() { 
  Str100 s = { 0 };
  int site = 0, column = gColumn;
  TokenType ready_TokenType;  
  if ( IsNum(gNextChar) ) {     // 整數 + 小數  ( 沒有負數, -1 = 1個special + 1個constant )
    ready_TokenType = CONSTANT;
    while ( IsNum(gNextChar) || gNextChar == '.' ) {
      s[site++] = gNextChar;
      GetNextChar();
    }
  }else if ( gNextChar == 39 ) {  // 字元   39 = '''
    s[0] = 39;
    ready_TokenType = CONSTANT;
    GetNextChar();
    s[1]=gNextChar;
    GetNextChar();
    s[2]=gNextChar;
    if ( s[2] != 39 ) {
      GetNextChar();
      s[3]=gNextChar;
    }
    GetNextChar();    // 出來的時候 gNextChar 是 單引號
  }else if ( gNextChar == '"' ) {  // 字串
    s[site++] = '"';
    ready_TokenType = CONSTANT;
    GetNextChar();
    bool String_End = false; 
    if ( gNextChar == '"' ) String_End = true; //  ""    
    while ( !String_End ) {  // 92 = '\'
      s[site++] = gNextChar;
      GetNextChar();
      if ( gNextChar == '"' ) {
        int NumOf92 = 0, temp_site = site-1;  // 現在是下雙引號, -1 之後是前一個位置
        if ( s[temp_site] == 92 ) {
          NumOf92++, temp_site--;
          while ( temp_site >= 1 && s[temp_site] == 92 ) NumOf92++, temp_site--;
          if ( NumOf92 % 2 == 0 ) String_End = true; // 偶數
        }else String_End = true;
      }
    }
    s[site] = '"';
    GetNextChar();  // 出來的時候 gNextChar 是 雙引號
  }else if ( IsLetter(gNextChar) ) {             
    ready_TokenType = IDENTIFIER;
    while ( IsNum(gNextChar) || IsLetter(gNextChar) ) {
      s[site++] = gNextChar ;
      GetNextChar();
    }
  }else if ( gNextChar == '/' ) {
    GetNextChar();
    if ( gNextChar == '/' ) {        //      // : 註解1
      SkipShortComment();
      s[0] = s[1] = '/';
    }else if ( gNextChar == '*' ){   //      /* : 註解2
      SkipLongComment();
      s[0] = '/', s[1] = '*';
    }else {                          //      / : 單純的除號
      s[0] = '/';
      ready_TokenType = SPECIAL;
    }
  }else{
    ready_TokenType = SPECIAL;
    s[0] = gNextChar ;
    GetNextChar();               // 連2的特殊符號
    if ( s[0] == '&' && gNextChar == '&' ) {                                // && 
      s[1] = s[0];  
      GetNextChar();  // 在getchar()前, gNextChar是符號的第二個字元
    } else if ( s[0] == '|' && gNextChar == '|' ) {                         // || 
      s[1] = s[0];  
      GetNextChar();  // 同上上上行
    } else if ( s[0] == '<' && gNextChar == '<' ) {                         // << 
      s[1] = s[0];  
      GetNextChar();  // 同上上上行
    } else if ( s[0] == '>' && gNextChar == '>' ) {                         // >>
      s[1] = s[0];  
      GetNextChar();  // 同上上上行
    } else if ( s[0] == '+' && gNextChar == '+' ) {                         // ++
      s[1] = s[0];  
      GetNextChar();  // 同上上上行    
    } else if ( s[0] == '-' && ( gNextChar == '-' || gNextChar == '>' ) ) { // --  ->
      s[1] = gNextChar;  
      GetNextChar();  // 同上上上行    
    }else if ( s[0] == '=' || s[0] == '<' || s[0] == '>' || s[0] == '!' || s[0] == '+' ) {
      if ( gNextChar == '=' ) {  // == <= >= != +=
        s[1] = '=';  
        GetNextChar();// 同上上上上行
      }
    }
  }
  CharPtr ready_CharPtr = CreateJustEnoughString(s);
  if ( !strcmp(s, "//") || !strcmp(s, "/*") ) ;  // do nothing ~
  else if ( !strcmp(s, "END_OF_FILE") ) gFindEND_OF_FILE = true;
  else{
    TokenPtr ready_TokenPtr;
    if ( gNextChar == '\n' ) ready_TokenPtr = MakeUpAllUnitToBeAToken(ready_CharPtr, ready_TokenType, CreateLinePtr(gLine - 1, CreateColumnPtr(column)));
    else ready_TokenPtr = MakeUpAllUnitToBeAToken(ready_CharPtr, ready_TokenType, CreateLinePtr(gLine, CreateColumnPtr(column)));  
    PutTokenIntoLinkList(ready_TokenPtr);
  }  
}
void GetNextChar(){ 
  gNextChar = getchar();
  if ( gNextChar == '\n' ) gLine++, gColumn = 0 ;
  else gColumn++;
}
void SkipLongComment() {
  bool SkipFinish = false;
  GetNextChar();
  while ( !SkipFinish ) {
    if ( gNextChar == '*' ) {
      GetNextChar();
      if ( gNextChar == '/' ) SkipFinish = true;
    }else GetNextChar();
  }
  GetNextChar();      // 從迴圈出來是 '/' 
}
void SkipShortComment() {
  GetNextChar();
  while ( gNextChar != '\n' ) GetNextChar();
}
void Reaction( int command ) {
  if ( command == 1 ) printf( "總共%d種\n\n", gIdentifier + gConstant + gSpecial );
  else if ( command == 2 ) {    
    printf( "Case1 共");
    if ( gIdentifier >= 10000 ) printf( "%d個\n", gIdentifier );
    else if ( gIdentifier >= 1000 ) printf( " %d個\n", gIdentifier );
    else if ( gIdentifier >= 100 ) printf( "  %d個\n", gIdentifier );
    else if ( gIdentifier >= 10 ) printf( "   %d個\n", gIdentifier );  
    else  printf( "    %d個\n", gIdentifier );  
    printf( "Case2 共");
    if ( gConstant >= 10000 ) printf( "%d個\n", gConstant );
    else if ( gConstant >= 1000 ) printf( " %d個\n", gConstant );
    else if ( gConstant >= 100 ) printf( "  %d個\n", gConstant );
    else if ( gConstant >= 10 ) printf( "   %d個\n", gConstant );
    else printf( "    %d個\n", gConstant );   
    printf( "Case3 共");
    if ( gSpecial >= 10000 ) printf( "%d個\n\n", gSpecial );
    else if ( gSpecial >= 1000 ) printf( " %d個\n\n", gSpecial );
    else if ( gSpecial >= 100 ) printf( "  %d個\n\n", gSpecial );
    else if ( gSpecial >= 10 ) printf( "   %d個\n\n", gSpecial );
    else printf( "    %d個\n\n", gSpecial );   
  }else if ( command == 3 ) {
    printf( "請輸入要搜尋的 token : " );
    Str100 s;   
    scanf( "%s", s );
    if ( FindTheStringAndPrintCoordinate(s) ) printf("\n"); // Function 幫你做好好
    else printf( "查無此token : %s\n\n", s );
  }else if ( command == 4 ) {
    printf( "請輸入要指定的 line : " );
    int line;
    scanf( "%d", &line );
    FindAllTokenInThisLine(line);
    printf( "\n" );
  }  
}
void Plus_gKindOfType( TokenType type ) {
  if ( type == IDENTIFIER ) gIdentifier++;
  else if ( type == CONSTANT ) gConstant++;
  else if ( type == SPECIAL ) gSpecial++; 
}
void FindAllTokenInThisLine( int line ) {
  TokenPtr TokenWalk = gFront; 
  while ( TokenWalk != NULL ) {
    LinePtr LineWalk = TokenWalk->firstAppearOn;
    while ( LineWalk != NULL ) {
      if ( LineWalk->line == line ) printf( "%s\n", TokenWalk->tokenStr );
      LineWalk = LineWalk->next;
    }  
    TokenWalk = TokenWalk->next;
  }
}
void PrintCoordinate( LinePtr FirstLine ) {
  while( FirstLine != NULL ) {
    int line = FirstLine->line;
    ColumnPtr WalkColumnToNull = FirstLine->firstAppearAt;
    while ( WalkColumnToNull != NULL ) {
      int column = WalkColumnToNull->column;
      printf( "(%d,%d)", line, column );
      WalkColumnToNull = WalkColumnToNull->next;
    }
    FirstLine = FirstLine->next;
  }
  printf("\n");
}
void PutTokenIntoLinkList( TokenPtr ready_TokenPtr ) {
  if ( gFront == NULL ) {          // LinkList 裡的第一個 Token
    Plus_gKindOfType(ready_TokenPtr->type);
    gFront = gRear = ready_TokenPtr; 
  }
  else{    // LinkList 裡的第2個以上 Token
    bool TokenWalkToNULL = false, SameToken = false;
    while ( !TokenWalkToNULL && !SameToken ) {
      if ( gRear == NULL ) TokenWalkToNULL = true; 
      else if ( !strcmp(ready_TokenPtr->tokenStr, gRear->tokenStr) ) SameToken = true;  
      else gRear = gRear->next; 
    }
    if ( SameToken ) {
      LinePtr LineWalk = gRear->firstAppearOn;
      bool LineWalkIsNULL = false, SameLine = false;
      while ( !LineWalkIsNULL && !SameLine ) {
        if ( LineWalk == NULL ) LineWalkIsNULL = true;
        else if ( LineWalk->line == ready_TokenPtr->firstAppearOn->line ) SameLine = true;  
        else LineWalk = LineWalk->next;
      }
      if ( SameLine ) {
        ColumnPtr ColumnWalk = LineWalk->firstAppearAt;
        while ( ColumnWalk->next != NULL ) ColumnWalk = ColumnWalk->next; // 從這個迴圈出來的時候 ColumnWalk 的下一個就是 NULL
        ColumnWalk->next = new Column;
        ColumnWalk->next= ready_TokenPtr->firstAppearOn->firstAppearAt;
      }else{  // SameToken  !SameLine
        LineWalk = gRear->firstAppearOn;
        while ( LineWalk->next != NULL ) LineWalk = LineWalk->next; // 從這個迴圈出來的時候 LineWalk 的下一個就是 NULL
        LineWalk->next = new Line;
        LineWalk->next = ready_TokenPtr->firstAppearOn;
      }
    }else{ // !SameToken 
      Plus_gKindOfType(ready_TokenPtr->type);
      gRear = gFront;
      while ( gRear->next != NULL ) gRear = gRear->next; // 從這個迴圈出來的時候 gRear 的下一個就是 NULL
      gRear->next = new Token;
      gRear->next = ready_TokenPtr;
    }
    gRear = gFront;
  }
}
CharPtr CreateJustEnoughString( Str100 s ) {
  CharPtr re = new char [ strlen(s) + 1 ]; 
  strcpy(re, s);
  return re;  
}
LinePtr CreateLinePtr( int line, ColumnPtr ready_ColumnPtr ) {
  LinePtr re = new Line;
  re->line = line;
  re->firstAppearAt = re->lastAppearAt = ready_ColumnPtr;
  re->next = NULL;
  return re;  
}
TokenPtr MakeUpAllUnitToBeAToken( CharPtr s, TokenType type, LinePtr ready_LinePtr ) {
  TokenPtr re = new Token;
  re->tokenStr = s; 
  re->type = type;              
  re->firstAppearOn = re->lastAppearOn = ready_LinePtr;
  re->next = NULL;
  return re;
}
ColumnPtr CreateColumnPtr( int column ){
   ColumnPtr re = new Column;
   re->column = column;
   re->next = NULL;
   return re;  
}