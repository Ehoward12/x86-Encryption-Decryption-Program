// The encryption program in C++ and ASM with a very simple encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
// Filename: "4473 FoCA 2016 Encryption Original with ASM.cpp"
//
// Elliot Howard, last update: April 28 2016
//  *** PLEASE CHANGE THE LINE ABOVE TO YOUR NAME & DATE ***
//  *** KEEP CODE COMMENTS UP-TO-DATE. COMMENT USEFULLY ALL CODE YOU PRODUCE. ***

#include <conio.h>		// for kbhit
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
using namespace std;

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum
#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
EChars[MAXCHARS],
DChars[MAXCHARS] = "Soon!";	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char;
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int slength, char EKey)
{
	__asm {							    // Start of assembly code
			push   eax					// Pushes eax to the stack to save the original value for later
			push   ecx					// Pushes ecx to the stack to save its original value for later
			push   ebx					// Pushes ebx to the stack to save its original value for later
			push   edx					// Pushes edx to the stack to save its original value for later
			//
			mov ebx, 6					// Sets up the loop counter, maximum amount of loops being 6 which is decremented each time, and stores the value in the ebx register
		forloop8:						// Everything inside of this for loop is looped 6 times for every character in the orginal characters array
			mov edx, 6					// Sets up a constant variable which the loop counter is subtracted from so that the right index for the array can be chosen, set at 6
			//
			sub edx, ebx				// Subtracts the loop counter from the maximum loop value
			movzx ecx, OChars[edx]		// Takes the current original character using the index of the maximum loops minus the loop counter, starting from 0 ending in 5
			push edx					// Saves the edx register (the maximum amount of loops) so that it will be restored after the call has happened which uses and chnages the edx value
			push ecx					// Pushes the original character through the parameters to the encrypt function using the cdecl calling convention
			lea eax, EKey				// Gets the memory address for the Ekey and stores it in the eax register
			push eax					// Pushes the memory address of the EKey on to the stack so that the value can be passed through the encrypt8 call parameter
			call   encrypt8				// Calls the encryption method
			add esp, 8                  // Adds 8 to the stack pointer to essentially remove the temp_char and EKey values from the stack
			pop edx						// Pops back the maximum amount of loops into the edx register after it is changed as it is needed again
			mov EChars[edx], al			// Puts the encrypted character in to the encrypted character array
			//
			dec ebx						// Decrements the loop counter, starting from 6 till it hits 0
			jnz forloop8				// Calls the loop again until the loop counter reaches 0
			//
			pop    edx					// Restores original value of edx, overwriting any changes that occured to it
			pop	   ebx					// Restores original value of ebx, overwriting any changes that occured to it
			pop    ecx					// Restores original value of ecx, overwriting any changes that occured to it
			pop    eax					// Restores original value of eax, overwriting any changes that occured to it
	}
	return;


	// Encrypt subroutine. You should paste in the encryption routine you've been allocated from Bb and
	// overwrite this initial, simple, version. Ensure you change the ‘call’ above to use the
	// correct 'encryptnn' label where nn is your encryption routine number.
	// Inputs: register EAX = 32-bit address of Ekey,
	//					ECX = the character to be encrypted (in the low 8-bit field, CL).
	// Output: register EAX = the encrypted value of the source character (in the low 8-bit field, AL).
	__asm {

	encrypt8:
		push ebp                        // Pushes the base pointer on to the stack to save the value before it's edited
			mov ebp, esp                // Copies the stack pointer value into the base pointer value so that it can be used without being edited and potentially cause errors
			mov    eax, [ebp + 8]		// Moves the memory address of the Ekey from the stack into the eax register for later use
			mov  edi, eax               // Stores the memory address of the Ekey in edi before it is edited
			movzx eax, byte ptr[eax]    // Used to move the Ekey address (which is converted to a byte) into the eax register so that it changes from 8 bits to 32 bits
			and eax, 0x3f               // Compares the binary version of the Ekey address with the value of 40 and returns all common bits in the eax register as a hexadecimal value
			rol al, 1                   // Shifts the binary value in al (the Ekey address and'd with 40) to the left by 1, essentially encrypting it
			mov byte ptr[edi], al       // Moves the newly encrypted version of the Ekey address to the area in memory where eax is stored
			cmp eax, 0x00               // Checks if the 8 bit shifted version of the Ekey's memory address is equal to 0
			jnz x8                      // Program jumps to the x8 function if the 8 bit shifted version of the Ekey's memory address is not equal to 0
			mov eax, 0x09               // If it is equal to 0, the encrypted value automatically gets set to 9
		x8:       mov edx, eax          // Moves the encrypted value to the edx register temporarily so that the value is saved
				  movzx  eax, [ebp + 12]		// Moves a single character from the word to be encrypted from the stack into the ecx register as a 32 bit value for later use
			  y8 : dec eax              // Reduces the value of the character to be encrypted by 1
				   dec edx              // Reduces the value of the encrypted memory address by 1
				   jnz y8               // If the value of the encrypted memory address is not equal to 0, it will loop and it will be decreased by 1 again, if it is equal to 0 it will continue
				   rol al, 1            // Shifts the binary values of the last 8 bits of the character to be encrypted to the left once, essesntially encrypting it
				   not al               // Reverses the binary values of the last 8 bits of the character to be encrypted, changing 1s to 0s and 0s to 1s
				   pop ebp              // Puts the original value of ebp back into the register after it was changed
				   ret                  // Returns the encyrpted character now stored in eax
	}

	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars(int length, const char EKey)
{
	char temp_char;
	for (int i = 0; i < length; ++i)
	{
		temp_char = EChars[i];			    // Get the next char from Original Chars array
		__asm {							    // Start of assembly code
			push   eax					    // Pushes eax onto the stack to save the original value for later
				push   ecx					// Pushes ecx to the stack to save its original value for later
				//
				push temp_char              // Pushes the current character to be encrypted onto the stack so that it can be passed through the encrypt8 call parameter
				lea eax, EKey				// Gets the memory address for the Ekey and stores it in the eax register
				push eax					// pushes the EKey on to the stack so that the value can be passed through the encrypt8 call parameter
				call   decrypt8				// Calls the encryption method
				add esp, 8                  // Adds 8 to the stack pointer to essentially remove the temp_char and EKey values from the stack
				or al, 0x20                 // Opposite of the &0xDF method, all lower case characters have an extra bit to the upper case countertypes which is 32 or in hexadecimal 20, or'ing this with the value adds the bit and makes it lower case
				mov    temp_char, al	    // Moves the newly encrypted character into the temp_char variable
				//
				pop    ecx					// Restores original value of ecx, overwriting any changes that occured to it
				pop    eax					// Restores original value of eax, overwriting any changes that occured to it
		}
		DChars[i] = temp_char;			    // Store encrypted char in the Encrypted Chars array

	}
	return;

	__asm{
	decrypt8:
		push ebp                        // Pushes the base pointer on to the stack to save the value before it's edited
			mov ebp, esp                // Copies the stack pointer value into the base pointer value so that it can be used without being edited and potentially cause errors
			mov    eax, [ebp + 8]		// Moves the memory address of the Ekey from the stack into the eax register for later use
			mov  edi, eax               // Stores the memory address of the Ekey in edi before it is edited
			movzx eax, byte ptr[eax]    // Used to move the Ekey address (which is converted to a byte) into the eax register so that it changes from 8 bits to 32 bits
			and eax, 0x3f				// Compares the binary version of the Ekey address with the value of 40 and returns all common bits in the eax register as a hexadecimal value
			rol al, 1					// Shifts the binary value in al (the Ekey address and'd with 40) to the left by 1, essentially encrypting it
			mov byte ptr[edi], al		// Moves the newly encrypted version of the Ekey address to the area in memory where eax is stored
			cmp eax, 0x00				//  Checks if the 8 bit shifted version of the Ekey's memory address is equal to 0
			jnz x8						// Program jumps to the x8 function if the 8 bit shifted version of the Ekey's memory address is not equal to 0
			mov eax, 0x09				// If it is equal to 0, the encrypted value automatically gets set to 9
		x8: mov edx, eax				// Moves the encrypted value to the edx register temporarily so that the value is saved
			movzx  eax, [ebp + 12]		// Moves a single character from the word to be encrypted from the stack into the ecx register as a 32 bit value for later use
			// Everything before this was the same as to obtain the same value from the encrypt for edx so it can be used like last time to find the encrypted value
			ror al, 1					// This rolls the last 8 bits of the character to be decrypted to the right, as opposed to the left which is done in the encryption, this is placed before the loop so that the value is obtained for the loop as a counter
			not al						// Reverses the last 8 bits of the character to be decrypted, this is done in the encrypt bit so it needs to be done in the decrypt bit to essentially reverse the encryption
		y8 : inc eax					// Character to be decrypted is incremented until the character to be decrypted hits 0 so that it can reach the value it is decremented from in the encrypt function
			 dec edx					// The encrypted memory address is decremented as it was in the encrypt function to find the exact amount that the eax register needs to be incremented by to find the original value
			 jnz y8						// Loops if the encyrpted memory address is not equal to 0
			 pop ebp					// Puts the original value of ebp back into the register after it was changed
			 ret						// Returns the encyrpted character now stored in eax
	}
}
// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------





int main(void)
{
	int  char_count;	// The number of actual characters entered (upto MAXCHARS limit).
	char EKey;			// Encryption key.

	cout << "\nPlease enter your Encryption Key (EKey) letter: "; get_char(EKey);

	cout << "\nNow enter upto " << MAXCHARS << " alphanumeric characters:\n";
	get_original_chars(char_count);
	cout << "\n\nOriginal source string = " << OChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";

	encrypt_chars(char_count, EKey);
	cout << "\n\nEncrypted string       = " << EChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(EChars[i])) & 0xFF) << "  ";

	decrypt_chars(char_count, EKey);
	cout << "\n\nDecrypted string       = " << DChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(DChars[i])) & 0xFF) << "  ";

	cout << "\n\nPress a key to end...";
	while (!_kbhit());					//hold the screen until a key is pressed
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------


