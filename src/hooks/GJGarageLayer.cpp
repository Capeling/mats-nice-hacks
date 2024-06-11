#include "GJGarageLayer.hpp"
#include "../Icons.hpp"

using namespace matdash;

Fields* fields = nullptr;

bool GJGarageLayer::init_() {
	delete fields;
	fields = new Fields;
	if (!orig<&GJGarageLayer::init_>(this))
		return false;


	return true;
}

void GJGarageLayer::setupIconSelect() {
	orig<&GJGarageLayer::setupIconSelect>(this);

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto GM = gd::GameManager::sharedState();

	auto itemMenu = cocos::getChildOfType<CCMenu>(this, 0);

	auto dartOff = CCSprite::createWithSpriteFrameName("gj_dartBtn_on_001.png");
	auto dartOn = CCSprite::createWithSpriteFrameName("gj_dartBtn_off_001.png");

	fields->m_dartBtn = gd::CCMenuItemToggler::create(dartOn, dartOff, this, menu_selector(gd::GJGarageLayer::onSelectTab));
	fields->m_dartBtn->setSizeMult(1.2);
	itemMenu->addChild(fields->m_dartBtn);
	fields->m_dartBtn->setTag(4);

	itemMenu->alignItemsHorizontallyWithPadding(0.0);

	fields->m_dartPage = CCMenu::create();

	auto specialNewX = fields->m_dartBtn->getPositionX() + 3.2;
	fields->m_dartBtn->setPositionX(getSpecialBtn()->getPositionX());
	getSpecialBtn()->setPositionX(specialNewX);

	fields->m_dartSelector = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
	fields->m_dartSelector->setScale(0.85);

	//fields->m_pagesArray = CCArray::create();
	int count = Icons::getCount("dart", "001");
	int totalWaves = 0;
	float pages = (ceilf(count / 36) + 1);
	getGaragePageArray()->insertObject(fields->m_dartPage, 4);
	for (int a = 0; a < pages; a++) {
		auto dartMenu = CCMenu::create();
		float rows = -1;
		for (int i = 0; i < 36; i++) {

			//if (i > 35) break;
			if (totalWaves > count - 1) break;

			std::cout << totalWaves << " " << i << " " << count << " " << a << std::endl;

			auto base = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("dart_%02d_001.png", i + 1)->getCString());
			auto extra = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("dart_%02d_2_001.png", i + 1)->getCString());
			base->addChild(extra, -1);
			base->setScale(0.8);
			base->setColor(GM->colorForIdx(17));

			base->setPosition(base->getContentSize() / 2);
			extra->setPosition(base->getContentSize() / 2);
			extra->setColor(GM->colorForIdx(12));


			float posLimit = i % 12;
			if (posLimit == 0)
				rows++;

			//player->updatePlayerFrame(i + 1, gd::IconType::Wave);
			auto btn = gd::CCMenuItemSpriteExtra::create(base, nullptr, this, menu_selector(GJGarageLayer::onDartIcon));
			totalWaves++;
			btn->setPosition({ -165 + (posLimit * 30), -6 - (rows * 30) }); //guh
			if (i + 1 == state().selected_dart) fields->m_dartSelector->setPosition(btn->getPosition());

			btn->setTag(totalWaves);
			dartMenu->addChild(btn);
		}
		std::cout << "page: " << a << std::endl;
		//dartMenu->setPositionX(dartMenu->getContentSize().width * (a));
		fields->m_dartPage->addChild(dartMenu);
		dartMenu->setPosition({ 0, 0 });
		//fields->m_pagesArray->addObject(dartMenu);
	}
	fields->m_totalWavePages = pages;
	fields->m_dartPage->addChild(fields->m_dartSelector);

	if (pages > 1) {
		fields->m_arrowMenu = CCMenu::create();

		auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		nextSpr->setFlipX(true);
		nextSpr->setScale(0.8);

		auto nextBtn = gd::CCMenuItemSpriteExtra::create(nextSpr, nullptr, this, menu_selector(GJGarageLayer::onNext));
		nextBtn->setPosition({ 220, -36 });

		auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		prevSpr->setScale(0.8);

		auto prevBtn = gd::CCMenuItemSpriteExtra::create(prevSpr, nullptr, this, menu_selector(GJGarageLayer::onPrev));
		prevBtn->setPosition({ -220, -36 });

		fields->m_arrowMenu->addChild(nextBtn);
		fields->m_arrowMenu->addChild(prevBtn);
		fields->m_arrowMenu->setPosition({ 0, 0 });

		fields->m_dartPage->addChild(fields->m_arrowMenu);
	}

	itemMenu->setPosition(winSize.width * 0.5 - 115, itemMenu->getPositionY());
	addChild(fields->m_dartPage);
	//fields->m_dartMenu->alignItemsHorizontallyWithPadding(10.f);

	//auto dartMenu = gd::GaragePage::create(gd::IconType::Ball, this, menu_selector(GJGarageLayer::onDartIcon));

	selectPage(0);
	switchToPage((ceilf(state().selected_dart / 36)));
}

