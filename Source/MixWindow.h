/*
==============================================================================

MixWindow.h
Created: 12 Feb 2018 10:21:25pm
Author:  User

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <sstream>

using namespace std;



class MixWindow : public Component, public Slider::Listener, public Button::Listener, public ChangeBroadcaster

{
public:
	MixWindow()
	{
		//Allocate memory for the volume sliders...
		for (int i = 0; i <= 15; i++) {
			sliders[i] = new Slider;
		}
		//...and the channels select buttons
		for (int i = 0; i <= 15; i++) {
			buttons[i] = new TextButton;
		}
		//...and the panner rotaries (again, could probably do this is one for loop now?)
		for (int i = 0; i <= 15; i++) {
			pans[i] = new Slider;
		}

		//Debug stuff
		//addAndMakeVisible(displayLabel);
		displayLabel.setColour(Label::backgroundColourId, Colours::black);
		displayLabel.setColour(Label::textColourId, Colours::white);

		//addAndMakeVisible(displayLabel2);
		displayLabel2.setColour(Label::backgroundColourId, Colours::black);
		displayLabel2.setColour(Label::textColourId, Colours::white);

		for (int i = 0; i <= 15; i++) {
			addAndMakeVisible(channelNames[i]);
			channelNames[i].setColour(Label::backgroundColourId, Colours::black);
			channelNames[i].setColour(Label::textColourId, Colours::white);
		}

		channelNames[0].setText("Kick", sendNotification);
		channelNames[1].setText("Snare", sendNotification);
		channelNames[2].setText("Toms L", sendNotification);
		channelNames[3].setText("Toms R", sendNotification);
		channelNames[4].setText("OH L", sendNotification);
		channelNames[5].setText("OH R", sendNotification);
		channelNames[6].setText("Amb L", sendNotification);
		channelNames[7].setText("Amb R", sendNotification);
		channelNames[8].setText("u87 OH", sendNotification);
		channelNames[9].setText("Bass", sendNotification);
		channelNames[10].setText("Guitars 1", sendNotification);
		channelNames[11].setText("Guitars 2", sendNotification);
		channelNames[12].setText("Lead", sendNotification);
		channelNames[13].setText("Vox Lead", sendNotification);
		channelNames[14].setText("BVs 1", sendNotification);
		channelNames[15].setText("BVs 2", sendNotification);

		//Make visible and set the values for the volume sliders...
		for (int i = 0; i <= 15; i++) {
			if (sliders[i] != nullptr) {
				addAndMakeVisible(sliders[i]);
				sliders[i]->setSliderStyle(Slider::LinearVertical);
				sliders[i]->setRange(0.0, 1.0);
				sliders[i]->setValue(1.0);
				sliders[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
				sliders[i]->addListener(this);
			}
			else {
				break;
			}
		}
		//...and panner rotaries...
		for (int i = 0; i <= 15; i++) {
			if (pans[i] != nullptr) {
				addAndMakeVisible(pans[i]);
				pans[i]->setSliderStyle(Slider::RotaryVerticalDrag);
				pans[i]->setRange(-1.0, 1.0);
				pans[i]->setValue(0.0);
				pans[i]->setTextBoxStyle(Slider::TextBoxAbove, false, 75, 15);
				pans[i]->addListener(this);
			}
			else {
				break;
			}
		}
		//...and channel select buttons
		for (int i = 0; i <= 15; i++) {
			if (buttons[i] != nullptr) {
				addAndMakeVisible(buttons[i]);
				buttons[i]->setButtonText("-");
				buttons[i]->addListener(this);
				buttons[i]->setColour(TextButton::buttonColourId, Colours::yellow);
				buttons[i]->setEnabled(true);
			}
			else {
				break;
			}
		}

	}
	//Call the release resources function in the destructor
	~MixWindow()
	{
		releaseResources();
	}

	//==============================================================================


	void releaseResources() {
		/*Manage all the allocated memory for the volume sliders, panner rotaries and channel select buttons.
		TODO: Again, more elegant to do in one for loop at some point*/
		for (int i = 0; i <= 15; i++) {
			if (sliders[i] != nullptr) {
				delete sliders[i];
			}
		}
		for (int i = 0; i <= 15; i++) {
			if (buttons[i] != nullptr) {
				delete buttons[i];
			}
		}
		for (int i = 0; i <= 15; i++) {
			if (pans[i] != nullptr) {
				delete pans[i];
			}
		}
	}

	void paint(Graphics& g) override
	{
		g.fillAll(Colours::grey);
		//Draw all of the components based on the local bounds of the Mix Window
		for (int i = 0; i <= 15; i++) {
			if (sliders[i] != nullptr) {
				sliders[i]->setBounds(((i + 1) * (getWidth() / 16)) - 45, 105, 50, (getHeight() / 1.50) - 25);
			}
			else {
				break;
			}
		}
		for (int i = 0; i <= 15; i++) {
			if (buttons[i] != nullptr) {
				buttons[i]->setBounds(((i + 1) * (getWidth() / 16)) - 5, 125, 10, 10);
			}
			else {
				break;
			}
		}
		for (int i = 0; i <= 15; i++) {
			if (pans[i] != nullptr) {
				pans[i]->setBounds(((i + 1) * (getWidth() / 16)) - 55, 10, 75, 75);
				channelNames[i].setBounds(((i + 1) * (getWidth() / 16)) - 42, 80, 50, 25);
			}
		}

		//displayLabel.setBounds(300, 50, 100, 50);
	}

	void resized() override
	{
		//Update the bounds when resized and redraw
		repaint();
	}

	void sliderValueChanged(Slider* slider) override
	{
		//Debug output
		/*for (int i = 0; i <= 3; i++) {
		if (slider == sliders[i]) {
		ss.str(" ");
		ss << sliders[i]->getValue();
		displayLabel.setText(ss.str(), sendNotification);
		}
		}*/
	}

	//Get functions for all sliders and buttons so that listeners can be added in the main component window
	Slider *getSlider(int sliderToUse) {
		return sliders[sliderToUse];
	}

	Slider *getPanner(int sliderToUse) {
		return pans[sliderToUse];
	}

	Button *getButton(int buttonToUse) {
		return buttons[buttonToUse];
	}


	void buttonClicked(Button* button) override
	{
		//Old debug output. May well still be useful at some point, so keep for the moment
		/*for (int i = 0; i <= 3; i++) {
		if (button == buttons[i]) {

		}
		}*/
	}

	

private:

	stringstream ss, ss2;

	Label displayLabel, displayLabel2;

	Label channelNames[16];

	//Arrays of pointers to create all of the volume sliders...
	Slider *sliders[16];
	//...and panner rotaries...
	Slider *pans[16];
	//...and channel select buttons
	TextButton *buttons[16];

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};



//#endif  // MIXWINDOW_H_INCLUDED
