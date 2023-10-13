# MTS-ESP Library

The MTS-ESP library is a simple but versatile C/C++ library for adding microtuning support to audio and MIDI plugins.  It allows for a single master plugin to simultaneously control the tuning of any number of connected client plugins across a DAW session.  Connection between a master and clients is automatic and invisible.

A free master plugin, [ODDSound MTS-ESP Mini](https://oddsound.com/mtsespmini.php), supports loading of .scl, .kbm, .tun and MTS SysEx files and provides a simple, no-cost way for anyone to start using MTS-ESP.

**Most developers will have no reason to read further than the Client section below, the rest is not essential unless you plan to build a master plugin.**


## Client

Any plugin that receives and processes MIDI note data can be made compatible with MTS-ESP using the Client API.  All it takes is to include libMTSClient.h and libMTSClient.cpp from the 'Client' folder in your build.

A client can query the re-tuning for a given MIDI note number either as an absolute frequency value or as the difference from the standard 12-TET tuning (i.e. 440*2^((midi_note-69) / 12)).  **NOTE:** Ideally it should do this as often as possible whilst a note is playing or sound is being processed, not just when a note-on is received, so that note frequencies can update in real-time (along the flight of a note) if the tuning is changed or automated in the master plugin.

When not connected to a master plugin, a client will automatically revert to a local tuning table, set to 12-TET by default.  As a bonus, this local tuning table can be updated with MIDI Tuning Standard (MTS) SysEx messages.  The client API includes a function that parses incoming MIDI SysEx data and identifies all message formats defined in the MTS standard.  Therefore even without using the MTS-ESP system, the client API can still add microtuning support to a plugin.

Some other useful optional implementation suggestions include:

* When a note-on message is received, a client should check to see if the note should be filtered out and ignored.  This allows a master plugin to define a keyboard map that includes unmapped keys.
* Allow users the choice of querying retuning only at note-on, or continuously whilst notes are playing.
* Display the MTS-ESP connection status on your UI.

## Max Package

A [Max Package](http://github.com/ODDSound/MTS-ESP-Max-Package) is available which includes objects that allow Max for Live devices to support MTS-ESP as a client.  Source code for the Max objects is included.


## Master

A master plugin dictates the tuning and keyboard mapping that all connected clients adhere to.

Only one master plugin may connect via MTS-ESP at any one time.  On instancing, a master plugin should check whether another master plugin has already been instanced before registering itself.

A master can optionally specify notes that clients should filter out, allowing e.g. a keyboard map with unmapped keys, or for specific keys to be used to switch tunings.


## Multi-Channel Mapping

The MTS-ESP library includes support for multi-channel tuning tables, usually used with MIDI controllers designed for microtonal music and having more than 128 keys.

It is optional for a client to provide a MIDI channel when querying tuning or whether a note should be filtered, as with some plugins the channel data may not be available.  For a client plugin to support multi-channel tuning tables it must supply a MIDI channel wherever possible.  The MTS-ESP library automatically detects if this is the case and will flag a client as able to support multi-channel tuning tables.  Since this is not guaranteed, any master plugin that implements multi-channel tuning tables should also provide a regular single-channel tuning table as a fallback.


## libMTS

This is the dynamic library through which a master connects to clients.  If you are building a master plugin, this should be placed at:

**Windows 64bit:** Program Files\Common Files\MTS-ESP (64 bit library) and Program Files (x86)\Common Files\MTS-ESP (32 bit library)  
**Windows 32bit:** Program Files\Common Files\MTS-ESP (32 bit library)  
**Mac OSX:** /Library/Application Support/MTS-ESP  
**Linux:** /usr/local/lib  
  
Windows and OSX Installers are provided which you can bundle into your own installer or, if you prefer, just include the library files and install to the above locations.  The Mac installers are notarised and compatible with OSX 10.15+.


## IPC Support

MTS-ESP supports inter-process communication, to allow for hosts which can run plug-ins in separate processes.  If the process in which the master plug-in is running crashes, it will not have a chance to de-register itself and clear the flag in MTS-ESP that keeps track of whether a Master is instanced.  For this case, a function is provided with which a master plug-in can re-initialize the MTS-ESP library to its default state, after which it can attempt to register itself again.

It is possible to disable IPC support by editing the config file in this repo, MTS-ESP.conf, and placing it at:

**Windows 64bit:** Program Files\Common Files\MTS-ESP (64 bit library) and Program Files (x86)\Common Files\MTS-ESP (32 bit library)  
**Windows 32bit:** Program Files\Common Files\MTS-ESP (32 bit library)  
**Mac OSX:** /Library/Application Support/MTS-ESP  
**Linux:** /usr/local/etc  

If the file is not found, IPC support will be enabled by default.  It is important that the function to re-initialize MTS-ESP is only called if IPC is enabled.  A further function is provided so a master plug-in can check if this is the case.


For any queries, assistance or bug reports contact tech@oddsound.com.

An MTS-ESP python wrapper for Linux can be found at [mtespy](https://github.com/narenratan/mtsespy), with thanks to [narenratan](https://github.com/narenratan).
