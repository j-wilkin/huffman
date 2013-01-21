huffman
Jason Wilkin
1/21/13
=======

A c++ huffman encoder and decoder

Run the encoder on a plain text file:

    hencode test.txt

This deletes test.txt and creates a new encoded file test.txt.huf

Run the decoder on an encoded .huf file:

    hdecode test.txt.huf

This deletes test.txt.huf and creates the decoded file test.txt
