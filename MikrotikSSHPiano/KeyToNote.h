#pragma once
class KeyToNote
{
public:
	explicit KeyToNote();
	~KeyToNote();
	float keyToFreq(unsigned char key);
	void octaveUp();
	void octaveDown();
	int getCurOctave() { return m_octave; };

private:
	int m_octave = 5;
};

