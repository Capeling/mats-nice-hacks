#include "UILayer.hpp"
#include <matdash.hpp>

using namespace matdash;

bool UILayer::_init() {
	if(!orig<&UILayer::_init>(this))
		return false;

	return true;
}