#include "Helper.h"

bool Helper::IPCompare(string first, string second) {
	string firstEnd = first.substr(first.rfind(".") + 1, string::npos);
	string secondtEnd = second.substr(second.rfind(".") + 1, string::npos);
	if (firstEnd.size() == secondtEnd.size())
		return first > second;
	return first.size() > second.size();
}

