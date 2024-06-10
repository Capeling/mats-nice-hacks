#include "GJGarageLayer.hpp"

using namespace matdash;

gd::CCMenuItemToggler* m_dartBtn = nullptr;

bool GJGarageLayer::init_() {
	if (!orig<&GJGarageLayer::init_>(this))
		return false;

	return true;
}

void GJGarageLayer::setupIconSelect() {
	delete m_dartBtn;
	orig<&GJGarageLayer::setupIconSelect>(this);

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto itemMenu = cocos::getChildOfType<CCMenu>(this, 0);

	auto dartOff = CCSprite::createWithSpriteFrameName("gj_dartBtn_on_001.png");
	auto dartOn = CCSprite::createWithSpriteFrameName("gj_dartBtn_off_001.png");

	m_dartBtn = gd::CCMenuItemToggler::create(dartOn, dartOff, this, menu_selector(gd::GJGarageLayer::onSelectTab));
	m_dartBtn->setSizeMult(1.2);
	itemMenu->addChild(m_dartBtn);
	m_dartBtn->setTag(4);

	itemMenu->alignItemsHorizontallyWithPadding(0.0);

	auto specialNewX = m_dartBtn->getPositionX() + 3.2;
	m_dartBtn->setPositionX(getSpecialBtn()->getPositionX());
	getSpecialBtn()->setPositionX(specialNewX);

	itemMenu->setPositionX(winSize.width * 0.5 - 115);

	selectPage(0);
}

void GJGarageLayer::selectPage(int iconType) {
	int page = 0;
	//gd::FLAlertLayer::create(nullptr, "test", std::to_string(iconType).c_str(), "ok", nullptr, 100, false, 100)->show();
	switch (iconType) {
		case 99: page = 4; break;
		default: page = iconType; break;
	}
	for (int i = 0; i < getGaragePageArray()->count(); i++) {
		auto node = static_cast<CCNode*>(getGaragePageArray()->objectAtIndex(i));
		if (i == page) node->setVisible(true);
		else node->setVisible(false);
	}
	getCubeBtn()->toggle(iconType == 0);
	getShipBtn()->toggle(iconType == 1);
	getBallBtn()->toggle(iconType == 2);
	getBirdBtn()->toggle(iconType == 3);
	m_dartBtn->toggle(iconType == 4);
	getSpecialBtn()->toggle(iconType == 99);

	getCubeBtn()->setEnabled(iconType != 0);
	getShipBtn()->setEnabled(iconType != 1);
	getBallBtn()->setEnabled(iconType != 2);
	getBirdBtn()->setEnabled(iconType != 3);
	m_dartBtn->setEnabled(iconType != 4);
	getSpecialBtn()->setEnabled(iconType != 99);
}

void GJGarageLayer::initHooks() {
	//add_hook<&GJGarageLayer::init_>(gd::base + 0x7c5c0);
	//add_hook<&GJGarageLayer::setupIconSelect>(gd::base + 0x7d790);
	//add_hook<&GJGarageLayer::selectPage>(gd::base + 0x7de70);
}