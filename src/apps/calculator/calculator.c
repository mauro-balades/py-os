#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include "stack.h"

#define bool char
#define true 1
#define false 0

#define PI 3.141592653589793

#define MAXTOKENLENGTH 512
#define MAXPRECISION 20
#define DEFAULTPRECISION 5
#define FUNCTIONSEPARATOR "|"

#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

typedef enum
{
	addop,
	multop,
	expop,
	lparen,
	rparen,
	digit,
	value,
	decimal,
	space,
	text,
	function,
	identifier,
	argsep,
	invalid
} Symbol;

struct Preferences
{
	struct Display
	{
		bool tokens;
		bool postfix;
	} display;
	struct Mode
	{
		bool degrees;
	} mode;
	int precision;
	int maxtokenlength;
} prefs;

typedef enum
{
	divZero,
	overflow,
	parenMismatch,
	inputMissing,
} Error;

typedef char* token;

typedef double number;

void raise(Error err)
{
	char* msg;
	switch(err)
	{
		case divZero:
			msg = "Dividir por 0";
			break;
		case overflow:
			msg = "Desbordamiento";
			break;
		case parenMismatch:
			msg = "Paréntesis no coincidentes";
			break;
		case inputMissing:
			msg = "Falta entrada de función";
			break;
	}
	printf("\tError: %s\n", msg);
}

inline unsigned int toDigit(char ch)
{
	return ch - '0';
}

number buildNumber(token str)
{
	number result = 0;
	/*while(*str && *str != '.')
	{
		result = result * 10 + toDigit(*str++);
	}*/
	result = strtod(str, NULL);
	return result;
}

token num2Str(number num)
{
	int len = 0;
	int precision = MAXPRECISION;
	if (prefs.precision >= 0 && prefs.precision < precision)
		precision = prefs.precision;
	token str = (token)malloc(prefs.maxtokenlength*sizeof(char));
	len = snprintf(str, prefs.maxtokenlength-1, "%.*f", precision, num);
	if (prefs.precision == -1)
	{
		while (str[len-1] == '0')
		{
			len = snprintf(str, prefs.maxtokenlength-1, "%.*f", --precision, num);
		}
	}

	return str;
}

number toRadians(number degrees)
{
	return degrees * PI / 180.0;
}

number toDegrees(number radians)
{
	return radians * 180.0 / PI;
}

