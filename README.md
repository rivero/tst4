
# Bus Variant Analysis

This project analyzes bus variants based on stop data. It calculates distances, identifies the longest variant, and finds mid-way coordinates on the longest variant.

## Overview

The code processes XML files containing bus stop information for different variants. It performs the following tasks:

1. Reads variant data from XML files.
2. Calculates total distances for each variant.
3. Identifies the longest variant by total traveling distance.
4. Determines mid-way/half-distance point coordinates on the longest variant.

## Code Structure

- `main.cpp`: Entry point of the program.
- `Variant.h`: Header file for the `Variant` class.
- `Stop.h`: Header file for the `Stop` class.
- `filereader.h`: Header file for reading XML files.
- Other implementation files (not provided in this snippet).

## Usage

1. Ensure that the necessary XML files are available in the specified download folder (`C:/tools/repos/tst4/xml/`).
2. Compile and run the program.
3. View the console output for variant information, distances, and mid-way coordinates.

## Important Notes

- The program assumes a straight-line path between consecutive stops for distance calculation.
- Stop data is organized by variant.
- The longest variant is determined based on total traveling distance.

## Contact

For any questions or issues, feel free to reach out to:

- Jason J. Rivero
- Email: sr.jrivero@gmail.com
- Phone: 1.778.552.5212

---

Feel free to customize this `readme.md` file further based on your project's specific requirements. If you need additional assistance or have any other requests, feel free to ask! 😊
