#ifndef Symbol_H
#define Symbol_H

#include <string>

class Symbol
{
private:
	std::string value;
	bool type; // if true then terminal.

public:
	Symbol();
	Symbol(std::string &, bool);
    Symbol(const std::string);
	std::string getValue() const;
	void setValue(std::string);
	void setType(bool);
	virtual bool isTerminal() const;
	bool operator==(const Symbol &s) const;
	bool operator!=(const Symbol &s) const;
	bool operator<(const Symbol &s) const;
	Symbol operator=(const Symbol &s);
};
#endif // Symbol_H