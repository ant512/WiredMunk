#include "serialisebase.h"

using namespace WiredMunk;

unsigned int SerialiseBase::serialise(unsigned short value, unsigned char* output) {
	*output = (char)(value >> 8);
	*(output + 1) = (char)value;
	
	return SERIALISED_SHORT_SIZE;
}

unsigned short SerialiseBase::deserialiseShort(const unsigned char* data) {
	
	unsigned short output;
	
	output = *data << 8;
	output |= *(data + 1);
	
	return output;
}

unsigned int SerialiseBase::serialise(unsigned int value, unsigned char* output) {
	*output = (char)(value >> 24);
	*(output + 1) = (char)(value >> 16);
	*(output + 2) = (char)(value >> 8);
	*(output + 3) = (char)value;
	
	return SERIALISED_INT_SIZE;
}

unsigned int SerialiseBase::deserialiseInt(const unsigned char* data) {
	
	unsigned int output;
	
	output = *data << 24;
	output |= *(data + 1) << 16;
	output |= *(data + 2) << 8;
	output |= *(data + 3);
	
	return output;
}

unsigned int SerialiseBase::serialise(bool value, unsigned char* output) {
	*output = (bool)value;
	
	return SERIALISED_BOOL_SIZE;
}

bool SerialiseBase::deserialiseBool(const unsigned char* data) {
	bool output;
	output = *data;
	return output;
}

unsigned int SerialiseBase::serialise(double value, unsigned char* output) {
	
	// Get a packed version of the double
	unsigned long long int packed = pack754(value, 64, 11);
	
	*output = (char)(packed >> 56);
	*(output + 1) = (char)(packed >> 48);
	*(output + 2) = (char)(packed >> 40);
	*(output + 3) = (char)(packed >> 32);
	*(output + 4) = (char)(packed >> 24);
	*(output + 5) = (char)(packed >> 16);
	*(output + 6) = (char)(packed >> 8);
	*(output + 7) = (char)packed;
	
	return SERIALISED_DOUBLE_SIZE;
}

double SerialiseBase::deserialiseDouble(const unsigned char* data) {
	
	// Restore the serialised double to a packed 64-bit int
	unsigned long long int packed;
	
	packed = ((unsigned long long int)((*data) & 0xFF)) << 56;
	packed |= ((unsigned long long int)(*(data + 1) & 0xFF)) << 48;
	packed |= ((unsigned long long int)(*(data + 2) & 0xFF)) << 40;
	packed |= ((unsigned long long int)(*(data + 3) & 0xFF)) << 32;
	packed |= ((unsigned long long int)(*(data + 4) & 0xFF)) << 24;
	packed |= ((unsigned long long int)(*(data + 5) & 0xFF)) << 16;
	packed |= ((unsigned long long int)(*(data + 6) & 0xFF)) << 8;
	packed |= ((unsigned long long int)(*(data + 7) & 0xFF));
	
	// Get an unpacked version of the double
	double unpacked = unpack754(packed, 64, 11);
	
	return unpacked;
}

unsigned int SerialiseBase::serialise(float value, unsigned char* output) {
	
	// Get a packed version of the float
	unsigned int packed = pack754(value, 32, 8);
	
	*output = (char)(packed >> 24);
	*(output + 1) = (char)(packed >> 16);
	*(output + 2) = (char)(packed >> 8);
	*(output + 3) = (char)packed;
	
	return SERIALISED_FLOAT_SIZE;
}

float SerialiseBase::deserialiseFloat(const unsigned char* data) {
	
	// Restore the serialised float to a packed int
	unsigned int packed;
	
	packed = ((unsigned int)((*data) & 0xFF)) << 24;
	packed |= ((unsigned int)(*(data + 1) & 0xFF)) << 16;
	packed |= ((unsigned int)(*(data + 2) & 0xFF)) << 8;
	packed |= ((unsigned int)(*(data + 3) & 0xFF));
	
	// Get an unpacked version of the float
	float unpacked = unpack754(packed, 32, 8);
	
	return unpacked;
}

long long SerialiseBase::pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	
    if (f == 0.0) return 0; // get this special case out of the way
	
	if (f == INFINITY) return INFINITY;	// Special case for infinity
	
    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }
	
    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;
	
    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);
	
    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias
	
    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double SerialiseBase::unpack754(long long i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	
    if (i == 0) return 0.0;
	
	if (i == INFINITY) return INFINITY;	// Special case for infinity
	
    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on
	
    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }
	
    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;
	
    return result;
}

unsigned int SerialiseBase::serialise(const cpVect &vector, unsigned char* output) {
	
	unsigned char* oldBuffer = output;
	
	output += serialise(vector.x, output);
	output += serialise(vector.y, output);
	
	return output - oldBuffer;
}

cpVect SerialiseBase::deserialiseVector(const unsigned char* data) {
	cpVect output;
	
	double x = deserialiseDouble(data);
	double y = deserialiseDouble(data + 8);
	
	output.x = x;
	output.y = y;
	
	return output;
}
