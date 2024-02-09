#!/bin/sh

sudo groupadd avgroup
sudo useradd -r -m -g avgroup -s /bin/false -d /home/av av
sudo usermod -L av

sudo apt install libssl-dev

openssl_output=$(openssl enc -aes-128-cbc -k secret -P -pbkdf2)
key=$(echo "$openssl_output" | grep 'key=' | cut -d'=' -f2)

sudo mkdir /home/av/security/
sudo sh -c 'echo "$key" > /home/av/security/enc.key'
sudo chmod 440 /home/av/security/enc.key
sudo chown av:avgroup /home/av/security/enc.key


