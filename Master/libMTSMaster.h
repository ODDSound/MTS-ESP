/*
Copyright (C) 2021 by ODDSound Ltd. info@oddsound.com

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
*/

#ifndef libMTSMaster_h
#define libMTSMaster_h

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     MTS Master interface - for creating MTS Master plugins, one per session, which will control
     tuning for all MTS-ESP-compatible plugins in the session.

     On startup in the constructor:

        MTS_RegisterMaster();


     On shutdown in the destructor:

        MTS_DeregisterMaster();


     To determine whether the user has already instanced a master (don’t instance if this returns false) call:

        bool can_register_master = MTS_CanRegisterMaster();


     To configure the tunings for the entire session, call:

        double frequencies_in_hz[128]; // Fill this in
        MTS_SetNoteTunings(frequencies_in_hz);
     OR
        MTS_SetNoteTuning(frequency_in_hz, midinote);


     To tell clients to ignore a note, call:

        MTS_FilterNote(should_ignore, midinote, midichannel);

     Supply -1 for the midichannel argument if the note should be ignored on all MIDI channels.
     Note that it is optional for a client to provide a MIDI channel when querying whether a note
     should be filtered. If not provided, a client will filter any notes set via this function
     regardless of MIDI channel. Although encouraged, it is also optional for a client to check note
     filtering at all, so it is suggested to provide a frequency for all MIDI notes, even unmapped ones.
     Ideally unmapped notes should use the frequency of the next lowest mapped note, or next highest
     if there is none lower.


     To reset the ignored notes, call:

        MTS_ClearNoteFilter();


     To tell the user how many clients you can see connected, call:

        int num_connected = MTS_GetNumClients();


     To tell clients the scale name, call:

        MTS_ScaleName(“Scale name”);


     Some MIDI controllers for microtonal work have more than 128 keys where the same note number
     may be mapped to different frequencies across different MIDI channels. To support these, use:

        MTS_SetMultiChannel(is_part_of_the_multichannel_system, midichannel);

     And then:

        MTS_SetMultiChannelNoteTunings, MTS_SetMultiChannelNoteTuning, MTS_FilterNoteMultiChannel, MTS_ClearNoteFilterMultiChannel

     Which are as above, but take the MIDI channel as an extra parameter. Multi-channel support will only
     work with clients that provide a MIDI channel when querying both note filtering and retuning.
     Clients that don't provide a MIDI channel will use the frequencies and note filtering provided using
     the non-multi-channel functions, therefore it is advised to always provide a general tuning table in addition
     to a multi-channel one.
     
     
     After registering, you may wish to let the user know if they have an older version of the libMTS dynamic library
     installed which may not support some features in this version of the API:
     
        bool should_update = MTS_Master_ShouldUpdateLibrary();
     
     The latest version of libMTS will always be backward compatible with masters built with
     an older version of the API. Users can update libMTS using the installers at
     https://github.com/ODDSound/MTS-ESP/tree/main/libMTS.
     
     
     IPC support:
     
     MTS_HasIPC() allows you to check if the process in which the plug-in is running is using IPC for sharing MTS-ESP
     tuning data. If a DAW crashes, MTS_DeregisterMaster() may not get called. If this happens when using IPC the
     shared memory will persist and MTS_HasMaster() will still return true, even if no other master is instanced.
     To allow for this case we have included the MTS_Reinitialize() function which will reset the MTS-ESP library,
     including tuning tables, scale name, note filters, client count and master connection status.
     
     IMPORTANT: ONLY if MTS_CanRegisterMaster() returns false and MTS_HasIPC() returns true is it advisable to offer an option to
     the user to reinitialize MTS-ESP. Follow reinitialization with a call to MTS_RegisterMaster(). The code for registering
     as a master should follow this pattern:
     
         if (MTS_CanRegisterMaster())
             MTS_RegisterMaster();
         else
         {
             if (MTS_HasIPC())
             {
                 Warn user another master is already connected, but provide an option to reinitialize MTS-ESP in case there was a crash and no master is connected any more;
                 if (user clicks to reinitialize MTS-ESP)
                 {
                     MTS_Reinitialize();
                     MTS_RegisterMaster();
                 }
             }
             else
                 Warn user another master is already connected, do not provide an option to reinitialize MTS-ESP;
         }

     */


    // Register/deregister as a master. Call from the plugin constructor and destructor.
    extern void MTS_RegisterMaster();
    extern void MTS_DeregisterMaster();

    // Check if a master plugin is already instanced before registering, as only one Master may be registered at any one time.
    // Don't call MTS_RegisterMaster() if this returns false.
    extern bool MTS_CanRegisterMaster();

    // Check if the process in which the master plug-in is running is using IPC for sharing MTS-ESP tuning data.
    extern bool MTS_HasIPC();
    
    // Reset everything in the MTS-ESP library, including the master connection status and client count.
    // IMPORTANT: This is only intended to be called if IPC is in use and only after the process in which the master
    // plug-in is running crashes.
    extern void MTS_Reinitialize();
    
    // Check if the MTS-ESP dynamic library needs to be updated to use all features in this version of the API.
    extern bool MTS_Master_ShouldUpdateLibrary();

    // Returns the number of connected clients.
    extern int MTS_GetNumClients();

    // Set frequencies for 128 MIDI notes.
    extern void MTS_SetNoteTunings(const double *freqs);
    extern void MTS_SetNoteTuning(double freq, char midinote);
    
    // Set a scale name, so it can be displayed in clients or included in MTS sysex messages sent by a client.
    extern void MTS_SetScaleName(const char *name);

    // Set the period ratio of the scale, e.g for a period of an octave supply 2.0.
    extern void MTS_SetPeriodRatio(double periodRatio);

    // Supply information about how a scale is mapped to the MIDI note range.
    //  - Map size: the number of notes between each repetition of the scale pattern.
    //  - Map start key: the note on which the map starts. This could technically be any key at which the scale pattern repeats but would typically be the one below but nearest to the ref key.
    //  - Ref key: the note for which a frequency has been explicitly defined and from which the frequency of all other notes is established.
    // Negative values are invalid. If you do not wish to supply this information, or it is not relevant for the current tuning, set -1 as the argument. 
    // This will indicate to client plug-ins that they cannot use the queried values.
    // Using these functions is optional and if you don't call them at all clients will receive values of -1 by default.
    extern void MTS_SetMapSize(signed char size);
    extern void MTS_SetMapStartKey(signed char key);
    extern void MTS_SetRefKey(signed char key);

    // Instruct clients to filter midi notes e.g. because they are not mapped to any scale steps.
    // MIDI channel argument is optional, filtering will apply to all channels if not provided.
    // Range for midichannel argument is 0-15, or -1 for all channels.
    extern void MTS_FilterNote(bool doFilter, char midinote, signed char midichannel);
    extern void MTS_ClearNoteFilter();

    //-------------------------------------------------------------------------------------------------------

    // Optional set of functions for mutli-channel tuning table.
    // Range for midichannel arguments is 0-15.

    // Set whether a specific MIDI channel is included in the multi-channel tuning table.
    extern void MTS_SetMultiChannel(bool set, signed char midichannel);
    
    // Set frequencies for 128 MIDI notes on a specific MIDI channel.
    extern void MTS_SetMultiChannelNoteTunings(const double *freqs, signed char midichannel);
    extern void MTS_SetMultiChannelNoteTuning(double freq, char midinote, signed char midichannel);
    
    // Instruct clients to filter midi notes on a specific MIDI channel.
    extern void MTS_FilterNoteMultiChannel(bool doFilter, char midinote, signed char midichannel);
    extern void MTS_ClearNoteFilterMultiChannel(signed char midichannel);

#ifdef __cplusplus
}
#endif

#endif

