## build mxchip az3166 base OS with Azure IoT Central sample

- Install [iotz](https://github.com/azure/iotz)

- Initialize `mbed` subsystem for `iotz` (run under repository root folder)
  Run => `iotz init mbed`

- Update sample application definitions under `mxchip_advanced/inc/definitions.h`

- Compile!
  Run => `iotz compile`