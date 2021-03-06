#include "stdafx.h"
#include <iostream>
#include <string>
#include <ctype.h>
#include <vector>

using namespace std;

double getCoefficient(string);
string removeTrailingZeros(string);

struct Term
{
	vector<Term> components;
	double coefficient = 0;
	string term;
	string derivative;
	string type;
	Term(string input, double c)
	{
		term = input;
		coefficient = c;
	}
	Term(string input, string t)
	{
		term = input;
		type = t;
	}

};

//calculates the derivative of the given string
//assumes no complex sub-expressions
//for powers, only constant sub-expressions
//for trig and ln, only sub-expressions of the form "ax"
string differentiate(string expression, string type)
{
	string derivative;
	string subExpression;
	string subExpressionDerivative;
	int size = expression.size();
	int count = 0;
	double coefficient = getCoefficient(expression);

	//finds the sub-expression
	if ((type != "") && (type != "constant"))
	{
		for (unsigned int i = 0; i < expression.size(); i++)
		{
			if (expression.at(i) == '(')
			{
				count = i;
			}
		}
		//checks if the sub-expression is just a variable by itself, in which case the coefficient is 1 
		if (expression.at(count + 1) == 'x')
		{
			subExpression = "1";
		}
		//checks if the sub-expression is "-x", in which case the coefficient is -1
		else if ((expression.at(count + 1) == '-') && (expression.at(count + 2) == 'x'))
		{
			subExpression = "-1";
		}
		else
		{
			subExpression = expression.substr(count + 1, size - count - 2);
		}
	}

	//finds the derivative of ln
	if (type == "ln")
	{
		derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "/";
		if (subExpression == "1")
		{
			derivative += "x";
		}
		else if (subExpression == "-1")
		{
			derivative += "-x";
		}
		else
		{
			derivative += subExpression;
		}
		return derivative;
	}

	//finds the derivative of a trig function
	if ((type == "sin") || (type == "cos") || (type == "tan") || (type == "csc") || (type == "sec") || (type == "cot"))
	{
		if (type == "sin")
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "cos(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)";
			}
			else
			{
				derivative += subExpression + ")";
			}
		}
		else if (type == "cos")
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient * -1)) + "sin(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)";
			}
			else
			{
				derivative += subExpression + ")";
			}
		}
		else if (type == "tan")
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "sec^(2)(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)";
			}
			else
			{
				derivative += subExpression + ")";
			}
		}
		else if (type == "csc")
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient * -1)) + "csc(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)cot(x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)cot(-x)";
			}
			else
			{
				derivative += subExpression + ")cot(" + subExpression + ")";
			}
		}
		else if (type == "sec")
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "sec(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)tan(x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)tan(-x)";
			}
			else
			{
				derivative += subExpression + ")tan(" + subExpression + ")";
			}
		}
		else
		{
			derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient * -1)) + "csc^(2)(";
			if (stod(subExpression) == 1)
			{
				derivative += "x)";
			}
			else if (stod(subExpression) == -1)
			{
				derivative += "-x)";
			}
			else
			{
				derivative += subExpression + ")";
			}
		}
		return derivative;
	}

	for (int i = 0; i < size; i++)
	{
		//checks if component is a power
		//assumes power is a constant
		if (expression.at(i) == '^')
		{
			subExpression = expression.substr(i + 2, size - (i + 3));
			if ((stod(subExpression) != 1) && (stod(subExpression) != 2))
			{
				derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "x^(" + removeTrailingZeros(to_string(stod(subExpression) - 1)) + ")";
			}
			else if (stod(subExpression) == 2)
			{
				derivative = removeTrailingZeros(to_string(stod(subExpression) * coefficient)) + "x";
			}
			else
			{
				derivative = removeTrailingZeros(to_string(coefficient));
			}
			return derivative;
		}
	}
	//checks if the component is a constant
	if (type == "constant")
	{
		return "0";
	}
	else
	{
		return removeTrailingZeros(to_string(coefficient));
	}


}