int doFunc(Stack *s, token function)
{
	if (stackSize(s) == 0)
	{
		raise(inputMissing);
		stackPush(s, num2Str(NAN));
		return -1;
	}
	else if (stackSize(s) == 1 && strcmp(stackTop(s), FUNCTIONSEPARATOR) == 0)
	{
		stackPop(s);
		raise(inputMissing);
		stackPush(s, num2Str(NAN));
		return -1;
	}
	token input = (token)stackPop(s);
	number num = buildNumber(input);
	number result = num;
	number counter = 0;

	if(strncmp(function, "abs", 3) == 0)
		result = fabs(num);
	else if(strncmp(function, "floor", 5) == 0)
		result = floor(num);
	else if(strncmp(function, "ceil", 4) == 0)
		result = ceil(num);
	else if(strncmp(function, "sin", 3) == 0)
		result = !prefs.mode.degrees ? sin(num) : sin(toRadians(num));
	else if(strncmp(function, "cos", 3) == 0)
		result = !prefs.mode.degrees ? cos(num) : cos(toRadians(num));
	else if(strncmp(function, "tan", 3) == 0)
		result = !prefs.mode.degrees ? tan(num) : tan(toRadians(num));
	else if(strncmp(function, "arcsin", 6) == 0
		 || strncmp(function, "asin", 4) == 0)
		result = !prefs.mode.degrees ? asin(num) : toDegrees(asin(num));
	else if(strncmp(function, "arccos", 6) == 0
		 || strncmp(function, "acos", 4) == 0)
		result = !prefs.mode.degrees ? acos(num) : toDegrees(acos(num));
	else if(strncmp(function, "arctan", 6) == 0
		 || strncmp(function, "atan", 4) == 0)
		result = !prefs.mode.degrees ? atan(num) : toDegrees(atan(num));
	else if(strncmp(function, "sqrt", 4) == 0)
		result = sqrt(num);
	else if(strncmp(function, "cbrt", 4) == 0)
		result = cbrt(num);
	else if(strncmp(function, "log", 3) == 0)
		result = log(num);
	else if(strncmp(function, "exp", 3) == 0)
		result = exp(num);
	else if(strncmp(function, "min", 3) == 0)
	{
		while (stackSize(s) > 0 && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			num = buildNumber(input);
			if (num < result)
				result = num;
		}
	}
	else if(strncmp(function, "max", 3) == 0)
	{
		while (stackSize(s) > 0 && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			num = buildNumber(input);
			if (num > result)
				result = num;
		}
	}
	else if(strncmp(function, "sum", 3) == 0)
	{
		while (stackSize(s) > 0  && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			num = buildNumber(input);
			result += num;
		}
	}
	else if(strncmp(function, "avg", 3) == 0 ||
			strncmp(function, "mean", 4) == 0)
	{
		counter = 1;
		while (stackSize(s) > 0  && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			num = buildNumber(input);
			result += num;
			counter++;
		}
		result /= counter;
	}
	else if(strncmp(function, "median", 6) == 0)
	{
		Stack tmp, safe;
		counter = 1;
		stackInit(&tmp, (stackSize(s) > 0 ? stackSize(s) : 1));
		stackInit(&safe, (stackSize(s) > 0 ? stackSize(s) : 1));
		stackPush(&tmp, input);
		while (stackSize(s) > 0  && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			num = buildNumber(input);
			while (stackSize(&tmp) > 0 && buildNumber(stackTop(&tmp)) < num)
			{
				stackPush(&safe, stackPop(&tmp));
			}
			stackPush(&tmp, input);
			while (stackSize(&safe) > 0)
			{
				stackPush(&tmp, stackPop(&safe));
			}
			counter++;
		}
		stackFree(&safe);
		counter = (number)(((int)counter+1)/2);
		while (counter > 1)
		{
			stackPop(&tmp);
			counter--;
		}
		result = buildNumber(stackPop(&tmp));
		while (stackSize(&tmp) > 0)
		{
			stackPop(&tmp);
		}
		stackFree(&tmp);
	}
	else if(strncmp(function, "var", 3) == 0)
	{
		Stack tmp;
		counter = 1;
		stackInit(&tmp, (stackSize(s) > 0 ? stackSize(s) : 1));
		stackPush(&tmp, input);
		number mean = result;
		while (stackSize(s) > 0  && strcmp(stackTop(s), FUNCTIONSEPARATOR) != 0)
		{
			input = (token)stackPop(s);
			stackPush(&tmp, input);
			num = buildNumber(input);
			mean += num;
			counter++;
		}
		mean /= counter;
		result = 0;
		while (stackSize(&tmp) > 0)
		{
			input = (token)stackPop(&tmp);
			num = buildNumber(input)-mean;
			result += pow(num,2);
		}
		result /= counter;
		stackFree(&tmp);
	}
	if (strcmp(stackTop(s), FUNCTIONSEPARATOR) == 0)
		stackPop(s);
	stackPush(s, num2Str(result));
	return 0;
}

int doOp(Stack *s, token op)
{
	int err = 0;
	token roperand = (token)stackPop(s);
	token loperand = (token)stackPop(s);
	number lside = buildNumber(loperand);
	number rside = buildNumber(roperand);
	number ret;
	switch(*op)
	{
		case '^':
			{
				ret = pow(lside, rside);
			}
			break;
		case '*':
			{
				ret = lside * rside;
			}
			break;
		case '/':
			{
				if(rside == 0)
				{
					raise(divZero);
					if (lside == 0)
						ret = NAN;
					else
						ret = INFINITY;
					err = -1;
				}
				else
					ret = lside / rside;
			}
			break;
		case '%':
			{
				if(rside == 0)
				{
					raise(divZero);
					if (lside == 0)
						ret = NAN;
					else
						ret = INFINITY;
					err = -1;
				}
				else
				{
					ret = (int)(lside / rside);
					ret = lside - (ret * rside);
				}
			}
			break;
		case '+':
			{
				ret = lside + rside;
			}
			break;
		case '-':
			{
				ret = lside - rside;
			}
			break;
	}
	stackPush(s, num2Str(ret));
	return err;
}

