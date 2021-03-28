#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ClarityPlugin3AudioProcessorEditor::ClarityPlugin3AudioProcessorEditor(ClarityPlugin3AudioProcessor& p)
    : AudioProcessorEditor(&p), 
      audioProcessor(p), 
      lowPassFrequency(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "lowPassFrequency", lowPass)),
      highPassFrequency(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.tree, "highPassFrequency", highPass))
{
    setSize(800, 650);

    initialize_projectName();
    initialize_muteButton();
    initialize_mGainControlSilder();
    initialize_gainLabel();
    initialize_FFT();

    // Filters
    initialize_lowPassLabel();
    initialize_lowPass(); //low pass knob
    initialize_highPassLabel();
    initialize_highPass();
}

ClarityPlugin3AudioProcessorEditor::~ClarityPlugin3AudioProcessorEditor()
{

}

//==============================================================================
void ClarityPlugin3AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setGradientFill(juce::ColourGradient(juce::Colours::lightcoral, 0, 0, juce::Colours::darkcyan, 1000, 1000, true));
    g.fillAll();
    g.setFont(15.0f);
}

void ClarityPlugin3AudioProcessorEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds();

    // Add Padding
    auto applicationBorderPadding = 10;
    area.removeFromTop(applicationBorderPadding);
    area.removeFromLeft(applicationBorderPadding);
    area.removeFromRight(applicationBorderPadding);
    area.removeFromBottom(applicationBorderPadding);

    // projectName Label
    projectName.setBounds(area.removeFromTop(area.getHeight() / 20));

    // SimpleFFT (i.e., Spectrogram)
    FFT.setBounds(area.removeFromTop(area.getHeight() / 3));

    area.removeFromTop(100); // Temporary Spacing
    
    // muteButton
    auto buttonArea = area.removeFromLeft(area.getWidth() / 4);
    muteButton.setBounds(buttonArea.removeFromBottom(buttonArea.getHeight() / 2));

    // mGainControlSlider
    mGainControlSlider.setBounds(area.removeFromLeft(area.getWidth() / 3));

    // lowPass
    lowPass.setBounds(area.removeFromLeft(area.getWidth() / 2));

    // highPass
    highPass.setBounds(area);
}



void ClarityPlugin3AudioProcessorEditor::muteButtonClicked()
{
    auto& params = processor.getParameters();

    //set gain to 0
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    *gainParameter = 0;

    if (*gainParameter == 0) {
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::indianred);
    }

}

void ClarityPlugin3AudioProcessorEditor::highPass_SliderValueChanged()
{

}

void ClarityPlugin3AudioProcessorEditor::lowPass_SliderValueChanged()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* lpParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    *lpParameter = lowPass.getValue();
}

void ClarityPlugin3AudioProcessorEditor::mGainControlSlider_SliderValueChanged()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    *gainParameter = mGainControlSlider.getValue();

    //sets mute button colour back to original if *gainParameter != 0
    if (*gainParameter != 0)
    {
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(101, 201, 134));
    }

}

void ClarityPlugin3AudioProcessorEditor::initialize_projectName()
{
    addAndMakeVisible(projectName);
    projectName.setFont(juce::Font(30.0f, juce::Font::bold));
    projectName.setText("CLARITY", juce::dontSendNotification);
    projectName.setColour(juce::Label::textColourId, juce::Colours::orange);
    projectName.setJustificationType(juce::Justification::centred);
}

void ClarityPlugin3AudioProcessorEditor::initialize_muteButton()
{
    addAndMakeVisible(muteButton);
    muteButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(101, 201, 134));
    muteButton.setButtonText("Mute");
    muteButton.onClick = [this] { muteButtonClicked(); };
}

void ClarityPlugin3AudioProcessorEditor::initialize_mGainControlSilder()
{
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* gainParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    addAndMakeVisible(mGainControlSlider);
    mGainControlSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mGainControlSlider.setRange(gainParameter->range.start, gainParameter->range.end);
    mGainControlSlider.setValue(*gainParameter);
    mGainControlSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    mGainControlSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    mGainControlSlider.onValueChange = [this] { mGainControlSlider_SliderValueChanged(); };
}

void ClarityPlugin3AudioProcessorEditor::initialize_gainLabel()
{
    addAndMakeVisible(gainLabel);
    gainLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    gainLabel.attachToComponent(&mGainControlSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setText("Gain", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_FFT()
{
    addAndMakeVisible(FFT);
}

void ClarityPlugin3AudioProcessorEditor::initialize_lowPassLabel()
{
    addAndMakeVisible(lowPassLabel);
    lowPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lowPassLabel.attachToComponent(&lowPass, false);
    lowPassLabel.setJustificationType(juce::Justification::centred);
    lowPassLabel.setText("Low-Pass Filter", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_lowPass()
{
    //lowPass knob
    addAndMakeVisible(&lowPass);
    lowPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lowPass.setRange(20.0f, 20000.0f);
    lowPass.setValue(20000.0f);
    lowPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    lowPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    lowPass.onValueChange = [this] { lowPass_SliderValueChanged(); };
}

void ClarityPlugin3AudioProcessorEditor::initialize_highPassLabel()
{
    addAndMakeVisible(highPassLabel);
    highPassLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    highPassLabel.attachToComponent(&highPass, false);
    highPassLabel.setJustificationType(juce::Justification::centred);
    highPassLabel.setText("High-Pass Filter", juce::dontSendNotification);
}

void ClarityPlugin3AudioProcessorEditor::initialize_highPass()
{
    //highPassKnob
    addAndMakeVisible(&highPass);
    highPass.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highPass.setRange(20.0f, 20000.0f);
    highPass.setValue(20.0f);
    highPass.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    highPass.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 50);
    highPass.onValueChange = [this] { highPass_SliderValueChanged(); };
}