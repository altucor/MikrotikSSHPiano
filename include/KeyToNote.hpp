#ifndef KEY_TO_NOTE_HPP
#define KEY_TO_NOTE_HPP

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

#endif // KEY_TO_NOTE_HPP
