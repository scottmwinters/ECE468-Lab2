all: rle lzw testFiles

rle: rle_encode.c rle_decode.c
	gcc -Wall --pedantic -O2 -c rle_encode.c
	gcc -Wall --pedantic -O2 -c rle_decode.c
	gcc -Wall --pedantic -O2 rle_encode.o -o rleEncode 
	gcc -Wall --pedantic -O2 rle_decode.o -o rleDecode

lzw: lzw_encode.c lzw_decode.c
	gcc -Wall --pedantic -O2 -c lzw_encode.c
	gcc -Wall --pedantic -O2 -c lzw_decode.c
	gcc -Wall --pedantic -O2 lzw_encode.o -o lzwEncode
	gcc -Wall --pedantic -O2 lzw_decode.o -o lzwDecode 

testFiles: test_if_copy.c
	gcc -Wall --pedantic -O2 -c test_if_copy.c
	gcc -Wall --pedantic -O2 test_if_copy.o -o isCopy

clean:
	rm *.o rleEncode rleDecode lzwEncode lzwDecode isCopy
