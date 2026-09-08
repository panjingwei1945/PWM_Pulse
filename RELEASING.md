# Library Manager release preparation

## Before creating a release

- [x] Set author and maintainer to Jingwei Pan, with public contact jwpan@ion.ac.cn.
- [x] Set `url` to https://github.com/panjingwei1945/PWM_Pulse.
- [ ] Confirm the proposed version `0.3.0` and library name `PWM_Pulse`.
- [ ] Check the current Library Manager index for a case-insensitive name collision.
- [x] Add the selected MIT License with copyright attribution to Jingwei Pan; update README.
- [x] Run Arduino Lint in submission mode and resolve metadata issues (2026-09-08: strict check, no errors or warnings).
- [x] Compile the example for Mega 2560 and review warnings (2026-09-08: compilation passed, no warnings reported).
- [ ] Measure the outputs on hardware; compilation does not validate timing or optical power.
- [ ] Update README to remove preparation placeholders before release.

The implementation is preserved during packaging. Source files remain in the root, which Arduino supports; examples reside under `examples/`. Each example is intended to compile independently.

## Local checks

Run from the library root with Arduino CLI, Arduino Lint and Arduino AVR Boards installed:

```sh
arduino-lint --library-manager submit --compliance strict /absolute/path/to/PWM_Pulse
arduino-cli compile --fqbn arduino:avr:mega --library . examples/example_version001
```

Review both commands' output rather than suppressing failures. Metadata has been filled in, but that alone does not establish that validation passes.

On Windows PowerShell, use `(Get-Location).Path` as the Arduino Lint path argument. Passing a literal `.` caused a root-folder-name error with the installed lint version.

### Validation recorded on 2026-09-08

- Arduino Lint: strict submission check passed for the library and example, with no errors or warnings.
- Arduino CLI 1.4.1, Arduino AVR Boards 1.8.7, `arduino:avr:mega`: original example compiled successfully.
- Program storage: 6862 bytes; global variables: 367 bytes.
- No hardware upload or waveform measurement was performed.
- The GitHub repository was reachable via `git ls-remote` and advertised no branches or tags at the time of the check.

## First public release

1. Finish the checks above and review the final diff.
2. Publish the project at https://github.com/panjingwei1945/PWM_Pulse. `library.properties` must be at the repository root.
3. Commit the complete release files, then create and push the agreed version tag, for example `v0.3.0`. The tagged files must contain the matching metadata version.
4. Follow the official registry instructions to add the repository URL to `repositories.txt` in a pull request to `arduino/library-registry`.
5. Resolve registry check findings and wait for acceptance and indexing. Verify installation in Library Manager before announcing availability.

Later updates require a new metadata version and a new tag. Do not change an existing release tag to publish an update.

## Official references

- [Library specification](https://docs.arduino.cc/arduino-cli/library-specification/)
- [Registry submission instructions](https://github.com/arduino/library-registry)
- [Registry requirements](https://github.com/arduino/library-registry/blob/main/FAQ.md)
- [Arduino Lint](https://arduino.github.io/arduino-lint/latest/)
