# harald-microphone-challange

Thanks to the assets that i got from itch.io :)

# Dependencies
 - raylib 
 - portaudio

 - raygui

 Install on Arch:
```bash 
sudo pacman -S portaudio raylib
```

Install on Ubuntu:
```bash 
sudo apt install portaudio19-dev
sudo add-apt-repository ppa:texus/raylib
sudo apt update
sudo apt install libraylib5-dev
```

# Building
If you didnt clone yet
```bash
git clone --recurse-submodules https://github.com/ttchef/harald-microphone-challange.git
```

Already cloned update all the sumbodules
```bash 
git submodule update --init --recursive
```
```bash 
make # To build
make run # To build and run
```

