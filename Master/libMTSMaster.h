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


     To determine whether the user has already instanced a master (don’t instance if so) call:

        bool already_have_a_master = MTS_HasMaster();


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
     work with clients that provide a MIDI channel both when requesting note filtering and retuning.
     Clients that don't provide a MIDI channel will use the frequencies and note filtering provided using
     the non-multi-channel functions, therefore it is advised to always provide a general mapping in addition
     to a multi-channel one.

     */


    // Register/deregister as a master. Call from the plugin constuctor and destructor.
    extern void MTS_RegisterMaster();
    extern void MTS_DeregisterMaster();

    // Check if a master plugin is already instanced before registering, as only one Master may be registered at any one time.
    extern bool MTS_HasMaster();

    // Returns the number of connected clients.
    extern int MTS_GetNumClients();

    // Set frequencies for 128 MIDI notes.
    extern void MTS_SetNoteTunings(const double *freqs);
    extern void MTS_SetNoteTuning(double freq, char midinote);
    
    // Set a scale name, so it can be displayed in clients or included in MTS sysex messages sent by a client.
    extern void MTS_SetScaleName(const char *name);

    // Instruct clients to filter midi notes e.g. because they are not mapped to any scale steps.
    // MIDI channel argument is optional, filtering will apply to all channels if not provided.
    // Range for midichannel argument is 0-15, or -1 for all channels.
    extern void MTS_FilterNote(bool doFilter, char midinote, char midichannel);
    extern void MTS_ClearNoteFilter();

    //-------------------------------------------------------------------------------------------------------

    // Optional set of funtions for mutli-channel mapping.
    // Range for midichannel arguments is 0-15.

    // Set whether a specific MIDI channel is included in the multi-channel mapping.
    extern void MTS_SetMultiChannel(bool set, char midichannel);
    
    // Set frequencies for 128 MIDI notes on a specific MIDI channel.
    extern void MTS_SetMultiChannelNoteTunings(const double *freqs, char midichannel);
    extern void MTS_SetMultiChannelNoteTuning(double freq, char midinote, char midichannel);
    
    // Instruct clients to filter midi notes on a specific MIDI channel.
    extern void MTS_FilterNoteMultiChannel(bool doFilter, char midinote, char midichannel);
    extern void MTS_ClearNoteFilterMultiChannel(char midichannel);

#ifdef __cplusplus
}
#endif

#endif

