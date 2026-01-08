#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

class LZ77 {

private:
    int searchSize;
    int lookaheadSize;
    struct Tuple {
        int offset;
        int length;
        char nextSymbol;
    };

public:
    LZ77(int searchBufferSize, int lookaheadBufferSize) {
        searchSize = searchBufferSize;
        lookaheadSize = lookaheadBufferSize;
    };
    void compress(std::istream& input, std::ostream& output);
    void decompress(std::istream& input, std::ostream& output);
};

void LZ77::compress(std::istream& input, std::ostream& output) {
    std::vector<char> searchBuffer(searchSize);
    std::vector<char> lookaheadBuffer(lookaheadSize);
    int searchLength = 0;
    int searchIndex = -1;
    int lookaheadIndex = 0;
    int shift = 0;
    bool read = true;

    // Fill the lookahead buffer
    int l = 0;
    while (l < lookaheadSize) {
        if (read) lookaheadBuffer[l] = input.get();
        else lookaheadBuffer[l] = EOF;

        if (lookaheadBuffer[lookaheadIndex] == EOF) read = false;
        ++l;
    }

    while (true) {
        // Shift the buffers
        for (int i = 0; i < shift; ++i) {
            searchIndex = (searchIndex + 1) % searchSize;
            searchBuffer[searchIndex] = lookaheadBuffer[lookaheadIndex];
            if (read) lookaheadBuffer[lookaheadIndex] = input.get();
            else lookaheadBuffer[lookaheadIndex] = EOF;

            // Stop reading new symbols when it arrives at EOF
            if (lookaheadBuffer[lookaheadIndex] == EOF) read = false;

            lookaheadIndex = (lookaheadIndex + 1) % lookaheadSize;

            if (searchLength < searchSize) ++searchLength;
        }

        // Stop compressing when the lookahead buffer is empty
        if (lookaheadBuffer[lookaheadIndex] == EOF) break;

        // Find the match
        int maxLen = 0;
        int maxOff = 0;
        for (int i = 0; i < searchLength && i < searchSize; ++i) {
            int matches = 0;
            int point;
            if (searchLength < searchSize) point = 0;
            else point = (searchIndex + 1) % searchSize;
            int head = point;
            char s = searchBuffer[(point + i) % searchSize];
            bool end = false;
            int count = 0;
            while (matches < lookaheadSize - 1 && point < searchLength && lookaheadBuffer[(lookaheadIndex + matches) % lookaheadSize] == s) {
                ++matches;
                point = (point + 1) % searchSize;
                if ((point + i) % searchSize == head) end = true;
                // If it reaches the end of the search buffer, continue through the lookahead
                if (!end) s = searchBuffer[(point + i) % searchSize];
                else {
                    s = lookaheadBuffer[lookaheadIndex + count];
                    ++count;
                }
            }
            if (matches > 0 && matches >= maxLen) {
                maxLen = matches;
                maxOff = searchLength - i;
            }
        }
        shift = maxLen + 1;

        // Output the encoding
        Tuple outp = { maxOff, maxLen, lookaheadBuffer[(lookaheadIndex + maxLen) % lookaheadSize] };
        if (outp.nextSymbol == EOF) { // Special treatment for the last tuple because there is not a next symbol
            outp.length -= 1;
            outp.nextSymbol = lookaheadBuffer[(lookaheadIndex + maxLen - 1) % lookaheadSize];
            if (outp.length == 0) outp.offset = 0;
        }
        output.write((char*)&outp, sizeof(outp));
    }
}

void LZ77::decompress(std::istream& input, std::ostream& output) {
    std::vector<char> outputBuffer(searchSize + lookaheadSize);
    int point = 0;

    while (input.peek() != EOF) {
        // Read the tuple from the input
        Tuple tuple;
        input.read((char*)&tuple, sizeof(tuple));

        // Copy the characters from the output buffer to the output stream
        for (int i = 0; i < tuple.length; i++) {
            char c = outputBuffer[(point - tuple.offset + i + searchSize + lookaheadSize) % (searchSize + lookaheadSize)];
            output.put(c);
            outputBuffer[(point + i) % (searchSize + lookaheadSize)] = c;
        }
        point = (point + tuple.length) % (searchSize + lookaheadSize);

        // Write the next character to the output
        output.put(tuple.nextSymbol);
        outputBuffer[point] = tuple.nextSymbol;
        point = (point + 1) % (searchSize + lookaheadSize);
    }
}


int main() {
    std::string in = "String to compress";
    std::istringstream inStream, inStream2;
    std::ostringstream outStream, outStream2;

    inStream.str(in);

    LZ77 lz77(5,8);

    lz77.compress(inStream, outStream);

    inStream2.str(outStream.str());
    lz77.decompress(inStream2, outStream2);

    assert(in==outStream2.str());

    return 0;
}