char* ufgets(FILE* stream)
{
	unsigned int maxlen = 128, size = 128;
	char* buffer = (char*)malloc(maxlen);

	if(buffer != NULL)
	{
		char ch = EOF;
		int pos = 0;

		while((ch = getchar()) != EOF && ch != '\n')
		{
			buffer[pos++] = ch;
			if(pos == size) 
			{
				size = pos + maxlen;
				buffer = (char*)realloc(buffer, size);
			}
		}
		buffer[pos] = '\0'; 
	}
	return buffer;
}

Symbol type(char ch)
{
	Symbol result;
	switch(ch)
	{
		case '+':
		case '-':
			result = addop;
			break;
		case '*':
		case '/':
		case '%':
			result = multop;
			break;
		case '^':
			result = expop;
			break;
		case '(':
			result = lparen;
			break;
		case ')':
			result = rparen;
			break;
		case '.':
			result = decimal;
			break;
		case ' ':
			result = space;
			break;
		case ',':
			result = argsep;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			result = digit;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			result = text;
			break;
		default:
			result = invalid;
			break;
	}
	return result;
}

bool isFunction(token tk)
{
	return (strncmp(tk, "abs", 3) == 0
		|| strncmp(tk, "floor", 5) == 0
		|| strncmp(tk, "ceil", 4) == 0
		|| strncmp(tk, "sin", 3) == 0
		|| strncmp(tk, "cos", 3) == 0
		|| strncmp(tk, "tan", 3) == 0
		|| strncmp(tk, "arcsin", 6) == 0
		|| strncmp(tk, "arccos", 6) == 0
		|| strncmp(tk, "arctan", 6) == 0
		|| strncmp(tk, "asin", 4) == 0
		|| strncmp(tk, "acos", 4) == 0
		|| strncmp(tk, "atan", 4) == 0
		|| strncmp(tk, "sqrt", 4) == 0
		|| strncmp(tk, "cbrt", 4) == 0
		|| strncmp(tk, "log", 3) == 0
		|| strncmp(tk, "min", 3) == 0
		|| strncmp(tk, "max", 3) == 0
		|| strncmp(tk, "sum", 3) == 0
		|| strncmp(tk, "avg", 3) == 0
		|| strncmp(tk, "mean", 4) == 0
		|| strncmp(tk, "median", 6) == 0
		|| strncmp(tk, "var", 3) == 0
		|| strncmp(tk, "exp", 3) == 0);
}

bool isSpecialValue(token tk)
{
	return (strncmp(tk, "nan", 3) == 0 || strncmp(tk, "inf", 3) == 0);
}

Symbol tokenType(token tk)
{
	if (!tk)
		return invalid;
	Symbol ret = type(*tk);
	switch(ret)
	{
		case text:
			if(isFunction(tk))
				ret = function;
			else if(isSpecialValue(tk))
				ret = value;
			else
				ret = identifier;
			break;
		case addop:
			if(*tk == '-' && strlen(tk) > 1)
				ret = tokenType(tk+1);
			break;
		case decimal:
		case digit:
			ret = value;
			break;
		default:
			break;
	}
	return ret;
}