void GJGarageLayer::onDartIcon(CCObject* sender) {
	auto newSender = static_cast<CCNode*>(sender);
	state().selected_dart = newSender->getTag();
	Icons::patchDart(Icons::getCount("dart", "001"), newSender->getTag());
	getPlayerIcon()->updatePlayerFrame(newSender->getTag(), gd::IconType::Wave);
	gd::GameManager::sharedState()->setPlayerType(gd::IconType::Wave);
	fields->m_dartSelector->setPosition(newSender->getPosition());
}

void GJGarageLayer::onNext(CCObject* sender) {
	fields->m_wavePage++;
	if (fields->m_wavePage > fields->m_totalWavePages - 1) fields->m_wavePage = 0;
	//std::cout << fields->m_wavePage << " " << fields->m_totalWavePages << std::endl;
	switchToPage(fields->m_wavePage);
}

void GJGarageLayer::onPrev(CCObject* sender) {
	fields->m_wavePage--;
	if (fields->m_wavePage < 0) fields->m_wavePage = fields->m_totalWavePages - 1;
	//std::cout << fields->m_wavePage << " " << fields->m_totalWavePages << std::endl;
	switchToPage(fields->m_wavePage);
}

void GJGarageLayer::switchToPage(int page) {
	std::cout << page << " " << fields->m_totalWavePages - 1 << std::endl;
	for (int i = 0; i < fields->m_totalWavePages; i++) {
		auto node = static_cast<CCNode*>(fields->m_dartPage->getChildren()->objectAtIndex(i));
		if (i == page) node->setVisible(true);
		else node->setVisible(false);
	}
	float selectedWavePage = (ceilf(state().selected_dart / 36));
	if (selectedWavePage == page) fields->m_dartSelector->setVisible(true);
	else fields->m_dartSelector->setVisible(false);
}

void GJGarageLayer::selectPage(int iconType) {
	int page = 0;
	//gd::FLAlertLayer::create(nullptr, "test", std::to_string(iconType).c_str(), "ok", nullptr, 100, false, 100)->show();
	switch (iconType) {
		case 99: page = 5; break;
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
	fields->m_dartBtn->toggle(iconType == 4);
	getSpecialBtn()->toggle(iconType == 99);

	getCubeBtn()->setEnabled(iconType != 0);
	getShipBtn()->setEnabled(iconType != 1);
	getBallBtn()->setEnabled(iconType != 2);
	getBirdBtn()->setEnabled(iconType != 3);
	fields->m_dartBtn->setEnabled(iconType != 4);
	getSpecialBtn()->setEnabled(iconType != 99);
}

void GJGarageLayer::initHooks() {
	add_hook<&GJGarageLayer::init_>(gd::base + 0x7c5c0);
	add_hook<&GJGarageLayer::setupIconSelect>(gd::base + 0x7d790);
	add_hook<&GJGarageLayer::selectPage>(gd::base + 0x7de70);
}