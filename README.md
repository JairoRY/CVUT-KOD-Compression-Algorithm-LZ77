# LZ77 Compression Algorithm

This repository contains a C++ implementation of the LZ77 compression algorithm. The project was developed as part of the Data Compression (KOD) course at the Czech Technical University (CVUT) in Prague, in the context of studies at the Universitat Politècnica de Catalunya (UPC).

The algorithm focuses on dictionary-based lossless data compression using a sliding window approach to identify and encode repeated patterns in a data stream.

## Project Structure

The project consists of a standalone implementation:

* **LZ77.cpp**: The main source file containing the implementation of the sliding window logic, including functions for both the compression of raw data into (offset, length, next character) triples and the corresponding decompression process.

## Getting Started

### Prerequisites

You will need a C++ compiler (such as g++) installed on your system.

```bash
sudo apt-get install g++

```

### Usage

1. **Clone the repository:**
```bash
git clone https://github.com/JairoRY/CVUT-KOD-Compression-Algorithm-LZ77.git
cd CVUT-KOD-Compression-Algorithm-LZ77

```


2. **Compile the program:**
```bash
g++ -o lz77 LZ77.cpp

```


3. **Run the simulation:**
```bash
./lz77

```



## Algorithm Details

The LZ77 algorithm achieves compression by replacing repeated occurrences of data with references to a single copy of that data existing earlier in the uncompressed data stream. The implementation includes:

* **Sliding Window**: A memory buffer that keeps track of the most recent portion of the data stream, divided into a search buffer and a look-ahead buffer.
* **Match Finding**: A search mechanism that scans the search buffer for the longest string that matches the start of the look-ahead buffer.
* **Encoding**: Outputting a sequence of tokens. Each token is a tuple consisting of the distance back to the match (offset), the number of characters that match (length), and the first character that deviates from the match.
* **Efficiency**: The implementation is designed to demonstrate the fundamental trade-offs between window size and compression ratio.