int tokenize(char *str, char *(**tokensRef))
{
	int i = 0;
	char** tokens = NULL;
	char** tmp = NULL;
	char* ptr = str;
	char ch = '\0';
	int numTokens = 0;
	char* tmpToken = malloc((prefs.maxtokenlength+1) * sizeof(char));
	if (!tmpToken)
	{
		fprintf(stderr, "Malloc del búfer temporal falló\n");
		return 0;
	}
	while((ch = *ptr++))
	{
		if(type(ch) == invalid)
			break;

		token newToken = NULL;
		tmpToken[0] = '\0';
		switch(type(ch))
		{
			case addop:
				{
					if(ch == '-'
						&& (numTokens == 0
							|| (tokenType(tokens[numTokens-1]) == addop
								|| tokenType(tokens[numTokens-1]) == multop
								|| tokenType(tokens[numTokens-1]) == expop
								|| tokenType(tokens[numTokens-1]) == lparen
								|| tokenType(tokens[numTokens-1]) == argsep)))
					{
						{
							int len = 1;
							bool hasDecimal = false;
							bool hasExponent = false;

							if(type(ch) == decimal)
							{
								hasDecimal = true;
								len++;
								tmpToken[0] = '0';
								tmpToken[1] = '.';
							}
							else
							{
								tmpToken[len-1] = ch;
							}

							for(; 
								*ptr
								&& len <= prefs.maxtokenlength
								&& (type(*ptr) == digit 
								 	|| ((type(*ptr) == decimal 
								 		&& hasDecimal == 0)) 
								 	|| ((*ptr == 'E' || *ptr == 'e')
								 		&& hasExponent == false)
								|| ((*ptr == '+' || *ptr == '-') && hasExponent == true));
								++len)
							{
								if(type(*ptr) == decimal)
									hasDecimal = true;
								else if(*ptr == 'E' || *ptr == 'e')
									hasExponent = true;
								tmpToken[len] = *ptr++;
							}

							tmpToken[len] = '\0';
						}
						break;
					}
				}
			case multop:
			case expop:
			case lparen:
			case rparen:
			case argsep:
				{
					tmpToken[0] = ch;
					tmpToken[1] = '\0';
				}
				break;
			case digit:
			case decimal:
				{
					int len = 1;
					bool hasDecimal = false;
					bool hasExponent = false;

					if(type(ch) == decimal)
					{
						hasDecimal = true;
						len++;
						tmpToken[0] = '0';
						tmpToken[1] = '.';
					}
					else 
					{
						tmpToken[len-1] = ch;
					}

					for(; 
						*ptr 
						&& len <= prefs.maxtokenlength
						&& (type(*ptr) == digit
						 	|| ((type(*ptr) == decimal 
						 		&& hasDecimal == 0))
						 	|| ((*ptr == 'E' || *ptr == 'e') 
						 		&& hasExponent == false) 
						 	|| ((*ptr == '+' || *ptr == '-') && hasExponent == true)); 
						++len)
					{
						if(type(*ptr) == decimal)
							hasDecimal = true;
						else if(*ptr == 'E' || *ptr == 'e')
							hasExponent = true;
						tmpToken[len] = *ptr++;
					}

					tmpToken[len] = '\0';
				}
				break;
			case text:
				{
					int len = 1;
					tmpToken[0] = ch;
					for(len = 1; *ptr && type(*ptr) == text && len <= prefs.maxtokenlength; ++len)
					{
						tmpToken[len] = *ptr++;
					}
					tmpToken[len] = '\0';
				}
				break;
			default:
				break;
		}
		
		if(tmpToken[0] != '\0' && strlen(tmpToken) > 0)
		{
			numTokens++;
			
			newToken = malloc((strlen(tmpToken)+1) * sizeof(char));
			if (!newToken)
			{
				numTokens--;
				break;
			}
			strcpy(newToken, tmpToken);
			newToken[strlen(tmpToken)] = '\0';
			tmp = (char**)realloc(tokens, numTokens * sizeof(char*));
			if (tmp == NULL)
			{
				if (tokens != NULL)
				{
					for(i=0;i<numTokens-1;i++)
					{
						if (tokens[i] != NULL)
							free(tokens[i]);
					}
					free(tokens);
				}
				*tokensRef = NULL;
				free(newToken);
				free(tmpToken);
				return 0;
			}
			tokens = tmp;
			tmp = NULL;
			tokens[numTokens - 1] = newToken;
		}
	}
	*tokensRef = tokens;
	free(tmpToken);
	tmpToken = NULL;
	return numTokens;
}

bool leftAssoc(token op)
{
	bool ret = false;
	switch(tokenType(op))
	{
		case addop:
		case multop:
		
			ret = true;
			break;
		case function:
		case expop:
			ret = false;
			break;
		default:
			break;
	}
	return ret;
}

