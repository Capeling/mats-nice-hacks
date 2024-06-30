#include "PlayLayer.hpp"
#include "../Labels.hpp"

using namespace matdash;

bool hasClicked = false;
float updateRgb = 0.0f;
Labels* labels = nullptr;

ccColor3B getChromaColour() {
	return ColorUtility::hsvToRgb(cchsv((updateRgb * 180) / 10.0f, 1.0f, 1.0f, true, true));
}

ccColor4F getChromaColour4() {
	auto color3b = ColorUtility::hsvToRgb(cchsv((updateRgb * 180) / 10.0f, 1.0f, 1.0f, true, true));
	return ccColor4F(color3b.r, color3b.g, color3b.b, 255);
}

void PlayLayer::spawnPlayer2() {
	this->player2()->setVisible(true);
	orig<&PlayLayer::spawnPlayer2>(this);
}

void PlayLayer::resetLevel() {
	labels->m_clickFrames.clear();
	labels->m_totalClicks = 0;
	orig<&PlayLayer::resetLevel>(this);
}

void PlayLayer::pushButton(int idk1, bool idk2) {
	labels->m_isHolding = true;
	if (!hasClicked) {
		labels->m_clickFrames.push_back(time::getTime());
		labels->m_totalClicks++;
		hasClicked = true;
	}
	orig<&PlayLayer::pushButton>(this, idk1, idk2);
}

void PlayLayer::releaseButton(int idk1, bool idk2) {
	labels->m_isHolding = false;
	orig<&PlayLayer::releaseButton>(this, idk1, idk2);
}

bool PlayLayer::init_(gd::GJGameLevel* level) {
	delete labels;
	labels = Labels::create();
	if (!orig<&PlayLayer::init_>(this, level)) return false;

	labels->m_labelMenu = CCMenu::create();
	labels->m_labelMenu->setZOrder(999999);

	for (size_t i = 0; i < 5; i++) {

		auto label = labels->createStandardLabel();

		labels->m_labelMenu->addChild(label);
		labels->m_labels.push_back(label);

		std::cout << labels->m_labels.size() << std::endl;
	}

	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	this->addChild(labels->m_labelMenu);

	labels->updateLabels(this);
	labels->updateLabelPositions();
	labels->updateBestRunLabel(this);
	labels->updateAttemptLabel(this);

	if (state().show_percent) {
		auto gm = gd::GameManager::sharedState();
		const auto bar = gm->getShowProgressBar();

		auto label = CCLabelBMFont::create("0.00%", "bigFont.fnt");
		label->setAnchorPoint({ bar ? 0.f : 0.5f, 0.5f });
		label->setScale(0.5f);
		label->setZOrder(999999);
		label->setPosition({ winSize.width / 2.f + (bar ? 107.2f : 0.f), winSize.height - 8.f });
		label->setTag(12345);
		this->addChild(label);
	}
	if (state().hide_attempts) {
		this->attemptsLabel()->setVisible(false);
	}

	return true;
}

void PlayLayer::updateAttempts() {
	orig<&PlayLayer::updateAttempts>(this);
	labels->updateAttemptLabel(this);
}

void PlayLayer::destroyPlayer(gd::PlayerObject* playerObject) {
	orig<&PlayLayer::destroyPlayer>(this, playerObject);
	labels->updateBestRunLabel(this);
}

cc::thiscall<void> PlayLayer::update_(float dt) {
	orig<&PlayLayer::update_>(this, dt);
	auto label = this->getChildByTag(12345);
	if (label) {
		const auto value = std::min(this->player1()->getPositionX() / this->levelLength(), 1.f) * 100.f;
		reinterpret_cast<CCLabelBMFont*>(label)->setString(CCString::createWithFormat("%.2f%%", value)->getCString());
	}

	labels->updateLabels(this);
	labels->updateLabelPositions();

	if (state().rainbow_color) {

		auto player1 = this->player1();
		auto player2 = this->player2();

		auto playerGlowNode = this->playerGlowNode();

		updateRgb += dt * state().rainbow_speed;

		for (size_t i = 0; i < player1->getChildrenCount(); i++) {
			auto node = static_cast<CCSprite*>(player1->getChildren()->objectAtIndex(i));
			node->setColor(getChromaColour());
		}

		for (size_t i = 0; i < player2->getChildrenCount(); i++) {
			auto node = static_cast<CCSprite*>(player2->getChildren()->objectAtIndex(i));
			for (size_t i = 0; i < node->getChildrenCount(); i++) {
				auto node2 = static_cast<CCSprite*>(node->getChildren()->objectAtIndex(i));
				node2->setColor(getChromaColour());
			}
		}

		if (auto player2Glow = static_cast<CCSprite*>(playerGlowNode->getChildren()->objectAtIndex(playerGlowNode->getChildrenCount()))) {
			for (size_t i = 0; i < player2Glow->getChildrenCount(); i++) {
				auto node = static_cast<CCSprite*>(player2Glow->getChildren()->objectAtIndex(i));
				node->setColor(getChromaColour());
			}
		};
	}

	// sick
	if (state().hide_player) {
		this->player1()->setVisible(false);
		this->player2()->setVisible(false);
	}

	if (state().hide_attempts) {
		this->attemptsLabel()->setVisible(false);
	}

	time_t currentTick = time::getTime();
	labels->m_clickFrames.erase(std::remove_if(labels->m_clickFrames.begin(), labels->m_clickFrames.end(), [currentTick](float tick) {
		return currentTick - tick > 1000;
		}), labels->m_clickFrames.end());

	hasClicked = false;
	return {};
}

void PlayLayer::initHooks() {
	add_hook<&PlayLayer::init_>(gd::base + 0xe35d0);
	add_hook<&PlayLayer::update_>(gd::base + 0xe9360);
	add_hook<&PlayLayer::updateAttempts>(gd::base + 0xf33a0);
	add_hook<&PlayLayer::resetLevel>(gd::base + 0xf1f20);
	add_hook<&PlayLayer::pushButton>(gd::base + 0xf0a00);
	add_hook<&PlayLayer::releaseButton>(gd::base + 0xf0af0);
	add_hook<&PlayLayer::destroyPlayer>(gd::base + 0xf04a0);
	add_hook<&PlayLayer::spawnPlayer2>(gd::base + 0xef0d0);
}