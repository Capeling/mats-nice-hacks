#include "Labels.hpp"
#include <imgui-hook.hpp>
#include "matdash/console.hpp"

using namespace matdash;

void Labels::updateLabelPositions() {
	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	int i = 0;

	for (auto label : m_labels) {
		if (label->isVisible()) {
			label->setOpacity(state().status_opacity);
			label->setPositionY(0 - (i * (17 * (state().status_scale * 2))));
			label->setScale(state().status_scale);
			i++;
		}
	}
	m_labelMenu->setPosition({ director->getScreenLeft() + (state().status_scale * 10), director->getScreenTop() - ((state().status_scale * 10) * 2) });
}

CCLabelBMFont* Labels::createStandardLabel() {
	auto label = CCLabelBMFont::create("Label", "bigFont.fnt");
	label->setAnchorPoint({ 0.f, 0.5f });
	label->setScale(0.5f);
	label->setOpacity(state().status_opacity);
	//label->retain();
	return label;
}

void Labels::createFpsLabel() {
	auto fpsLabel = createStandardLabel();
	fpsLabel->setZOrder(0);
	m_labelMenu->addChild(fpsLabel);
}

void Labels::createCpsLabel() {
	auto cpsLabel = createStandardLabel();
	cpsLabel->setZOrder(1);
	m_labelMenu->addChild(cpsLabel);
}

void Labels::createBestRunLabel() {
	auto bestRunLabel = createStandardLabel();
	bestRunLabel->setZOrder(2);
	m_labelMenu->addChild(bestRunLabel);
}

void Labels::createAttemptsLabel() {
	auto attemptsLabel = createStandardLabel();
	attemptsLabel->setZOrder(3);
	m_labelMenu->addChild(attemptsLabel);
}

void Labels::createMessageLabel() {
	auto messageLabel = createStandardLabel();
	messageLabel->setZOrder(-1);
	m_labelMenu->addChild(messageLabel);
}

void Labels::updateAttemptLabel(gd::PlayLayer* playLayer) {
	if (state().attempts_label) {
		int attemptCount = playLayer->attemptsCount();
		attemptCount = std::clamp(attemptCount, 1, INT_MAX);
		m_labels[3]->setString(CCString::createWithFormat("Attempt %i", attemptCount)->getCString());
	}
}

void Labels::updateBestRunLabel(gd::PlayLayer* playLayer) {
	int newBest = playLayer->getNewBest();
	if (state().best_run && newBest >= m_currentBest) {
		m_currentBest = newBest;
		m_labels[4]->setString(CCString::createWithFormat("Best run: %i%%", newBest)->getCString());
	}
}

void Labels::updateLabels(gd::PlayLayer* playLayer) {

	auto director = CCDirector::sharedDirector();

	if (state().message) {
		m_labels[0]->setString(state().message_text.c_str());
	}

	if (state().fps_label) {
		std::string prefix;
		if (state().fps_prefix)
			prefix = "fps: ";
		m_labels[1]->setString((prefix + std::to_string(static_cast<int>(ImGui::GetIO().Framerate))).c_str());
	}

	if (state().cps_label) {
		std::string cpsCurrent = CCString::createWithFormat("%i", m_clickFrames.size())->getCString();
		std::string cpsTotal;
		if (state().cps_total)
			cpsTotal = CCString::createWithFormat("/%i", m_totalClicks)->getCString();
		std::string prefix;
		if (state().cps_prefix)
			prefix = "cps: ";

		m_labels[2]->setString(std::string(prefix + cpsCurrent + cpsTotal).c_str());
		if (m_isHolding) {
			m_labels[2]->setColor({ 0, 255, 0 });
		}
		else {
			m_labels[2]->setColor({ 255, 255, 255 });
		}
	}

	m_labels[0]->setVisible(state().message);
	m_labels[1]->setVisible(state().fps_label);
	m_labels[2]->setVisible(state().cps_label);
	m_labels[3]->setVisible(state().attempts_label);
	m_labels[4]->setVisible(state().best_run);
}

Labels* Labels::create() {
	auto ret = new Labels;
	return ret;
}