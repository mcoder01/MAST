/bin/g++ -Wall -Wextra -g \
    'source/crypto.cpp' \
    'source/cryptofile.cpp' \
    'source/cryptofilemap.cpp' \
    'test/test.cpp' \
-lpthread -lcryptopp -o test.out