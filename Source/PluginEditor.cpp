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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimplePluginAudioProcessorEditor::SimplePluginAudioProcessorEditor (SimplePluginAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      valueTreeState (vts)
{

    addAndMakeVisible (slParam1);
    slParam1Attachment.reset (new SliderAttachment (valueTreeState, "slider1", slParam1));
    
}

SimplePluginAudioProcessorEditor::~SimplePluginAudioProcessorEditor()
{
    removeAllChildren();
    setLookAndFeel (nullptr);
}

//==============================================================================
void SimplePluginAudioProcessorEditor::paint (Graphics& g)
{
    //g.fillAll (SSR::colour::get_colour(SSR::colour::Colours::white));
    //g.setColour(SSR::colour::get_colour(SSR::colour::Colours::black));
}

void SimplePluginAudioProcessorEditor::resized()
{

}

void SimplePluginAudioProcessorEditor::timerCallback()
{

}