//breaks term into components and determines component type
//assumes term is in correct format; parentheses after power, trig expression or ln
//returns the term's derivative
string readTerm(Term expression)
{
	int startOfComponent = 0;
	int endOfComponent = 0;
	int size = expression.term.size();
	bool isConstant = true;
	bool canPushBack = true;
	string type;
	string termDerivative;
	int numerator = 0;
	int denominator = 0;
	bool canUseQuotientRule = false;

	for (int i = 0; i < size; i++)
	{
		if (expression.term.at(i) == '/')
		{
			canUseQuotientRule = true;
			if (isConstant)
			{
				//isConstant = false;
				expression.components.push_back(Term(expression.term.substr(startOfComponent, i - startOfComponent), "constant"));
			}
			numerator = expression.components.size() - 1;
			denominator = expression.components.size();
			startOfComponent = i + 1;
			isConstant = true;
		}
		if (isalpha(expression.term.at(i)))
		{
			isConstant = false;
		}

		if ((expression.term.at(i) == 'x') && (i == (size - 1)))
		{
			expression.components.push_back(Term(expression.term.substr(startOfComponent, size - startOfComponent), ""));
		}
		else if ((expression.term.at(i) == 'x') && (expression.term.at(i + 1) != '^') && (expression.term.at(i + 1) != ')'))
		{
			startOfComponent = i + 1;
			expression.components.push_back(Term(expression.term.substr(0, i + 1), ""));
		}

		if (expression.term.at(i) == '^')
		{
			type = "power";
		}

		if (i + 2 < size)
		{
			if ((expression.term.at(i) == 's') && (expression.term.at(i + 1) == 'i') && (expression.term.at(i + 2) == 'n'))
			{
				type = "sin";
			}

			if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 'o') && (expression.term.at(i + 2) == 's'))
			{
				type = "cos";
			}

			if ((expression.term.at(i) == 't') && (expression.term.at(i + 1) == 'a') && (expression.term.at(i + 2) == 'n'))
			{
				type = "tan";
			}

			if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 's') && (expression.term.at(i + 2) == 'c'))
			{
				type = "csc";
			}

			if ((expression.term.at(i) == 's') && (expression.term.at(i + 1) == 'e') && (expression.term.at(i + 2) == 'c'))
			{
				type = "sec";
			}

			if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 'o') && (expression.term.at(i + 2) == 't'))
			{
				type = "cot";
			}
		}

		if (i + 1 < size)
		{
			if ((expression.term.at(i) == 'l') && (expression.term.at(i + 1) == 'n'))
			{
				type = "ln";
			}
		}

		if (expression.term.at(i) == ')')
		{
			endOfComponent = i + 1;
			expression.components.push_back(Term(expression.term.substr(startOfComponent, endOfComponent - startOfComponent), type));
			if (i + 1 < size)
			{
				startOfComponent = i + 1;
			}
		}

		if ((i != startOfComponent) && ((expression.term.at(i) == 'l')
			|| (expression.term.at(i) == 'c') || (expression.term.at(i) == 's') || (expression.term.at(i) == 't') || (expression.term.at(i) == 'e')))
		{
			//checks for cos, csc, sec, and cot
			if (size > (i + 2))
			{
				if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 'o') && (expression.term.at(i + 2) == 's'))
				{
					canPushBack = false;
				}

				if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 's') && (expression.term.at(i + 2) == 'c'))
				{
					canPushBack = false;
				}

				if ((expression.term.at(i) == 's') && (expression.term.at(i + 1) == 'e') && (expression.term.at(i + 2) == 'c'))
				{
					canPushBack = false;
				}

				if ((expression.term.at(i) == 'c') && (expression.term.at(i + 1) == 'o') && (expression.term.at(i + 2) == 't'))
				{
					canPushBack = false;
				}
			}

			else if (canPushBack)
			{
				endOfComponent = i;
				expression.components.push_back(Term(expression.term.substr(startOfComponent, endOfComponent), type));
				startOfComponent = i;
			}
			canPushBack = true;
		}
	}

	if (isConstant)
	{
		type = "constant";
	}

	if (expression.components.size() == 0)
	{
		expression.components.push_back(Term(expression.term, ""));
	}

	if ((expression.components.size() == 1) && (canUseQuotientRule))
	{
		expression.components.push_back(Term(expression.term.substr(startOfComponent, expression.term.size() - startOfComponent), type));
	}

	//product rule
	if ((expression.components.size() > 1) && !(canUseQuotientRule))
	{
		for (unsigned int i = 0; i < expression.components.size(); i++)
		{
			termDerivative += differentiate(expression.components[i].term, expression.components[i].type);
			for (unsigned int j = 0; j < expression.components.size(); j++)
			{
				if (i != j)
				{
					termDerivative += expression.components[j].term;
				}
			}
			if (i != (expression.components.size() - 1))
			{
				termDerivative += "+";
			}
		}
		cout << endl;
	}
	//quotient rule
	else if ((expression.components.size() > 1) && (canUseQuotientRule))
	{
		if (expression.components.size() > 2)
		{
			cout << "The program does not currently support product rule and quotient rule in the same equation" << endl;
		}
		else
		{
			termDerivative = "(";

			if (differentiate(expression.components[numerator].term, expression.components[numerator].type) != "0")
			{
				termDerivative += differentiate(expression.components[numerator].term, expression.components[numerator].type) + expression.components[denominator].term;
			}

			if (differentiate(expression.components[denominator].term, expression.components[denominator].type) != "0")
			{
				termDerivative += "-" + differentiate(expression.components[denominator].term, expression.components[denominator].type) + expression.components[numerator].term;
			}

			if ((differentiate(expression.components[numerator].term, expression.components[numerator].type) == "0")
				&& (differentiate(expression.components[denominator].term, expression.components[denominator].type) == "0"))
			{
				termDerivative = "0";
			}
			else
			{
				termDerivative += ")/(" + expression.components[denominator].term + ")^2";
			}
		}
	}
	else
	{
		termDerivative = differentiate(expression.term, type);
	}
	return termDerivative;
}

