bash -c "strip ../bin/Debug-linux-x86_64/Anarchy-ServerHost/Anarchy-ServerHost"
scp -i C:/.aws/JordanNewSydney.pem ../bin/Debug-linux-x86_64/Anarchy-ServerHost/Anarchy-ServerHost ubuntu@13.210.164.248:~/.