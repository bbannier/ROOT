static const unsigned long noperator = 66;
static const char *operator_control_sequence[noperator] = {
	"\\Pr", "\\arccos", "\\arcsin", "\\arctan", "\\arg", "\\awoint",
	"\\awointop", "\\barint", "\\barintop", "\\bigcap", "\\bigcup",
	"\\bigodot", "\\bigoplus", "\\bigotimes", "\\bigsqcup",
	"\\biguplus", "\\bigvee", "\\bigwedge", "\\coprod", "\\cos",
	"\\cosh", "\\cot", "\\coth", "\\csc", "\\cwint", "\\cwintop",
	"\\cwoint", "\\cwointop", "\\deg", "\\det", "\\dim", "\\exp",
	"\\gcd", "\\hom", "\\iiint", "\\iiintop", "\\iint", "\\iintop",
	"\\inf", "\\int", "\\intop", "\\ker", "\\lg", "\\lim", "\\liminf",
	"\\limsup", "\\ln", "\\log", "\\max", "\\min", "\\oiiint",
	"\\oiiintop", "\\oiint", "\\oiintop", "\\oint", "\\ointop",
	"\\prod", "\\sec", "\\sin", "\\sinh", "\\slashint",
	"\\slashintop", "\\sum", "\\sup", "\\tan", "\\tanh"
};
static const wchar_t operator_code_point[noperator] = {
	L'\0', L'\0', L'\0', L'\0', L'\0', L'\u2233', L'\u2233',
	L'\u2a0d', L'\u2a0d', L'\u22c2', L'\u22c3', L'\u2a00', L'\u2a01',
	L'\u2a02', L'\u2a06', L'\u2a04', L'\u22c1', L'\u22c0', L'\u2210',
	L'\0', L'\0', L'\0', L'\0', L'\0', L'\u2231', L'\u2231',
	L'\u2232', L'\u2232', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0',
	L'\u222d', L'\u222d', L'\u222c', L'\u222c', L'\0', L'\u222b',
	L'\u222b', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0', L'\0',
	L'\0', L'\u2230', L'\u2230', L'\u222f', L'\u222f', L'\u222e',
	L'\u222e', L'\u220f', L'\0', L'\0', L'\0', L'\u2a0f', L'\u2a0f',
	L'\u2211', L'\0', L'\0', L'\0'
};
static const char *operator_content[noperator] = {
	"Pr", "arccos", "arcsin", "arctan", "arg", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "cos", "cosh", "cot", "coth",
	"csc", "", "", "", "", "deg", "det", "dim", "exp", "gcd", "hom",
	"", "", "", "", "inf", "", "", "ker", "lg", "lim", "lim\\,inf",
	"lim\\,sup", "ln", "log", "max", "min", "", "", "", "", "", "",
	"", "sec", "sin", "sinh", "", "", "", "sup", "tan", "tanh"
};
static const bool operator_nolimits[noperator] = {
	false, true, true, true, true, true, false, true, false, false,
	false, false, false, false, false, false, false, false, false,
	true, true, true, true, true, true, false, true, false, true,
	false, true, true, false, true, true, false, true, false, false,
	true, false, true, true, false, false, false, true, true, false,
	false, true, false, true, false, true, false, false, true, true,
	true, true, false, false, false, true, true
};
