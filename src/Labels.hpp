#include <cocos2d.h>
#include "gd.hpp"
#include "utils.hpp"
#include "state.hpp"
#include "menu.hpp"
#include "state.hpp"
#include "matdash.hpp"

class Labels {
public:
	//int m_labelCount = 0;
	CCMenu* m_labelMenu = nullptr;
	std::vector<CCLabelBMFont*> m_labels = {};
	//CCLabelBMFont* m_fpsLabel = nullptr;
	//CCLabelBMFont* m_attemptsLabel = nullptr;
	//CCLabelBMFont* m_cpsLabel = nullptr;
	//CCLabelBMFont* m_bestRunLabel = nullptr;
	//CCLabelBMFont* m_messageLabel = nullptr;
	std::vector<time_t> m_clickFrames;
	int m_totalClicks = 0;
	bool m_isHolding = false;
	int m_currentBest = 0;
public:
	void updateLabelPositions();
	CCLabelBMFont* createStandardLabel();
	inline void createFpsLabel();
	inline void createCpsLabel();
	inline void createBestRunLabel();
	inline void createAttemptsLabel();
	inline void createMessageLabel();
	void updateAttemptLabel(gd::PlayLayer* playLayer);
	void updateBestRunLabel(gd::PlayLayer* playLayer);
	void updateLabels(gd::PlayLayer* playLayer);
	static Labels* create();
};