//returns the coefficient of the given term
double getCoefficient(string expression)
{
	double coefficient = 0;
	int count = 0;

	//accounts for the opening parentheses if the quotient rule had already been applied to the original equation
	if (expression.at(0) == '(')
	{
		count = 1;
	}
	//checks if component is a variable
	if ((expression.size() > 1) && (isalpha(expression.at(expression.size() - 1))) && (isdigit(expression.at(expression.size() - 2))))
	{
		return stod(expression.substr(0, expression.size() - 1));
	}
	for (unsigned int i = 0; i < expression.size(); i++)
	{
		if (isalpha(expression.at(i)))
		{
			if (i == 0 + count)
			{
				return 1;
			}
			else if ((i == 1 + count) && (expression.at(0 + count) == '-'))
			{
				return -1;
			}
			else if (isdigit(expression.at(i - 1)))
			{
				return stod(expression.substr(0 + count, i - count));
			}
		}
	}
	return stod(expression);
}
//breaks equation into terms, seperated by '+' or '-'
//gets coefficient of each term
vector<Term> readEquation(string userInput)
{
	vector<Term> terms;
	int size = userInput.size();
	double coefficient = 0;
	int startOfCoefficient = 0;
	int endOfCoefficient = 0;
	int startOfTerm = 0;
	int endOfTerm = 0;
	double sign = 1.0;
	string newEquation;

	//returns the input immediately if the quotient rule has already been used on the original equation
	if (userInput.at(0) == '(')
	{
		terms.push_back(Term(userInput, coefficient));
		return terms;
	}
	//assumes user input is valid; terms are broken when a '+' or '-' is found, assuming no spaces
	for (int i = 0; i < size; i++)
	{
		if (userInput.at(i) == '/')
		{
			coefficient = stod(userInput.substr(startOfCoefficient, i - startOfCoefficient)) *sign;
		}
		if (isalpha(userInput.at(i)))
		{
			if (i == startOfTerm)
			{
				coefficient = 1;
			}
			else if ((i == startOfTerm) && (userInput.at(startOfTerm) == '-'))
			{
				coefficient = -1;
			}
			else if (isdigit(userInput.at(i - 1)) && (i != size - 1))
			{
				if (userInput.at(i + 1) != ')')
				{
					endOfCoefficient = i;
					coefficient = stod(userInput.substr(startOfCoefficient, endOfCoefficient)) *sign;
					sign = 1;
				}
			}
		}

		if (userInput.at(i) == '+')
		{
			endOfTerm = i;
			terms.push_back(Term(userInput.substr(startOfTerm, i - startOfTerm), coefficient));
			startOfTerm = i + 1;
			startOfCoefficient = i + 1;
		}
		else if (userInput.at(i) == '-')
		{
			if ((i != startOfTerm) && userInput.at(i - 1) != '(')
			{
				endOfTerm = i;
				terms.push_back(Term(userInput.substr(startOfTerm, i - startOfTerm), coefficient));
				startOfTerm = i;
				startOfCoefficient = i + 1;
				sign = -1;
			}
		}
	}
	terms.push_back(Term(userInput.substr(startOfTerm, size - startOfTerm), coefficient));
	return terms;
}
//checks if the input is valid
bool checkInput(string equation)
{
	bool isValid = false;
	bool insideParentheses = false;
	bool powerSubExpression = false;

	for (unsigned int i = 0; i < equation.size(); i++)
	{
		//checks if the character is the variable 'x' or part of a trig function or ln function
		if (isalpha(equation.at(i)))
		{
			if ((equation.at(i) == 's') || (equation.at(i) == 'i') || (equation.at(i) == 'n') || (equation.at(i) == 'c') || (equation.at(i) == 'o')
				|| (equation.at(i) == 't') || (equation.at(i) == 'a') || (equation.at(i) == 'e') || (equation.at(i) == 'l') || (equation.at(i) == 'x'))
			{
				if (!insideParentheses)
				{
					//checks for sin and sec
					if ((equation.at(i) == 's') && !(isValid))
					{
						if (i + 3 < equation.size())
						{
							if ((equation.at(i + 1) == 'i') && (equation.at(i + 2) == 'n') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else if ((equation.at(i + 1) == 'e') && (equation.at(i + 2) == 'c') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					//checks for cos, csc, and cot
					if ((equation.at(i) == 'c') && !(isValid))
					{
						if (i + 3 < equation.size())
						{
							if ((equation.at(i + 1) == 'o') && (equation.at(i + 2) == 's') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else if ((equation.at(i + 1) == 's') && (equation.at(i + 2) == 'c') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else if ((equation.at(i + 1) == 'o') && (equation.at(i + 2) == 't') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					//checks for tan
					if ((equation.at(i) == 't') && !(isValid))
					{
						if (i + 3 < equation.size())
						{
							if ((equation.at(i + 1) == 'a') && (equation.at(i + 2) == 'n') && (equation.at(i + 3) == '('))
							{
								isValid = true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}

					//checks for ln
					if ((equation.at(i) == 'l') && !(isValid))
					{
						if (i + 2 < equation.size())
						{
							if ((equation.at(i + 1) == 'n') && (equation.at(i + 2) == '('))
							{
								isValid = true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}
				}
				else if (!(insideParentheses && !powerSubExpression && equation.at(i) == 'x'))
				{
					return false;
				}

			}
			else
			{
				return false;
			}
		}

		if (!(isalpha(equation.at(i))) && !(isdigit(equation.at(i))))
		{
			//checks if the current position is outside the parentheses 
			if (equation.at(i) == ')')
			{
				isValid = false;
				powerSubExpression = false;
				insideParentheses = false;
			}
			//checks if the current position is inside the parentheses
			else if (equation.at(i) == '(')
			{
				//checks for consequtive opening parentheses
				if (!insideParentheses)
				{
					insideParentheses = true;
				}
				else
				{
					return false;
				}

				//checks if the equation begins with an opening parentheses
				if (i == 0)
				{
					return false;
				}
				//checks if parentheses are not around sub-expression
				else if (!(isalpha(equation.at(i - 1))))
				{
					return false;
				}
			}
			//checks if the current position is inside the subExpression of a power
			//checks if the next character after '^' is '('
			else if (equation.at(i) == '^')
			{
				powerSubExpression = true;

				if (i + 1 < equation.size())
				{
					if (equation.at(i + 1) != '(')
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			//checks for correctness of '+'
			else if (equation.at(i) == '+')
			{
				if (i == equation.size() - 1)
				{
					return false;
				}

				if (insideParentheses)
				{
					return false;
				}

				if (i == 0)
				{
					return false;
				}

				if (!(isalpha(equation.at(i + 1))) && !(isdigit(equation.at(i + 1))))
				{
					return false;
				}
			}
			//needs to be filled out
			else if (equation.at(i) == '-')
			{
				if (i == equation.size() - 1)
				{
					return false;
				}

				if (!(isalpha(equation.at(i + 1))) && !(isdigit(equation.at(i + 1))))
				{
					return false;
				}

				if (insideParentheses)
				{
					if (equation.at(i - 1) != '(')
					{
						return false;
					}
				}
			}
			//checks for decimal point
			else if (equation.at(i) == '.')
			{
				if (i == equation.size() - 1)
				{
					return false;
				}

				if (i == 0)
				{
					return false;
				}

				if (!(isdigit(equation.at(i - 1)) && isdigit(equation.at(i + 1))))
				{
					return false;
				}
			}
			//not allowing quotient rule for now
			/*else if (equation.at(i) == '/')
			{

			}*/
			else
			{
				return false;
			}
		}
		//checks if the user forgot to add a closing parentheses
		if ((i == equation.size() - 1) && insideParentheses)
		{
			return false;
		}

		if (isdigit(equation.at(i)))
		{
			if (i > 0)
			{
				if (!((equation.at(i - 1) == '+') || (equation.at(i - 1) == '-') || (equation.at(i - 1) == '(') || isdigit(equation.at(i - 1)) || (equation.at(i - 1) == '.')))
				{
					return false;
				}
			}
		}
	}
	return true;
}

//combines the derivative of each term into a final result
string getResult(vector<Term> &terms)
{
	string derivative;

	for (unsigned int i = 0; i < terms.size(); i++)
	{
		derivative += terms[i].derivative;
		if (i != (terms.size() - 1))
		{
			derivative += "+";
		}
		cout << endl;
	}

	return derivative;
}

string formatResult(string derivative)
{
	vector<Term> finalTerms = readEquation(derivative);
	string formattedDerivative;
	int startOfNumber = 0;
	int lengthOfCoefficient = 0;
	double number = 1;
	int count = 0;
	int numberOfZeroTerms = 0;
	bool withinMisplacedNumber = false;

	if (derivative == "0")
	{
		return derivative;
	}

	//accounts for the opening parentheses at the start of the derivative if the quotient rule was applied
	if (derivative.at(0) == '(')
	{
		count = 1;
	}

	for (unsigned int i = 0; i < finalTerms.size(); i++)
	{
		for (unsigned int j = 0; j < finalTerms[i].term.size(); j++)
		{
			if (j > 0)
			{
				if (isdigit(finalTerms[i].term.at(j)) && ((finalTerms[i].term.at(j - 1) == ')') || (finalTerms[i].term.at(j - 1) == 'x')))
				{
					startOfNumber = j;
					withinMisplacedNumber = true;
				}

				if ((finalTerms[i].term.at(j) == ')') && (withinMisplacedNumber))
				{
					number = stod(finalTerms[i].term.substr(startOfNumber, j - startOfNumber));
					finalTerms[i].term.erase(startOfNumber, j - startOfNumber);
					withinMisplacedNumber = false;
				}

				if ((j == finalTerms[i].term.size() - 1) && (withinMisplacedNumber))
				{
					number = stod(finalTerms[i].term.substr(startOfNumber, j - startOfNumber));
					finalTerms[i].term.erase(startOfNumber, j - startOfNumber);
					withinMisplacedNumber = false;
				}

				if (isalpha(finalTerms[i].term.at(j)) && (withinMisplacedNumber))
				{
					number = stod(finalTerms[i].term.substr(startOfNumber, j - startOfNumber));
					finalTerms[i].term.erase(startOfNumber, j - startOfNumber);
					withinMisplacedNumber = false;
				}
			}
		}
		finalTerms[i].coefficient = getCoefficient(finalTerms[i].term);
		finalTerms[i].coefficient *= number;

		if (finalTerms[i].term != "0")
		{
			lengthOfCoefficient = removeTrailingZeros(to_string(finalTerms[i].coefficient)).size();

			if (count == 1)
			{
				finalTerms[i].term = "(" + removeTrailingZeros(to_string(finalTerms[i].coefficient)) + finalTerms[i].term.substr(lengthOfCoefficient + count, finalTerms[i].term.size() - lengthOfCoefficient - count);
			}
			else
			{
				finalTerms[i].term = removeTrailingZeros(to_string(finalTerms[i].coefficient)) + finalTerms[i].term.substr(lengthOfCoefficient + count, finalTerms[i].term.size() - lengthOfCoefficient - count);
			}
		}
		else
		{
			numberOfZeroTerms++;
		}
		number = 1;
	}
	for (int j = 0; j < finalTerms.size(); j++)
	{
		if (finalTerms[j].term != "0")
		{
			formattedDerivative += finalTerms[j].term;
		}
		if (j < finalTerms.size() - 1 - numberOfZeroTerms)
		{
			formattedDerivative += "+";
		}
	}

	for (unsigned int i = 0; i < formattedDerivative.size(); i++)
	{
		if (i < (formattedDerivative.size() - 1))
		{
			if ((formattedDerivative.at(i) == '1') && (isalpha(formattedDerivative.at(i + 1))))
			{
				formattedDerivative.erase(i, 1);
			}
		}

		if (i > 0)
		{
			if ((formattedDerivative.at(i) == '-') && (formattedDerivative.at(i - 1) == '+'))
			{
				formattedDerivative.erase(i - 1, 1);
				i--;
			}
		}
	}
	return formattedDerivative;
}

string removeTrailingZeros(string input)
{
	int index = input.size() - 1;

	while ((input.at(index) == '0') || (input.at(index) == '.'))
	{
		double temp = stod(input);
		input.pop_back();
		if (stod(input) < temp)
		{
			input += "0";
		}
		index--;
	}

	if (input == "")
	{
		return "0";
	}
	else
	{
		return input;
	}
}

int main()
{
	string userInput;
	string derivative;
	string finalResult;
	vector<Term> terms;

	cout << "***************************************************************************************************" << endl;
	cout << "*The program does not currently support fractions, complex sub-expressions, exponentials, " << endl;
	cout << "log, functions raised to powers, chain-rule, and arc-trigonometric functions" << endl;
	cout << "*Only use 'x' as a variable" << endl;
	cout << "*Seperate the terms in the equation with '+' and '-', but without including spaces" << endl;
	cout << "*Use parentheses for trigonometric expressions, powers, and natural log expressions" << endl;
	cout << "***************************************************************************************************" << endl << endl;
	cout << "Please enter an equation" << endl << endl;
	getline(cin, userInput);

	if (checkInput(userInput))
	{
		terms = readEquation(userInput);

		for (unsigned int i = 0; i < terms.size(); i++)
		{
			terms[i].derivative = readTerm(terms[i]);
		}

		derivative = getResult(terms);
		finalResult = formatResult(derivative);

		cout << "The derivative of " << userInput << " is " << finalResult << endl;
	}
	else
	{
		cout << "checkInput() returned false" << endl;
	}
	return 0;
}



