%{
	#include <stdio.h>
	#include "globals.h"
	#include "y.tab.h"

	/* parsing data fields */
	extern char tokenString[50];
	extern int lineno;

	/* streams */
	extern FILE * source;
	extern FILE * out;
	extern FILE * error;

%}

letter			[a-zA-Z]
digit			[0-9]
id				{letter}+
number			{digit}+
whitespace		[ \t]+
newline			\n
openComment		\/\*
closeComment 	\*\/

%%

else				{return ELSE;}
if					{return IF;}
int					{return INT;}
void				{return VOID;}
while				{return WHILE;}
return				{return RETURN;}
{id}				{return ID;}
{number}			{return NUMBER;}
{whitespace}		{/*skip whitespace*/}
{newline}			{lineno++;}
\+					{return PLUS;}
\-					{return MINUS;}
\*					{return MULTIPLY;}
\/					{return DIVIDE;}
\=\=				{return EQUAL;}
\!\=				{return NOTEQUAL;}
\<\=				{return LESSOREQUAL;}
\>\=				{return MOREOREQUAL;}
\<					{return LESS;}
\>					{return MORE;}
\=					{return ASSIGN;}
;					{return ENDOFLINE;}
,					{return COMMA;}
\{					{return OPENBRACE;}
\}					{return CLOSEBRACE;}
\[					{return OPENSQUARE;}
\]					{return CLOSESQUARE;}
\(					{return OPENPAREN;}
\)					{return CLOSEPAREN;}
{openComment}.*{closeComment}	{/*skip comments*/}

%%

int getToken()
{
    int placeholder = 0;
    static int firstTime = TRUE;
    
    if (firstTime)
    {
		firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = out;
    }
	
	/* grab the next token */
    placeholder = yylex();
    
    if (placeholder == ID || placeholder == NUMBER) 
    {
		/* if this is an id or number, put it in the token field */
	    strncpy(tokenString, yytext, MAXTOKENLEN);
    }
    
    return placeholder;
}