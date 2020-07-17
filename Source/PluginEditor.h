/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Author: Daniel Rudrich
 Copyright (c) 2017 - Institute of Electronic Music and Acoustics (IEM)
 https://iem.at

 The IEM plug-in suite is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 The IEM plug-in suite is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <https://www.gnu.org/licenses/>.
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//Plugin Design Essentials
#include "../IEMPluginSuite/resources/lookAndFeel/IEM_LaF.h"
#include "../IEMPluginSuite/resources/customComponents/TitleBar.h"

//Custom Components
#include "../IEMPluginSuite/resources/customComponents/ReverseSlider.h"
#include "../IEMPluginSuite/resources/customComponents/SimpleLabel.h"

typedef ReverseSlider::SliderAttachment SliderAttachment; // all ReverseSliders will make use of the parameters' valueToText() function

//==============================================================================
/**
*/
class SimplePluginAudioProcessorEditor  : public AudioProcessorEditor, private Timer
{
public:
    SimplePluginAudioProcessorEditor (SimplePluginAudioProcessor&, AudioProcessorValueTreeState&);
    ~SimplePluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void chooseHrirFile();
    void timerCallback() override;
    
    SimplePluginAudioProcessor* getProcessor() const {
        return static_cast<SimplePluginAudioProcessor*>(getAudioProcessor());
    }
    
private:
    // stored references to the AudioProcessor and ValueTreeState holding all the parameters
    SimplePluginAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;

    // sliders
    Slider slParam1;
    std::unique_ptr<SliderAttachment> slParam1Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimplePluginAudioProcessorEditor)
};
