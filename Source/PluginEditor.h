/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
struct CustomRotarySlider:public juce::Slider
{
    CustomRotarySlider() :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

struct ResponseCurveComponent :public juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
    ResponseCurveComponent(SpecEQAudioProcessor&);
    ~ResponseCurveComponent();
    juce::Atomic<bool> parametersChanged{ false };
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterType, bool gestureIsStarting) override {};
    void timerCallback() override;
    SpecEQAudioProcessor& audioProcessor;
    void paint(juce::Graphics& g) override;
    MonoChain monoChain;
};

class SpecEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SpecEQAudioProcessorEditor (SpecEQAudioProcessor&);
    ~SpecEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    std::vector<juce::Component*> getComps();



private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpecEQAudioProcessor& audioProcessor;
    
    CustomRotarySlider peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;

    ResponseCurveComponent responseCurveComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpecEQAudioProcessorEditor)

};
