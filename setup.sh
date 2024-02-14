
sudo groupadd avgroup 2>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Group avgroup created\e[m"
else
    echo "\e[0;31m [-] Failed to create group. Group avgroup already exists\e[m"
fi

sudo groupadd dummygroup 2>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Group dummygroup created\e[m"
else
    echo "\e[0;31m [-] Failed to create group. Group dummygroup already exists\e[m"
fi


sudo useradd -r -m -g avgroup -s /bin/false -d /home/av av 2>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] User av created\e[m"
else
    echo "\e[0;31m [-] Failed to create user. User av already exists\e[m"
fi


sudo useradd -g dummygroup --shell=/bin/false --no-create-home dummyuser 2>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] User dummyuser created\e[m"
else
    echo "\e[0;31m [-] Failed to create user. User dummyuser already exists\e[m"
fi

sudo usermod -L av 2>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Locked AV account.\e[m"
else
    echo "\e[0;31m [-] Failed to lock AV account.\e[m"
fi

echo "\e[0;34m [*] Downloading libssl-dev...\e[m"
sudo apt install libssl-dev 2>/dev/null 1>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] libssl-dev installed.\e[m"
else
    echo "\e[0;31m [-] Failed to install libssl-dev.\e[m"
fi


# Create AV encryption key
if [ ! -d "/home/av/security/" ]; then
    sudo mkdir /home/av/security/
    echo "\e[0;32m [+] Created security directory.\e[m"
else
    echo "\e[0;31m [-] Failed to create security directory. Directory already exists\e[m"
fi


echo "\e[0;34m [*] Creating encryption key...\e[m"
sudo sh -c 'openssl_output=$(openssl enc -aes-128-cbc -k secret -P -pbkdf2);key=$(echo "$openssl_output" | grep 'key=' | cut -d'=' -f2);echo "$key" > /home/av/security/enc.key'
echo "\e[0;32m [+] Created encryption key.\e[m"

sudo chmod 440 /home/av/security/enc.key
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set permissions for encryption key.\e[m"
else
    echo "\e[0;31m [-] Failed to set permissions for encryption key.\e[m"
fi

sudo chown av:avgroup /home/av/security/enc.key
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set ownership for encryption key.\e[m"
else
    echo "\e[0;31m [-] Failed to set ownership for encryption key.\e[m"
fi

# Create quarantine catalog

if [ ! -d "/var/lib/av/quarantine" ]; then
    sudo mkdir /var/lib/av/quarantine
    echo "\e[0;32m [+] Created quarantine directory.\e[m"
else 
    echo "\e[0;31m [-] Failed to create quarantine directory. Directory already exists\e[m"
fi

sudo chown av:avgroup /var/lib/av/
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set ownership for quarantine directory.\e[m"
else
    echo "\e[0;31m [-] Failed to set ownership for quarantine directory.\e[m"
fi

# Create log catalog

if [ ! -d "/var/lib/av/logs" ]; then
    sudo mkdir /var/lib/av/logs
    echo "\e[0;32m [+] Created log directory.\e[m"
else 
    echo "\e[0;31m [-] Failed to create logs directory. Directory already exists\e[m"
fi

sudo chown av:avgroup /var/lib/av/logs
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set ownership for logs directory.\e[m"
else
    echo "\e[0;31m [-] Failed to set ownership for logs directory.\e[m"
fi


# Compile binary
echo "\e[0;34m [*] Compiling binary...\e[m"
make 2>/dev/null 1>/dev/null
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Binary compiled.\e[m"
else
    echo "\e[0;31m [-] Failed to compile binary.\e[m"
fi
sudo chown av:avgroup avdaemon
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set ownership for binary.\e[m"
else
    echo "\e[0;31m [-] Failed to set ownership for binary.\e[m"
fi

# Set capabilities

sudo setcap cap_dac_read_search=+ep ./avdaemon
if [ $? -eq 0 ]; then
    echo "\e[0;32m [+] Set capabilities for binary.\e[m"
else
    echo "\e[0;31m [-] Failed to set capabilities for binary.\e[m"
fi



