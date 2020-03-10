#include "Helper.h"
bool Helper::IPCompare(string first, string second) {
	string firstLast = first.substr(first.rfind(".") + 1, string::npos);
	string secondtLast = second.substr(first.rfind(".") + 1, string::npos);
	if (firstLast.size() == secondtLast.size())
		return first > second;
	return first.size() > second.size();
}