int precedence(token op1, token op2)
{
	int ret = 0;

	if (op2 == NULL)
		ret = 1;
	else if(tokenType(op1) == tokenType(op2)) 
		ret = 0;
	else if(tokenType(op1) == addop
			&& (tokenType(op2) == multop || tokenType(op2) == expop)) 
		ret = -1;
	else if(tokenType(op2) == addop
			&& (tokenType(op1) == multop || tokenType(op1) == expop)) 
		ret = 1;
	else if(tokenType(op1) == multop
			&& tokenType(op2) == expop) 
		ret = -1;
	else if(tokenType(op1) == expop
			&& tokenType(op2) == multop) 
		ret = 1;
	else if (tokenType(op1) == function 
			&& (tokenType(op2) == addop || tokenType(op2) == multop || tokenType(op2) == expop || tokenType(op2) == lparen))
		ret = 1;
	else if ((tokenType(op1) == addop || tokenType(op1) == multop || tokenType(op1) == expop)
			&& tokenType(op2) == function)
		ret = -1;
	return ret;
}

void evalStackPush(Stack *s, token val)
{
	if(prefs.display.postfix)
		printf("\t%s\n", val);

	switch(tokenType(val))
	{
		case function:
			{
				if (doFunc(s, val) < 0)
					return;
			}
			break;
		case expop:
		case multop:
		case addop:
			{
				if(stackSize(s) >= 2)
				{
					if (doOp(s, val) < 0)
						return;
				}
				else
				{
					stackPush(s, val);
				}
			}
			break;
		case value:
			{
				stackPush(s, val);
			}
			break;
		default:
			break;
	}
}

bool postfix(token *tokens, int numTokens, Stack *output)
{
	Stack operators, intermediate;
	int i;
	bool err = false;
	stackInit(&operators, numTokens);
	stackInit(&intermediate, numTokens);
	for(i = 0; i < numTokens; i++)
	{
		switch(tokenType(tokens[i]))
		{
			case value:
				{
					evalStackPush(output, tokens[i]);
				}
				break;
			case function:
				{
					while(stackSize(&operators) > 0
						&& (tokenType(tokens[i]) != lparen)
						&& ((precedence(tokens[i], (char*)stackTop(&operators)) <= 0)))
					{
						evalStackPush(output, stackPop(&operators));
						stackPush(&intermediate, stackTop(output));
					}

					stackPush(&operators, tokens[i]);
				}
				break;
			case argsep:
				{
					while(stackSize(&operators) > 0
						&& tokenType((token)stackTop(&operators)) != lparen
						&& stackSize(&operators) > 1)
					{
						evalStackPush(output, stackPop(&operators));
						stackPush(&intermediate, stackTop(output));
					}
				}
				break;
			case addop:
			case multop:
			case expop:
				{
					while(stackSize(&operators) > 0
						&& (tokenType((char*)stackTop(&operators)) == addop || tokenType((char*)stackTop(&operators)) == multop || tokenType((char*)stackTop(&operators)) == expop)
						&& ((leftAssoc(tokens[i]) && precedence(tokens[i], (char*)stackTop(&operators)) <= 0)
							|| (!leftAssoc(tokens[i]) && precedence(tokens[i], (char*)stackTop(&operators)) < 0)))
					{
						evalStackPush(output, stackPop(&operators));
						stackPush(&intermediate, stackTop(output));
					}
					stackPush(&operators, tokens[i]);
				}
				break;
			case lparen:
				{
					if (tokenType(stackTop(&operators)) == function)
						stackPush(output, FUNCTIONSEPARATOR);
					stackPush(&operators, tokens[i]);
				}
				break;
			case rparen:
				{
					while(stackSize(&operators) > 0
						&& tokenType((token)stackTop(&operators)) != lparen
						&& stackSize(&operators) > 1)
					{
						evalStackPush(output, stackPop(&operators));
						stackPush(&intermediate, stackTop(output));
					}
					if(stackSize(&operators) > 0
						&& tokenType((token)stackTop(&operators)) != lparen)
					{
						err = true;
						raise(parenMismatch);
					}
					stackPop(&operators);
					while (stackSize(&operators) > 0 && tokenType((token)stackTop(&operators)) == function)
					{
						evalStackPush(output, stackPop(&operators));
						stackPush(&intermediate, stackTop(output));
					}
				}
				break;
			default:
				break;
		}
	}
	while(stackSize(&operators) > 0)
	{
		if(tokenType((token)stackTop(&operators)) == lparen)
		{
			raise(parenMismatch);
			err = true;
		}
		evalStackPush(output, stackPop(&operators));
		stackPush(&intermediate, stackTop(output));
	}
	stackPop(&intermediate);
	while (stackSize(&intermediate) > 0)
	{
		token s = stackPop(&intermediate);
		free(s);
	}
	if (err == true)
	{
		while (stackSize(&operators) > 0)
		{
			token s = stackPop(&operators);
			free(s);
		}
	}
	stackFree(&intermediate);
	stackFree(&operators);
	return err;
}

