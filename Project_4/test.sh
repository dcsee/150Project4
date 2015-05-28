./vm ../example/mutex.so > mymutex
./vm ../example/thread.so > mythread
./vm ../example/sleep.so > mysleep
./vm ../example/hello.so > myhello
./vm ../example/memory.so > mymemory
./vm ../example/file.so > myfile

../example/vm3 ../example/mutex.so > hismutex
../example/vm3 ../example/thread.so > histhread
../example/vm3 ../example/sleep.so > hissleep
../example/vm3 ../example/hello.so > hishello
../example/vm3 ../example/memory.so > hismemory
../example/vm3 ../example/file.so > hisfile

diff mymutex hismutex
diff mythread histhread
diff mysleep hissleep
diff myhello hishello
diff mymemory hismemory
diff myfile hisfile
