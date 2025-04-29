# Credit for much of the this script to 'mell-o-tron'
# https://github.com/mell-o-tron
# I did modify it a fair bit but it served as the basis and saved a lot of time

sudo rm -rf /var/cache/apt
sudo mkdir -p /var/cache/apt/archives/partial
sudo rm -rf /var/lib/dpkg/lock-frontend
sudo rm -rf /var/lib/dpkg/lock
sudo mkdir -p /var/lib/dpkg/{alternatives,info,parts,triggers,updates}
sudo cp /var/backups/dpkg.status.0 /var/lib/dpkg/status
sudo apt-get download dpkg
sudo dpkg -i dpkg*.deb
sudo apt-get download base-files
sudo dpkg -i base-files*.deb
sudo apt-get update
sudo apt-get check

sudo apt-get -y install texinfo
sudo apt-get -y install nasm

sudo apt-get -y update
sudo apt-get -y install build-essential
sudo apt-get -y awk
sudo apt-get -y install bison
sudo apt-get -y install flex
sudo apt-get -y install libgmp3-dev
sudo apt-get -y install libmpc-dev
sudo apt-get -y install libmpfr-dev
sudo apt-get -y install texinfo

sudo apt-get -y install curl
sudo apt-get -y update; apt-get -y install curl

export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

mkdir /tmp/src
cd /tmp/src
sudo curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
sudo tar xf binutils-2.39.tar.gz
mkdir binutils-build
cd binutils-build
../binutils-2.39/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
sudo make all install 2>&1 | tee make.log

cd /tmp/src
sudo curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir gcc-build
cd gcc-build
echo Configure: . . . . . . .
../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-language=c,c++ --without-headers
sudo make all-gcc
sudo make all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
ls /usr/local/i386elfgcc/bin
export PATH="$PATH:/usr/local/i386elfgcc/bin"
ls "/usr/local/i386elfgcc/bin"
alias i386-elf-gcc="/usr/local/i386elfgcc/bin/i386-elf-c++"

# After the installation is complete we will just run the makefile
# The root of the doros repo is mounted at /var in the docker image
cd /var
sudo make
