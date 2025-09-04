# libMTS Changelog

## [1.03] - 2025-09-04

- Changed map start key, map size and ref key default values to -1, to indicate they have not been supplied by a master
- Fixed tuning data not getting reset on de-registering master, so that old values don't linger when instancing a different master which doesn't support some MTS-ESP features

## [1.02] - 2021-12-22

- Fixed call to MTS_GetMultiChannelTuningTable so it does not return global table if the supplied channel is not in use
- Fixed Windows installer when running on 32-bit Windows


## [1.01] - 2021-11-11

- Fixed IPC on macOS
- Fixed multi-channel note filtering