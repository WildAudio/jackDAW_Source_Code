#pragma once

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <sstream>
#include "ContentComponent.h"

class MainContentComponent : public Component,
	private AsyncUpdater
{
public:
	MainContentComponent()
	{
		setSize(1004, 800);

		contentComponent = new ContentComponent();
		addAndMakeVisible(contentComponent);

		// This lets the command manager use keypresses that arrive in our window to send out commands
		addKeyListener(getApplicationCommandManager().getKeyMappings());
		triggerAsyncUpdate();
	}

	~MainContentComponent()
	{

	}

	ApplicationCommandManager& getApplicationCommandManager()
	{
		if (applicationCommandManager == nullptr)
			applicationCommandManager = new ApplicationCommandManager();

		return *applicationCommandManager;
	}

	void handleAsyncUpdate() override
	{
		// This registers all of our commands with the command manager but has to be done after the window has
		// been created to find the number of rendering engines available
		auto& commandManager = getApplicationCommandManager();

		commandManager.registerAllCommandsForTarget(contentComponent);
		//commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());
	}

	void paint(Graphics& g) override
	{
		contentComponent->setBounds(0, 0, getWidth(), getHeight());
	}

	void resized() override
	{
		//Get new bounds and repaint
		repaint();
	}



private:

	ScopedPointer<ContentComponent> contentComponent;

	//Create the Command Manager to handle all of the keyboard commands (needs to be created outside of the main class)
	ScopedPointer<ApplicationCommandManager> applicationCommandManager;

	//==========================================================================
	/*Create scoped pointer's to the three main elements of the main window. The Viewport will contain
	both the edit window and the file select window*/


	//Create the Command Manager to handle all of the keyboard commands (needs to be created outside of the main class)
	//ScopedPointer<ApplicationCommandManager> applicationCommandManager;

};


Component* createMainContentComponent() { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
