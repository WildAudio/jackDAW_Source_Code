/*
==============================================================================

EffectsPanel.h
Created: 15 Feb 2018 8:35:25pm
Author:  User

==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <sstream>

class EffectsPanel : public Component, public ChangeBroadcaster, public Slider::Listener, public Timer
{
public:
	EffectsPanel()
		:arrayPositionToUse(0),
		channelBeingEffected(0)
	{
		setSize(300, 400);
		//Allocate memory for all of the filter rotaries...
		for (int i = 0; i <= 7; i++) {
			filterSettings[i] = new Slider;
		}
		//...and the reverb rotaries
		for (int i = 0; i <= 3; i++) {
			reverbSettings[i] = new Slider;
		}
		/*Make visible and set the values/ranges for all of the filter rotaries, as
		well as text box outputs to display the settings*/
		for (int i = 0; i <= 7; i++) {
			addAndMakeVisible(filterSettings[i]);
			addAndMakeVisible(eqSettingDescriptions[i]);
			filterSettings[i]->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
			filterSettings[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
			if (i == 0 || i == 1 || i == 5 || i == 7) {
				filterSettings[i]->setRange(20.0, 20000.0);
			}
			else {
				filterSettings[i]->setRange(0.1, 10.0);
			}
			if (i == 0) {
				filterSettings[i]->setValue(20000.0);
			}
			if (i == 1) {
				filterSettings[i]->setValue(20.0);
			}
			if (i == 2) {
				filterSettings[i]->setValue(1.0);
			}
			if (i == 3) {
				filterSettings[i]->setValue(1.0);
			}
			if (i == 4) {
				filterSettings[i]->setValue(1.0);
			}
			if (i == 5) {
				filterSettings[i]->setValue(400.0);
			}
			if (i == 6) {
				filterSettings[i]->setValue(1.0);
			}
			if (i == 7) {
				filterSettings[i]->setValue(1000.0);
			}
			filterSettings[i]->addListener(this);
		}

		//Set the description boxes for each of the filter rotaries
		eqSettingDescriptions[0].setText("Low Pass", dontSendNotification);
		eqSettingDescriptions[1].setText("High Pass", dontSendNotification);
		eqSettingDescriptions[2].setText("Low Shelf", dontSendNotification);
		eqSettingDescriptions[3].setText("High Shelf", dontSendNotification);
		eqSettingDescriptions[4].setText("Middle 1 Gain", dontSendNotification);
		eqSettingDescriptions[5].setText("Middle 1 Freq", dontSendNotification);
		eqSettingDescriptions[6].setText("Middle 2 Gain", dontSendNotification);
		eqSettingDescriptions[7].setText("Middle 2 Freq", dontSendNotification);

		/*Make visible and set the values/ranges for all of the reverb rotaries, as
		well as text box outputs to display the settings*/
		for (int i = 0; i <= 3; i++) {
			addAndMakeVisible(reverbSettings[i]);
			addAndMakeVisible(reverbSettingDescriptions[i]);
			reverbSettings[i]->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
			reverbSettings[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
			reverbSettings[i]->setRange(0.0, 1.0);
			if (i == 0) {
				reverbSettings[i]->setValue(0.0);
			}
			else {
				reverbSettings[i]->setValue(0.5);
			}
			reverbSettings[i]->addListener(this);
		}

		//Set the description boxes for all of the reverb rotaries
		reverbSettingDescriptions[0].setText("Reverb Level", dontSendNotification);
		reverbSettingDescriptions[1].setText("Room Size", dontSendNotification);
		reverbSettingDescriptions[2].setText("Damping", dontSendNotification);
		reverbSettingDescriptions[3].setText("Width", dontSendNotification);

		addAndMakeVisible(displayChannel);
		displayChannel.setColour(Label::backgroundColourId, Colours::black);
		displayChannel.setColour(Label::textColourId, Colours::white);
		ss << channelBeingEffected;
		displayChannel.setText(ss.str(), sendNotification);

		startTimer(20);
	}

	~EffectsPanel()
	{
		/*Manage the memory for all of the rotaries created. TODO; delete the reverb settings
		in a for loop*/
		for (int i = 0; i <= 7; i++) {
			delete filterSettings[i];
		}
		delete reverbSettings[0];
		delete reverbSettings[1];
		delete reverbSettings[2];
		delete reverbSettings[3];
	}

	void paint(Graphics &g) override {
		g.fillAll(Colours::grey);
		//Draw all of the rotaries based on the local bounds of the effects panel 
		for (int i = 0; i <= 7; i++) {
			if (i <= 3) {
				filterSettings[i]->setBounds(0, (i * (getHeight() / 4) + 25), 100, 100);
				eqSettingDescriptions[i].setBounds(17, (i * (getHeight() / 4) + 10), 75, 25);
			}
			else {
				filterSettings[i]->setBounds(getWidth() / 3, ((i - 4) * (getHeight() / 4) + 25), 100, 100);
				eqSettingDescriptions[i].setBounds((getWidth() / 3) + 16, ((i - 4) * (getHeight() / 4) + 10), 75, 25);
			}
		}
		for (int i = 0; i <= 3; i++) {
			if (i <= 3) {
				reverbSettings[i]->setBounds((getWidth() / 3) * 2, (i * (getHeight() / 4)) + 25, 100, 100);
				reverbSettingDescriptions[i].setBounds(((getWidth() / 3) * 2) + 16, (i * (getHeight() / 4)) + 10, 75, 25);
			}
			else {
				//reverbSettings[i]->setBounds(300, (i - 4) * 100, 100, 100);
			}
		}
		displayChannel.setBounds(getWidth() - 150, 0, 150, 15);
	}

	void resized() override {
		//Update the size of the local bounds and repaint
		repaint();
		displayChannel.setBounds(0, 0, 25, 25);
	}

	//Nor is this?
	void sliderValueChanged(Slider *slider) override {

	}

	void setChannelBeingEffected(int i) {
		channelBeingEffected = i;
	}

	//Functions that allow the main component window to add listeners to the effect's panels rotaries
	Slider *getFilterSlider(int sliderToGet) {
		if (sliderToGet < 8) {
			return filterSettings[sliderToGet];
		}
	}

	Slider *getReverbSlider(int sliderToGet) {
		if (sliderToGet < 4) {
			return reverbSettings[sliderToGet];
		}
	}

	void timerCallback() {
		ss.str("");
		ss << "Channel being effected: " << channelBeingEffected + 1;
		displayChannel.setText(ss.str(), sendNotification);
	}



private:

	Slider lowPassSlider, highPassSlider, highShelfSlider, lowShelfSlider, middle1GainSlider, middle1FreqSlider, middle2GainSlider, middle2FreqSlider;
	//Arrays of sliders to create the filter and reverb rotaries
	Slider *filterSettings[8];

	Slider *reverbSettings[4];

	float lowPassCutoff, highPassCutoff, highShelfFreq, lowShelfFreq, middle1Gain, middle1Freq, middle2Gain, middle2Freq;

	int channelBeingEffected;

	int arrayPositionToUse, channelToEffect;

	Label displayChannel;

	//Arrays of labels to display the descriptions of all the rotaries
	Label eqSettingDescriptions[8];

	Label reverbSettingDescriptions[4];

	stringstream ss;
};