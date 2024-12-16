# smallSpaceReverb
emulated reflections in a small space chamber.

# most recent additions
- it works??? i guess. /september 2024 i understand how it works now lol /october 2024 it actually does not work completely lol buffer needs to be cleared inside the startup but it is causing problems. /dec 2024
- the plugin doesn't crash computers anymore /september 2024
- okay so the reverb is actually working. the dsp might need some tweaking to get the tails lasting longer and reflecting more. /october 2024
- added a highpass input filter. planning on changing this to bandpass input as well as a shelf eq output. /october 2024 changed to high shelf /dec 2024
- dry wet knob works lol /october 2024

# problems: 

- extreme clipping on lauch /september 2024 
- i'm solving this via clear buffer function. gimme a minute /october 2024 not working yet /dec 2024
- no tails in the reverb.
- SOLVED: dry wet knob is not working since the final process of audio is in a subclass and the value of the dry wet knob is not a static variable. /august 2024
- partially solved: no UI at the moment but that will come soon.


# socials:
@publicservices.web IG
