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

struct LookAndFeel :juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override ;
};


struct RotarySliderWithLabels:public juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap,
    juce::String unitSuffix) :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 
        juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos 
    {
        float pos;
        juce::String labels;

    };
    juce::Array<LabelPos> labels;
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplyString() const;

private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
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
    void updateChain();
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
    
    RotarySliderWithLabels peakFreqSlider,
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
