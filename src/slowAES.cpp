/**
 * @file slowaes.cpp
 * @author Hakan ÖZMEN (hakkanr@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "slowAES.h"
slowAES::slowAES()
{
}
void slowAES::decrypt(uint8_t *cipherin, uint8_t *key, uint8_t *iv, uint8_t *bytesOut)
{
	// return cipherin;
	uint8_t output[16];
	decrypt(cipherin, key, output);
	for (int i = 0; i < 16; i++)
		bytesOut[i] = (iv[i] ^ output[i]);
}
void slowAES::decrypt(uint8_t *input, uint8_t *key, uint8_t *output)
{
	uint8_t size = 16;

	uint8_t block[16];
	for (int i = 0; i < 16; i++)
	{
		output[i] = 0;
		block[i] = 0;
	}
	uint8_t nbrRounds = numberOfRounds(size);
	for (uint8_t i = 0; i < 4; i++)		/* iterate over the columns */
		for (uint8_t j = 0; j < 4; j++) /* iterate over the rows */
			block[(i + (j * 4))] = input[(i * 4) + j];
	/* expand the key into an 176, 208, 240 bytes key */
	uint8_t *expandedKey;
	uint8_t expandedKeySize = (16 * (numberOfRounds(size) + 1));
	expandedKey = new uint8_t[expandedKeySize];
	expandKey(key, size, expandedKey);
	/* decrypt the block using the expandedKey */
	invMain(block, expandedKey, nbrRounds);
	for (uint8_t k = 0; k < 4; k++)		/* unmap the block again into the output */
		for (uint8_t l = 0; l < 4; l++) /* iterate over the rows */
			output[(k * 4) + l] = block[(k + (l * 4))];
	delete[] expandedKey;
}
void slowAES::invMain(uint8_t *state, uint8_t *expandedKey, uint8_t nbrRounds)
{
	uint8_t *roundKey = new uint8_t[16];
	createRoundKey(expandedKey, 16 * nbrRounds, roundKey);
	addRoundKey(state, roundKey);

	for (int i = nbrRounds - 1; i > 0; i--)
	{
		createRoundKey(expandedKey, 16 * i, roundKey);
		invRound(state, roundKey);
	}
	shiftRows(state, true);
	subBytes(state, true);
	createRoundKey(expandedKey, 0, roundKey);
	addRoundKey(state, roundKey);
	delete[] roundKey;
}
void slowAES::invRound(uint8_t *state, uint8_t *roundKey)
{
	shiftRows(state, true);
	subBytes(state, true);
	addRoundKey(state, roundKey);
	mixColumns(state, true);
}
void slowAES::mixColumns(uint8_t *state, bool isInv)
{
	uint8_t column[4];
	for (int i = 0; i < 4; i++)
		column[i] = 0;
	/* iterate over the 4 columns */
	for (uint8_t i = 0; i < 4; i++)
	{
		/* construct one column by iterating over the 4 rows */
		for (uint8_t j = 0; j < 4; j++)
			column[j] = state[(j * 4) + i];
		/* apply the mixColumn on one column */
		mixColumn(column, isInv);
		/* put the values back into the state */
		for (uint8_t k = 0; k < 4; k++)
			state[(k * 4) + i] = column[k];
	}
}
void slowAES::mixColumn(uint8_t *column, bool isInv)
{
	uint8_t *mult = new uint8_t[4];
	uint8_t a[] = {14, 9, 13, 11};
	uint8_t b[] = {2, 1, 1, 3};
	if (isInv)
		mult = a;
	else
		mult = b;
	uint8_t cpy[4];
	for (uint8_t i = 0; i < 4; i++)
		cpy[i] = column[i];

	column[0] = galois_multiplication(cpy[0], mult[0]) ^
				galois_multiplication(cpy[3], mult[1]) ^
				galois_multiplication(cpy[2], mult[2]) ^
				galois_multiplication(cpy[1], mult[3]);
	column[1] = galois_multiplication(cpy[1], mult[0]) ^
				galois_multiplication(cpy[0], mult[1]) ^
				galois_multiplication(cpy[3], mult[2]) ^
				galois_multiplication(cpy[2], mult[3]);
	column[2] = galois_multiplication(cpy[2], mult[0]) ^
				galois_multiplication(cpy[1], mult[1]) ^
				galois_multiplication(cpy[0], mult[2]) ^
				galois_multiplication(cpy[3], mult[3]);
	column[3] = galois_multiplication(cpy[3], mult[0]) ^
				galois_multiplication(cpy[2], mult[1]) ^
				galois_multiplication(cpy[1], mult[2]) ^
				galois_multiplication(cpy[0], mult[3]);
}
uint8_t slowAES::galois_multiplication(uint16_t a, uint16_t b)
{
	uint16_t p = 0;
	for (int counter = 0; counter < 8; counter++)
	{
		if ((b & 1) == 1)
			p ^= a;
		if (p > 0x100)
			p ^= 0x100;
		auto hi_bit_set = (a & 0x80); // keep p 8 bit
		a <<= 1;
		if (a > 0x100)
			a ^= 0x100; // keep a 8 bit
		if (hi_bit_set == 0x80)
			a ^= 0x1b;
		if (a > 0x100)
			a ^= 0x100; // keep a 8 bit
		b >>= 1;
		if (b > 0x100)
			b ^= 0x100; // keep b 8 bit
	}
	return (uint8_t)p;
}
void slowAES::subBytes(uint8_t *state, bool isInv)
{
	for (int i = 0; i < 16; i++)
		state[i] = isInv ? rsbox[state[i]] : sbox[state[i]];
}
void slowAES::shiftRows(uint8_t *state, bool isInv)
{
	for (int i = 0; i < 4; i++)
		shiftRow(state, i * 4, i, isInv);
}
void slowAES::shiftRow(uint8_t *state, uint8_t statePointer, uint8_t nbr, bool isInv)
{
	for (uint8_t i = 0; i < nbr; i++)
	{
		if (isInv)
		{
			uint8_t tmp = state[statePointer + 3];
			for (int8_t j = 3; j > 0; j--)
				state[statePointer + j] = state[statePointer + j - 1];
			state[statePointer] = tmp;
		}
		else
		{
			uint8_t tmp = state[statePointer];
			for (uint8_t j = 0; j < 3; j++)
				state[statePointer + j] = state[statePointer + j + 1];
			state[statePointer + 3] = tmp;
		}
	}
}
void slowAES::createRoundKey(uint8_t *expandedKey, uint8_t roundKeyPointer, uint8_t *roundKey)
{
	for (int i = 0; i < 16; i++)
		roundKey[i] = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			roundKey[j * 4 + i] = expandedKey[roundKeyPointer + i * 4 + j];
}
void slowAES::addRoundKey(uint8_t *state, uint8_t *roundKey)
{
	for (uint8_t i = 0; i < 16; i++)
		state[i] ^= roundKey[i];
}
uint8_t slowAES::numberOfRounds(uint8_t size)
{
	uint8_t nbrRounds = 0;
	switch (size)
	{
	case (SIZE_128):
		nbrRounds = 10;
		break;
	case (SIZE_192):
		nbrRounds = 12;
		break;
	case (SIZE_256):
		nbrRounds = 14;
		break;
	}
	return nbrRounds;
}
void slowAES::expandKey(uint8_t *key, uint8_t size, uint8_t *expandedKey)
{
	uint8_t expandedKeySize = (16 * (numberOfRounds(size) + 1));
	uint8_t currentSize = 0;
	uint8_t rconIteration = 1;
	uint8_t t[4] = {0, 0, 0, 0};
	for (uint8_t i = 0; i < expandedKeySize; i++)
		expandedKey[i] = 0;
	/* set the 16,24,32 bytes of the expanded key to the input key */
	for (uint8_t j = 0; j < size; j++)
		expandedKey[j] = key[j];
	currentSize += size;
	while (currentSize < expandedKeySize)
	{
		/* assign the previous 4 bytes to the temporary value t */
		for (uint8_t k = 0; k < 4; k++)
			t[k] = expandedKey[(currentSize - 4) + k];

		/* every 16,24,32 bytes we apply the core schedule to t
		 * and increment rconIteration afterwards
		 */
		if (currentSize % size == 0)
			core(t, rconIteration++);

		/* For 256-bit keys, we add an extra sbox to the calculation */
		if (size == SIZE_256 && ((currentSize % size) == 16))
			for (uint8_t l = 0; l < 4; l++)
				t[l] = sbox[t[l]];

		/* We XOR t with the four-byte block 16,24,32 bytes before the new expanded key.
		 * This becomes the next four bytes in the expanded key.
		 */
		for (uint8_t m = 0; m < 4; m++)
		{
			expandedKey[currentSize] = (expandedKey[currentSize - size] ^ t[m]);
			currentSize++;
		}
	}
}
void slowAES::rotate(uint8_t *word)
{
	uint8_t c = word[0];
	for (uint8_t i = 0; i < 3; i++)
		word[i] = word[i + 1];
	word[3] = c;
}
void slowAES::core(uint8_t *word, uint16_t iteration)
{
	rotate(word);
	for (uint8_t i = 0; i < 4; ++i)
		word[i] = sbox[word[i]];
	/* XOR the output of the rcon operation with i to the first part (leftmost) only */
	word[0] = word[0] ^ Rcon[iteration];
}
void toNumbers(const char *hexStr, uint8_t *arrayPtr)
{
	for (uint8_t i = 0; i < 16; i++)
	{
		uint8_t ch1 = hexStr[i * 2];
		if (ch1 < 58)
			ch1 = ch1 - 48;
		else
			ch1 = ch1 - 87;
		uint8_t ch2 = hexStr[i * 2 + 1];
		if (ch2 < 58)
			ch2 = ch2 - 48;
		else
			ch2 = ch2 - 87;
		arrayPtr[i] = ch1 * 16 + ch2 * 1;
	}
}
void toHex(uint8_t *arrayPtr, uint8_t *result)
{
	for (uint8_t i = 0; i < 16; i++)
	{
		uint8_t ch2 = arrayPtr[i] % 16;
		uint8_t ch1 = (uint8_t)((arrayPtr[i] - ch2) / 16);
		if (ch2 < 10)
			result[i * 2 + 1] = (char)ch2 + 48;
		else
			result[i * 2 + 1] = (char)ch2 + 87;
		if (ch1 < 10)
			result[i * 2] = (char)ch1 + 48;
		else
			result[i * 2] = (char)ch1 + 87;
	}
	result[32] = '\0';
}