char* substr(char *str, size_t begin, size_t len)
{
	if(str == NULL
		|| strlen(str) == 0
		|| strlen(str) < (begin+len))
		return NULL;

	char *result = (char*)malloc((len + 1) * sizeof(char));
	int i;
	for(i = 0; i < len; i++)
		result[i] = str[begin+i];
	result[i] = '\0';
	return result;
}

bool strBeginsWith(char *haystack, char *needle)
{
	bool result;
	if(strlen(haystack) < strlen(needle))
	{
		return false;
	}
	else
	{
		char *sub = substr(haystack, 0, strlen(needle));
		result = (strcmp(sub, needle) == 0);
		free(sub);
		sub = NULL;
	}
	return result;
}

int strSplit(char *str, const char split, char *(**partsRef))
{
	char **parts = NULL;
	char **tmpparts = NULL;
	char *ptr = str;
	char *part = NULL;
	char *tmppart = NULL;
	int numParts = 0;
	char ch;
	int len = 0;
	while(1)
	{
		ch = *ptr++;

		if((ch == '\0' || ch == split) && part != NULL) 
		{
			tmppart = (char*)realloc(part, (len+1) * sizeof(char));
			if (tmppart == NULL)
			{
				free(part);
				part = NULL;
				for(len=0;len<numParts;len++)
				{
					if (parts[len])
						free(parts[len]);
				}
				if (parts)
					free(parts);
				parts = NULL;
				numParts = 0;
				break;
			}
			part = tmppart;
			part[len] = '\0';

			numParts++;
			if(parts == NULL)
				parts = (char**)malloc(sizeof(char**));
			else
			{
				tmpparts = (char**)realloc(parts, numParts * sizeof(char*));
				if (tmpparts == NULL)
				{
					free(part);
					part = NULL;
					for(len=0;len<numParts-1;len++)
					{
						if (parts[len])
							free(parts[len]);
					}
					if (parts)
						free(parts);
					parts = NULL;
					numParts = 0;
					break;
				}
				parts = tmpparts;
			}
			parts[numParts - 1] = part;
			part = NULL;
			len = 0;
		}
		else
		{
			len++;
			if(part == NULL)
			{
				part = (char*)malloc(sizeof(char));
			}
			else
			{
				tmppart = (char*)realloc(part, len * sizeof(char));
				if (tmppart == NULL)
				{
					free(part);
					part = NULL;
					for(len=0;len<numParts;len++)
					{
						if (parts[len])
							free(parts[len]);
					}
					free(parts);
					numParts = 0;
					parts = NULL;
					break;
				}
				part = tmppart;
			}
			part[len - 1] = ch;
		}

		if(ch == '\0')
			break;
	}
	*partsRef = parts;
	return numParts;
}

