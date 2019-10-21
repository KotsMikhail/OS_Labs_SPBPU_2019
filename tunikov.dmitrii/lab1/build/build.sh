cmake -Wall -Werror ..
make
sudo sh -c "echo 'local0.*                        /var/log/local0.log' >> /etc/rsyslog.d/50-default.conf"
CURRENT=$(dirname $0)
rm $CURRENT/CMakeCache.txt $CURRENT/Makefile $CURRENT/cmake_install.cmake
rm -r $CURRENT/CMakeFiles/ 
