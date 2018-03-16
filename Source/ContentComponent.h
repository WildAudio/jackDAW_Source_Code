#pragma once

#ifndef CONTENTCOMPONENT_H_INCLUDED
#define CONTENTCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <sstream>
#include "MixWindow.h"
#include "TrackContainer.h"
#include "EffectsPanel.h"
#include "ViewPortComponent.h"
#include "CustomAudioTransportSource.h"



class ContentComponent : public AudioAppComponent,
	public ChangeListener,
	public Slider::Listener,
	public Button::Listener,
	public Timer,
	public ApplicationCommandTarget
{
public:
	ContentComponent()
		:channelToEffect(0),
		state(Stopped)
	{
		setSize(1004, 800);

		//Add the play and stop buttons to the main window and set their values

		addAndMakeVisible(playButton);
		playButton.setButtonText("Play");
		playButton.addListener(this);
		playButton.setColour(TextButton::buttonColourId, Colours::green);
		playButton.setEnabled(true);

		addAndMakeVisible(stopButton);
		stopButton.setButtonText("Stop");
		stopButton.addListener(this);
		stopButton.setColour(TextButton::buttonColourId, Colours::red);
		stopButton.setEnabled(false);

		//Make the Mix Window pointer a new Mix Window to hold all of the volume sliders and pan pots,
		mixWindow = new MixWindow;
		addAndMakeVisible(mixWindow); //Then make it visible

									  //Add listeners to the Volume sliders...
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getSlider(i) != nullptr) {
				mixWindow->getSlider(i)->addListener(this);
			}
		}

		//...and the pan pots...
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getPanner(i) != nullptr) {
				mixWindow->getPanner(i)->addListener(this);
			}
		}

		//...and the channel buttons (could probably do this in one for loop?)
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getButton(i) != nullptr) {
				mixWindow->getButton(i)->addListener(this);
			}
		}

		effectsPanel = new EffectsPanel; //Create a new Effects Panel from the pointer
		addAndMakeVisible(effectsPanel); //And make it visible

										 //Add listeners to the filter rotaries...
		for (int i = 0; i <= 7; i++) {
			effectsPanel->getFilterSlider(i)->addListener(this);
		}

		//...and the reverb rotaries...
		for (int i = 0; i <= 3; i++) {
			effectsPanel->getReverbSlider(i)->addListener(this);
		}

		//...and allocate memory for the new reverb parameters (again, could probably do this in one for loop)
		for (int i = 0; i <= 15; i++) {
			reverbParameters[i] = new Reverb::Parameters;
		}


		addAndMakeVisible(positionLabel);
		positionLabel.setColour(Label::backgroundColourId, Colours::black);
		positionLabel.setColour(Label::textColourId, Colours::white);

		addAndMakeVisible(displayChannel);
		displayChannel.setColour(Label::backgroundColourId, Colours::black);
		displayChannel.setColour(Label::textColourId, Colours::white);

		//Display label for debug
		addAndMakeVisible(displayLabel);
		displayLabel.setColour(Label::backgroundColourId, Colours::black);
		displayLabel.setColour(Label::textColourId, Colours::white);

		/*Set up the format manager for the reference timer (TODO: investigate the Playhead class for
		a better way to implement a master timer*/
		formatManager2.registerBasicFormats();
		timer.addChangeListener(this);

		/*Allocate memory for all of the transport sources, panners, filters, reverbs and plug them
		into each other, plus setup the format managers*/
		for (int i = 0; i <= 15; i++) {
			pTransportSource[i] = new AudioTransportSource();
			pans[i] = new CustomAudioTransportSource(pTransportSource[i], false);
			filterLow[i] = new IIRFilterAudioSource(pans[i], false);
			filterHigh[i] = new IIRFilterAudioSource(filterLow[i], false);
			filterLowShelf[i] = new IIRFilterAudioSource(filterHigh[i], false);
			filterHighShelf[i] = new IIRFilterAudioSource(filterLowShelf[i], false);
			filterMiddle1[i] = new IIRFilterAudioSource(filterHighShelf[i], false);
			filterMiddle2[i] = new IIRFilterAudioSource(filterMiddle1[i], false);
			reverb[i] = new ReverbAudioSource(filterMiddle2[i], false);
			formatManager[i] = new AudioFormatManager();
			formatManager[i]->registerBasicFormats();
		}

		/*name0 = File::getCurrentWorkingDirectory().getChildFile("Kick.wav").getFullPathName();
		name1 = File::getCurrentWorkingDirectory().getChildFile("Snare.wav").getFullPathName();
		name2 = File::getCurrentWorkingDirectory().getChildFile("OHL.wav").getFullPathName();
		name3 = File::getCurrentWorkingDirectory().getChildFile("OHR.wav").getFullPathName();

		files[0] = new File(name0);
		files[1] = new File(name1);
		files[2] = new File(name2);
		files[3] = new File(name3);*/

		files[0] = new File(File::getCurrentWorkingDirectory().getChildFile("Kick.wav").getFullPathName());
		files[1] = new File(File::getCurrentWorkingDirectory().getChildFile("Snare.wav").getFullPathName());
		files[2] = new File(File::getCurrentWorkingDirectory().getChildFile("Toms_L.wav").getFullPathName());
		files[3] = new File(File::getCurrentWorkingDirectory().getChildFile("Toms_R.wav").getFullPathName());
		files[4] = new File(File::getCurrentWorkingDirectory().getChildFile("OH_L.wav").getFullPathName());
		files[5] = new File(File::getCurrentWorkingDirectory().getChildFile("OH_R.wav").getFullPathName());
		files[6] = new File(File::getCurrentWorkingDirectory().getChildFile("Drums_Ambience_L.wav").getFullPathName());
		files[7] = new File(File::getCurrentWorkingDirectory().getChildFile("Drums_Ambience_R.wav").getFullPathName());
		files[8] = new File(File::getCurrentWorkingDirectory().getChildFile("OH_u87.wav").getFullPathName());
		files[9] = new File(File::getCurrentWorkingDirectory().getChildFile("Bass.wav").getFullPathName());
		files[10] = new File(File::getCurrentWorkingDirectory().getChildFile("Guitars_1.wav").getFullPathName());
		files[11] = new File(File::getCurrentWorkingDirectory().getChildFile("Guitars_2.wav").getFullPathName());
		files[12] = new File(File::getCurrentWorkingDirectory().getChildFile("Guitar_Lead.wav").getFullPathName());
		files[13] = new File(File::getCurrentWorkingDirectory().getChildFile("Vox_Lead.wav").getFullPathName());
		files[14] = new File(File::getCurrentWorkingDirectory().getChildFile("BVs_L.wav").getFullPathName());
		files[15] = new File(File::getCurrentWorkingDirectory().getChildFile("BVs_R.wav").getFullPathName());
		

		

		//String name1 = File::getCurrentWorkingDirectory().getChildFile("Kick.wav").getFullPathName();

		//*files[0] = name1;

		//Set up the initial settings for the reverbs, and apply it to the appropriate channel
		for (int i = 0; i <= 15; i++) {
			reverbParameters[i]->damping = 0.5;
			reverbParameters[i]->roomSize = 0.5;
			reverbParameters[i]->wetLevel = 0.0;
			reverbParameters[i]->width = 0.5;
			reverb[i]->setParameters(*reverbParameters[i]);
		}

		//Point the transport sources to the files and add the transport sources to the Mixer
		for (int i = 0; i <= 15; i++) {
				AudioFormatReader* reader = formatManager[i]->createReaderFor(*files[i]);
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true);
				pTransportSource[i]->setSource(newSource, 0, nullptr, reader->sampleRate);
				readerSource[i] = newSource.release();
				mixer.addInputSource(reverb[i], false);
			
		}

		//Set the file for the timer reference and set it as the timer transport source, then send through the mixer

		File file2(File::getCurrentWorkingDirectory().getChildFile("reference.wav").getFullPathName());
		AudioFormatReader* reader2 = formatManager2.createReaderFor(file2);
		ScopedPointer<AudioFormatReaderSource> newSource2 = new AudioFormatReaderSource(reader2, true);
		timer.setSource(newSource2, 0, nullptr, reader2->sampleRate);
		readerSource2 = newSource2.release();
		mixer.addInputSource(&timer, false);

		/*Create the Viewport component that will hold the edit window and the file selection window.
		Note - passing the transport sources in this way is very inelegant. TODO; look into passing
		arrays of pointers too functions*/

		viewPort = new ViewportComponent(pTransportSource, timer, files);
		addAndMakeVisible(viewPort);
		addMouseListener(this, true);

		//Add listeners to the close/open buttons in the file select window

		for (int i = 0; i <= 15; i++) {
			viewPort->getTrackContainer()->getFileSelectWindow()->getCloseButton(i)->addListener(this);
			viewPort->getTrackContainer()->getFileSelectWindow()->getOpenButton(i)->addListener(this);
		}

		/*for (int i = 0; i <= 3; i++) {
		pTransportSource[i]->start();
		}
		timer.start();*/
		timer.setGain(0.0);

		viewPort->getTrackContainer()->getEditWindow()->addChangeListener(this);

		

		//Initialise the audio channels and start the timer
		setAudioChannels(0, 2);
		startTimer(20);
		timer.setPosition(0.0);

		//viewPort->getTrackContainer()->getEditWindow()->getTrack(5)->getAudioTransportSource().start();
		//pTransportSource[5]->start();
		//pTransportSource[5]->stop();
		//pTransportSource[5]->setPosition(0.0);
	}

	~ContentComponent()
	{
		shutdownAudio();
	}

	

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		//This prepares to play all of the transport sources attached to it
		mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		//Gets the output of all the transport sources attached to it
		mixer.getNextAudioBlock(bufferToFill);
	}

	void releaseResources() override
	{
		//use the release resource functions in all of the regularly created objects...
		transportSource.releaseResources();
		timer.releaseResources();
		mixer.releaseResources();

		//...and delete the memory of all the objects we created using "new"
		for (int i = 0; i <= 15; i++) {
			delete pTransportSource[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete filterLow[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete filterHigh[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete filterLowShelf[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete filterHighShelf[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete filterMiddle1[i];
		}
		for (int i = 0; i <= 15; i++) {
			delete pans[i];
			delete filterMiddle2[i];
			delete reverb[i];
			delete files[i];
			delete formatManager[i];
			//delete readerSource[i];
			delete reverbParameters[i];
		}

		for (int i = 0; i <= 15; i++) {
			delete readerSource[i];
		}

		//delete applicationCommandManager;
	}

	ViewportComponent *getViewPort() {
		return viewPort;
	}

	ApplicationCommandTarget* getNextCommandTarget() override
	{
		// this will return the next parent component that is an ApplicationCommandTarget (in this
		// case, there probably isn't one, but it's best to use this method in your own apps).
		return findFirstTargetParentComponent();
	}

	void getAllCommands(Array<CommandID>& commands) override
	{
		// this returns the set of all commands that this target can perform..
		CommandID ids[] = {
			begin,
			pause,
			split
		};

		commands.addArray(ids, numElementsInArray(ids));
	}

	void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override
	{
		String generalCategory("General");
		String demosCategory("Demos");

		switch (commandID)
		{
		case 8448:
			result.setInfo("Play", "Plays the Track", demosCategory, 0);
			result.addDefaultKeypress('f', ModifierKeys::commandModifier);
			break;

		case 8449:
			result.setInfo("Pause", "Pauses the track", demosCategory, 0);
			result.addDefaultKeypress('g', ModifierKeys::commandModifier);
			break;

		case 8450:
			result.setInfo("Split", "Pauses the track", demosCategory, 0);
			result.addDefaultKeypress('B', ModifierKeys::commandModifier);
			break;

		default:
			break;
		}
	}

	bool perform(const InvocationInfo& info) override
	{

		switch (info.commandID)
		{
		case 8448:   playButtonClicked(); break;
		case 8449:   stopButtonClicked(); break;

		case 8450:
			for (int i = 0; i <= 15; i++) {
				if (viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(i) != nullptr) {
					viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(i)->breakRegion(viewPort->getTrackContainer()->getEditWindow()->getPlayHeadX(), timer);
					viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(i)->repaint();
				}
			}; break;

		default:
			return false;
		}


		return true;
	}

	void paint(Graphics& g) override
	{
		//Draw all of the components based on the local bounds of the componenet that they are created in 
		g.fillAll(Colours::white);
		mixWindow->setBounds(10, ((getHeight() / 3) * 2) - 15, getWidth() - 20, (getHeight() / 3) + 5);
		viewPort->setBounds((getWidth() / 3) + 10, 50, ((getWidth() / 3) * 2) - 20, ((getHeight() / 3) * 2) - 75);
		effectsPanel->setBounds(10, 50, (getWidth() / 3) - 20, ((getHeight() / 3) * 2) - 75);
		playButton.setBounds((getWidth() / 3) - 37.5, 10, 75, 25);
		stopButton.setBounds(((getWidth() / 3) * 2) - 37.5, 10, 75, 25);
		positionLabel.setBounds((getWidth() / 2) - 37.5, 10, 75, 25);
		//displayChannel.setBounds(300, 400, 25, 25);
		//displayLabel.setBounds(0, 0, 300, 50);
	}

	void resized() override
	{
		//Get new bounds and repaint
		repaint();
	}

	void sliderValueChanged(Slider *slider) override {
		//If the slider moved is the volume slider from the Mix Window, set the gain of the channel attached to it...
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getSlider(i) != nullptr) {
				if (slider == mixWindow->getSlider(i)) {
					pTransportSource[i]->setGain(mixWindow->getSlider(i)->getValue());
				}
			}
		}
		//..or if it's the panning rotary, set the panning value for the appropriate channel
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getPanner(i) != nullptr) {
				if (slider == mixWindow->getPanner(i)) {
					pans[i]->setPanL(mixWindow->getPanner(i)->getValue());
				}
			}
		}

		/*If the sliders are the filter sliders from the Effects Panel, check which one it is, then set
		the gain values and coefficients for the appropriate channel as set by the channel select button*/
		if (slider == effectsPanel->getFilterSlider(0)) {
			filterLow[channelToEffect]->setCoefficients(IIRCoefficients::makeLowPass(44100.0, effectsPanel->getFilterSlider(0)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(0, effectsPanel->getFilterSlider(0)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(1)) {
			filterHigh[channelToEffect]->setCoefficients(IIRCoefficients::makeHighPass(44100.0, effectsPanel->getFilterSlider(1)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(1, effectsPanel->getFilterSlider(1)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(2)) {
			filterLowShelf[channelToEffect]->setCoefficients(IIRCoefficients::makeLowShelf(44100.0, 400.0, 2.0, effectsPanel->getFilterSlider(2)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(2, effectsPanel->getFilterSlider(2)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(3)) {
			filterHighShelf[channelToEffect]->setCoefficients(IIRCoefficients::makeHighShelf(44100.0, 10000.0, 2.0, effectsPanel->getFilterSlider(3)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(3, effectsPanel->getFilterSlider(3)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(4)) {
			filterMiddle1[channelToEffect]->setCoefficients(IIRCoefficients::makePeakFilter(44100.0, effectsPanel->getFilterSlider(5)->getValue(), 3.0, effectsPanel->getFilterSlider(4)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(4, effectsPanel->getFilterSlider(4)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(5)) {
			filterMiddle1[channelToEffect]->setCoefficients(IIRCoefficients::makePeakFilter(44100.0, effectsPanel->getFilterSlider(5)->getValue(), 3.0, effectsPanel->getFilterSlider(4)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(5, effectsPanel->getFilterSlider(5)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(6)) {
			filterMiddle2[channelToEffect]->setCoefficients(IIRCoefficients::makePeakFilter(44100.0, effectsPanel->getFilterSlider(7)->getValue(), 3.0, effectsPanel->getFilterSlider(6)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(6, effectsPanel->getFilterSlider(6)->getValue());
		}
		if (slider == effectsPanel->getFilterSlider(7)) {
			filterMiddle2[channelToEffect]->setCoefficients(IIRCoefficients::makePeakFilter(44100.0, effectsPanel->getFilterSlider(7)->getValue(), 3.0, effectsPanel->getFilterSlider(6)->getValue()));
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setFilterValues(7, effectsPanel->getFilterSlider(7)->getValue());
		}

		/*If the slider moved is one of the reverb rotaries from the Effects Panel, update the appropriate
		channel's reverb Parameters (based on the channel select button) and update the channel's reverb source*/
		if (slider == effectsPanel->getReverbSlider(0)) {
			reverbParameters[channelToEffect]->wetLevel = effectsPanel->getReverbSlider(0)->getValue();
			reverb[channelToEffect]->setParameters(*reverbParameters[channelToEffect]);
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setReverbValues(0, effectsPanel->getReverbSlider(0)->getValue());
		}
		if (slider == effectsPanel->getReverbSlider(1)) {
			reverbParameters[channelToEffect]->roomSize = effectsPanel->getReverbSlider(1)->getValue();
			reverb[channelToEffect]->setParameters(*reverbParameters[channelToEffect]);
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setReverbValues(1, effectsPanel->getReverbSlider(1)->getValue());
		}
		if (slider == effectsPanel->getReverbSlider(2)) {
			reverbParameters[channelToEffect]->damping = effectsPanel->getReverbSlider(2)->getValue();
			reverb[channelToEffect]->setParameters(*reverbParameters[channelToEffect]);
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setReverbValues(2, effectsPanel->getReverbSlider(2)->getValue());
		}
		if (slider == effectsPanel->getReverbSlider(3)) {
			reverbParameters[channelToEffect]->width = effectsPanel->getReverbSlider(3)->getValue();
			reverb[channelToEffect]->setParameters(*reverbParameters[channelToEffect]);
			viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->setReverbValues(3, effectsPanel->getReverbSlider(3)->getValue());
		}
	}

	void buttonClicked(Button *button) override {
		if (button == &playButton) {
			playButtonClicked();
		}
		if (button == &stopButton) {
			stopButtonClicked();
		}
		//If the button clicked is the Mix Window's channel select, update the channel select integer
		for (int i = 0; i <= 15; i++) {
			if (mixWindow->getButton(i) != nullptr) {
				if (button == mixWindow->getButton(i)) {
					channelToEffect = i;
					effectsPanel->setChannelBeingEffected(channelToEffect);
				}
				for (int i = 0; i <= 7; i++) {
					effectsPanel->getFilterSlider(i)->setValue(viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->getFilterValues(i));
					//effectsPanel->getReverbSlider(i)->setValue(viewPort->getTrackContainer()->getEditWindow()->getAudioTracks(channelToEffect)->getFilterValues(i));
				}
				
			}
		}

		for (int i = 0; i <= 15; i++) {
			/*If the button clicked is a close button on the file select window, make the asppropriate
			channels transport source a null ptr and delete the track's regions...*/
			if (button == viewPort->getTrackContainer()->getFileSelectWindow()->getCloseButton(i)) {
				viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->deleteRegions();
				pTransportSource[i]->setSource(nullptr);
				ss.str(" ");
				ss << i;
				displayLabel.setText(ss.str(), sendNotification);
			}
			/*...or if it is an open buttons, create a file chooser window, then set the apparopriate
			transport source's source as the new selection and update the regions on the audio track*/
			if (button == viewPort->getTrackContainer()->getFileSelectWindow()->getOpenButton(i)) {
				//viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->openButtonClicked();

				FileChooser chooser("Select a Wave file to play...",
					File::nonexistent,
					"*.wav");

				if (chooser.browseForFileToOpen())
				{
					File fileChosen(chooser.getResult());
					AudioFormatReader* reader = formatManager[i]->createReaderFor(fileChosen);

					if (reader != nullptr)
					{
						ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true);
						pTransportSource[i]->setSource(newSource, 0, nullptr, reader->sampleRate);
						readerSource[i] = newSource.release();
						
						viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->getRegions(0)->setFile(fileChosen);
						viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->setFile(fileChosen.getFullPathName());

						viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->getRegions(0)->setEndTime(pTransportSource[i]->getLengthInSeconds());

						viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->updateStartAndEndTimes();

						ss.str(" ");
						ss << viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->getRegions(0)->getXValueEnd();
						displayLabel.setText(ss.str(), sendNotification);
					}

				}

			}
		}
	}

	void playButtonClicked() {
		if (timer.isPlaying()) {
			changeState(Paused);
			viewPort->getTrackContainer()->getEditWindow()->setCurrentState(2);
		}
		else {
			changeState(Playing);
			viewPort->getTrackContainer()->getEditWindow()->setCurrentState(1);
		}
	}

	void stopButtonClicked() {
		changeState(Stopped);
		viewPort->getTrackContainer()->getEditWindow()->setCurrentState(0);
	}

	//TODO: Learn how to sue Juce's OwnedArrays rather than regular arrays of pointers to cut down on the memory management needed
	//OwnedArray<AudioTransportSource> sources;

	void timerCallback() {
		const RelativeTime position(timer.getCurrentPosition());

		const int minutes = ((int)position.inMinutes()) % 60;
		const int seconds = ((int)position.inSeconds()) % 60;
		const int millis = ((int)position.inMilliseconds()) % 1000;

		const String positionString(String::formatted("%02d:%02d:%03d", minutes, seconds, millis));

		positionLabel.setText(positionString, dontSendNotification);

		ss.str("");
		ss << channelToEffect;
		displayChannel.setText(ss.str(), sendNotification);
	}

	void changeListenerCallback(ChangeBroadcaster *source) override {
		if (source == viewPort->getTrackContainer()->getEditWindow()) {
			if (viewPort->getTrackContainer()->getEditWindow()->getCurrentState() == 0) {
				stopButtonClicked();
			}
			else if (viewPort->getTrackContainer()->getEditWindow()->getCurrentState() == 1) {
				playButtonClicked();
			}
			else if (viewPort->getTrackContainer()->getEditWindow()->getCurrentState() == 2) {
				playButtonClicked();
			}
		}
	}

private:
	enum TransportState
	{
		Stopped,
		Playing,
		Paused
	};

	void changeState(TransportState newState)
	{
		if (state != newState)
		{
			state = newState;

			switch (state)
			{
			case Stopped:
				timer.stop();
				timer.setPosition(0.0);
				for (int i = 0; i <= 15; i++) {
					pTransportSource[i]->stop();
					pTransportSource[i]->setPosition(0.0);
				}
				playButton.setEnabled(true);
				stopButton.setEnabled(false);
				playButton.setButtonText("Play");
				stopButton.setButtonText("Stop");
				break;

			case Playing:
				timer.start();
				//tracks[0]->reposition(playHeadX, timer);
				if (timer.getCurrentPosition() == 0.0) {
					for (int i = 0; i <= 15; i++) {
						pTransportSource[i]->start();
					}
				}
				else {
					for (int i = 0; i <= 15; i++) {
						viewPort->getTrackContainer()->getEditWindow()->getTrack(i)->reposition(viewPort->getTrackContainer()->getEditWindow()->getPlayHeadX(), timer);
					}
				}
				stopButton.setEnabled(true);
				playButton.setButtonText("Pause");
				stopButton.setButtonText("Stop");
				break;

			case Paused:
				timer.stop();
				for (int i = 0; i <= 15; i++) {
					pTransportSource[i]->stop();
				}
				playButton.setEnabled(true);
				stopButton.setEnabled(true);
				playButton.setButtonText("Play");
				stopButton.setButtonText("Retur To Zero");
				break;
			}
		}
	}

	//==========================================================================
	/*Create scoped pointer's to the three main elements of the main window. The Viewport will contain
	both the edit window and the file select window*/
	ScopedPointer<MixWindow> mixWindow;
	ScopedPointer<EffectsPanel> effectsPanel;
	ScopedPointer<ViewportComponent> viewPort;

	

	Label displayLabel;
	Label positionLabel;

	TransportState state;

	stringstream ss;

	//Arrays of pointers to create all of the objects needed for the audio transport sources
	File *files[16];

	AudioFormatManager *formatManager[16];
	AudioFormatReaderSource *readerSource[16];
	AudioTransportSource transportSource;

	AudioFormatManager formatManager2;
	ScopedPointer<AudioFormatReaderSource> readerSource2;
	AudioTransportSource timer;

	//This will tell the sliders which channel to effect
	int channelToEffect;

	Label currentPositionLabel;
	Label displayChannel;

	//Pointers to create the transport sources, panners, filters and reverb sources needed
	AudioTransportSource *pTransportSource[16];
	CustomAudioTransportSource *pans[16];
	IIRFilterAudioSource *filterLow[16];
	IIRFilterAudioSource *filterHigh[16];
	IIRFilterAudioSource *filterLowShelf[16];
	IIRFilterAudioSource *filterHighShelf[16];
	IIRFilterAudioSource *filterMiddle1[16];
	IIRFilterAudioSource *filterMiddle2[16];
	ReverbAudioSource *reverb[16];
	Reverb::Parameters *reverbParameters[16];

	//A mixer object to add the last audio source in the chain to
	MixerAudioSource mixer;

	TextButton playButton, stopButton;

	double file1Length, file2Length;

	double positionPermanent;

	float overlayX;
	float thumbnailX, thumbnail2X;
	float originalPress, originalPress2;

	double transportMoved, transporttMoved2;

	String name0, name1, name2, name3;

	CommandID begin = 8448;
	CommandID pause = 8449;
	CommandID split = 8450;
};


#endif  // MAINCOMPONENT_H_INCLUDED
