# MAST

## Description
MAST is a modular intrusion detection system able of detecting any kind of anomaly which could be related to a malware activity.
Indeed, it checks if your computer is doing suspicious network activities or file system operations to determine the presence of a running ransomware or spyware.
Initially, MAST does a profiling of your system for a short period of time to figure out what type of activities should be considered as normal.

⚠: in this stage MAST assumes that your computer is clean and works as normal.

To guarantee a correct functioning, MAST uses cryptography and hashing algorithms to protect produced data and be always sure of their authenticity.
If some data get modified, MAST alerts the user through a notification for warning him that the system may not work properly.

## Used technologies
- Java and Java Swing for realizing the user interface;
- C++ and Crypto library have been used to develop the softwares which helps the system to encrypt and hash produced data
- Shell script (Bash) does all the job for both profiling and analyzing the system. Moreover, the system is thought to be modular: each script occupies to analyze one specific activity, so it become easy to add other functionalities in a future.

## How to install
Download the whole repository and execute the install.sh script. During the installation the program will ask you for a password, 
it will be used to encrypt all the encryption keys, so choose a difficult password to stay safe.

## How to execute
MAST starts automatically on system boot, then it will ask you for the password given during the installation to access encrypted data.

## Usage
At any moment you can open the user interface to verify the status of your system. There you can check what is doing each MAST module and if some suspicious activity has been detected.
The UI gives you also the opportunity to disable the MAST system or to launch it again. Note that if a suspicious activity is detected the system alerts you with a system notification.

⚠: You should not rely only on the statuses and the informations shown in the UI, because they could be not truthful.

## License
General Purpose License v3.0