bool execCommand(char *str)
{
	int i = 0;
	bool recognized = false;
	char **words = NULL;
	int len = strSplit(str, ' ', &words);
	if(len >= 1 && strcmp(words[0], "get") == 0)
	{
		if(len >= 2 && strcmp(words[1], "display") == 0)
		{
			if(len >= 3 && strcmp(words[2], "tokens") == 0)
			{
				recognized = true;
				printf("\t%s\n", (prefs.display.tokens ? "on" : "off"));
			}
			else if(len >= 3 && strcmp(words[2], "postfix") == 0)
			{
				recognized = true;
				printf("\t%s\n", (prefs.display.postfix ? "on" : "off"));
			}
		}
		else if(len >= 2 && strcmp(words[1], "mode") == 0)
		{
			recognized = true;
			printf("\t%s\n", (prefs.mode.degrees ? "degrees" : "radians"));
		}
		else if(len >= 2 && strcmp(words[1], "precision") == 0)
		{
			recognized = true;
			if (prefs.precision > 0)
				printf("\t%d\n", prefs.precision);
			else
				printf("\tauto\n");
		}
	}
	else if(len >= 1 && strcmp(words[0], "set") == 0)
	{
		if(len >= 2 && strcmp(words[1], "display") == 0)
		{
			if(len >= 3 && strcmp(words[2], "tokens") == 0)
			{
				if(len >= 4 && strcmp(words[3], "on") == 0)
				{
					recognized = true;
					prefs.display.tokens = true;
				}
				else if(len >= 4 && strcmp(words[3], "off") == 0)
				{
					recognized = true;
					prefs.display.tokens = false;
				}
			}
			else if(len >= 3 && strcmp(words[2], "postfix") == 0)
			{
				if(len >= 4 && strcmp(words[3], "on") == 0)
				{
					recognized = true;
					prefs.display.postfix = true;
				}
				else if(len >= 4 && strcmp(words[3], "off") == 0)
				{
					recognized = true;
					prefs.display.postfix = false;
				}
			}
		}
		else if(len >= 2 && strcmp(words[1], "mode") == 0)
		{
			if(len >= 3 && strcmp(words[2], "radians") == 0)
			{
				recognized = true;
				prefs.mode.degrees = false;
			}
			else if(len >= 3 && strcmp(words[2], "degrees") == 0)
			{
				recognized = true;
				prefs.mode.degrees = true;
			}
		}
		else if (len >= 2 && strcmp(words[1], "precision") == 0)
		{
			if(len >= 3 && strcmp(words[2], "auto") == 0)
			{
				recognized = true;
				prefs.precision = -1;
			}
			else if (len >= 3 && type(words[2][0]) == digit)
			{
				recognized = true;
				prefs.precision = atoi(words[2]);
			}
		}
	}
	if (words)
	{
		for (i=0;i<len;i++)
		{
			if (words[i])
				free(words[i]);
		}
		free(words);
	}

	return recognized;
}

int main(int argc, char *argv[])
{
	char* str = NULL;
	token* tokens = NULL;
	int numTokens = 0;
	Stack expr;
	int i;
	int ch, rflag = 0;
	prefs.precision = DEFAULTPRECISION;
	prefs.maxtokenlength = MAXTOKENLENGTH;

	while ((ch = getopt(argc, argv, "rm:")) != -1) {
		switch (ch) {
			case 'r':
				rflag = 1;
				break;
			case 'm':
				prefs.maxtokenlength = atoi(optarg);
		}
	}
	str = ufgets(stdin);
	while(str != NULL && strcmp(str, "quit") != 0)
	{
		if (strlen(str) == 0)
			goto get_new_string;
		if(type(*str) == text)
		{
			if (!execCommand(str))
				goto no_command;

			free(str);
			str = NULL;
		}
		else
		{
no_command:
			numTokens = tokenize(str, &tokens);
			free(str);
			str = NULL;

			if(prefs.display.tokens)
			{
				printf("\t%d tokens:\n", numTokens);
				for(i = 0; i < numTokens; i++)
				{
					printf("\t\"%s\"", tokens[i]);
					if(tokenType(tokens[i]) == value)
						printf(" = %f", buildNumber(tokens[i]));
					printf("\n");
				}
			}

			stackInit(&expr, numTokens);
			if(prefs.display.postfix)
				printf("\tPila de Postfix:\n");
			postfix(tokens, numTokens, &expr);
			if(stackSize(&expr) != 1)
			{
				printf("\tError evaluando expresion\n");
			}
			else
			{
				if (!rflag)
					printf("\t= ");
				printf("%s\n", (char*)stackTop(&expr));
				for (i=0; i< numTokens; i++)
				{
					if (tokens[i] == stackTop(&expr))
						tokens[i] = NULL;
				}
				free(stackPop(&expr));
			}

			for(i = 0; i < numTokens; i++)
			{
				if (tokens[i] != NULL)
					free(tokens[i]);
			}
			free(tokens);
			tokens = NULL;
			numTokens = 0;
			stackFree(&expr);
		}
get_new_string:
		str = ufgets(stdin);
	}

	free(str);
	str = NULL;


	return EXIT_SUCCESS;
}
