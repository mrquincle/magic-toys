// CPP program to demonstrate Morse code 
#include <iostream> 
using namespace std; 

// function to encode a alphabet as 
// Morse code 
string morseEncode(char x) { 

	// refer to the Morse table 
	// image attached in the article 
	switch (x) { 
	case 'a': 
		return ".-"; 
	case 'b': 
		return "-..."; 
	case 'c': 
		return "-.-."; 
	case 'd': 
		return "-.."; 
	case 'e': 
		return "."; 
	case 'f': 
		return "..-."; 
	case 'g': 
		return "--."; 
	case 'h': 
		return "...."; 
	case 'i': 
		return ".."; 
	case 'j': 
		return ".---"; 
	case 'k': 
		return "-.-"; 
	case 'l': 
		return ".-.."; 
	case 'm': 
		return "--"; 
	case 'n': 
		return "-."; 
	case 'o': 
		return "---"; 
	case 'p': 
		return ".--."; 
	case 'q': 
		return "--.-"; 
	case 'r': 
		return ".-."; 
	case 's': 
		return "..."; 
	case 't': 
		return "-"; 
	case 'u': 
		return "..-"; 
	case 'v': 
		return "...-"; 
	case 'w': 
		return ".--"; 
	case 'x': 
		return "-..-"; 
	case 'y': 
		return "-.--"; 
		// for space 
	case 'z': 
		return "--.."; 
	}
	return "";
} 

string morseCode(string s) { 
	// character by character print 
	// Morse code 
	std::stringstream sout;
	for (int i = 0; s[i]; i++) {
		sout << morseEncode(s[i]) << ' ';
	}
	return sout.str();
} 

/*
// Driver's code 
int main() { 
string s = "geeksforgeeks"; 
morseCode(s); 
return 0; 
} 

*/
