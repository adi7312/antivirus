#!/bin/sh

sudo groupadd avgroup
sudo groupadd dummygroup
sudo useradd -r -m -g avgroup -s /bin/false -d /home/av av
sudo useradd -g dummygroup --shell=/bin/false --no-create-home dummyuser
sudo usermod -L av

sudo apt install libssl-dev

openssl_output=$(openssl enc -aes-128-cbc -k secret -P -pbkdf2)
key=$(echo "$openssl_output" | grep 'key=' | cut -d'=' -f2)

# Create AV encryption key
if [ ! -d "/home/av/security/" ]; then
    sudo mkdir /home/av/security/
fi
sudo sh -c 'openssl_output=$(openssl enc -aes-128-cbc -k secret -P -pbkdf2);key=$(echo "$openssl_output" | grep 'key=' | cut -d'=' -f2);echo "$key" > /home/av/security/enc.key'
sudo chmod 440 /home/av/security/enc.key
sudo chown av:avgroup /home/av/security/enc.key

# Create quarantine catalog

if [ ! -d "/var/lib/av/quarantine"]; then
    sudo mkdir /var/lib/av/quarantine
fi

sudo chown av:avgroup /var/lib/av/

# Compile binary

make
sudo chown av:avgroup avdaemon




