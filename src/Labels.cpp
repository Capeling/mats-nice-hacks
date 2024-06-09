#include "Labels.hpp"
#include <imgui-hook.hpp>
#include "matdash/console.hpp"

using namespace matdash;

void Labels::updateLabelPositions() {
	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	for (int i = 0; i < m_labelMenu->getChildrenCount(); i++) {
		auto node = static_cast<CCLabelBMFont*>(m_labelMenu->getChildren()->objectAtIndex(i));
		node->setOpacity(state().status_opacity);
		node->setPositionY(0 - (i * (17 * (state().status_scale * 2))));
		node->setScale(state().status_scale);
	}
	m_labelMenu->setPosition({ director->getScreenLeft() + (state().status_scale * 10), director->getScreenTop() - ((state().status_scale * 10) * 2) });
}

CCLabelBMFont* Labels::createStandardLabel() {
	auto label = CCLabelBMFont::create("Label", "bigFont.fnt");
	label->setAnchorPoint({ 0.f, 0.5f });
	label->setScale(0.5f);
	label->setOpacity(state().status_opacity);
	m_labelCount++;
	return label;
}

void Labels::createFpsLabel() {
	m_fpsLabel = createStandardLabel();
	m_fpsLabel->setZOrder(0);
	m_labelMenu->addChild(m_fpsLabel);
}

void Labels::createCpsLabel() {
	m_cpsLabel = createStandardLabel();
	m_cpsLabel->setZOrder(1);
	m_labelMenu->addChild(m_cpsLabel);
}

void Labels::createBestRunLabel() {
	m_bestRunLabel = createStandardLabel();
	m_bestRunLabel->setZOrder(2);
	m_labelMenu->addChild(m_bestRunLabel);
}

void Labels::createAttemptsLabel() {
	m_attemptsLabel = createStandardLabel();
	m_attemptsLabel->setZOrder(3);
	m_labelMenu->addChild(m_attemptsLabel);
}

void Labels::createMessageLabel() {
	m_messageLabel = createStandardLabel();
	m_messageLabel->setZOrder(-1);
	m_labelMenu->addChild(m_messageLabel);
}

void Labels::updateAttemptLabel(gd::PlayLayer* playLayer) {
	if (m_attemptsLabel) {
		int attemptCount = playLayer->attemptsCount();
		m_attemptsLabel->setString(CCString::createWithFormat("Attempt %i", attemptCount)->getCString());
	}
}

void Labels::updateBestRunLabel(gd::PlayLayer* playLayer) {
	int newBest = playLayer->getNewBest();
	if (m_bestRunLabel && newBest >= m_currentBest) {
		m_currentBest = newBest;
		m_bestRunLabel->setString(CCString::createWithFormat("Best run: %i%%", newBest)->getCString());
	}
}

void Labels::updateLabels(gd::PlayLayer* playLayer) {

	auto director = CCDirector::sharedDirector();

	if (m_labelMenu) {
		if (state().message && !m_messageLabel) {
			createMessageLabel();
		}

		if (state().fps_label && !m_fpsLabel) {
			createFpsLabel();
		}

		if (state().cps_label && !m_cpsLabel) {
			createCpsLabel();
		}

		if (state().best_run && !m_bestRunLabel) {
			createBestRunLabel();
			updateBestRunLabel(playLayer);
		}

		if (state().attempts_label && !m_attemptsLabel) {
			createAttemptsLabel();
			updateAttemptLabel(playLayer);
		}


		if (m_fpsLabel) {
			std::string prefix;
			if (state().fps_prefix)
				prefix = "fps: ";
			m_fpsLabel->setString((prefix + std::to_string(static_cast<int>(ImGui::GetIO().Framerate))).c_str());
			if (!state().fps_label) {
				m_fpsLabel->removeFromParent();
				m_fpsLabel = nullptr;
				m_labelCount--;
			}
		}

		if (m_messageLabel) {
			m_messageLabel->setString(state().message_text.c_str());
			if (!state().message) {
				m_messageLabel->removeFromParent();
				m_messageLabel = nullptr;
				m_labelCount--;
			}
		}


		if (m_cpsLabel) {
			std::string cpsCurrent = CCString::createWithFormat("%i", m_clickFrames.size())->getCString();
			std::string cpsTotal;
			if (state().cps_total)
				cpsTotal = CCString::createWithFormat("/%i", m_totalClicks)->getCString();
			std::string prefix;
			if (state().cps_prefix)
				prefix = "cps: ";

			m_cpsLabel->setString(std::string(prefix + cpsCurrent + cpsTotal).c_str());
			if (m_isHolding) {
				m_cpsLabel->setColor({ 0, 255, 0 });
			}
			else {
				m_cpsLabel->setColor({ 255, 255, 255 });
			}
			if (!state().cps_label) {
				m_cpsLabel->removeFromParent();
				m_cpsLabel = nullptr;
				m_labelCount--;
			}
		}

		if (!state().attempts_label && m_attemptsLabel) {
			m_attemptsLabel->removeFromParent();
			m_attemptsLabel = nullptr;
			m_labelCount--;
		}

		if (!state().best_run && m_bestRunLabel) {
			m_bestRunLabel->removeFromParent();
			m_bestRunLabel = nullptr;
			m_labelCount--;
		}
	}
}

Labels* Labels::create() {
	auto ret = new Labels;
	return ret;
}