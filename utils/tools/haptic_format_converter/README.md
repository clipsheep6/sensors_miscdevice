## Introduction

This is a python script to convert between OH Haptic JSON and HE Haptic JSON formats.

## Requirements

- Python 3.8+
- `pip3 install jsonschema` if needed

## Usage
```
usage: converter.py [-h] [-o OUTPUT] -f {oh,he_v1,he_v2} [-s SCHEMA_DIR] [-v] input

Convert between OH Haptic JSON and HE Haptic JSON formats.

positional arguments:
  input                 Path to the input JSON file or directory.

options:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        Path to the output directory (default: input directory with '_out' suffix).
  -f {oh,he_v1,he_v2}, --format {oh,he_v1,he_v2}
                        Target format: 'oh', 'he_v1', or 'he_v2'.
  -s SCHEMA_DIR, --schema_dir SCHEMA_DIR
                        Directory containing JSON schema files (default: 'schemas').
  -v, --version_suffix  Include version suffix ('_v1' or '_v2') in output HE file names.

```


## Run command example

convert oh to he_v2

```
./converter.py ../tests/test_data/oh_sample.json -o output_dir -f he_v2

```


## Test command example

Need add more test cases to cover corner case

```
python3 -m unittest discover -s tests
```
