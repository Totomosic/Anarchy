bash -c "strip ../bin/Debug-linux-x86_64/Anarchy-ServerHost/Anarchy-ServerHost"
bash -c "scp -i /.ssh/JordanSydney.pem ../bin/Debug-linux-x86_64/Anarchy-ServerHost/Anarchy-ServerHost ubuntu@52.64.129.71:~/Anarchy/"