# Deck'o'Cards

This is a simple implementation of a deck of cards, with the minimum possible use of memory was a challenge set by a fellow co-worker.

Any player's hand can be represented within 8 bytes, and a deck with 16 + 24 bytes with the ability to peek upto several future cards, and to reshuffle. (Without the ability to peek, it can be just 16 bytes).

This does, however, incur a higher computational cost, but such is the eternal struggle with computers of memory vs cycles.