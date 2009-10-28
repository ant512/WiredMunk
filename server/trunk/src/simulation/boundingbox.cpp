#include "boundingbox.h"

using namespace WiredMunk;

BoundingBox::BoundingBox() {
	_boundingBox.l = 0.0f;
	_boundingBox.b = 0.0f;
	_boundingBox.r = 0.0f;
	_boundingBox.t = 0.0f;
}

BoundingBox::BoundingBox(cpBB box) {
	_boundingBox = box;
}

BoundingBox::BoundingBox(const cpFloat left, const cpFloat bottom, const cpFloat right, const cpFloat top) {
	_boundingBox.l = left;
	_boundingBox.b = bottom;
	_boundingBox.r = right;
	_boundingBox.t = top;
}

BoundingBox::BoundingBox(const unsigned char* serialisedData) {
	deserialise(serialisedData);
}

BoundingBox::~BoundingBox() {
	
}

bool BoundingBox::intersects(const BoundingBox& box) const {
	return cpBBintersects(_boundingBox, box.getBoundingBox());
}

bool BoundingBox::containsBoundingBox(const BoundingBox& box) const {
	return cpBBcontainsBB(_boundingBox, box.getBoundingBox());
}

bool BoundingBox::containsVect(const cpVect& vector) const {
	return cpBBcontainsVect(_boundingBox, vector);
}

cpVect BoundingBox::clampVect(const cpVect& vector) const {
	return cpBBClampVect(_boundingBox, vector);
}

cpVect BoundingBox::wrapVect(const cpVect& vector) const {
	return cpBBWrapVect(_boundingBox, vector);
}

cpBB BoundingBox::getBoundingBox() const {
	return _boundingBox;
}

unsigned int BoundingBox::serialise(unsigned char* buffer) {
	buffer += SerialiseBase::serialise(_boundingBox.l, buffer);
	buffer += SerialiseBase::serialise(_boundingBox.b, buffer);
	buffer += SerialiseBase::serialise(_boundingBox.r, buffer);
	buffer += SerialiseBase::serialise(_boundingBox.t, buffer);
	
	return getSerialisedLength();
}

unsigned int BoundingBox::deserialise(const unsigned char* data) {
	// Extract data from serialised form
	_boundingBox.l = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	_boundingBox.b = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	_boundingBox.r = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	_boundingBox.t = SerialiseBase::deserialiseDouble(data);
	
	return getSerialisedLength();
}

unsigned int BoundingBox::getSerialisedLength() {
	return SERIALISED_DOUBLE_SIZE * 4;
}
