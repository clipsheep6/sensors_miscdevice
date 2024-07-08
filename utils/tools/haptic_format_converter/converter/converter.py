#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Haptic JSON Converter

This tool converts between OH Haptic JSON and HE Haptic JSON formats (v1 and v2).

Version: v0.0.1

Usage:
    converter.py <input> [-o <output>] -f <format> [-s <schema_dir>] [-v]

Arguments:
    input               Path to the input JSON file or directory.
    -o, --output        Path to the output directory (default: input directory with '_out' suffix).
    -f, --format        Target format: 'oh', 'he_v1', or 'he_v2'.
    -s, --schema_dir    Directory containing JSON schema files (default: 'schemas').
    -v, --version_suffix Include version suffix ('_v1' or '_v2') in output HE file names.
"""

import json
import argparse
import os
from pathlib import Path
import logging
from typing import Union, Dict, Any, Tuple

import jsonschema
from jsonschema import validate

# Configure logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s')

# Constants for target formats
FORMAT_OH = 'oh'
FORMAT_HE_V1 = 'he_v1'
FORMAT_HE_V2 = 'he_v2'

# Type alias for JSON data
JsonData = Dict[str, Any]


def load_schema(file_path: Union[str, Path]) -> JsonData:
    """
    Load a JSON schema from the specified file.

    Args:
        file_path (Union[str, Path]): Path to the JSON schema file.

    Returns:
        JsonData: Parsed JSON schema.
    """
    file_path = Path(file_path)
    try:
        with file_path.open('r', encoding='utf-8') as file:
            return json.load(file)
    except (FileNotFoundError, json.JSONDecodeError) as err:
        logging.error("Error loading schema from %s: %s", file_path, err)
        raise


def read_json(file_path: Union[str, Path]) -> JsonData:
    """
    Read JSON data from a file.

    Args:
        file_path (Union[str, Path]): Path to the JSON file.

    Returns:
        JsonData: Parsed JSON data.
    """
    file_path = Path(file_path)
    try:
        with file_path.open('r', encoding='utf-8') as file:
            return json.load(file)
    except (FileNotFoundError, json.JSONDecodeError) as err:
        logging.error("Error reading JSON file %s: %s", file_path, err)
        raise


def write_json(data: JsonData, file_path: Path) -> None:
    """
    Write JSON data to a file.

    Args:
        data (JsonData): JSON data to write.
        file_path (Path): Path to the output file.
    """
    try:
        file_path.parent.mkdir(parents=True, exist_ok=True)
        with file_path.open('w', encoding='utf-8') as file:
            json.dump(data, file, indent=2)
    except (OSError, json.JSONDecodeError) as err:
        logging.error("Error writing JSON file %s: %s", file_path, err)
        raise


def validate_json(data: JsonData, schema: JsonData) -> Tuple[bool, Union[None, jsonschema.exceptions.ValidationError]]:
    """
    Validate JSON data against a schema.

    Args:
        data (JsonData): JSON data to validate.
        schema (JsonData): JSON schema to validate against.

    Returns:
        Tuple[bool, Union[None, jsonschema.exceptions.ValidationError]]: Validation result and error if invalid.
    """
    try:
        validate(data, schema)
        return True, None
    except jsonschema.exceptions.ValidationError as err:
        return False, err


def convert_oh_to_he_v1(oh_data: JsonData) -> JsonData:
    """
    Convert OH JSON data to HE v1 format.

    Args:
        oh_data (JsonData): OH JSON data.

    Returns:
        JsonData: Converted HE v1 JSON data.
    """
    he_data = {
        "Metadata": {
            "Version": 1
        },
        "Pattern": []
    }

    for channel in oh_data['Channels']:
        for pattern in channel['Pattern']:
            event = pattern['Event']
            he_event = {
                "Type": event['Type'],
                "RelativeTime": event['StartTime'],
                "Parameters": {
                    "Intensity": event['Parameters']['Intensity'],
                    "Frequency": event['Parameters']['Frequency']
                }
            }
            if event['Type'] == 'continuous':
                he_event['Duration'] = event['Duration']
                if 'Curve' in event['Parameters']:
                    he_event['Parameters']['Curve'] = [
                        {
                            "Time": curve_point['Time'],
                            "Intensity": curve_point.get('Intensity', 0),
                            "Frequency": curve_point.get('Frequency', 0)
                        }
                        for curve_point in event['Parameters']['Curve']
                    ]
                else:
                    # Add default curve information if 'Curve' is not in parameters
                    he_event['Parameters']['Curve'] = [
                        {
                            "Time": time,
                            "Intensity": 100,
                            "Frequency": 0
                        }
                        for time in range(4)  # Add four default points to satisfy CURVE_POINT_NUM_MIN
                    ]
            he_data['Pattern'].append(he_event)

    return he_data


def convert_oh_to_he_v2(oh_data: JsonData) -> JsonData:
    """
    Convert OH JSON data to HE v2 format.

    Args:
        oh_data (JsonData): OH JSON data.

    Returns:
        JsonData: Converted HE v2 JSON data.
    """
    he_v2_data = {
        "Metadata": {
            "Version": 2
        },
        "PatternList": []
    }

    current_pattern = None
    last_event_end_time = -1  # Tracks the end time of the last event in the current pattern

    for channel in oh_data["Channels"]:
        for pattern in channel["Pattern"]:
            event = pattern["Event"]
            event_start_time = event["StartTime"]
            event_end_time = event_start_time + \
                (event["Duration"] if event["Type"] ==
                 "continuous" else 48)  # Default transient duration

            if current_pattern is None or not isinstance(current_pattern, dict) or len(current_pattern.get("Pattern", [])) >= 16 \
                or (last_event_end_time != -1 and event_start_time - last_event_end_time > 1000):
                if current_pattern and isinstance(current_pattern, dict):
                    he_v2_data["PatternList"].append(current_pattern)
                current_pattern = {
                    "AbsoluteTime": event_start_time,
                    "Pattern": []
                }

            if event["Type"] == "continuous":
                he_event = {
                    "Type": event["Type"],
                    "RelativeTime": event_start_time - current_pattern["AbsoluteTime"],
                    "Duration": event["Duration"],
                    "Parameters": {
                        "Intensity": event["Parameters"]["Intensity"],
                        "Frequency": event["Parameters"].get("Frequency", 50)
                    }
                }
                if "Curve" in event["Parameters"]:
                    he_event["Parameters"]["Curve"] = [
                        {
                            "Time": point["Time"],
                            "Intensity": point.get("Intensity", 100),
                            # Assigning default value of 0 if Frequency is None
                            "Frequency": point.get("Frequency", 0)
                        }
                        for point in event["Parameters"]["Curve"]
                        if "Intensity" in point or "Frequency" in point
                    ]
                else:
                    # Add default curve information if 'Curve' is not in parameters
                    he_event["Parameters"]["Curve"] = [
                        {
                            "Time": time,
                            "Intensity": 100,
                            "Frequency": 0
                        }
                        for time in range(0, 4)  # Add four default points to satisfy least need curve in hev2
                    ]
                current_pattern["Pattern"].append(he_event)
            else:
                he_event = {
                    "Type": event["Type"],
                    "RelativeTime": event_start_time - current_pattern["AbsoluteTime"],
                    "Parameters": {
                        "Intensity": event["Parameters"]["Intensity"],
                        "Frequency": event["Parameters"].get("Frequency", 50)
                    }
                }
                current_pattern["Pattern"].append(he_event)
            last_event_end_time = event_end_time

    if current_pattern:
        he_v2_data["PatternList"].append(current_pattern)

    return he_v2_data


def convert_he_v1_to_v2(he_v1_data: JsonData) -> JsonData:
    """
    Convert HE V1 JSON data to HE V2 format.

    Args:
        data (JsonData): HE V1 JSON data.

    Returns:
        JsonData: Converted HE V2 JSON data.
    """
    converted_data = {
        "Metadata": {
            "Version": 2
        },
        "PatternList": []
    }

    current_pattern = None
    last_event_end_time = -1  # Tracks the end time of the last event in the current pattern

    for event in he_v1_data["Pattern"]:
        event_start_time = event["RelativeTime"]
        event_end_time = event_start_time + \
            (event["Duration"] if event["Type"] == "continuous" else 48)

        if current_pattern is None or not isinstance(current_pattern, dict) or len(current_pattern.get("Pattern", [])) >= 16 \
            or (last_event_end_time != -1 and event_start_time - last_event_end_time > 1000):
            if current_pattern and isinstance(current_pattern, dict):
                converted_data["PatternList"].append(current_pattern)
            current_pattern = {
                "AbsoluteTime": event_start_time,
                "Pattern": []
            }

        # Order of dictionary insertion matters
        event_dict = {
            "Type": event["Type"],
            "RelativeTime": event_start_time - current_pattern["AbsoluteTime"]
        }

        if event["Type"] == "continuous":
            event_dict["Duration"] = event["Duration"]

        event_dict["Parameters"] = event["Parameters"]

        current_pattern["Pattern"].append(event_dict)
        last_event_end_time = event_end_time

    if current_pattern:
        converted_data["PatternList"].append(current_pattern)

    return converted_data


def convert_he_v2_to_v1(he_v2_data: JsonData) -> JsonData:
    """
    Convert HE v2 JSON data to HE v1 format.

    Args:
        he_v2_data (JsonData): HE v2 JSON data.

    Returns:
        JsonData: Converted HE v1 JSON data.
    """
    he_v1_data = {
        "Metadata": {
            "Version": 1
        },
        "Pattern": []
    }

    for pattern_list_entry in he_v2_data['PatternList']:
        for event in pattern_list_entry['Pattern']:
            he_v1_event = event.copy()
            he_v1_event['RelativeTime'] = event['RelativeTime'] + \
                pattern_list_entry['AbsoluteTime']
            he_v1_data['Pattern'].append(he_v1_event)

    return he_v1_data


def clamp(value: float, min_value: float, max_value: float) -> float:
    """
    Clamp a value between a minimum and maximum value.

    Args:
        value (float): Value to clamp.
        min_value (float): Minimum value.
        max_value (float): Maximum value.

    Returns:
        float: Clamped value.
    """
    return max(min_value, min(value, max_value))


def convert_he_v1_to_oh(he_v1_data: JsonData) -> JsonData:
    """
    Convert HE v1 JSON data to OH format.

    Args:
        he_v1_data (JsonData): HE v1 JSON data.

    Returns:
        JsonData: Converted OH JSON data.
    """
    output_data: JsonData = {
        "MetaData": {
            "Version": 1.0,
            "ChannelNumber": 1
        },
        "Channels": [{
            "Parameters": {
                "Index": 0
            },
            "Pattern": []
        }]
    }

    for pattern in he_v1_data.get('Pattern', []):
        event = {
            "Event": {
                "Type": pattern['Type'],
                "StartTime": pattern['RelativeTime'],
                "Parameters": {
                    "Intensity": clamp(pattern['Parameters']['Intensity'], 0, 100),
                    "Frequency": clamp(pattern['Parameters'].get('Frequency', 50), -100, 100)
                }
            }
        }
        if pattern['Type'] == 'continuous':
            event['Event']['Duration'] = pattern['Duration']
            event['Event']['Parameters']['Curve'] = [
                {
                    "Time": point['Time'],
                    "Intensity": clamp(point.get('Intensity', 100), 0, 100),
                    "Frequency": clamp(point.get('Frequency', 0), -100, 100)
                }
                for point in pattern['Parameters'].get('Curve', [])
            ]
        output_data['Channels'][0]['Pattern'].append(event)

    return output_data


def convert_he_v2_to_oh(he_v2_data: JsonData) -> JsonData:
    """
    Convert HE v2 JSON data to OH format.

    Args:
        he_v2_data (JsonData): HE v2 JSON data.

    Returns:
        JsonData: Converted OH JSON data.
    """
    event_num_max = 128

    output_data = {
        "MetaData": {
            "Version": 1.0,
            "ChannelNumber": 1
        },
        "Channels": [
            {
                "Parameters": {
                    "Index": 0
                },
                "Pattern": []
            }
        ]
    }

    all_events = []

    for pattern_list_entry in he_v2_data.get('PatternList', []):
        absolute_time = pattern_list_entry.get('AbsoluteTime', 0)
        for pattern in pattern_list_entry.get('Pattern', []):
            if pattern['Type'] == 'continuous':
                event = {
                    "Event": {
                        "Type": pattern['Type'],
                        "StartTime": absolute_time + pattern.get('RelativeTime', 0),
                        "Duration": clamp(pattern.get('Duration', 1000), 1, 5000),
                        "Parameters": {
                            "Intensity": clamp(pattern['Parameters'].get('Intensity', 100), 0, 100),
                            "Frequency": clamp(pattern['Parameters'].get('Frequency', 50), -100, 100),
                            "Curve": [
                                {
                                    "Time": clamp(point.get('Time', 0), 0, 10000),
                                    "Intensity": clamp(point.get('Intensity', 100), 0, 100),
                                    "Frequency": clamp(point.get('Frequency', 0), -100, 100)
                                }
                                for point in pattern['Parameters'].get('Curve', [])
                            ]
                        }
                    }
                }
            else:
                event = {
                    "Event": {
                        "Type": pattern['Type'],
                        "StartTime": absolute_time + pattern.get('RelativeTime', 0),
                        "Parameters": {
                            "Intensity": clamp(pattern['Parameters'].get('Intensity', 100), 0, 100),
                            "Frequency": clamp(pattern['Parameters'].get('Frequency', 50), -100, 100)
                        }
                    }
                }
            all_events.append(event)

    all_events.sort(key=lambda e: e['Event']['StartTime'])

    for event in all_events:
        index = event['Event'].get('Index', 0)
        channel_found = False

        for channel in output_data['Channels']:
            if channel['Parameters']['Index'] == index:
                if len(channel['Pattern']) < event_num_max:
                    channel['Pattern'].append(event)
                channel_found = True
                break

        if not channel_found:
            if len(output_data['Channels']) < 3:
                new_channel = {
                    "Parameters": {
                        "Index": index
                    },
                    "Pattern": [event]
                }
                output_data['Channels'].append(new_channel)

    return output_data


def process_file(input_file: Union[str, Path], output_dir: Union[str, Path], target_format: str, schema_dir: Union[str, Path], version_suffix: bool) -> None:
    """
    Process a single JSON file and convert it to the target format.

    Args:
        input_file (Union[str, Path]): Path to the input JSON file.
        output_dir (Union[str, Path]): Path to the output directory.
        target_format (str): Target format: 'oh', 'he_v1', or 'he_v2'.
        schema_dir (Union[str, Path]): Directory containing JSON schema files.
        version_suffix (bool): Include version suffix ('_v1' or '_v2') in output HE file names.
    """
    input_file = Path(input_file)
    output_dir = Path(output_dir)
    schema_dir = Path(schema_dir)

    try:
        input_data = read_json(input_file)

        # Load schemas
        schemas = {
            FORMAT_OH: load_schema(os.path.join(schema_dir, 'oh_schema.json')),
            FORMAT_HE_V1: load_schema(os.path.join(schema_dir, 'he_v1_schema.json')),
            FORMAT_HE_V2: load_schema(os.path.join(schema_dir, 'he_v2_schema.json')),
        }

        # Determine the schema to validate against
        input_format = None
        for in_format, schema in schemas.items():
            is_valid, _ = validate_json(input_data, schema)
            if is_valid:
                input_format = in_format
                break

        if not input_format:
            logging.error("No valid schema found for file %s", input_file)
            return

        # Define a mapping of conversion functions and output extensions
        conversion_map = {
            (FORMAT_OH, FORMAT_HE_V1): (convert_oh_to_he_v1, '_v1.he', '.he'),
            (FORMAT_OH, FORMAT_HE_V2): (convert_oh_to_he_v2, '_v2.he', '.he'),
            (FORMAT_HE_V1, FORMAT_OH): (convert_he_v1_to_oh, '.json', '.json'),
            (FORMAT_HE_V1, FORMAT_HE_V2): (convert_he_v1_to_v2, '_v2.he', '.he'),
            (FORMAT_HE_V2, FORMAT_OH): (convert_he_v2_to_oh, '.json', '.json'),
            (FORMAT_HE_V2, FORMAT_HE_V1): (convert_he_v2_to_v1, '_v1.he', '.he'),
        }

        # Perform conversion based on input format and target format
        if (input_format, target_format) in conversion_map:
            convert_func, versioned_ext, default_ext = conversion_map[(input_format, target_format)]
            output_data = convert_func(input_data)
            output_ext = versioned_ext if version_suffix else default_ext
        else:
            output_data = input_data if input_format in [FORMAT_OH, FORMAT_HE_V1, FORMAT_HE_V2] else None
            output_ext = '.json' if input_format == FORMAT_OH else (f'_{input_format.lower()}.he' if version_suffix else '.he')
            if output_data is None:
                logging.error("Unsupported input format for file %s", input_file)
                return

        # Validate the output data with the target format schema
        target_schema = schemas[target_format]
        is_valid, error = validate_json(output_data, target_schema)
        if not is_valid:
            logging.error("Validation error for the converted data against %s schema: %s", target_format, error)
            return

        output_file = output_dir / (input_file.stem + output_ext)
        write_json(output_data, output_file)
    except (FileNotFoundError, json.JSONDecodeError, jsonschema.exceptions.ValidationError) as err:
        logging.error("Error processing file %s: %s", input_file, err)


def process_directory(input_dir: Union[str, Path], output_dir: Union[str, Path], target_format: str, schema_dir: Union[str, Path], version_suffix: bool) -> None:
    """
    Process all JSON files in a directory and convert them to the target format.

    Args:
        input_dir (Union[str, Path]): Path to the input directory.
        output_dir (Union[str, Path]): Path to the output directory.
        target_format (str): Target format: 'oh', 'he_v1', or 'he_v2'.
        schema_dir (Union[str, Path]): Directory containing JSON schema files.
        version_suffix (bool): Include version suffix ('_v1' or '_v2') in output HE file names.
    """
    input_dir = Path(input_dir)
    output_dir = Path(output_dir)

    for root, _, files in os.walk(input_dir):
        for file in files:
            if file.endswith('.json') or file.endswith('.he'):
                input_file = os.path.join(root, file)
                process_file(input_file, output_dir, target_format,
                             schema_dir, version_suffix)


def main() -> None:
    """
    Main entry point of the script. Parses arguments and processes the input accordingly.
    """
    parser = argparse.ArgumentParser(
        description="Convert between OH Haptic JSON and HE Haptic JSON formats.")
    parser.add_argument("input", type=Path,
                        help="Path to the input JSON file or directory.")
    parser.add_argument("-o", "--output", type=Path,
                        help="Path to the output dir (default: input dir with '_out' suffix).")
    parser.add_argument("-f", "--format", choices=[FORMAT_OH, FORMAT_HE_V1, FORMAT_HE_V2],
                        required=True, help="Target format: 'oh', 'he_v1', or 'he_v2'.")
    parser.add_argument("-s", "--schema_dir", type=Path, default=Path("schemas"),
                        help="Directory containing JSON schema files (default: 'schemas').")
    parser.add_argument("-v", "--version_suffix", action="store_true",
                        help="Include version suffix ('_v1' or '_v2') in output HE file names.")

    args = parser.parse_args()

    if args.input.is_file():
        input_file = args.input
        output_dir = args.output or args.input.parent
        process_file(input_file, output_dir, args.format,
                     args.schema_dir, args.version_suffix)
    elif args.input.is_dir():
        input_dir = args.input
        output_dir = args.output or input_dir.with_name(
            input_dir.name + '_out')
        process_directory(input_dir, output_dir, args.format,
                          args.schema_dir, args.version_suffix)
    else:
        raise ValueError(f"Invalid input path: {args.input}")

if __name__ == "__main__":
    main()
