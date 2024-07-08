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
Haptic JSON Converter testcases

run `python3 -m unittest discover -s tests` in repo base folder:

"""

import unittest
from pathlib import Path
import json

from converter.converter import (
    convert_oh_to_he_v1,
    convert_oh_to_he_v2,
    convert_he_v1_to_oh,
    convert_he_v2_to_oh,
    convert_he_v1_to_v2,
    convert_he_v2_to_v1
)


class TestConverter(unittest.TestCase):

    def setUp(self):
        self.schema_dir = Path('converter/schemas')
        self.test_data_dir = Path('tests/test_data')
        self.output_dir = Path('converter/output_dir')
        self.output_dir.mkdir(parents=True, exist_ok=True)

    def read_json(self, file_path):
        with open(file_path, 'r', encoding='utf-8') as file:
            return json.load(file)

    def test_oh_to_he_v1(self):
        input_data = self.read_json(self.test_data_dir / 'oh_sample.json')
        expected_output = self.read_json(
            self.test_data_dir / 'oh_sample_to_he_v1.he')
        converted_data = convert_oh_to_he_v1(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_oh_to_he_v2(self):
        input_data = self.read_json(self.test_data_dir / 'oh_sample.json')
        expected_output = self.read_json(
            self.test_data_dir / 'oh_sample_to_he_v2.he')
        converted_data = convert_oh_to_he_v2(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_he_v1_to_oh(self):
        input_data = self.read_json(self.test_data_dir / 'he_v1_sample.he')
        expected_output = self.read_json(
            self.test_data_dir / 'he_v1_sample_to_oh.json')
        converted_data = convert_he_v1_to_oh(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_he_v2_to_oh(self):
        input_data = self.read_json(self.test_data_dir / 'he_v2_sample.he')
        expected_output = self.read_json(
            self.test_data_dir / 'he_v2_sample_to_oh.json')
        converted_data = convert_he_v2_to_oh(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_he_v1_to_v2(self):
        input_data = self.read_json(self.test_data_dir / 'he_v1_sample.he')
        expected_output = self.read_json(
            self.test_data_dir / 'he_v1_sample_to_he_v2.he')
        converted_data = convert_he_v1_to_v2(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_he_v2_to_v1(self):
        input_data = self.read_json(self.test_data_dir / 'he_v2_sample.he')
        expected_output = self.read_json(
            self.test_data_dir / 'he_v2_sample_to_he_v1.he')
        converted_data = convert_he_v2_to_v1(input_data)
        self.assertEqual(converted_data, expected_output)

    def test_he_v1_to_v2_to_v1(self):
        input_data = self.read_json(self.test_data_dir / 'he_v1_sample.he')
        expected_output1 = self.read_json(
            self.test_data_dir / 'he_v1_sample_to_he_v2.he')
        converted_data1 = convert_he_v1_to_v2(input_data)
        self.assertEqual(converted_data1, expected_output1)
        converted_data2 = convert_he_v2_to_v1(converted_data1)
        self.assertEqual(converted_data2, input_data)

    def test_he_v2_to_he_v1_to_he_v2(self):
        input_data = self.read_json(self.test_data_dir / 'he_v2_sample.he')
        expected_output1 = self.read_json(
            self.test_data_dir / 'he_v2_sample_to_he_v1.he')
        converted_data1 = convert_he_v2_to_v1(input_data)
        self.assertEqual(converted_data1, expected_output1)
        converted_data2 = convert_he_v1_to_v2(converted_data1)
        self.assertEqual(converted_data2, input_data)

    def test_oh_to_he_v2_to_oh(self):
        input_data = self.read_json(self.test_data_dir / 'oh_sample_1.json')
        expected_output1 = self.read_json(
            self.test_data_dir / 'oh_sample_to_he_v2.he')
        converted_data1 = convert_oh_to_he_v2(input_data)
        self.assertEqual(converted_data1, expected_output1)
        converted_data2 = convert_he_v2_to_oh(converted_data1)
        self.assertEqual(converted_data2, input_data)

    def test_he_v2_to_oh_to_he_v2(self):
        input_data = self.read_json(self.test_data_dir / 'he_v2_sample.he')
        expected_output1 = self.read_json(
            self.test_data_dir / 'he_v2_sample_to_oh.json')
        converted_data1 = convert_he_v2_to_oh(input_data)
        self.assertEqual(converted_data1, expected_output1)
        converted_data2 = convert_oh_to_he_v2(converted_data1)
        self.assertEqual(converted_data2, input_data)

    def test_oh_to_he_v2_with_default_curve(self):
        input_data = self.read_json(self.test_data_dir / 'oh_no_curve_sample.json')
        expected_output = self.read_json(
            self.test_data_dir / 'oh_no_curve_sample_to_he_v2.json')
        converted_data = convert_oh_to_he_v2(input_data)
        self.assertEqual(converted_data, expected_output)
        
    def test_oh_to_he_v1_with_default_curve(self):
        input_data = self.read_json(self.test_data_dir / 'oh_no_curve_sample.json')
        expected_output = self.read_json(
            self.test_data_dir / 'oh_no_curve_sample_to_he_v1.json')
        converted_data = convert_oh_to_he_v1(input_data)
        self.assertEqual(converted_data, expected_output)

if __name__ == '__main__':
    unittest.main()
