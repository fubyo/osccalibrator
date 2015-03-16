/* OscCalibrator - A mapping and routing tool for use with the Open Sound Control protocol.
   Copyright (C) 2012  Dionysios Marinos - fewbio@googlemail.com

   This program is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software Foundation,
   either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.
   You should have received a copy of the GNU General Public License along with this program.
   If not, see <http://www.gnu.org/licenses/>.
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "Pool.h"

class MainWindow  : public DocumentWindow
{
public:
    //==============================================================================
    MainWindow() : DocumentWindow (T("OscCalibrator v0.15"), Colours::lightgrey, DocumentWindow::allButtons, true)
    {
        MainComponent* const mainComponent = new MainComponent();

        setContentComponent (mainComponent, true, true);

        centreWithSize (800, 600);

		setResizable(true, true);
        setVisible (true);
    }

    ~MainWindow()
    {
        // (the content component will be deleted automatically, so no need to do it here)
    }

    //==============================================================================
    void closeButtonPressed()
    {
		((MainComponent*)getContentComponent())->oscManager.stop();
        JUCEApplication::quit();
    }
};

//==============================================================================
class OscCalibratorApplication  : public JUCEApplication
{
public:
    //==============================================================================
    OscCalibratorApplication() : mainWindow(0)
    {
    }

    ~OscCalibratorApplication()
    {
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
		/*
		#ifdef _DEBUG
			int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
			flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
			_CrtSetDbgFlag(flag);
			//_CrtSetBreakAlloc(9667);
		#endif
		*/

        mainWindow = new MainWindow();
    }

    void shutdown()
    {
        mainWindow = 0;

		int curlong, totlong;
		qh_memfreeshort (&curlong, &totlong);  /* free short memory and memory allocator */
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        quit();
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "OscCalibrator";
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        
    }

private:
    ScopedPointer<MainWindow> mainWindow;

};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(OscCalibratorApplication)
