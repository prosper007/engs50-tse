# ENGS 50 Module 6 Testing Script F20
# Author Brandon Guzman


./querier ./ # number of arg test 

./querier ./ ../idxfiles/indexnm # not valid crawler dir test

./querier ./ file   # unreadable file test 

valgrind ./querier ../pages ../indexer/indexnm < error-queries   # prof's error tests

valgrind querier ../pages ../indexer/indexnm -q < good-queries.txt > myoutput  # run quietly test with professor's queries 



