./vm ../example/printOver512.so > myprint512
./vm ../example/readOver512.so > myread512
./vm ../example/outOfOrderDeallocate.so > myoutoforder

../example/vm3 ../example/printOver512.so > hisprint512
../example/vm3 ../example/readOver512.so > hisread512
../example/vm3 ../example/outOfOrderDeallocate.so > hisoutoforder

diff myprint512 hisprint512
diff myread512 hisread512
diff myoutoforder hisoutoforder
