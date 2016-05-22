cd third_party/muduo && chmod u+x ./build.sh && ./build.sh && cd ../../
cd cppmysql && make -f Makefile.mk && cd ../
cd server && make -f Makefile.mk 
