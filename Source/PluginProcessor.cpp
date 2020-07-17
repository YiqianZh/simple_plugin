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
SimplePluginAudioProcessor::SimplePluginAudioProcessor()
                    : AudioProcessorBase (
#ifndef JucePlugin_PreferredChannelConfigurations
                      BusesProperties()
                      .withInput ("Input",  AudioChannelSet::discreteChannels (2), true)
                      .withOutput ("Output", AudioChannelSet::discreteChannels (2), true),
#endif
                       createParameterLayout())
{
    // get pointers to the parameters
    inputChannelsSetting = parameters.getRawParameterValue ("inputChannelsSetting");
    outputOrderSetting = parameters.getRawParameterValue ("outputOrderSetting");
    slider1 = parameters.getRawParameterValue ("slider1");


    // add listeners to parameter changes
    parameters.addParameterListener ("inputChannelsSetting", this);
    parameters.addParameterListener ("outputOrderSetting", this);
    parameters.addParameterListener ("slider1", this);

}

SimplePluginAudioProcessor::~SimplePluginAudioProcessor()
{
}

//==============================================================================
int SimplePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimplePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimplePluginAudioProcessor::setCurrentProgram (int index)
{
    ignoreUnused (index);
}

const String SimplePluginAudioProcessor::getProgramName (int index)
{
    ignoreUnused (index);
    return {};
}

void SimplePluginAudioProcessor::changeProgramName (int index, const String& newName)
{
    ignoreUnused (index, newName);
}

//==============================================================================
void SimplePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    checkInputAndOutput (this, static_cast<int> (*inputChannelsSetting), static_cast<int> (*outputOrderSetting), true);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SimplePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void SimplePluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer&)
{
    checkInputAndOutput (this, static_cast<int> (*inputChannelsSetting), static_cast<int> (*outputOrderSetting), false);
    ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // This clears any output channels that didn't contain input data
    // prevents feedback and ill-defined behaviour
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Run the callback
    applyDistortion(buffer);
}

//==============================================================================
bool SimplePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimplePluginAudioProcessor::createEditor()
{
    return new SimplePluginAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SimplePluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  auto state = parameters.copyState();

  auto oscConfig = state.getOrCreateChildWithName ("OSCConfig", nullptr);
  oscConfig.copyPropertiesFrom (oscParameterInterface.getConfig(), nullptr);

  std::unique_ptr<XmlElement> xml (state.createXml());
  copyXmlToBinary (*xml, destData);
}


void SimplePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (ValueTree::fromXml (*xmlState));
            if (parameters.state.hasProperty ("OSCPort")) // legacy
            {
                oscParameterInterface.getOSCReceiver().connect (parameters.state.getProperty ("OSCPort", var (-1)));
                parameters.state.removeProperty ("OSCPort", nullptr);
            }

            auto oscConfig = parameters.state.getChildWithName ("OSCConfig");
            if (oscConfig.isValid())
                oscParameterInterface.setConfig (oscConfig);
        }
}

//==============================================================================
void SimplePluginAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    DBG ("Parameter with ID " << parameterID << " has changed. New value: " << newValue);

    if (parameterID == "inputChannelsSetting" || parameterID == "outputOrderSetting" )
        userChangedIOSettings = true;

    if (parameterID == "slider1"){
        // do something with new value of slider, *slider1
    }
    
}

void SimplePluginAudioProcessor::updateBuffers()
{
    DBG ("IOHelper:  input size: " << input.getSize());
    DBG ("IOHelper: output size: " << output.getSize());
}


//==============================================================================
std::vector<std::unique_ptr<RangedAudioParameter>> SimplePluginAudioProcessor::createParameterLayout()
{
    // add your audio parameters here
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (OSCParameterInterface::createParameterTheOldWay ("inputChannelsSetting", "Number of input channels ", "",
                                                       NormalisableRange<float> (0.0f, 10.0f, 1.0f), 0.0f,
                                                       [](float value) {return value < 0.5f ? "Auto" : String (value);}, nullptr));

    params.push_back (OSCParameterInterface::createParameterTheOldWay ("outputOrderSetting", "Ambisonic Order", "",
                                                       NormalisableRange<float> (0.0f, 8.0f, 1.0f), 0.0f,
                                                       [](float value) {
                                                           if (value >= 0.5f && value < 1.5f) return "0th";
                                                           else if (value >= 1.5f && value < 2.5f) return "1st";
                                                           else if (value >= 2.5f && value < 3.5f) return "2nd";
                                                           else if (value >= 3.5f && value < 4.5f) return "3rd";
                                                           else if (value >= 4.5f && value < 5.5f) return "4th";
                                                           else if (value >= 5.5f && value < 6.5f) return "5th";
                                                           else if (value >= 6.5f && value < 7.5f) return "6th";
                                                           else if (value >= 7.5f) return "7th";
                                                           else return "Auto";},
                                                       nullptr));
                                                       
    params.push_back (OSCParameterInterface::createParameterTheOldWay ("slider1", "Slider One", "units",
                                                       NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0,
                                                       [](float value) {return String (value);}, nullptr));

    return params;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimplePluginAudioProcessor();
}
